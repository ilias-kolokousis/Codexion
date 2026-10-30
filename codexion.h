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

# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <string.h>

typedef struct s_data	t_data;
typedef struct s_coder	t_coder;
typedef struct s_heap	t_heap;
typedef struct s_item	t_item;
typedef struct s_dongle	t_dongle;

typedef enum e_scheduler {
	FIFO_SCHED,
	EDF_SCHED
}	t_scheduler;

struct	s_item {
	int		id;
	long	key;
};

struct s_heap {
	t_item	items[2];
	int		size;
};

struct s_dongle {
	pthread_mutex_t	state_mtx;
	pthread_cond_t	state_cond;
	int				available;
	t_heap			heap;
	long			released_at;
};

struct s_data {
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
	pthread_mutex_t	sim_mtx;
	pthread_mutex_t	print_mtx;
	t_dongle		*dongles;
	pthread_t		monitor;
};

struct s_coder {
	int				id;
	long			last_compile_start;
	int				compile_count;
	t_data			*data;
	pthread_mutex_t	state_mtx;
	pthread_t		thread;
};

/* codexion.c */
void	start_codexion(t_data *data);
void	destroy_n_free(t_data *data);
void	*coder_thread(void *data);

/* dongles_init.c */
void	*dongles_init(t_data *codexion);
void	*destroy(t_data *codexion, int size);

/* compile.c */
void	compile(t_coder *coder, int first, int second);

/* helpers.c */
void	debug_n_refactor(t_coder *coder);
void	start_coder_threads(t_data *data);

/* monitor.c */
void	*monitor(void *data);

/* heap_ops.c */
void	push(t_heap *heap, int id, long key);
t_item	peek_top(t_heap *heap);
t_item	pop(t_heap *heap);
long	priority_key(t_coder *coder);

/* utils.c */
long	get_time_ms(void);
void	deadline_in_us(struct timespec *ts, long us);
void	safe_print(t_data *data, int id, char *msg);
int		is_running(t_data *data);

#endif