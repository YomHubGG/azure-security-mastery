/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_helper2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 13:07:36 by tritter           #+#    #+#             */
/*   Updated: 2025/07/28 15:52:25 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_last_exit_status(t_data *data, int to_set)
{
	data->last_exit_status = to_set;
}

void	safe_close(int write, int read)
{
	if (write != STDOUT_FILENO)
		close(write);
	if (read != STDIN_FILENO)
		close(read);
}
