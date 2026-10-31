/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 10:39:39 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:04:20 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_atol(const char *str, long *result)
{
	long	number;
	int		i;
	int		digit;

	if (!str || !result || !str[0])
		return (0);
	number = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		digit = str[i] - '0';
		if (number > (LONG_MAX - digit) / 10)
			return (0);
		number = number * 10 + digit;
		i++;
	}
	*result = number;
	return (1);
}

static int	parse_values(char **argv, long *values)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		if (!ft_atol(argv[i + 1], &values[i]))
			return (0);
		i++;
	}
	if (values[0] == 0 || values[0] > INT_MAX)
		return (0);
	if (values[5] == 0 || values[5] > INT_MAX)
		return (0);
	return (1);
}

static void	set_config_values(t_config *config, long *values)
{
	config->number_of_coders = (int)values[0];
	config->time_to_burnout = values[1];
	config->time_to_compile = values[2];
	config->time_to_debug = values[3];
	config->time_to_refactor = values[4];
	config->number_of_compiles_required = (int)values[5];
	config->dongle_cooldown = values[6];
}

int	parser_args(t_config *config, char **argv)
{
	long	values[7];

	if (!parse_values(argv, values))
		return (0);
	set_config_values(config, values);
	if (strcmp(argv[8], "fifo") == 0)
		config->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		config->scheduler = EDF;
	else
		return (0);
	return (1);
}
