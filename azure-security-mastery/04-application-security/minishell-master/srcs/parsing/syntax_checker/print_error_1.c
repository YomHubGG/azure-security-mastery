/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_error_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:56:33 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 20:29:38 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_syntax_error(t_data *data)
{
	if (data->set_syntax_error && data->set_syntax_error->has_error)
	{
		printf("Syntax error: %s\n", data->set_syntax_error->message);
	}
}
