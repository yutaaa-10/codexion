/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 17:23:51 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:06:54 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	get_ordered_dongles(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	*first = &coder->sim->dongles[coder->left_dongle];
	*second = &coder->sim->dongles[coder->right_dongle];
	if (coder->left_dongle > coder->right_dongle)
	{
		*first = &coder->sim->dongles[coder->right_dongle];
		*second = &coder->sim->dongles[coder->left_dongle];
	}
}

void	release_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long		now;

	get_ordered_dongles(coder, &first, &second);
	if (first == second)
		return ;
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	now = get_time_ms();
	if (now >= 0)
	{
		first->available_at = now + coder->sim->config.dongle_cooldown;
		second->available_at = now + coder->sim->config.dongle_cooldown;
	}
	first->in_use = 0;
	second->in_use = 0;
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

static int	reserve_dongles(t_dongle *first, t_dongle *second)
{
	long	now;

	now = get_time_ms();
	if (now < 0 || first->in_use || second->in_use
		|| now < first->available_at || now < second->available_at)
		return (0);
	first->in_use = 1;
	second->in_use = 1;
	return (1);
}

int	try_acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	int			success;

	get_ordered_dongles(coder, &first, &second);
	if (first == second)
		return (0);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	success = reserve_dongles(first, second);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (success);
}
