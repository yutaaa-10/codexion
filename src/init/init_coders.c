/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 04:17:32 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:03:37 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coders(t_simulation *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (0);
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left_dongle = (i + n - 1) % n;
		sim->coders[i].right_dongle = i;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		i++;
	}
	return (1);
}
