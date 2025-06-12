#include "../includes/minishell.h"
#include <signal.h>

// Calculate length of an integer when converted to string
static int calc_int_length(int n)
{
	int len = 1;
	int temp = n;
	
	while (temp / 10 != 0)
	{
		temp /= 10;
		len++;
	}
	return len;
}

// Convert an integer to a string
char *int_to_str(int n)
{
	char *str;
	int len = calc_int_length(n);
	
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

// Get appropriate flags for output redirection
static int get_output_flags(int append)
{
	int flags = O_WRONLY | O_CREAT;
	
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	return flags;
}

int	redirect_output(char *file, int append)
{
	int	fd;
	int	flags;

	flags = get_output_flags(append);
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
	}
}

// Set up signal handling for heredoc
static void setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_sig_handler);
	g_signal_code = 0; // Reset signal code
}

// Restore original signal handling
static void restore_heredoc_signals(void)
{
	signal(SIGINT, SIG_DFL);
}

// Create heredoc filename
static char *create_heredoc_filename(int counter)
{
	char *num_str;
	char *filename;
	
	num_str = int_to_str(counter);
	if (!num_str)
		return (NULL);
	
	filename = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	
	return filename;
}

// Create a temporary file for storing heredoc content
char *create_heredoc_tempfile(void)
{
	static int counter = 0;
	return create_heredoc_filename(counter++);
}

// Process heredoc input lines
static int process_heredoc_lines(t_shell *shell, char *delimiter, int fd)
{
	char *line;
	char *expanded_line;
	
	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
		{
			// EOF or Ctrl+C was received
			if (g_signal_code == 130)
				return (130);
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
			ft_putendl_fd(line, fd);
		
		free(line);
	}
	return (0);
}

// Handle creating and filling heredoc
int	handle_heredoc(t_shell *shell, t_redir *redir, char **temp_file)
{
	int fd;
	int result;
	
	// Create temporary file for heredoc content
	*temp_file = create_heredoc_tempfile();
	if (!*temp_file)
		return (print_error("malloc", NULL, strerror(errno)), 1);
	
	fd = open(*temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		print_error(*temp_file, NULL, strerror(errno));
		free(*temp_file);
		*temp_file = NULL;
		return (1);
	}
	
	// Set up special signal handling for heredoc
	setup_heredoc_signals();
	
	// Process heredoc input
	result = process_heredoc_lines(shell, redir->file, fd);
	
	// Restore original signal handlers
	restore_heredoc_signals();
	close(fd);
	
	return (result);
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

// Clean up a single temporary file
static void cleanup_single_tempfile(char *temp_file)
{
	if (temp_file)
	{
		unlink(temp_file); // Delete the temporary file
		free(temp_file);
	}
}

// Free all temporary heredoc files
void cleanup_heredoc_tempfiles(char **temp_files, int count)
{
	int i;
	
	for (i = 0; i < count; i++)
		cleanup_single_tempfile(temp_files[i]);
	
	free(temp_files);
}

// Count heredoc redirections in a command
static int count_heredocs(t_command *cmd)
{
	t_redir *redir;
	int count = 0;
	
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			count++;
		redir = redir->next;
	}
	return count;
}

// Initialize array of heredoc tempfiles
static char **init_heredoc_tempfiles(int count)
{
	char **tempfiles;
	int i;
	
	tempfiles = (char **)malloc(sizeof(char *) * count);
	if (!tempfiles)
	{
		print_error("malloc", NULL, strerror(errno));
		return NULL;
	}
	
	for (i = 0; i < count; i++)
		tempfiles[i] = NULL;
	
	return tempfiles;
}

// Process all heredocs for a command
static int process_heredocs(t_shell *shell, t_command *cmd, char **tempfiles)
{
	t_redir *redir;
	int i = 0;
	int result;
	
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			result = handle_heredoc(shell, redir, &tempfiles[i]);
			if (result != 0)
				return result;
			i++;
		}
		redir = redir->next;
	}
	return 0;
}

// Apply a single redirection
static int apply_redirection(t_redir *redir, 
                            char **heredoc_tempfiles, int *heredoc_index)
{
	if (redir->type == TOKEN_REDIR_IN)
		return redirect_input(redir->file);
	else if (redir->type == TOKEN_REDIR_OUT)
		return redirect_output(redir->file, 0);
	else if (redir->type == TOKEN_REDIR_APPEND)
		return redirect_output(redir->file, 1);
	else if (redir->type == TOKEN_HEREDOC)
		return apply_heredoc_redirection(heredoc_tempfiles[(*heredoc_index)++]);
	
	return 0;
}

// Apply all redirections in order
static int apply_all_redirections(t_command *cmd, 
                                 char **heredoc_tempfiles)
{
	t_redir *redir;
	int heredoc_index = 0;
	int result;
	
	redir = cmd->redirs;
	while (redir)
	{
		result = apply_redirection(redir, heredoc_tempfiles, &heredoc_index);
		if (result != 0)
			return result;
		redir = redir->next;
	}
	return 0;
}

int	setup_redirections(t_shell *shell, t_command *cmd)
{
	int     heredoc_count = 0;
	char    **heredoc_tempfiles = NULL;
	int     result;

	if (!cmd)
		return (0);
	
	// First, count how many heredocs we have
	heredoc_count = count_heredocs(cmd);
	
	// Allocate memory for the heredoc tempfiles if needed
	if (heredoc_count > 0)
	{
		heredoc_tempfiles = init_heredoc_tempfiles(heredoc_count);
		if (!heredoc_tempfiles)
			return (1);
			
		// Process all heredocs first
		result = process_heredocs(shell, cmd, heredoc_tempfiles);
		if (result != 0)
		{
			cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
			return (result);
		}
	}
	
	// Now apply all redirections in order
	result = apply_all_redirections(cmd, heredoc_tempfiles);
	if (result != 0)
	{
		if (heredoc_tempfiles)
			cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
		return (result);
	}
	
	// Clean up the temporary files
	if (heredoc_tempfiles)
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
	
	return (0);
} 