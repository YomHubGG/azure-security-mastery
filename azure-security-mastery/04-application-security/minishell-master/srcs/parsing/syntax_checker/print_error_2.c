/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_error_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:56:33 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 20:31:01 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_token_char_for_error_2(t_token_type type)
{
	if (type == TOKEN_L_PARENTHESIS)
		return ("(");
	if (type == TOKEN_R_PARENTHESIS)
		return (")");
	if (type == TOKEN_SINGLE_QUOTE)
		return ("'");
	if (type == TOKEN_DOUBLE_QUOTE)
		return ("\"");
	if (type == TOKEN_DOLLAR)
		return ("$");
	if (type == TOKEN_TILDE)
		return ("~");
	if (type == TOKEN_ASTERISK)
		return ("*");
	return ("newline");
}

char	*get_token_char_for_error(t_token_type type)
{
	if (type == TOKEN_PIPE || type == TOKEN_BAR)
		return ("|");
	if (type == TOKEN_LOGICAL_AND)
		return ("&&");
	if (type == TOKEN_LOGICAL_OR)
		return ("||");
	if (type == TOKEN_LESS)
		return ("<");
	if (type == TOKEN_GREATER)
		return (">");
	if (type == TOKEN_HEREDOC)
		return ("<<");
	if (type == TOKEN_APPEND)
		return (">>");
	if (type == TOKEN_AMPERSAND)
		return ("&");
	return (get_token_char_for_error_2(type));
}

char	*get_token_type_str_3(t_token_type type)
{
	if (type == TOKEN_L_PARENTHESIS)
		return ("TOKEN_L_PARENTHESIS");
	if (type == TOKEN_R_PARENTHESIS)
		return ("TOKEN_R_PARENTHESIS");
	if (type == TOKEN_HEREDOC_DELIM)
		return ("TOKEN_HEREDOC_DELIM");
	if (type == TOKEN_HEREDOC_DELIM_QUOTED)
		return ("TOKEN_HEREDOC_DELIM_QUOTED");
	return ("TOKEN_UNDEFINED");
}

char	*get_token_type_str_2(t_token_type type)
{
	if (type == TOKEN_HEREDOC)
		return ("TOKEN_HEREDOC");
	if (type == TOKEN_APPEND)
		return ("TOKEN_APPEND");
	if (type == TOKEN_SINGLE_QUOTE)
		return ("TOKEN_SINGLE_QUOTE");
	if (type == TOKEN_DOUBLE_QUOTE)
		return ("TOKEN_DOUBLE_QUOTE");
	if (type == TOKEN_PIPE)
		return ("TOKEN_PIPE");
	if (type == TOKEN_LOGICAL_AND)
		return ("TOKEN_LOGICAL_AND");
	if (type == TOKEN_LOGICAL_OR)
		return ("TOKEN_LOGICAL_OR");
	if (type == TOKEN_EXIT_STATUS)
		return ("TOKEN_EXIT_STATUS");
	if (type == TOKEN_EXPAND_VARIABLE)
		return ("TOKEN_EXPAND_VARIABLE");
	if (type == TOKEN_TILDE_EXPANSION)
		return ("TOKEN_TILDE_EXPANSION");
	if (type == TOKEN_WILDCARD)
		return ("TOKEN_WILDCARD");
	return (get_token_type_str_3(type));
}

char	*get_token_type_str(t_token_type type)
{
	if (type == TOKEN_CHAR)
		return ("TOKEN_CHAR");
	if (type == TOKEN_ASTERISK)
		return ("TOKEN_ASTERISK");
	if (type == TOKEN_INT_MARK)
		return ("TOKEN_INT_MARK");
	if (type == TOKEN_SPACE)
		return ("TOKEN_SPACE");
	if (type == TOKEN_BAR)
		return ("TOKEN_BAR");
	if (type == TOKEN_AMPERSAND)
		return ("TOKEN_AMPERSAND");
	if (type == TOKEN_DOLLAR)
		return ("TOKEN_DOLLAR");
	if (type == TOKEN_TILDE)
		return ("TOKEN_TILDE");
	if (type == TOKEN_WORD)
		return ("TOKEN_WORD");
	if (type == TOKEN_LESS)
		return ("TOKEN_LESS");
	if (type == TOKEN_GREATER)
		return ("TOKEN_GREATER");
	return (get_token_type_str_2(type));
}
