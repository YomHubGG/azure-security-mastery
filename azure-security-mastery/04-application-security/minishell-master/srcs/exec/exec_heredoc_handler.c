/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc_handler.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 16:04:11 by tritter           #+#    #+#             */
/*   Updated: 2025/08/04 11:00:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "global.h"

int	handle_heredoc(char *h_delim, t_data *data, bool shouldexpand)
{
	int		fd[2];
	int		status;
	int		pid;

	if (!shouldexpand)
		h_delim = hd_quote_clear(data, h_delim);
	if (pipe(fd) == -1)
		return (perror("pipe failed"), -1);
	signal(SIGINT, heredoc_sigint_handler);
	pid = fork();
	if (pid == 0)
		child_heredoc(data, h_delim, fd, shouldexpand);
	close(fd[1]);
	waitpid(pid, &status, 0);
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	if (WIFEXITED(status) == false)
		return (-2);
	if (WEXITSTATUS(status) != 0)
		return (-2);
	return (fd[0]);
}

static void	ft_hd_handler(int signum)
{
	g_signal = signum;
	close(STDIN_FILENO);
	g_signal = 2;
}

void	child_heredoc(t_data *data, const char *h_delim, int *fd,
	bool shouldexpand)
{
	char	*line;

	(void)data;
	signal(SIGINT, ft_hd_handler);
	while (true)
	{
		line = readline(BRIGHT_CYAN"HEREDOC> "RESET);
		if (!line || ft_strcmp(line, h_delim) == 0)
		{
			free(line);
			break ;
		}
		if (shouldexpand)
			hd_init_expand_word(&line, data);
		write(fd[1], line, strlen(line));
		write(fd[1], "\n", 1);
		if (!shouldexpand)
			free(line);
	}
	fd_nuke(data);
	g_arena_clean(data->g_arena);
	clean_exit(fd, g_signal);
}

void	clean_exit(int *fd, int ex)
{
	if (close(fd[0]))
		perror("Close failed");
	if (close(fd[1]))
		perror("Close failed");
	exit(ex);
}
