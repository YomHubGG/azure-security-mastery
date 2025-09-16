/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_pipe_handler.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 13:17:55 by tritter           #+#    #+#             */
/*   Updated: 2025/07/19 14:52:54 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*parse_pipe(t_token **list, t_data *data)
{
	t_ppa	acc;

	acc.i = 0;
	if (is_parentheses_surounding(*list))
		return (parse_group(list, data));
	acc.current = *list;
	while (acc.current)
	{
		if (acc.current->type == TOKEN_PIPE
			&& is_out_of_parentheses(*list, acc.i))
		{
			acc.butchered_list = butcher(list, TOKEN_PIPE);
			acc.left_ast = parse_group(list, data);
			if (acc.current->prev)
				return (create_pipe_node(acc.left_ast,
						parse_pipe(&acc.butchered_list, data), data));
			else
				return (create_pipe_node(NULL,
						parse_pipe(&acc.butchered_list, data), data));
		}
		acc.current = acc.current->next;
		acc.i++;
	}
	return (parse_redir(list, data));
}

/*will cut in half t_token **list at the first sight 
of to_cut out of parentheses, will return the pointer after to_cut*/
t_token	*butcher(t_token **list, t_token_type to_cut)
{
	t_token	*current;
	t_token	*prev;
	int		i;

	i = 0;
	current = *list;
	prev = NULL;
	while (current)
	{
		if (current->type == to_cut && is_out_of_parentheses(*list, i))
		{
			if (prev)
				prev->next = NULL;
			else
				*list = NULL;
			return (current->next);
		}
		i++;
		prev = current;
		current = current->next;
	}
	return (*list);
}
