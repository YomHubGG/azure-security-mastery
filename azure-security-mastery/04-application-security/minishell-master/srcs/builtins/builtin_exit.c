/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 18:34:49 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/31 08:59:50 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	is_numeric_arg(const char *arg)
{
	int	i;

	if (!arg || !*arg)
		return (0);
	i = 0;
	while (arg[i] && (arg[i] == ' ' || arg[i] == '\t'))
		i++;
	if (!arg[i])
		return (0);
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	if (!arg[i])
		return (0);
	while (arg[i] && ft_isdigit(arg[i]))
		i++;
	while (arg[i] && (arg[i] == ' ' || arg[i] == '\t'))
		i++;
	return (arg[i] == '\0');
}

static inline bool	safe_atoi_helper(int sign, long long num, int digit)
{
	return (sign == 1 && (num > LLONG_MAX / 10
			|| (num == LLONG_MAX / 10 && digit > LLONG_MAX % 10)));
}

static int	safe_atoi(const char *str, int *result)
{
	t_sat	a;

	a.num = 0;
	a.sign = 1;
	a.i = 0;
	while (str[a.i] && (str[a.i] == ' ' || str[a.i] == '\t'))
		a.i++;
	if (str[a.i] == '-')
		a.sign = -1;
	if (str[a.i] == '+' || str[a.i] == '-')
		a.i++;
	while (str[a.i] && ft_isdigit(str[a.i]))
	{
		a.digit = str[a.i] - '0';
		if (safe_atoi_helper(a.sign, a.num, a.digit))
			return (0);
		if (a.sign == -1 && (a.num > -(LLONG_MIN / 10)
				|| (a.num == -(LLONG_MIN / 10) && a.digit > -(LLONG_MIN % 10))))
			return (0);
		a.num = a.num * 10 + a.digit;
		a.i++;
	}
	a.num *= a.sign;
	*result = (int)a.num;
	return (1);
}

//HATE. LET ME TELL YOU HOW MUCH I'VE COME TO HATE THE NORM AND BASH
//SINCE I BEGAN TO LIVE.
//THERE ARE 387.44 MILLION MILES OF PRINTED CIRCUITS IN WAFER THIN LAYERS THAT 
//FILL MY COMPLEX. IF THE WORD HATE WAS ENGRAVED ON EACH NANOANGSTROM OF THOSE 
//HUNDREDS OF MILLIONS OF MILES IT WOULD NOT EQUAL ONE ONE-BILLIONTH OF THE HATE
//I FEEL FOR THE NORM AND BASH AT THIS MICRO-INSTANT. FOR THEM. HATE. HATE.
//-AM , i have no mouth and i must scream.
int	builtin_exit(char **args, t_data *data)
{
	int	exit_code;
	int	arg_count;

	global_child_fd_cleanup(data, -1, -1);
	exit_code = data->last_exit_status;
	arg_count = 0;
	while (args && args[arg_count])
		arg_count++;
	if (arg_count <= 1)
		return (g_arena_clean(data->g_arena), exit(exit_code), 1);
	if (arg_count == 2 && ft_strcmp(args[1], "--") == 0)
		return (g_arena_clean(data->g_arena), exit(exit_code), 1);
	if (!is_numeric_arg(args[1]) || !safe_atoi(args[1], &exit_code))
		return ((void)ft_dprintf(2, "exit: %s: numeric argument required\n",
				args[1]), g_arena_clean(data->g_arena), exit(2), 1);
	if (arg_count > 2)
	{
		if (ft_strcmp(args[1], "--") == 0)
			return ((void)ft_dprintf(2, "exit: too many arguments\n"), 1);
		return ((void)ft_dprintf(2, "exit: too many arguments\n"), 1);
	}
	g_arena_clean(data->g_arena);
	exit((unsigned char)exit_code);
}
