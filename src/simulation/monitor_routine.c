/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_routine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:00:44 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:07:19 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_simulation *sim, t_coder *coder, long now)
{
	if (now - coder->last_compile_start < sim->config.time_to_burnout)
		return (0);
	sim->stopped = 1;
	printf("%ld %d burned out\n", now - sim->start_time, coder->id);
	return (1);
}

static void	check_coders(t_simulation *sim, long now)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].compile_count
			>= sim->config.number_of_compiles_required)
			finished++;
		else if (check_burnout(sim, &sim->coders[i], now))
			break ;
		i++;
	}
	if (finished == sim->config.number_of_coders)
		sim->stopped = 1;
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			now;

	sim = (t_simulation *)arg;
	while (!is_simulation_stopped(sim))
	{
		now = get_time_ms();
		pthread_mutex_lock(&sim->state_mutex);
		check_coders(sim, now);
		pthread_mutex_unlock(&sim->state_mutex);
		usleep(1000);
	}
	return (NULL);
}
