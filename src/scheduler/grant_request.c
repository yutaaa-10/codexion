/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grant_request.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-11 05:23:33 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-11 05:23:33 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_dongles_available(t_request *request, long now)
{
	t_dongle	*first;
	t_dongle	*second;
	int			available;

	get_ordered_dongles(request->customer, &first, &second);
	if (first == second)
		return (0);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	available = (!first->in_use && !second->in_use && now >= first->available_at
			&& now >= second->available_at);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (available);
}

int	reserve_request_dongles(t_request *request, long now)
{
	t_dongle	*first;
	t_dongle	*second;
	int			success;

	get_ordered_dongles(request->customer, &first, &second);
	if (first == second)
		return (0);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	success = (!first->in_use && !second->in_use && now >= first->available_at
			&& now >= second->available_at);
	if (success)
	{
		first->in_use = 1;
		second->in_use = 1;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (success);
}

static int	find_candidate_index(t_simulation *sim, long now)
{
	t_dispatcher	*dispatcher;
	t_request		*candidate;
	int				best;
	int				i;

	dispatcher = &sim->dispatcher;
	best = -1;
	i = 0;
	while (i < dispatcher->waiting.size)
	{
		candidate = dispatcher->waiting.items[i];
		if (request_dongles_available(candidate, now) && (best == -1
				|| request_has_priority(candidate,
					dispatcher->waiting.items[best], sim->config.scheduler)))
			best = i;
		i++;
	}
	return (best);
}

static int	grant_one_request(t_simulation *sim)
{
	t_dispatcher	*dispatcher;
	t_request		*request;
	long			now;
	int				index;

	dispatcher = &sim->dispatcher;
	now = get_time_ms();
	if (now < 0)
		return (0);
	index = find_candidate_index(sim, now);
	if (index < 0)
		return (0);
	request = dispatcher->waiting.items[index];
	if (!reserve_request_dongles(request, now))
		return (0);
	heap_remove_at(&dispatcher->waiting, index, sim->config.scheduler);
	request->queued = 0;
	request->granted = 1;
	return (1);
}

int	grant_waiting_requests(t_simulation *sim)
{
	t_dispatcher	*dispatcher;
	int				granted;

	dispatcher = &sim->dispatcher;
	granted = 0;
	pthread_mutex_lock(&dispatcher->mutex);
	while (!is_simulation_stopped(sim) && grant_one_request(sim))
		granted++;
	if (granted > 0)
		pthread_cond_broadcast(&dispatcher->cond);
	pthread_mutex_unlock(&dispatcher->mutex);
	return (granted);
}
