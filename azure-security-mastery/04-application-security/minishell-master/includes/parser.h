/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:13:44 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 20:32:58 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minishell.h"

typedef struct s_data			t_data;
typedef struct s_syntax_error	t_syntax_error;

typedef enum t_token_type
{
	TOKEN_CHAR,
	TOKEN_ASTERISK,
	TOKEN_INT_MARK,
	TOKEN_SPACE,
	TOKEN_BAR,
	TOKEN_AMPERSAND,
	TOKEN_DOLLAR,
	TOKEN_TILDE,
	TOKEN_WORD,
	TOKEN_LESS,
	TOKEN_GREATER,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_SINGLE_QUOTE,
	TOKEN_DOUBLE_QUOTE,
	TOKEN_PIPE,
	TOKEN_LOGICAL_AND,
	TOKEN_LOGICAL_OR,
	TOKEN_EXIT_STATUS,
	TOKEN_EXPAND_VARIABLE,
	TOKEN_TILDE_EXPANSION,
	TOKEN_WILDCARD,
	TOKEN_L_PARENTHESIS,
	TOKEN_R_PARENTHESIS,
	TOKEN_HEREDOC_DELIM,
	TOKEN_HEREDOC_DELIM_QUOTED
}								t_token_type;

typedef struct s_token
{
	t_token_type				type;
	char						*value;
	bool						quoted;
	bool						fully_quoted;
	bool						joined;
	int							quote_type;
	struct s_token				*next;
	struct s_token				*prev;
}								t_token;

typedef struct s_pre_token
{
	t_token_type				type;
	char						character;
	char						*str;
	bool						in_squote;
	bool						in_dquote;
	struct s_pre_token			*next;
	struct s_pre_token			*prev;
}								t_pre_token;

typedef struct s_tokenize_helper
{
	t_pre_token					*head;
	t_pre_token					*current;
	int							i;
	char						*new_input;
	bool						in_squote;
	bool						in_dquote;
	int							consumed;
}								t_tokenize_helper;

typedef struct s_word_flags
{
	bool						has_var;
	bool						has_wild;
	bool						has_tilde;
}								t_word_flags;

// tokenizer
t_pre_token						*add_pre_token_node(t_data *data,
									t_pre_token *prev);
t_pre_token						*tokenize(t_data *data, const char *input);
char							*process_token(char *prompt, t_data *data,
									t_pre_token *node);
void							run_tokenization_pipeline(t_data *data,
									char *input);

// tokenizer_utils
t_token_type					pre_tokenize(char c, char next);
t_token_type					pre_tokenize_b(char c);
t_token_type					pre_tokenize_c(char c);
t_pre_token						*create_pre_token_node(t_data *data);
t_pre_token						*add_pre_token_node(t_data *data,
									t_pre_token *prev);

// tokenizer_utils_2
void							join_token_pair(t_data *data, t_pre_token *a,
									t_token_type new_type);
void							join_double_char_tokens(t_data *data,
									t_pre_token *head);
bool							should_join_pair(char c1, char c2,
									t_token_type *out_type);
bool							is_double_char_token(t_token_type type);
bool							is_metacharacter(t_pre_token *tok);

// tokenizer_utils_3
t_pre_token						*sanitize_pre_tokens(t_data *data,
									t_pre_token *head);
t_pre_token						*trim_token_whitespace(t_pre_token *head);
int								is_quote_open(char c, int *s_quote,
									int *d_quote);
void							compress_spaces(t_pre_token **head);
char							safe_check_next(char *str);

// tokenizer_utils_4
t_tokenize_helper				*init_tokenize_helper(t_data *data);
void							secure_add_pre_token_node(t_data *data,
									t_tokenize_helper *helper);

// early_error_check_1
bool							update_paren_counter(t_pre_token *tok,
									int *count);
bool							parenthesis_is_valid(t_pre_token *head);
bool							quotes_are_closed(t_pre_token *tok,
									t_data *data);
bool							has_unexpected_tokens_helper(t_pre_token *tok,
									t_data *data, t_pre_token *next);
bool							is_bad_sequence(t_pre_token *tok);

// early_error_check_2

