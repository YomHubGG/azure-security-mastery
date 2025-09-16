/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:13:44 by tritter           #+#    #+#             */
/*   Updated: 2025/08/06 13:44:27 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <errno.h>    // errno
# include <limits.h>   // LLONG_MAX, LLONG_MIN
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h> //printf(), perror()
# include <stdlib.h>
# include <string.h>   // strerror()
# include <sys/stat.h> //stat()
# include <sys/wait.h> // WIFEXITED ( used to get child exit status in exec)
# include <unistd.h>   // execve
# include "parser.h"
# include "ast.h"
# include "color.h" //color font for printf
# include "builtins_envar.h"
# include "expand.h"
# include "globbing.h"
# include <dirent.h>  //To explore directories
# include "exec.h"
# include <limits.h>
# include "subshell.h"
# include "global.h"

# define MAX_LINE_LEN 1024

typedef struct sat
{
	long long	num;
	int			sign;
	int			i;
	int			digit;
}	t_sat;

typedef struct s_envp
{
	char				*key;
	char				*value;
	char				*full_str;
	struct s_envp		*next;
}						t_envp;

typedef struct s_syntax_error
{
	bool				has_error;
	const char			*message;
	t_pre_token			*token;
}						t_syntax_error;

typedef struct s_data
{
	t_pre_token			*pre_token_head;
	size_t				last_exit_status;
	t_envp				*envp_h;
	t_token_type		*type;
	t_token				*token_head;
	t_tree_node			*tree_root;
	// trash zone
	t_arena_g			*g_arena;
	t_zone_g			**hot_zone;
	t_zone_g			**cold_zone;
	t_syntax_error		*set_syntax_error;
	// for exec
	t_pid_list			*pid_list;
	t_pipe_list			*pipe_list;
	t_redir_list		*redir_list;
	//logical_bool
	bool				can_exec;
	bool				is_piped;
}						t_data;

// main.c
int						init_trash(t_data *data);
t_data					*init_data(void);
int						init_syntax_error(t_data *data);

// init_envp.c
int						ft_fill_envp(t_zone_g **zone, char **envp,
							t_data *data);
t_envp					*ft_new_env_node(t_zone_g **zone, char *key, char *val,
							char *fstr);
void					ft_env_add_back(t_envp **lst, t_envp *new);

// debug
void					print_pre_token_list(t_pre_token *head);
void					print_syntax_error(t_data *data);
char					*get_token_type_str(t_token_type type);
char					*get_token_char_for_error(t_token_type type);
void					print_token_list(t_token *head);
void					print_globing(t_glob_node	*g_head);

//exec_loop.c
int						exec_loop(t_data *data);

//signal_handler.c
void					ft_signal_handler(void);
void					sigint_handler(int signum);
void					heredoc_sigint_handler(int signum);
void					sigint_handler_2(int signum);

#endif