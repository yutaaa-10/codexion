#include "codexion.h"

long	get_time_ms(void)
{
	struct timespec	ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		return (-1);
	return (ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

int	precise_sleep(t_simulation *sim, long duration_ms)
{
	long start;
	long now;

	start = get_time_ms();
	if (start < 0)
		return (0);
	while (!is_simulation_stopped(sim))
	{
		now = get_time_ms();
		if (now < 0)
			return (0);
		if (now - start >= duration_ms)
			return (1);
		usleep(500);
	}
	return (0);
}
