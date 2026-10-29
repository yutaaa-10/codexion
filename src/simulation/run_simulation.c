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

int	run_simulation(t_simulation *sim)
{
	int i;
	int n;

	n = sim->config.number_of_coders;
	i = 0;
	while (i < n)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			join_threads(sim, i);
			return (0);
		}
		i++;
	}
	return (join_threads(sim, n));
}