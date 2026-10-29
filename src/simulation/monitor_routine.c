#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_simulation *sim;
	long now;
	long last;
	int i;
	int finished;

	sim = (t_simulation *)arg;
	while (!is_simulation_stopped(sim))
	{
		now = get_time_ms();
		finished = 0;
		i = 0;
		pthread_mutex_lock(&sim->state_mutex);
		while (i < sim->config.number_of_coders)
		{
			if (sim->coders[i].compile_count >= sim->config.number_of_compiles_required)
				finished++;
			else
			{
				last = sim->coders[i].last_compile_start;
				if (now - last >= sim->config.time_to_burnout)
				{
					sim->stopped = 1;
					printf("%ld %d burned out\n", now - sim->start_time,
						sim->coders[i].id);
					break ;
				}
			}
			i++;
		}
		if (finished == sim->config.number_of_coders)
			sim->stopped = 1;
		pthread_mutex_unlock(&sim->state_mutex);
		usleep(1000);
	}
	return (NULL);
}
