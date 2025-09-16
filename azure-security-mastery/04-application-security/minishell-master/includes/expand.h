/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 14:34:44 by tritter           #+#    #+#             */
/*   Updated: 2025/07/23 17:59:54 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_H
# define EXPAND_H

# include "minishell.h"

typedef enum err
{
	EXPAND_MALLOC_ERR = 1,
	EXPAND_UNVALID_AST,
	EXPAND_UNKNOWN_AST_NODE_TYPE
}	t_err;

typedef struct init_expand_word
{
	int		index;
	char	**word;
	bool	s_quoted;
	bool	d_quoted;
}	t_init_expand_word;

typedef struct expand_word
{
	char	*begining;
	char	*inbetween;
	char	*end;
	char	*ret;
	int		fdtok;
	int		i2;
}	t_expand_word;

//expand_helper.c
char	*ft_strjoin_injector(const char *s1, const char *s2, t_zone_g **zone);
char	*ft_itoa_injector(int n, t_zone_g **zone);
char	**ft_split_injector(char const *s, char c, t_zone_g **zone);
char	*ft_substr_injector(const char *s, unsigned int start, size_t len,
			t_zone_g **zone);
bool	expand_word_helper(char **to_ex, t_data *data,
			t_expand_word *access);

//expand_init.c
int		expand_explore_tree(t_tree_node *node, t_data *data);
int		expand_explore_tree_2(t_tree_node *node, t_data *data);
int		expand_explore_tree_3(t_tree_node *node, t_data *data);
int		expand_explore_tree_4(t_tree_node *node, t_data *data);
int		expand_explore_tree_5(t_tree_node *node, t_data *data);

//expand_word.c
int		init_expand_word(t_tree_node *node, t_data *data);
char	*expand_word(int *i, char **to_ex, t_data *data);
char	*expand_word_join(t_expand_word access, int *i, t_data *data);
int		expand_validate(t_init_expand_word	access, t_data *data);
char	*hardcode_tilde_expansion(char *word, t_data *data);

//expand_error_parsing.c
void	expand_err_msg(int err_msg);

//expand_helper2.c
void	expand_word_helper_subwrp(t_expand_word *ac, t_data *data);

//expand_utils.c
char	*extract_word(char *key, t_data *data);
int		ft_strcmp(const char *s1, const char *s2);

#endif