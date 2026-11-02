/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dispatcher.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:12:54 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:12:54 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dispatcher(t_simulation *sim)
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

void	destroy_dispatcher(t_dispatcher *dispatcher)
{
	if (!dispatcher || !dispatcher->initialized)
		return ;
	pthread_cond_destroy(&dispatcher->cond);
	pthread_mutex_destroy(&dispatcher->mutex);
	heap_destroy(&dispatcher->waiting);
	dispatcher->initialized = 0;
}
