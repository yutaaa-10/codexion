#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder *coder;
	t_simulation *sim;
	int acquired;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!is_simulation_stopped(sim))
	{
		pthread_mutex_lock(&sim->state_mutex);
		if (coder->compile_count >= sim->config.number_of_compiles_required)
		{
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->state_mutex);

		acquired = 0;
		while (!is_simulation_stopped(sim))
		{
			if (try_acquire_dongles(coder))
			{
				acquired = 1;
				break ;
			}
			usleep(1000);
		}
		if (!acquired)
			break ;

		/* 変更①：コンパイル開始 */
		if (!start_compiling(coder))
		{
			release_dongles(coder);
			break ;
		}

		/* 変更②：コンパイル中の停止に対応 */
		if (!precise_sleep(sim, sim->config.time_to_compile))
		{
			release_dongles(coder);
			break ;
		}
		release_dongles(coder);
		finish_compiling(coder);

		/* 変更③：デバッグ */
		if (!log_coder_action(coder, "is debugging"))
			break ;
		if (!precise_sleep(sim, sim->config.time_to_debug))
			break ;

		/* 変更④：リファクタリング */
		if (!log_coder_action(coder, "is refactoring"))
			break ;
		if (!precise_sleep(sim, sim->config.time_to_refactor))
			break ;
	}
	return (NULL);
}
