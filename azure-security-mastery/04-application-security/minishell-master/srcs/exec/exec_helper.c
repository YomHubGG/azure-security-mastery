/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:27:37 by tritter           #+#    #+#             */
/*   Updated: 2025/07/06 18:53:26 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//return -1 if the file couldn't be opened
int	open_file(t_tree_node *access, int oflag)
{
	int	ret;

	if (oflag == O_WRONLY)
		ret = open(access->token.word->word_subtype->value,
				oflag | O_CREAT | O_TRUNC, 0644);
	else if (oflag == (O_APPEND | O_WRONLY))
		ret = open(access->token.word->word_subtype->value,
				oflag | O_CREAT, 0644);
	else
		ret = open(access->token.word->word_subtype->value, oflag);
	if (ret == -1)
	{
		ft_dprintf(2, "%s: No such file or directory\n",
			access->token.word->word_subtype->value);
	}
	return (ret);
}

char	**get_paths(t_data *data)
{
	t_envp	*current;
	char	**ret;

	ret = NULL;
	current = data->envp_h;
	while (current)
	{
		if (ft_strcmp(current->key, "PATH") == 0)
		{
			ret = ft_split_injector(current->value, ':', data->hot_zone);
			if (!ret)
				return (NULL);
			return (ret);
		}
		current = current->next;
	}
	return (NULL);
}

//return NULL in case of error or if the command does not exist
char	*discover_exec_path(t_data *data, char *to_find, char **paths)
{
	char	*temp;
	int		i;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin_injector(paths[i], "/", data->hot_zone);
		temp = ft_strjoin_injector(temp, to_find, data->hot_zone);
		if (access(temp, X_OK) == 0)
		{
			return (temp);
		}
		i++;
	}
	return (NULL);
}

char	**create_args(t_data *data, t_tree_node *current)
{
	char	**ret;
	int		i;

	i = 0;
	if (!current)
		return (NULL);
	ret = g_malloc(data->hot_zone, (count_node(current) + 1) * sizeof(char *));
	if (!ret)
		return (NULL);
	while (current)
	{
		ret[i] = ft_strdup(current->token.word->word_subtype->value);
		insert_malloc(data->hot_zone, ret[i]);
		current = current->token.word->next_word;
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

//returns the ammount of node , counting the head
int	count_node(t_tree_node *current)
{
	int	ret;

	ret = 0;
	while (current)
	{
		ret++;
		current = current->token.word->next_word;
	}
	return (ret);
}
