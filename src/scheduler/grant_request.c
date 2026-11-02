/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grant_request.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:32:14 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:32:14 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		if (!request_has_higher_conflict(sim, candidate)
			&& request_dongles_available(candidate, now) && (best == -1
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
