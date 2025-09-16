/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_envar.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:45:21 by ggrisole          #+#    #+#             */
/*   Updated: 2025/08/05 16:34:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_ENVAR_H
# define BUILTINS_ENVAR_H

typedef struct s_envp	t_envp;
typedef struct s_zone_g	t_zone_g;
typedef struct s_data	t_data;

# define MIN_PATH "/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"

typedef enum e_builtin_type
{
	BUILTIN_NONE,
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_UNSET,
	BUILTIN_ENV,
	BUILTIN_EXIT
}						t_builtin_type;

typedef struct s_ecev
{
	char	cwd[1024];
	char	*shlvl_str;
	int		shlvl_val;
	char	level_str[12];
}	t_ecev;

// init_envp.c
int						ft_fill_envp(t_zone_g **zone, char **envp,
							t_data *data);
t_envp					*ft_new_env_node(t_zone_g **zone, char *key, char *val,
							char *fstr);
void					ft_env_add_back(t_envp **lst, t_envp *new);
void					ensure_critical_env_vars(t_data *data);

// env_utils.c
t_envp					*find_env_var(t_envp *head, const char *key);
char					*get_env_value(t_envp *head, const char *key);
int						is_valid_env_name(const char *name);
char					*ft_strjoin_injector(const char *s1, const char *s2,
							t_zone_g **zone);
char					*create_env_string(t_zone_g **zone, char *key,
							char *value);

// env_manipulation.c
int						update_existing_env(t_envp *node, const char *key,
							const char *value, t_data *data);
int						create_new_env_node(t_data *data, const char *key,
							const char *value);
int						update_env_var(t_data *data, const char *key,
							const char *value);
int						add_env_var(t_data *data, const char *key,
							const char *value);
int						remove_env_var(t_data *data, const char *key);

// env_display.c
void					print_env_unsorted(t_envp *head, int write_to);
void					print_env_sorted(t_envp *head, int write_to);

// env_conversion.c
char					**envp_list_to_array(t_data *data);
void					pes_helper(int write_to, char **env_array,
							int i, char *equals_pos);

// builtin dispatcher.c
t_builtin_type			get_builtin_type(const char *cmd);
int						is_builtin(const char *cmd);
int						execute_builtin(const char *cmd, char **args,
							t_data *data, int write_to);

// builtin_pwd.c
int						builtin_pwd(t_data *data, int write_to);

// builtin_env.c
int						builtin_env(t_data *data, char **argv, int write_to);

// builtin_exit.c
int						builtin_exit(char **args, t_data *data);

// builtin_unset.c
int						builtin_unset(char **args, t_data *data);

// builtin_export.c
int						builtin_export(char **args, t_data *data, int write_to);

// builtin_echo.c
int						builtin_echo(char **args, int write_to);

// builtin_cd.c
int						builtin_cd(char **args, t_data *data);
int						args_count(char **to_count);

#endif