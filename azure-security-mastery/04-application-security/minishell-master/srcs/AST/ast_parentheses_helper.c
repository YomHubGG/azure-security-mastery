/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_parentheses_helper.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 09:10:29 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 11:48:36 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_out_of_parentheses(t_token *list, int index)
{
	int		i;
	int		depth;
	t_token	*curr;

	curr = list;
	depth = 0;
	i = 0;
	if (!curr)
		return (true);
	while (curr)
	{
		if (curr->type == TOKEN_L_PARENTHESIS)
			depth++;
		if (curr->type == TOKEN_R_PARENTHESIS)
			depth--;
		if (i == index)
			return (!depth);
		i++;
		curr = curr->next;
	}
	return (false);
}

bool	is_parentheses_surounding(t_token *list)
{
	t_token	*curr;
	int		depth;

	curr = list;
	depth = 0;
	if (!curr || curr->type != TOKEN_L_PARENTHESIS)
		return (false);
	while (curr)
	{
		if (curr->type == TOKEN_L_PARENTHESIS)
			depth++;
		else if (curr->type == TOKEN_R_PARENTHESIS)
			depth--;
		if (depth == 0 && curr->next)
			return (false);
		curr = curr->next;
	}
	return (depth == 0);
}

//removes surounding parentheses, 
//SHOULD ONLY BE USED AFTER is_parentheses_surounding as been assesed
//failure to follow this while result in undefined behavior
t_token	*rm_parentheses(t_token *list)
{
	t_token	*curr;
	t_token	*ret;

	ret = list->next;
	curr = list->next;
	list->next->prev = NULL;
	list->next = NULL;
	while (curr)
	{
		if (!curr->next)
			break ;
		curr = curr->next;
	}
	curr->prev->next = NULL;
	curr->prev = NULL;
	return (ret);
}

t_tree_node	*parse_group(t_token **list, t_data *data)
{
	t_tree_node	*group;

	if (is_parentheses_surounding(*list))
	{
		*list = rm_parentheses(*list);
		group = create_pareng_node(parse_lor_land(list, data), data);
		if (!group)
			return (NULL);
		return (group);
	}
	else
		return (parse_lor_land(list, data));
}
