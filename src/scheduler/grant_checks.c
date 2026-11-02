/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grant_checks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:31:15 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:31:15 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	get_request_dongles(t_request *request, t_dongle **first,
		t_dongle **second)
{
	t_coder	*coder;

	coder = request->customer;
	*first = &coder->sim->dongles[coder->left_dongle];
	*second = &coder->sim->dongles[coder->right_dongle];
	if (coder->left_dongle > coder->right_dongle)
	{
		*first = &coder->sim->dongles[coder->right_dongle];
		*second = &coder->sim->dongles[coder->left_dongle];
	}
}

static int	requests_share_dongle(t_request *first, t_request *second)
{
	t_coder	*a;
	t_coder	*b;

	a = first->customer;
	b = second->customer;
	return (a->left_dongle == b->left_dongle
		|| a->left_dongle == b->right_dongle
		|| a->right_dongle == b->left_dongle
		|| a->right_dongle == b->right_dongle);
}

int	request_has_higher_conflict(t_simulation *sim, t_request *candidate)
{
	t_dispatcher	*dispatcher;
	t_request		*other;
	int				i;

	dispatcher = &sim->dispatcher;
	i = 0;
	while (i < dispatcher->waiting.size)
	{
		other = dispatcher->waiting.items[i];
		if (other != candidate && requests_share_dongle(other, candidate)
			&& request_has_priority(other, candidate, sim->config.scheduler))
			return (1);
		i++;
	}
	return (0);
}

int	request_dongles_available(t_request *request, long now)
{
	t_dongle	*first;
	t_dongle	*second;
	int			available;

	get_request_dongles(request, &first, &second);
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

	get_request_dongles(request, &first, &second);
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
