/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 08:55:45 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 14:06:54 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "minishell.h"

typedef struct snc
{
	char	*ret;
	char	*to_clear;
	bool	d_quoted;
	bool	s_quoted;
	int		i;
	int		i2;
}	t_snc;

typedef struct mini_fd
{
	int	read;
	int	write;
}	t_mini_fd;

typedef struct s_pid_list
{
	pid_t				pid;
	struct s_pid_list	*next;
}	t_pid_list;

typedef struct s_pipe_list
{
	int					fd0;
	int					fd1;
	struct s_pipe_list	*next;
}	t_pipe_list;

typedef struct s_redir_list
{
	int					redir;
	struct s_redir_list	*next;
}	t_redir_list;

//exec_child_process.c
void			create_child(t_data *data, char *exec_path,
					char **args, t_mini_fd fdx);
void			child_process(t_data *data, char **args, char *exec_path,
					t_mini_fd fdx);
void			exec_explore(t_data *data, t_tree_node *ast, int read,
					int write);
void			exec_explore2(t_data *data, t_tree_node *ast, int read,
					int write);
void			exec_explore3(t_data *data, t_tree_node *ast, int read,
					int write);
void			ft_hat(int signum);

//exec_explore.c
void			exec_explore(t_data *data, t_tree_node *ast,
					int read, int write);
void			exec_explore2(t_data *data, t_tree_node *ast,
					int read, int write);
void			exec_explore3(t_data *data, t_tree_node *ast,
					int read, int write);
void			exec_explore4(t_data *data, t_tree_node *ast,
					int read, int write);
void			exec_explore5(t_data *data, t_tree_node *ast,
					int read, int write);

//exec_pipeline.c
void			exec_pipeline(t_data *data, t_tree_node *ast,
					int read, int write);

//exec_helper.c
int				open_file(t_tree_node *access, int oflag);
char			**get_paths(t_data *data);
char			*discover_exec_path(t_data *data, char *to_find, char **paths);
char			**create_args(t_data *data, t_tree_node *current);
int				count_node(t_tree_node *current);

//exec_init.c
void			init_exec(t_data *data, t_tree_node *current,
					t_mini_fd fdx);
void			exec_error(t_data *data, int error_code, char *str);
void			init_helper(t_data *data, t_tree_node *current, t_mini_fd fdx,
					char	*exec_path);
void			clean_exec_error_exit(t_mini_fd fdx, int code, t_data *data,
					char *errinfo);

//exec_handle_heredoc.c
int				handle_heredoc(char *h_delim, t_data *data, bool shouldexpand);
void			child_heredoc(t_data *data, const char *h_delim, int *fd,
					bool shouldexpand);
void			clean_exit(int *fd, int exit_sig);

//exec_clear_quote.c
void			exec_clear_quote(t_data *data, t_tree_node *ast);
void			single_node_clear(t_data *data, t_tree_node *ast);
t_snc			init_snc(void);

//exec_child_tracer.c
t_pid_list		*new_pid_node(t_data *data, pid_t new_pid);
t_pid_list		*last_pid_node(t_data *data);
void			pid_add_back(t_data *data, t_pid_list *new);
void			add_trace(t_data *data, pid_t new_pid);

//exec_wait_child.c
void			exec_entry(t_data *data, t_tree_node *ast);
int				process_count(t_data *data);
void			wait_for_all_children(t_data *data);
void			global_child_fd_cleanup(t_data *data, int read, int write);

//exec_pipe_cleaner.c
t_pipe_list		*new_pipe_store_node(t_data *data, int fd0, int fd1);
t_pipe_list		*last_pipe_store_node(t_data *data);
void			pipe_store_add_back(t_data *data, t_pipe_list *new);
void			close_all_pipe_fds_except(t_data *data,
					int keep_read, int keep_write);
void			fd_nuke(t_data *data);

//exec_redir_cleaner.c
void			close_all_redir_fds_except(t_data *data,
					int keep_read, int keep_write);
void			redir_store_add_back(t_data *data, t_redir_list *new);
t_redir_list	*last_redir_store_node(t_data *data);
t_redir_list	*new_redir_store_node(t_data *data, int redir);

//exec_helper2.c
void			set_last_exit_status(t_data *data, int to_set);
void			safe_close(int to_close1, int to_close2);

//exec_redir_subwraper.c
void			exec_redir_out_subwraper(t_data *data, t_tree_node *ast,
					int read, int write);
void			exec_redir_in_subwraper(t_data *data, t_tree_node *ast,
					int read, int write);

//exec_heredoc_expand.c
int				hd_init_expand_word(char **to_ex, t_data *data);
char			*hd_quote_clear(t_data *data, char *to_clear);
int				hd_expand_validate(t_init_expand_word	access, t_data *data);

#endif