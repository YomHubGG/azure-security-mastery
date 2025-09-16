/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_envp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 13:47:23 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 13:01:30 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_fill_envp(t_zone_g **zone, char **envp, t_data *data)
{
	t_envp	*env_node;
	char	**temp;
	int		i;

	data->envp_h = NULL;
	i = 0;
	while (envp[i])
	{
		temp = ft_g_split(zone, envp[i], '=');
		if (!temp)
			return (0);
		env_node = ft_new_env_node(zone, temp[0], temp[1], envp[i]);
		ft_env_add_back(&data->envp_h, env_node);
		i++;
	}
	return (1);
}

t_envp	*ft_new_env_node(t_zone_g **zone, char *key, char *val, char *fstr)
{
	t_envp	*node;

	node = g_malloc(zone, sizeof(t_envp));
	if (!node)
		return (NULL);
	node->key = key;
	node->value = val;
	node->full_str = fstr;
	node->next = NULL;
	return (node);
}

void	ft_env_add_back(t_envp **lst, t_envp *new)
{
	t_envp	*temp;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	temp = *lst;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = new;
}

static void	ensure_critical_env_vars_helper(int *shlvl_val)
{
	ft_dprintf(2, "minishell: warning: shell level");
	ft_dprintf(2, " (%d) too high, resetting to 1\n", *shlvl_val);
	*shlvl_val = 1;
}

void	ensure_critical_env_vars(t_data *data)
{
	t_ecev	acc;

	if (!get_env_value(data->envp_h, "PWD"))
		if (getcwd(acc.cwd, 1024))
			add_env_var(data, "PWD", acc.cwd);
	if (!get_env_value(data->envp_h, "OLDPWD"))
		if (getcwd(acc.cwd, 1024))
			add_env_var(data, "OLDPWD", acc.cwd);
	acc.shlvl_str = get_env_value(data->envp_h, "SHLVL");
	if (!acc.shlvl_str)
		add_env_var(data, "SHLVL", "1");
	else
	{
		acc.shlvl_val = ft_atoi(acc.shlvl_str) + 1;
		if (acc.shlvl_val < 0)
			acc.shlvl_val = 0;
		else if (acc.shlvl_val > 1000)
		{
			ensure_critical_env_vars_helper(&acc.shlvl_val);
		}
		snprintf(acc.level_str, sizeof(acc.level_str), "%d", acc.shlvl_val);
		update_env_var(data, "SHLVL", acc.level_str);
	}
	if (!get_env_value(data->envp_h, "PATH"))
		add_env_var(data, "PATH", MIN_PATH);
}
