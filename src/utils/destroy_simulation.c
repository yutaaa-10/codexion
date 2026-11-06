/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_simulation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:01:03 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/31 12:18:26 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_dispatcher(t_dispatcher *dispatcher)
{
	if (!dispatcher || !dispatcher->initialized)
		return ;
	pthread_cond_destroy(&dispatcher->cond);
	pthread_mutex_destroy(&dispatcher->mutex);
	heap_destroy(&dispatcher->waiting);
	dispatcher->initialized = 0;
}

static void	destroy_dongles(t_simulation *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

void	destroy_simulation(t_simulation *sim)
{
	destroy_dispatcher(&sim->dispatcher);
	destroy_dongles(sim);
	pthread_mutex_destroy(&sim->state_mutex);
	free(sim->coders);
	sim->coders = NULL;
}
