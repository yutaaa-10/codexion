/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:42 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:02:02 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}							t_scheduler;

typedef struct s_config
{
	int						number_of_coders;
	long					time_to_burnout;
	long					time_to_compile;
	long					time_to_debug;
	long					time_to_refactor;
	int						number_of_compiles_required;
	long					dongle_cooldown;
	t_scheduler				scheduler;
}							t_config;

typedef struct s_dongle
{
	pthread_mutex_t			mutex;
	int						in_use;
	long					available_at;
}							t_dongle;

/* Forward declaration */

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int						id;
	int						left_dongle;
	int						right_dongle;
	long					last_compile_start;
	int						compile_count;
	pthread_t				thread;
	t_simulation			*sim;
}							t_coder;

struct						s_simulation
{
	t_config				config;
	t_coder					*coders;
	t_dongle				*dongles;
	long					start_time;
	int						stopped;
	pthread_mutex_t			state_mutex;
	pthread_t				monitor_thread;
};

int							parser_args(t_config *config, char **argv);
int							init_coders(t_simulation *sim);
int							init_dongles(t_simulation *sim);
void						destroy_simulation(t_simulation *sim);
void						*coder_routine(void *arg);
int							run_simulation(t_simulation *sim);
long						get_time_ms(void);
int							try_acquire_dongle(t_dongle *dongle);
void						release_dongle(t_dongle *dongle, long cooldown);
int							try_acquire_dongles(t_coder *coder);
void						release_dongles(t_coder *coder);
int							is_simulation_stopped(t_simulation *sim);
void						stop_simulation(t_simulation *sim);

int							start_compiling(t_coder *coder);
void						finish_compiling(t_coder *coder);

void						*monitor_routine(void *arg);

int							log_coder_action(t_coder *coder,
								const char *message);
int							precise_sleep(t_simulation *sim, long duration_ms);
#endif
