/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_simulation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:01:03 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:08:03 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	if (sim->dongles)
	{
		while (i < sim->config.number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			i++;
		}
		free(sim->dongles);
	}
	free(sim->coders);
}
