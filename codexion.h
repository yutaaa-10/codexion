/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:42 by yukurosa          #+#    #+#             */
/*   Updated: 2026/11/06 12:18:53 by yukurosa         ###   ########.fr       */
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

typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;

typedef struct s_request
{
	t_coder					*customer;
	long					arrival_order;
	long					deadline;
	int						granted;
	int						queued;
}							t_request;

typedef struct s_heap
{
	t_request				**items;
	int						size;
	int						capacity;
}							t_heap;

typedef struct s_dispatcher
{
	pthread_mutex_t			mutex;
	pthread_cond_t			cond;
	t_heap					waiting;
	long					next_order;
	int						initialized;
}							t_dispatcher;

struct						s_coder
{
	int						id;
	int						left_dongle;
	int						right_dongle;
	long					last_compile_start;
	int						compile_count;
	pthread_t				thread;
	t_simulation			*sim;
	t_request				request;
};

struct						s_simulation
{
	t_config				config;
	t_coder					*coders;
	t_dongle				*dongles;
	long					start_time;
	int						stopped;
	pthread_mutex_t			state_mutex;
	pthread_t				monitor_thread;
	t_dispatcher			dispatcher;
	int						started;
};

/* parse/parser.c */
int							ft_atol(const char *str, long *result);
int							parser_args(t_config *config, char **argv);

/* init/init_simulation.c */
int							init_simulation(t_simulation *sim);

/* utils/destroy_simulation.c */
void						destroy_simulation(t_simulation *sim);

/* utils/time_utils.c */
long						get_time_ms(void);
int							precise_sleep(t_simulation *sim, long duration_ms);

/* simulation/run_simulation.c */
int							run_simulation(t_simulation *sim);

/* simulation/coder_routine.c */
void						*coder_routine(void *data);

/* simulation/monitor_routine.c */
void						*monitor_routine(void *data);

/* simulation/simulation_state.c */
int							is_simulation_stopped(t_simulation *sim);
void						stop_simulation(t_simulation *sim);
int							start_compiling(t_coder *coder);
void						finish_compiling(t_coder *coder);
int							log_coder_action(t_coder *coder,
								const char *message);

/* simulation/dongle.c */
void						get_ordered_dongles(t_coder *coder,
								t_dongle **first, t_dongle **second);
void						release_dongles(t_coder *coder);

/* simulation/start_barrier.c */
int							wait_for_simulation_start(t_simulation *sim);
int							open_start_barrier(t_simulation *sim);

/* scheduler/heap_utils.c */
int							request_has_priority(t_request *first,
								t_request *second, t_scheduler policy);
int							heap_init(t_heap *heap, int capacity);
void						heap_destroy(t_heap *heap);

/* scheduler/heap.c */
int							heap_push(t_heap *heap, t_request *request,
								t_scheduler policy);
t_request					*heap_remove_at(t_heap *heap, int index,
								t_scheduler policy);

/* scheduler/request.c */
int							submit_request(t_coder *coder);

/* scheduler/grant_request.c */
int							request_dongles_available(t_request *request,
								long now);
int							reserve_request_dongles(t_request *request,
								long now);
int							grant_waiting_requests(t_simulation *sim);

/* scheduler/wait_request.c */
int							wait_for_dongles(t_coder *coder);
void						wake_waiting_coders(t_simulation *sim);

#endif
