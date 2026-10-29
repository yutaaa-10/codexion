/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 17:23:51 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/29 17:36:54 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongles(t_coder *coder)
{
	t_simulation	*sim;
	t_dongle		*first;
	t_dongle		*second;
	long			now;

	sim = coder->sim;
	first = &sim->dongles[coder->left_dongle];
	second = &sim->dongles[coder->right_dongle];
	if (coder->left_dongle > coder->right_dongle)
	{
		first = &sim->dongles[coder->right_dongle];
		second = &sim->dongles[coder->left_dongle];
	}
	if (first == second)
		return ;
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	now = get_time_ms();
	if (now >= 0)
	{
		first->available_at = now + sim->config.dongle_cooldown;
		second->available_at = now + sim->config.dongle_cooldown;
	}
	first->in_use = 0;
	second->in_use = 0;
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

int	try_acquire_dongles(t_coder *coder)

{
	t_simulation *sim;
	t_dongle *first;
	t_dongle *second;
	long now;
	int success;
	sim = coder->sim;
	first = &sim->dongles[coder->left_dongle];
	second = &sim->dongles[coder->right_dongle];
	if (coder->left_dongle > coder->right_dongle)
	{
		first = &sim->dongles[coder->right_dongle];
		second = &sim->dongles[coder->left_dongle];
	}
	if (first == second)
		return (0);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	success = 0;
	now = get_time_ms();
	if (now >= 0 && !first->in_use && !second->in_use
		&& now >= first->available_at && now >= second->available_at)
	{
		first->in_use = 1;
		second->in_use = 1;
		success = 1;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (success);
}
