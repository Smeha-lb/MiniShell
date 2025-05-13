#include "../inc/minishell.h"
#include <math.h>

// typedef struct s_queue
// {
// 	t_list	*front;
// 	t_list	*rear;
// } t_queue;

// bool	queue_is_empty(t_queue *q)
// {
// 	return (q->front == NULL);
// }

// static void	enqueue(t_queue *queue, t_astree *node)
// {
// 	t_list	*new;

// 	new = ft_lstnew(node);
// 	if (queue->rear == NULL)
// 	{
// 		queue->front = new;
// 		queue->rear = new;
// 		return ;
// 	}

// 	queue->rear->next = new;
// 	queue->rear = new;
// }

// t_astree	*dequeue(t_queue *queue)
// {
// 	t_list		*temp;
// 	t_astree	*node;

// 	if (queue_is_empty(queue))
// 		return (printf("Error: Queue Underflow\n"), NULL);

// 	temp = queue->front;
// 	node = temp->content;

// 	queue->front = queue->front->next;

// 	if (queue->front == NULL)
// 		queue->rear = NULL;

// 	free(temp);
// 	return (node);
// }

const char *token_to_str(t_token token)
{
    switch (token)
    {
        case NOT:			return "NOT";
        case PIPE:			return "PIPE";
        case HEREDOC:		return "HEREDOC";
        case LEFT_PAREN:	return "LEFT_PAREN";
        case RIGHT_PAREN:	return "RIGHT_PAREN";
        case AND:			return "AND";
        case OR:			return "OR";
        case APPEND:		return "APPEND";
        case OUT:			return "OUT";
        case IN:			return "IN";
        case END:			return "END";
		case NONE:			return "∅";
        default:			return "ERR?";
    }
}
