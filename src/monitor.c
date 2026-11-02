/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:43:52 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 18:34:26 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wake_all_waiters(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_coders)
	{
		pthread_mutex_lock(&data->dongles[i].mtx);
		pthread_cond_broadcast(&data->dongles[i].cond);
		pthread_mutex_unlock(&data->dongles[i].mtx);
		i++;
	}
}

/*
** Ends the simulation and prints the final message (if any) while holding
** the print lock, so no coder can log anything after it. id 0 prints the
** message without a coder id.
*/
void	stop_simulation(t_data *data, int id, char *msg)
{
	int	was_running;

	pthread_mutex_lock(&data->print_mtx);
	pthread_mutex_lock(&data->sim_mtx);
	was_running = data->sim_running;
	data->sim_running = 0;
	pthread_mutex_unlock(&data->sim_mtx);
	if (was_running && msg != NULL && id > 0)
		printf("%ld %d %s\n", get_time_ms() - data->start_time, id, msg);
	else if (was_running && msg != NULL)
		printf("%ld %s\n", get_time_ms() - data->start_time, msg);
	pthread_mutex_unlock(&data->print_mtx);
	wake_all_waiters(data);
}

static int	check_burnout(t_data *data)
{
	int		i;
	long	idle;

	i = 0;
	while (i < data->n_coders)
	{
		pthread_mutex_lock(&data->coders[i].mtx);
		idle = get_time_ms() - data->coders[i].last_compile_start;
		pthread_mutex_unlock(&data->coders[i].mtx);
		if (idle >= data->burnout_time)
		{
			stop_simulation(data, data->coders[i].id, "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_compiles_done(t_data *data)
{
	int	i;
	int	count;

	i = 0;
	while (i < data->n_coders)
	{
		pthread_mutex_lock(&data->coders[i].mtx);
		count = data->coders[i].compile_count;
		pthread_mutex_unlock(&data->coders[i].mtx);
		if (count < data->compiles_needed)
			return (0);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (is_running(data))
	{
		if (check_burnout(data))
			break ;
		if (all_compiles_done(data))
		{
			stop_simulation(data, 0, "all coders finished");
			break ;
		}
		usleep(POLL_US);
	}
	return (NULL);
}
