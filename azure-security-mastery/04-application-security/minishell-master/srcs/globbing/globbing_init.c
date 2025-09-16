/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing_init.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:14:32 by tritter           #+#    #+#             */
/*   Updated: 2025/07/23 16:50:50 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	init_globbing(t_tree_node **node, t_data *data)
{
	DIR				*dir;
	t_glob_node		**g_head;
	struct dirent	*entry;

	g_head = g_malloc(data->hot_zone, sizeof(t_glob_node *));
	if (!g_head)
		return (1);
	(*g_head) = NULL;
	dir = opendir(".");
	if (!dir)
	{
		perror("opendir");
		return (1);
	}
	entry = readdir(dir);
	while (entry)
	{
		create_add_back_glob_node(entry->d_name, data, g_head);
		entry = readdir(dir);
	}
	closedir(dir);
	return (process_globe_nodes(node, g_head, data));
}

int	process_globe_nodes(t_tree_node **node, t_glob_node **g_head, t_data *data)
{
	t_pgn	ac;

	ac = init_pgn(node, g_head);
	while (ac.g_current)
	{
		if (wildcard_match(ac.to_comp, ac.g_current->d_name))
		{
			ac.n_token = token_divider(ac.old, ac.g_current->d_name, data);
			if (!ac.n_token)
				return (1);
			ac.new_t_node = create_word_node(ac.n_token, data);
			if (!ac.new_t_node)
				return (1);
			insert_tree_node(node, ac.new_t_node, ac.t_current, ac.i);
			if (ac.i != 0)
				ac.t_current = ac.new_t_node;
			ac.i++;
		}
		ac.g_current = ac.g_current->next;
	}
	return (0);
}

t_pgn	init_pgn(t_tree_node **node, t_glob_node **g_head)
{
	t_pgn	ac;

	ac.i = 0;
	ac.t_current = (*node);
	ac.to_comp = (*node)->token.word->word_subtype->value;
	ac.old = (*node)->token.word->word_subtype;
	ac.g_current = (*g_head);
	return (ac);
}
