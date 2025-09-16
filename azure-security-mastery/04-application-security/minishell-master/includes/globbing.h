/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 12:29:36 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 14:07:06 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBBING_H
# define GLOBBING_H

# include "minishell.h"

typedef enum g_err
{
	GLOBBING_MALLOC_ERR = 1,
	GLOBBING_UNVALID_AST,
	GLOBBING_UNKNOWN_AST_NODE_TYPE
}	t_g_err;

typedef struct glob_node
{
	char				*d_name;
	struct glob_node	*next;
	struct glob_node	*prev;
}	t_glob_node;

typedef struct pgn
{
	t_glob_node	*g_current;
	t_tree_node	*t_current;
	t_tree_node	*new_t_node;
	t_token		*old;
	t_token		*n_token;
	char		*to_comp;
	int			i;
}	t_pgn;

//globbing_explore.c
int			globbing_explore_tree(t_tree_node *node, t_data *data);
int			globbing_explore_tree_2(t_tree_node *node, t_data *data);
int			globbing_explore_tree_3(t_tree_node *node, t_data *data);
int			globbing_explore_tree_4(t_tree_node *node, t_data *data);

//globbing_helper.c
void		insert_tree_node(t_tree_node **node, t_tree_node *new_t_node,
				t_tree_node *t_current, int i);
bool		wildcard_match(char *pattern, char *text);
t_token		*token_divider(t_token *old, char *value, t_data *data);

//globbing_init.c
int			init_globbing(t_tree_node **node, t_data *data);
int			process_globe_nodes(t_tree_node **node,
				t_glob_node **g_head, t_data *data);
t_pgn		init_pgn(t_tree_node **node, t_glob_node **g_head);

//globbing_node.c
void		create_add_back_glob_node(char *d_name, t_data *data,
				t_glob_node **g_head);
t_glob_node	*new_glob_node(char *d_name, t_data *data);
void		node_glob_add_back(t_glob_node *new_glob_node,
				t_glob_node **g_head);
t_glob_node	*ft_last_gn(t_glob_node *g_head);

#endif