/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:19:29 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:19:29 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_simulation *sim)
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
		sim->coders[i].request.customer = &sim->coders[i];
		sim->coders[i].request.arrival_order = 0;
		sim->coders[i].request.deadline = 0;
		sim->coders[i].request.granted = 0;
		sim->coders[i].request.queued = 0;
		i++;
	}
	return (1);
}

static int	init_dongles(t_simulation *sim)
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

static int	init_dispatcher(t_simulation *sim)
{
	t_dispatcher	*dispatcher;

	dispatcher = &sim->dispatcher;
	dispatcher->initialized = 0;
	dispatcher->next_order = 0;
	if (!heap_init(&dispatcher->waiting, sim->config.number_of_coders))
		return (0);
	if (pthread_mutex_init(&dispatcher->mutex, NULL) != 0)
	{
		heap_destroy(&dispatcher->waiting);
		return (0);
	}
	if (pthread_cond_init(&dispatcher->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dispatcher->mutex);
		heap_destroy(&dispatcher->waiting);
		return (0);
	}
	dispatcher->initialized = 1;
	return (1);
}

int	init_simulation(t_simulation *sim)
{
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->start_time = 0;
	sim->started = 0;
	sim->stopped = 0;
	sim->dispatcher.initialized = 0;
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (0);
	if (!init_coders(sim) || !init_dongles(sim) || !init_dispatcher(sim))
	{
		destroy_simulation(sim);
		return (0);
	}
	return (1);
}
