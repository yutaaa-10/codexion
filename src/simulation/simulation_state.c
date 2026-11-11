/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_state.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-11 05:24:42 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-11 05:24:42 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_simulation_stopped(t_simulation *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->state_mutex);
	stopped = sim->stopped;
	pthread_mutex_unlock(&sim->state_mutex);
	return (stopped);
}

void	stop_simulation(t_simulation *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	sim->stopped = 1;
	pthread_mutex_unlock(&sim->state_mutex);
	wake_waiting_coders(sim);
}

int	start_compiling(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
	int				success;

	sim = coder->sim;
	success = 0;
	pthread_mutex_lock(&sim->state_mutex);
	now = get_time_ms();
	if (!sim->stopped && now >= 0 && now
		- coder->last_compile_start < sim->config.time_to_burnout)
	{
		coder->last_compile_start = now;
		printf("%ld %d is compiling\n", now - sim->start_time, coder->id);
		success = 1;
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (success);
}

void	finish_compiling(t_coder *coder)
{
	t_simulation	*sim;
	int				completed;
	int				i;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	coder->compile_count++;
	completed = 1;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].compile_count
			< sim->config.number_of_compiles_required)
		{
			completed = 0;
			break ;
		}
		i++;
	}
	if (completed)
		sim->stopped = 1;
	pthread_mutex_unlock(&sim->state_mutex);
	if (completed)
		wake_waiting_coders(sim);
}

int	log_coder_action(t_coder *coder, const char *message)
{
	t_simulation	*sim;
	int				success;

	sim = coder->sim;
	success = 0;
	pthread_mutex_lock(&sim->state_mutex);
	if (!sim->stopped)
	{
		printf("%ld %d %s\n", get_time_ms() - sim->start_time, coder->id,
			message);
		success = 1;
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (success);
}
