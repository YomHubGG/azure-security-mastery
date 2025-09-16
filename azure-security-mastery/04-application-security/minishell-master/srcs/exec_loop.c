/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:23:04 by tritter           #+#    #+#             */
/*   Updated: 2025/08/04 09:48:48 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	cleanup_iteration(t_data *data)
{
	if (data->hot_zone && *data->hot_zone)
	{
		g_zone_clean(data->hot_zone);
		*data->hot_zone = ft_new_zone(NULL);
	}
	data->pre_token_head = NULL;
	data->token_head = NULL;
	data->tree_root = NULL;
	data->pid_list = NULL;
	data->pipe_list = NULL;
	data->redir_list = NULL;
	data->can_exec = true;
	data->is_piped = false;
	if (data->set_syntax_error)
		data->set_syntax_error->has_error = false;
}

static int	handle_syntax_error(t_data *data)
{
	if (!data->set_syntax_error || !data->set_syntax_error->has_error)
		return (0);
	fprintf(stderr, "bash: syntax error near unexpected token `%s'\n",
		data->set_syntax_error->message);
	data->last_exit_status = 2;
	cleanup_iteration(data);
	return (1);
}

static int	process_command_safely(t_data *data, char *input)
{
	run_tokenization_pipeline(data, input);
	if (handle_syntax_error(data))
		return (0);
	if (!data->token_head)
	{
		cleanup_iteration(data);
		return (0);
	}
	data->tree_root = create_ast(&data->token_head, data);
	if (!data->tree_root)
	{
		cleanup_iteration(data);
		return (0);
	}
	expand_explore_tree(data->tree_root, data);
	globbing_explore_tree(data->tree_root, data);
	exec_entry(data, data->tree_root);
	g_zone_clean(data->hot_zone);
	cleanup_iteration(data);
	return (0);
}

int	exec_loop(t_data *data)
{
	char	*raw_line;
	int		lexsts;

	while (true)
	{
		if (g_signal != 0)
			data->last_exit_status = g_signal + 128;
		raw_line = readline("🦥Minishell🦥> ");
		if (!raw_line)
		{
			if (isatty(STDIN_FILENO))
				write(2, "exit\n", 6);
			lexsts = data->last_exit_status;
			g_arena_clean(data->g_arena);
			exit(lexsts);
		}
		insert_malloc(data->hot_zone, (void *)raw_line);
		add_history(raw_line);
		if (!raw_line || !*raw_line)
			continue ;
		process_command_safely(data, raw_line);
		g_signal = 0;
	}
	return (0);
}
