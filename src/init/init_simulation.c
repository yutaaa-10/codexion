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