bool							has_unexpected_tokens(t_data *data);
bool							is_separator(t_pre_token *tok);
char							*get_error_token_str(t_pre_token *tok);

// early_error_check_3

bool							has_empty_command_between(t_data *data);
bool							handle_separator_state(t_data *data,
									bool *prev_sep, t_pre_token *token);

// early_error_check_4

bool							has_empty_parentheses(t_data *data,
									t_pre_token *head);
bool							has_consecutive_separators(t_data *data);
bool							has_invalid_redirections(t_data *data);

// early_error_check_5
t_syntax_error					*has_early_syntax_error(t_data *data);
bool							set_syntax_error(t_data *data,
									t_pre_token *token, const char *message);
// print_error.c
void							print_syntax_error(t_data *data);

// print_error2.c
char							*get_token_char_for_error_2(t_token_type type);
char							*get_token_char_for_error(t_token_type type);
char							*get_token_type_str_3(t_token_type type);
char							*get_token_type_str_2(t_token_type type);
char							*get_token_type_str(t_token_type type);

// final_tokenizer.c
void							handle_word_token(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
void							handle_expand_var_token(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
void							handle_single_token(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
t_token							*group_pre_tokens(t_pre_token *pre_head,
									t_data *data);

// final_tokenizer_2.c
t_token							*collect_word_token(t_pre_token **cur,
									t_data *data);
t_token							*collect_expand_var_token(t_pre_token **cur,
									t_data *data);
t_token							*create_single_token(t_pre_token *cur,
									t_data *data);
void							append_token(t_token **head, t_token **tail,
									t_token *new_token);
int								is_var_start(char c);

// final_tokenizer_3.c
char							*pre_tokens_to_str(t_data *data,
									t_pre_token *start, t_pre_token *end);
t_token							*collect_single_quoted_token(t_pre_token **cur,
									t_data *data);
t_token							*collect_double_quoted_token(t_pre_token **cur,
									t_data *data);

// final_tokenizer_4.c
void							handle_heredoc_del(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
t_token							*collect_heredoc_delimiter(t_pre_token **cur,
									t_data *data, bool is_quoted);
void							handle_quote_heredoc_del(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
void							set_heredoc_token_quoted_attrs(t_token *token,
									bool is_single_quote);
void							advance_to_closing_quote(t_pre_token **cur,
									t_token_type quote_type);
// final_tokenizer_5.c
void							collect_delimiter_content(t_pre_token **cur,
									bool is_quoted);
void							finalize_heredoc_token(t_token *token,
									t_pre_token **cur, bool is_quoted);
void							process_quoted_delimiter(t_pre_token **cur,
									t_pre_token **start, t_pre_token **end);
void							process_unquoted_delimiter(t_pre_token **cur,
									t_pre_token **start, t_pre_token **end);
t_token							*create_delimiter_token(t_data *data,
									char *word, bool is_quoted,
									t_pre_token **cur);

// final_tokenizer_6.c
void							set_quote_flags_from_word(t_token *token,
									char *word);
bool							delimiter_contains_quotes(t_pre_token *cur);
void							handle_heredoc_del_with_embedded_quotes(
									t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);

// gc_strdup.c
char							*gc_strdup(t_data *data, const char *s);

// final_tokenizer_screwnorm.c
t_pre_token						*skip_spaces(t_pre_token *cur);

// final_tokenizer_wildcard.c
void							handle_wildcard_token(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);
int								is_var_char(char c);
t_token							*new_token(t_token_type type, char *value,
									t_data *data);
void							handle_tilde_token(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);

// final_tokenizer_grouping.c
void							handle_word_sequence(t_token **token_head,
									t_token **token_tail, t_pre_token **cur,
									t_data *data);

// final_tokenizer_grouping_2.c
t_token							*create_word_token_with_flags(t_data *data,
									t_pre_token *start, t_pre_token *end,
									t_word_flags *flags);
void							analyze_word_flags(t_pre_token *start,
									t_pre_token *end, t_word_flags *flags);

// run_pipeline_wrapper.c
void							run_tokenization_pipeline(t_data *data,
									char *input);

#endif