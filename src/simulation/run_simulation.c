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

int	run_simulation(t_simulation *sim)
{
	int i;
	int n;
	int status;

	n = sim->config.number_of_coders;
	sim->start_time = get_time_ms();
	if (sim->start_time == -1)
		return (0);
	init_start_times(sim);
	i = 0;
	while (i < n)
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
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		stop_simulation(sim);
		join_threads(sim, n);
		return (0);
	}
	status = join_threads(sim, n);
	if (pthread_join(sim->monitor_thread, NULL) != 0)
		status = 0;
	return (status);
}
