/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 13:07:42 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 20:03:06 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/*
** Each dongle sits between exactly two coders, so its request queue never
** holds more than two entries.
*/
# define QUEUE_CAPACITY 2

/* Granularity (in microseconds) of every polling loop in the simulation. */
# define POLL_US 500

typedef struct s_data	t_data;
typedef struct s_coder	t_coder;
typedef struct s_queue	t_queue;
typedef struct s_item	t_item;
typedef struct s_dongle	t_dongle;

typedef enum e_scheduler
{
	FIFO_SCHED,
	EDF_SCHED
}	t_scheduler;

struct s_item
{
	int		id;
	long	key;
};

/* Tiny priority queue: items[0] is always the request with the lowest key. */
struct s_queue
{
	t_item	items[QUEUE_CAPACITY];
	int		size;
};

struct s_dongle
{
	pthread_mutex_t	mtx;
	pthread_cond_t	cond;
	int				available;
	long			released_at;
	t_queue			queue;
};

struct s_coder
{
	int				id;
	long			last_compile_start;
	int				compile_count;
	t_data			*data;
	pthread_mutex_t	mtx;
	pthread_t		thread;
};

struct s_data
{
	long			n_coders;
	long			burnout_time;
	long			compile_time;
	long			debug_time;
	long			refactor_time;
	long			compiles_needed;
	long			cooldown;
	t_scheduler		scheduler;
	int				sim_running;
	long			start_time;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	sim_mtx;
	pthread_mutex_t	print_mtx;
	pthread_t		monitor;
};

/* init.c */
int		init_simulation(t_data *data);
void	destroy_coders(t_data *data, int count);
void	destroy_dongles(t_data *data, int count);

/* simulation.c */
int		run_simulation(t_data *data);
void	cleanup_simulation(t_data *data);

/* coder.c */
void	*coder_routine(void *arg);

/* dongle.c */
int		acquire_dongle(t_coder *coder, int index);
void	release_dongle(t_data *data, int index);

/* scheduler.c */
long	priority_key(t_coder *coder);
void	queue_push(t_queue *queue, int id, long key);
t_item	queue_peek(t_queue *queue);
void	queue_pop(t_queue *queue);

/* monitor.c */
void	*monitor_routine(void *arg);
void	stop_simulation(t_data *data, int id, char *msg);

/* utils.c */
long	get_time_ms(void);
void	deadline_in_us(struct timespec *ts, long us);
void	log_state(t_data *data, int id, char *msg);
int		is_running(t_data *data);
void	sleep_ms(t_data *data, long ms);

#endif
