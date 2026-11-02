/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_request.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:35:22 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:35:22 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_waiting_coders(t_simulation *sim)
{
	pthread_mutex_lock(&sim->dispatcher.mutex);
	pthread_cond_broadcast(&sim->dispatcher.cond);
	pthread_mutex_unlock(&sim->dispatcher.mutex);
}

static int	wait_briefly(t_dispatcher *dispatcher)
{
	struct timespec	timeout;

	if (clock_gettime(CLOCK_REALTIME, &timeout) != 0)
		return (0);
	timeout.tv_nsec += 1000000L;
	if (timeout.tv_nsec >= 1000000000L)
	{
		timeout.tv_sec++;
		timeout.tv_nsec -= 1000000000L;
	}
	pthread_cond_timedwait(&dispatcher->cond, &dispatcher->mutex, &timeout);
	return (1);
}

static int	wait_until_granted(t_coder *coder)
{
	t_dispatcher	*dispatcher;

	dispatcher = &coder->sim->dispatcher;
	while (1)
	{
		grant_waiting_requests(coder->sim);
		pthread_mutex_lock(&dispatcher->mutex);
		if (coder->request.granted)
		{
			pthread_mutex_unlock(&dispatcher->mutex);
			return (1);
		}
		if (is_simulation_stopped(coder->sim) || !wait_briefly(dispatcher))
		{
			pthread_mutex_unlock(&dispatcher->mutex);
			return (0);
		}
		pthread_mutex_unlock(&dispatcher->mutex);
	}
}

static int	wait_single_coder(t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*dongle;

	sim = coder->sim;
	dongle = &sim->dongles[0];
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	log_coder_action(coder, "has taken a dongle");
	pthread_mutex_lock(&sim->dispatcher.mutex);
	while (!is_simulation_stopped(sim))
		pthread_cond_wait(&sim->dispatcher.cond, &sim->dispatcher.mutex);
	pthread_mutex_unlock(&sim->dispatcher.mutex);
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	wait_for_dongles(t_coder *coder)
{
	if (coder->sim->config.number_of_coders == 1)
		return (wait_single_coder(coder));
	if (!submit_request(coder))
		return (0);
	if (!wait_until_granted(coder))
		return (0);
	log_coder_action(coder, "has taken a dongle");
	log_coder_action(coder, "has taken a dongle");
	return (1);
}
