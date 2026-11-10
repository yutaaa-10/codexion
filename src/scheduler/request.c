/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 05:24:21 by yukurosa          #+#    #+#             */
/*   Updated: 2026/11/09 20:06:41 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_request_deadline(t_coder *coder, long *deadline)
{
	t_simulation	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->stopped)
	{
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	*deadline = coder->last_compile_start + sim->config.time_to_burnout;
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}

static int	push_request(t_coder *coder, long deadline)
{
	t_dispatcher	*dispatcher;
	t_request		*request;

	dispatcher = &coder->sim->dispatcher;
	request = &coder->request;
	request->arrival_order = dispatcher->next_order;
	request->deadline = deadline;
	request->granted = 0;
	if (!heap_push(&dispatcher->waiting, request, coder->sim->config.scheduler))
		return (0);
	request->queued = 1;
	dispatcher->next_order++;
	return (1);
}

int	submit_request(t_coder *coder)
{
	t_dispatcher	*dispatcher;
	long			deadline;
	int				success;

	if (!get_request_deadline(coder, &deadline))
		return (0);
	dispatcher = &coder->sim->dispatcher;
	pthread_mutex_lock(&dispatcher->mutex);
	success = 0;
	if (!coder->request.queued)
		success = push_request(coder, deadline);
	pthread_mutex_unlock(&dispatcher->mutex);
	return (success);
}
