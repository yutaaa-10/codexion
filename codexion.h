/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:42 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/31 22:01:56 by yukurosa         ###   ########.fr       */
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

/* Parser */
int							ft_atol(const char *str, long *result);
int							parser_args(t_config *config, char **argv);

/* Initialization */
int							init_coders(t_simulation *sim);
int							init_dongles(t_simulation *sim);
int							init_dispatcher(t_simulation *sim);
int							init_simulation(t_simulation *sim);

/* Destruction */
void						destroy_simulation(t_simulation *sim);
void						destroy_dispatcher(t_dispatcher *dispatcher);

/* Simulation */
int							run_simulation(t_simulation *sim);
void						*coder_routine(void *data);
void						*monitor_routine(void *data);

/* Simulation state */
int							is_simulation_stopped(t_simulation *sim);
void						stop_simulation(t_simulation *sim);
int							start_compiling(t_coder *coder);
void						finish_compiling(t_coder *coder);

/* Dongles */
int							try_acquire_dongle(t_dongle *dongle);
void						release_dongle(t_dongle *dongle, long cooldown);
int							try_acquire_dongles(t_coder *coder);
void						release_dongles(t_coder *coder);

/* Scheduler heap */
int							heap_init(t_heap *heap, int capacity);
void						heap_destroy(t_heap *heap);
int							heap_push(t_heap *heap, t_request *request,
								t_scheduler policy);
t_request					*heap_peek(t_heap *heap);
t_request					*heap_pop(t_heap *heap, t_scheduler policy);
int							request_has_priority(t_request *first,
								t_request *second, t_scheduler policy);
int							submit_request(t_coder *coder);

/* Time and logging */
long						get_time_ms(void);
int							precise_sleep(t_simulation *sim, long duration_ms);
int							log_coder_action(t_coder *coder,
								const char *message);
/* Scheduler grants */
t_request					*heap_remove_at(t_heap *heap, int index,
								t_scheduler policy);
int							request_has_higher_conflict(t_simulation *sim,
								t_request *candidate);
int							request_dongles_available(t_request *request,
								long now);
int							reserve_request_dongles(t_request *request,
								long now);
int							grant_waiting_requests(t_simulation *sim);

/* Scheduler waiting */
int							wait_for_dongles(t_coder *coder);
void						wake_waiting_coders(t_simulation *sim);

/* Start barrier */
int							wait_for_simulation_start(t_simulation *sim);
int							open_start_barrier(t_simulation *sim);

#endif