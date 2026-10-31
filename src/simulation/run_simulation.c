/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_simulation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:00:54 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:00:57 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	join_threads(t_simulation *sim, int count)
{
	int	i;
	int	status;

	i = 0;
	status = 1;
	while (i < count)
	{
		if (pthread_join(sim->coders[i].thread, NULL) != 0)
			status = 0;
		i++;
	}
	return (status);
}

static void	init_start_times(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
}

static int	start_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			stop_simulation(sim);
			join_threads(sim, i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	run_simulation(t_simulation *sim)
{
	int	status;

	sim->start_time = get_time_ms();
	if (sim->start_time == -1)
		return (0);
	init_start_times(sim);
	if (!start_coder_threads(sim))
		return (0);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		stop_simulation(sim);
		join_threads(sim, sim->config.number_of_coders);
		return (0);
	}
	status = join_threads(sim, sim->config.number_of_coders);
	if (pthread_join(sim->monitor_thread, NULL) != 0)
		status = 0;
	return (status);
}
