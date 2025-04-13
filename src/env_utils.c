#include "../inc/minishell.h"
#include <stdio.h>

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int ft_strcmp(char *s1, char *s2)
{
	while(s1)
	{
		while(s2)
		{
			if(s2 != s1)
				return (0);
			s2++;
		}
		s1++;
	}
	return (1);
}

static char **ft_alloc_split(char const *s, char c)
{
	size_t	i;
	char	**split;
	size_t	total;

	i = 0;
	total = 0;
	while (s[i])
	{
		if (s[i] == c)
			total++;
		i++;
	}
	split = (char**)malloc(sizeof(s) * (total + 2));
	if (!split)
		return (NULL);
	return (split);
}

void	*ft_free_all_split_alloc(char **split, size_t elts)
{
	size_t	i;

	i = 0;
	while (i < elts)
	{
		free(split[i]);
		i++;
	}
	free(split);
	return (NULL);
}

static void *ft_split_range(char **split, char const *s, t_split_next *st, t_split_next *lt)
{
	split[lt->length] = ft_substr(s, st->start, st->length);
	if (!split[lt->length])
		return (ft_free_all_split_alloc(split, lt->length));
	lt->length++;
	return (split);
}

static void *ft_split_by_char(char **split, char const *s, char c)
{
	size_t			i;
	t_split_next	st;
	t_split_next	lt;
    int break_flag;

	i = 0;
    break_flag = 0;
	lt.length = 0;
	lt.start = 0;
	while (s[i] && break_flag != 1)
	{
		if (s[i] == c)
		{
			st.start = lt.start;
			st.length = (i - lt.start);
			if (i > lt.start && !ft_split_range(split, s, &st, &lt))
				return (NULL);
			lt.start = i + 1;
            break_flag = 1;
		}
		i++;
	}
	st.start = lt.start;
	st.length = (ft_strlen(s) - lt.start);
	if (ft_strlen(s) > lt.start && ft_strlen(s) > 0 && !ft_split_range(split, s, &st, &lt))
		return (NULL);
	split[lt.length] = 0;
	return (split);
}

char	**ft_split_once(char const *s, char c)
{
	char	**split;

	if (!(split = ft_alloc_split(s, c)))
		return (NULL);
	if (!ft_split_by_char(split, s, c))
		return (NULL);
	return (split);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	j;
	char	*str;

	str = (char*)malloc(sizeof(*s) * (len + 1));
	if (!str)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		if (i >= start && j < len)
		{
			str[j] = s[i];
			j++;
		}
		i++;
	}
	str[j] = 0;
	return (str);
}

char	*ft_strdup(const char *s1)
{
	char	*str;
	size_t	i;

	if (!s1)
		return (NULL);
	str = (char*)malloc(sizeof(*s1) * (ft_strlen(s1) + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	str = (char*)malloc(
		sizeof(*s1) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!str)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[i])
	{
		str[j++] = s1[i];
		i++;
	}
	i = 0;
	while (s2[i])
	{
		str[j++] = s2[i];
		i++;
	}
	str[j] = 0;
	return (str);
}

//! TO_REVIEW		All functions below do not check for input validity

int	env_var_count(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		i++;
	return (i);
}

t_env_list *make_linked_list(char **env) {
    t_env_list *head;
    t_env_list *current;
    int i;
    char **data;

    i = 0;
    head = NULL;
    while (env[i]) {
        data = ft_split_once(env[i], '=');
        current = (t_env_list *)malloc(sizeof(t_env_list));
        current->name = ft_strdup(data[0]);
        current->value = ft_strdup(data[1]);
        current->data = ft_strdup(env[i]);
        current->next = head;
        head = current;
        i++;
    }
    return head;
}

void print_env(t_env_list *head)
{
	t_env_list *current;

	current = head;
	while (current->next) {
		printf("%s=", current->name);
		printf("%s\n", current->value);
        current = current->next;
    }
	printf("%s=", current->name);
	printf("%s\n", current->value);
}

void print_env_export(t_env_list *head)
{
	t_env_list *current;

	current = head;
	while (current->next) {
		printf("declare -x %s=", current->name);
		printf("\"%s\"\n", current->value);
        current = current->next;
    }
	printf("declare -x %s=", current->name);
	printf("\"%s\"\n", current->value);
}

t_env_list *find_env_var(char *name, t_env_list *head) {
    t_env_list *current;

    current = head;
    while (current) {
        if (ft_strcmp(current->name, name) == 1) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int check_list_dup(char *data, t_env_list *head)
{
	t_env_list *current;

	current = head;
	while (current)
	{
		if (ft_strcmp(current->data, data) == 0)
			return (1);
		current = current->next;
	}
	return (0);
}

void add_env_var(char *data, t_env_list *head)
{
    t_env_list *current;
    t_env_list *new;
	char **temp;

    current = head;
    new = (t_env_list *)malloc(sizeof(t_env_list));
    temp = ft_split_once(data, '=');
	if(find_env_var(temp[0], head) == NULL)
	{
		new->name = ft_strdup(temp[0]);
		new->value = ft_strdup(temp[1]);
		new->data = ft_strdup(data);
		new->next = NULL;
		while (current->next) {
			current = current->next;
		}
		current->next = new;
	}
}

char *expand(char *name, t_env_list *head)
{
	t_env_list *current;

	current = find_env_var(name, head);
	if(current != NULL)
		return current->value;
	return NULL;
	//! check if the expanded string is a file/command/dir
}

int is_less_than(char *s1 , char *s2)
{
	int i;

	i=0;
	while(s1[i] && s2[i])
	{
		if(s1[i] < s2[i])
			return 1;
		else if(s1[i] > s2[i])
			return 0;
		i++;
	}
	return(1);
}

//! OPTI_ANGLE : Needs to get slightly quicker with the checking of if the list is sorted or not and maybe only calling it once every few iterations
int check_if_sorted(t_env_list *head)
{
	t_env_list *current;

	current = head;
	while (current->next)
	{
		if (!is_less_than(current->name, current->next->name))
			return (0);
		current = current->next;
	}
	return (1);
}

t_env_list *sort_env_list(t_env_list *head)
{
	t_env_list *current;
	char *temp_name;
	char *temp_value;
	char *temp_data;

	current = head;
	while (!check_if_sorted(head))
	{
		if(!current->next)
			current = head;
		if (!is_less_than(current->name, current->next->name))
		{
			temp_name = current->next->name;
			temp_value = current->next->value;
			temp_data = current->next->data;
			current->next->name = current->name;
			current->next->value = current->value;
			current->next->data = current->data;
			current->name = temp_name;
			current->value = temp_value;
			current->data = temp_data;
			current = head;
		}
		current = current->next;
	}
	return (head);
}

void export(t_env_list *head)
{
	sort_env_list(head);
	print_env_export(head);
}

// int main(int argc, char **argv, char **envp){
//     (void) argv;
//     (void) argc;
// 	// t_env_list *head = make_linked_list(envp);
// 	t_env_list *head2 = make_linked_list(envp);
// 	export(head2);
//     return(0);
// }
