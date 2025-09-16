/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 08:35:38 by tritter           #+#    #+#             */
/*   Updated: 2025/07/19 15:01:54 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
# define AST_H

# include "minishell.h"

struct					s_data;
typedef struct s_data	t_data;

typedef enum error_type
{
	INVALID_HEREDOC,
	INVALID_REDIRECTION,
	INVALID_WORD
}	t_error_type;

typedef enum content_type
{
	WORD,
	HERE_DOC,
	PIPE,
	PARENTHESIS_P,
	REDIRECT_IN,
	REDIRECT_OUT,
	LOGICAL_OR,
	LOGICAL_AND,
	APPEND,
	ERROR
}	t_content_type;

typedef union node_token
{
	struct word_token				*word;
	struct redirect_in				*redirect_in;
	struct redirect_out				*redirect_out;
	struct logical_or_token			*logical_or;
	struct logical_and_token		*logical_and;
	struct pipe_token				*pipe;
	struct parenthesis_placeholder	*parenthesis;
	struct heredoc_token			*heredoc;
	struct append_token				*append;
	struct error_token				*error;
}	t_node_token;

typedef struct error_token
{
	t_error_type	error;
	t_token			*leftovers;
}	t_error_token;

typedef struct word_token
{
	struct tree_node	*next_word;
	t_token				*word_subtype;
}	t_word_token;

typedef struct tree_node
{
	t_content_type	type;
	t_node_token	token;
}	t_tree_node;

typedef struct append_token
{
	t_tree_node		*left;
	t_tree_node		*right;
}	t_append_token;

typedef struct pipe_token
{
	t_tree_node		*word1;
	t_tree_node		*word2;
}	t_pipe_token;

typedef struct redirect_in
{
	t_tree_node		*left;
	t_tree_node		*right;
}	t_redirect_in;

typedef struct redirect_out
{
	t_tree_node		*left;
	t_tree_node		*right;
}	t_redirect_out;

typedef struct heredoc_token
{
	int				premade_fd;
	t_tree_node		*left;
	t_tree_node		*right;
}	t_heredoc_token;

typedef struct parenthesis_placeholder
{
	t_tree_node		*subtree;
}	t_parenthesis_placeholder;

typedef struct logical_or_token
{
	t_tree_node		*left;
	t_tree_node		*right;
}	t_logical_or_token;

typedef struct logical_and_token
{
	t_tree_node		*left;
	t_tree_node		*right;
}	t_logical_and_token;

typedef struct ppa
{
	t_token		*current;
	t_tree_node	*left_ast;
	t_token		*butchered_list;
	int			i;
}	t_ppa;

typedef struct pwa
{
	t_tree_node	**word_h;
	t_tree_node	*new_word;
	t_token		*current;
}	t_pwa;

//ast_creation.c
t_tree_node	*create_ast(t_token **list, t_data *data);

//ast_lor_land_handler.c
t_tree_node	*parse_lor_land(t_token **list, t_data *data);
t_tree_node	*lor_subwrapper(t_token **list, t_data *data);
t_tree_node	*land_subwrapper(t_token **list, t_data *data);
t_tree_node	*parse_lor_land_helper(t_token *current, t_token **list,
				t_data *data, int i);

//ast_node_creation_1.c
t_tree_node	*create_word_node(t_token *token, t_data *data);
t_tree_node	*create_pipe_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_redirect_in_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_redirect_out_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_pareng_node(t_tree_node *subtree, t_data *data);

//ast_node_creation_2.c
t_tree_node	*create_lor_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_land_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_hdoc_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_append_node(t_tree_node *left,
				t_tree_node *right, t_data *data);
t_tree_node	*create_error_node(t_token *leftovers,
				t_error_type error_type, t_data *data);

//ast_parentheses_helper.c
bool		is_parentheses_surounding(t_token *list);
bool		is_out_of_parentheses(t_token *list, int index);
t_token		*rm_parentheses(t_token *list);
t_tree_node	*parse_group(t_token **list, t_data *data);

//ast_pipe_handler.c
t_tree_node	*parse_pipe(t_token **list, t_data *data);
t_token		*butcher(t_token **list, t_token_type to_cut);

//ast_redirection_handler.c
t_token		**rm_redir(t_token **token, t_token_type type, t_token *curr);
t_tree_node	*parse_redir(t_token **token, t_data *data);
t_token		**rm_redir_helper(t_token *curr, t_token *target, t_token *next,
				t_token **token);
int			token_counter(t_token **token);

//ast_word_handler.c
t_tree_node	*parse_word(t_token **token, t_data *data);
bool		is_word_subtype(t_token_type type);
void		word_add_back(t_tree_node **words, t_tree_node *new_word);
t_tree_node	*ft_last_word(t_tree_node *lst);

//ast_redirection_handler_subwrapper.c
t_tree_node	*redir_append_subwrapper(t_tree_node *new_node, t_token **token,
				t_token *current, t_data *data);
t_tree_node	*redir_rout_subwrapper(t_tree_node *new_node, t_token **token,
				t_token *current, t_data *data);
t_tree_node	*redir_rin_subwrapper(t_tree_node *new_node, t_token **token,
				t_token *current, t_data *data);
t_tree_node	*parse_heredoc_subwrapper(t_token *current, t_tree_node *new_node,
				t_token **token, t_data *data);

#endif // AST_H
