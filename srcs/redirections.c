#include "../includes/minishell.h"

// Convert an integer to a string without using sprintf
char *int_to_str(int n)
{
	char *str;
	int len = 1;
	int temp = n;
	
	// Calculate length
	while (temp / 10 != 0)
	{
		temp /= 10;
		len++;
	}
	
	str = (char *)malloc(len + 1);
	if (!str)
		return (NULL);
	
	str[len] = '\0';
	
	// Convert number to string
	while (len > 0)
	{
		str[--len] = '0' + (n % 10);
		n /= 10;
	}
	
	return (str);
}

int	redirect_input(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		print_error(file, NULL, strerror(errno));
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	redirect_output(char *file, int append)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		print_error(file, NULL, strerror(errno));
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

// Special signal handler for heredoc mode
void	heredoc_sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		g_signal_code = 130; // Set exit code to 130 (interrupted by Ctrl+C)
		ft_putchar_fd('\n', STDOUT_FILENO);
		close(STDIN_FILENO); // This will cause readline to return NULL
		// Don't call rl_redisplay or other readline functions here
	}
}

// Create a temporary file for storing heredoc content
char *create_heredoc_tempfile(void)
{
	static int counter = 0;
	char *filename = NULL;
	char *num_str;
	
	// Convert counter to string using our custom function
	num_str = int_to_str(counter++);
	if (!num_str)
		return (NULL);
	
	// Create filename like /tmp/minishell_heredoc_123
	filename = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	
	return filename;
}

int	handle_heredoc(t_shell *shell, t_redir *redir, char **temp_file)
{
	int		fd;
	char	*line;
	char	*expanded_line;
	char	*delimiter;
	struct sigaction	sa_heredoc, sa_old;

	delimiter = redir->file;
	
	// Create temporary file for heredoc content
	*temp_file = create_heredoc_tempfile();
	if (!*temp_file)
	{
		print_error("malloc", NULL, strerror(errno));
		return (1);
	}
	
	fd = open(*temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		print_error(*temp_file, NULL, strerror(errno));
		free(*temp_file);
		*temp_file = NULL;
		return (1);
	}
	
	// Set up special signal handling for heredoc
	sa_heredoc.sa_handler = heredoc_sig_handler;
	sigemptyset(&sa_heredoc.sa_mask);
	sa_heredoc.sa_flags = 0;
	sigaction(SIGINT, &sa_heredoc, &sa_old);
	
	g_signal_code = 0; // Reset signal code
	
	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
		{
			// EOF or Ctrl+C was received
			if (g_signal_code == 130)
			{
				// Restore signal handlers before returning
				sigaction(SIGINT, &sa_old, NULL);
				close(fd);
				return (130); // Return the specific exit code for interruption
			}
			ft_putendl_fd("warning: here-document delimited by end-of-file", 2);
			break;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		
		// Expand variables in the heredoc line
		expanded_line = expand_variables(shell, line, 0);
		if (expanded_line)
		{
			ft_putendl_fd(expanded_line, fd);
			free(expanded_line);
		}
		else
		{
			ft_putendl_fd(line, fd);
		}
		
		free(line);
	}
	
	// Restore original signal handlers
	sigaction(SIGINT, &sa_old, NULL);
	
	close(fd);
	return (0);
}

// Apply a heredoc redirection using the temporary file
int apply_heredoc_redirection(char *temp_file)
{
	int fd;
	
	fd = open(temp_file, O_RDONLY);
	if (fd == -1)
	{
		print_error(temp_file, NULL, strerror(errno));
		return (1);
	}
	
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(fd);
		return (1);
	}
	
	close(fd);
	return (0);
}

// Free all temporary heredoc files
void cleanup_heredoc_tempfiles(char **temp_files, int count)
{
	int i;
	
	for (i = 0; i < count; i++)
	{
		if (temp_files[i])
		{
			unlink(temp_files[i]); // Delete the temporary file
			free(temp_files[i]);
		}
	}
	free(temp_files);
}

int	setup_redirections(t_shell *shell, t_command *cmd)
{
	t_redir	*redir;
	int     result;
	int     heredoc_count = 0;
	char    **heredoc_tempfiles = NULL;
	int     i = 0;

	if (!cmd)
		return (0);
	
	// First, count how many heredocs we have
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			heredoc_count++;
		redir = redir->next;
	}
	
	// Allocate memory for the heredoc tempfiles if needed
	if (heredoc_count > 0)
	{
		heredoc_tempfiles = (char **)malloc(sizeof(char *) * heredoc_count);
		if (!heredoc_tempfiles)
		{
			print_error("malloc", NULL, strerror(errno));
			return (1);
		}
		
		// Initialize array
		for (i = 0; i < heredoc_count; i++)
			heredoc_tempfiles[i] = NULL;
			
		// Process all heredocs first
		redir = cmd->redirs;
		i = 0;
		while (redir && i < heredoc_count)
		{
			if (redir->type == TOKEN_HEREDOC)
			{
				result = handle_heredoc(shell, redir, &heredoc_tempfiles[i]);
				if (result != 0)
				{
					cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
					return (result);
				}
				i++;
			}
			redir = redir->next;
		}
	}
	
	// Now apply all redirections in order
	redir = cmd->redirs;
	i = 0; // Reset heredoc counter
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
		{
			if (redirect_input(redir->file) != 0)
			{
				if (heredoc_tempfiles)
					cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
				return (1);
			}
		}
		else if (redir->type == TOKEN_REDIR_OUT)
		{
			if (redirect_output(redir->file, 0) != 0)
			{
				if (heredoc_tempfiles)
					cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
				return (1);
			}
		}
		else if (redir->type == TOKEN_REDIR_APPEND)
		{
			if (redirect_output(redir->file, 1) != 0)
			{
				if (heredoc_tempfiles)
					cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
				return (1);
			}
		}
		else if (redir->type == TOKEN_HEREDOC)
		{
			// Apply the heredoc redirection from the temporary file
			if (apply_heredoc_redirection(heredoc_tempfiles[i]) != 0)
			{
				cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
				return (1);
			}
			i++;
		}
		redir = redir->next;
	}
	
	// Clean up the temporary files
	if (heredoc_tempfiles)
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
	
	return (0);
} 