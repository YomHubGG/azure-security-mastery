/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:00:00 by ggrisole          #+#    #+#             */
/*   Updated: 2025/08/05 11:58:38 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	hardcode_pwd_edge_cases(t_data *data)
{
	char	*pwd;
	char	*oldpwd;
	char	current_dir[1024];

	pwd = get_env_value(data->envp_h, "PWD");
	if (!pwd || !getcwd(current_dir, 1024))
	{
		if (getcwd(current_dir, 1024))
			update_env_var(data, "PWD", current_dir);
		else
		{
			update_env_var(data, "PWD", "/tmp");
		}
	}
	oldpwd = get_env_value(data->envp_h, "OLDPWD");
	if (!oldpwd)
	{
		update_env_var(data, "OLDPWD", current_dir);
	}
}

static int	update_pwd_vars(t_data *data, char *old_pwd)
{
	char	cwd[1024];

	hardcode_pwd_edge_cases(data);
	if (!getcwd(cwd, 1024))
	{
		ft_dprintf(2, "cd: getcwd: No such file or directory\n");
		return (1);
	}
	if (old_pwd)
		update_env_var(data, "OLDPWD", old_pwd);
	update_env_var(data, "PWD", cwd);
	return (0);
}

static char	*resolve_target_directory(char **args, t_data *data)
{
	char	*target;

	if (args_count(args) == 1)
	{
		target = get_env_value(data->envp_h, "HOME");
		if (!target || !*target)
			return ((void)ft_dprintf(2, "cd: HOME not set\n"), NULL);
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		target = get_env_value(data->envp_h, "OLDPWD");
		if (!target)
			return ((void)ft_dprintf(2, "cd: OLDPWD not set\n"), NULL);
		ft_printf("%s\n", target);
	}
	else if (ft_strcmp(args[1], "") == 0)
		return (".");
	else
		target = args[1];
	return (target);
}

int	builtin_cd(char **args, t_data *data)
{
	char	*target;
	char	old_pwd[1024];

	if (args_count(args) > 2)
	{
		ft_dprintf(2, "cd: too many arguments\n");
		return (1);
	}
	if (!getcwd(old_pwd, 1024))
	{
		ft_dprintf(2, "cd: getcwd: No such file or directory\n");
		return (1);
	}
	target = resolve_target_directory(args, data);
	if (!target)
		return (1);
	if (chdir(target) == -1)
	{
		ft_dprintf(2, "cd: %s: %s\n", target, strerror(errno));
		return (1);
	}
	return (update_pwd_vars(data, old_pwd));
}

int	args_count(char **to_count)
{
	int	ret;

	ret = 0;
	if (!to_count)
		return (0);
	while (to_count[ret])
		ret++;
	return (ret);
}
