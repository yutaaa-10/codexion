/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 04:17:32 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:03:26 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_simulation *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (!sim->dongles)
		return (0);
	i = 0;
	while (i < n)
	{
		sim->dongles[i].in_use = 0;
		sim->dongles[i].available_at = 0;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&sim->dongles[i].mutex);
			free(sim->dongles);
			sim->dongles = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}
