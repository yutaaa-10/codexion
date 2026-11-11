/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_barrier.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-11 05:24:49 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-11 05:24:49 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_initial_times(t_simulation *sim, long start_time)
{
	int	i;

	pthread_mutex_lock(&sim->state_mutex);
	sim->start_time = start_time;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = start_time;
		i++;
	}
	pthread_mutex_unlock(&sim->state_mutex);
}

int	wait_for_simulation_start(t_simulation *sim)
{
	t_dispatcher	*dispatcher;

	dispatcher = &sim->dispatcher;
	pthread_mutex_lock(&dispatcher->mutex);
	while (!sim->started)
	{
		if (is_simulation_stopped(sim))
		{
			pthread_mutex_unlock(&dispatcher->mutex);
			return (0);
		}
		pthread_cond_wait(&dispatcher->cond, &dispatcher->mutex);
	}
	pthread_mutex_unlock(&dispatcher->mutex);
	return (1);
}

int	open_start_barrier(t_simulation *sim)
{
	long	start_time;

	start_time = get_time_ms();
	if (start_time < 0)
		return (0);
	set_initial_times(sim, start_time);
	pthread_mutex_lock(&sim->dispatcher.mutex);
	sim->started = 1;
	pthread_cond_broadcast(&sim->dispatcher.cond);
	pthread_mutex_unlock(&sim->dispatcher.mutex);
	return (1);
}
