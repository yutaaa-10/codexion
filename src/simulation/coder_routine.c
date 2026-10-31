/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 11:00:32 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:05:50 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	has_finished(t_coder *coder)
{
	t_simulation	*sim;
	int				finished;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	finished = (coder->compile_count
			>= sim->config.number_of_compiles_required);
	pthread_mutex_unlock(&sim->state_mutex);
	return (finished);
}

static int	wait_for_dongles(t_coder *coder)
{
	while (!is_simulation_stopped(coder->sim))
	{
		if (try_acquire_dongles(coder))
			return (1);
		usleep(1000);
	}
	return (0);
}

static int	compile_code(t_coder *coder)
{
	if (!start_compiling(coder))
	{
		release_dongles(coder);
		return (0);
	}
	if (!precise_sleep(coder->sim, coder->sim->config.time_to_compile))
	{
		release_dongles(coder);
		return (0);
	}
	release_dongles(coder);
	finish_compiling(coder);
	return (1);
}

static int	debug_and_refactor(t_coder *coder)
{
	if (!log_coder_action(coder, "is debugging"))
		return (0);
	if (!precise_sleep(coder->sim, coder->sim->config.time_to_debug))
		return (0);
	if (!log_coder_action(coder, "is refactoring"))
		return (0);
	if (!precise_sleep(coder->sim, coder->sim->config.time_to_refactor))
		return (0);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!is_simulation_stopped(coder->sim))
	{
		if (has_finished(coder))
			break ;
		if (!wait_for_dongles(coder))
			break ;
		if (!compile_code(coder))
			break ;
		if (!debug_and_refactor(coder))
			break ;
	}
	return (NULL);
}
