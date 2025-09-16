/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_lor_land_handler.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:37:56 by tritter           #+#    #+#             */
/*   Updated: 2025/06/25 16:51:10 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*parse_lor_land(t_token **list, t_data *data)
{
	t_token		*current;

	current = *list;
	if (is_parentheses_surounding(*list))
		return (parse_group(list, data));
	return (parse_lor_land_helper(current, list, data, 0));
}

t_tree_node	*parse_lor_land_helper(t_token *current, t_token **list,
	t_data *data, int i)
{
	while (current)
	{
		if (current->type == TOKEN_LOGICAL_AND
			&& is_out_of_parentheses(*list, i))
		{
			if (current->prev)
				return (land_subwrapper(list, data));
			else
				return (create_land_node(NULL, NULL, data));
		}
		if (current->type == TOKEN_LOGICAL_OR
			&& is_out_of_parentheses(*list, i))
		{
			if (current->prev)
				return (lor_subwrapper(list, data));
			else
				return (create_lor_node(NULL, NULL, data));
		}
		current = current->next;
		i++;
	}
	return (parse_pipe(list, data));
}

t_tree_node	*lor_subwrapper(t_token **list, t_data *data)
{
	t_tree_node	*left_ast;
	t_token		*butchered_list;

	butchered_list = butcher(list, TOKEN_LOGICAL_OR);
	left_ast = parse_pipe(list, data);
	return (create_lor_node(left_ast,
			parse_lor_land(&butchered_list, data), data));
}

t_tree_node	*land_subwrapper(t_token **list, t_data *data)
{
	t_token		*butchered_list;
	t_tree_node	*left_ast;

	butchered_list = butcher(list, TOKEN_LOGICAL_AND);
	left_ast = parse_pipe(list, data);
	return (create_land_node(left_ast,
			parse_lor_land(&butchered_list, data), data));
}
