/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-11 05:24:22 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-11 05:24:22 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	get_ordered_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
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
	grant_waiting_requests(coder->sim);
}
