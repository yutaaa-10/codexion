#include "codexion.h"
#include <stdlib.h>

void	destroy_simulation(t_simulation *sim)
{
	int i;

	i = 0;
	if (sim->dongles)
	{
		while (i < sim->config.number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			i++;
		}
		free(sim->dongles);
	}
	free(sim->coders);
}