/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_error_parsing.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 13:06:45 by tritter           #+#    #+#             */
/*   Updated: 2025/06/25 19:31:51 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	expand_err_msg(int err_msg)
{
	if (err_msg == EXPAND_MALLOC_ERR)
		printf(RED"/!\\ FATAL: MALLOC FAILED IN EXPAND VAR,\
			QUITTING SHELL/!\\");
	if (err_msg == EXPAND_UNVALID_AST)
		printf(YELLOW"WARNING: UNVALID AST NODE DETECTED, \
			BEHAVIOR MIGHT BE UNDEFINED");
	if (err_msg == EXPAND_UNKNOWN_AST_NODE_TYPE)
		printf(YELLOW "WARNING UNKNOWN AST NODE TYPE DETECTED \
			BEHAVIOR MIGHT BE UNDEFINED");
}
