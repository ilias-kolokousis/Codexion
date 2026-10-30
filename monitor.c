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

int	burnout(t_data *codex, int id)
{
	int	i;

	safe_print(codex, id, "burned out");
	pthread_mutex_lock(&codex->sim_mtx);
	codex->sim_running = 0;
	pthread_mutex_unlock(&codex->sim_mtx);
	i = 0;
	while (i < codex->n_coders)
	{
		pthread_mutex_lock(&codex->dongles[i].state_mtx);
		pthread_cond_broadcast(&codex->dongles[i].state_cond);
		pthread_mutex_unlock(&codex->dongles[i].state_mtx);
		i++;
	}
	return (1);
}

static int	any_burnout(t_data *data)
{
	int		i;
	long	time;
	int		id;

	i = 0;
	while (i < data->n_coders)
	{
		pthread_mutex_lock(&data->coders[i].state_mtx);
		time = get_time_ms() - data->coders[i].last_compile_start;
		id = data->coders[i].id;
		pthread_mutex_unlock(&data->coders[i].state_mtx);
		if (time >= data->burnout_time)
		{
			burnout(data, id);
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
		pthread_mutex_lock(&data->coders[i].state_mtx);
		count = data->coders[i].compile_count;
		pthread_mutex_unlock(&data->coders[i].state_mtx);
		if (count < data->compiles_needed)
			return (0);
		i++;
	}
	return (1);
}

int	continue_coders(t_data *data)
{
	int	i;

	if (any_burnout(data))
		return (0);
	if (all_compiles_done(data))
	{
		safe_print(data, 0, "all coders finished");
		pthread_mutex_lock(&data->sim_mtx);
		data->sim_running = 0;
		pthread_mutex_unlock(&data->sim_mtx);
		i = 0;
		while (i < data->n_coders)
		{
			pthread_mutex_lock(&data->dongles[i].state_mtx);
			pthread_cond_broadcast(&data->dongles[i].state_cond);
			pthread_mutex_unlock(&data->dongles[i++].state_mtx);
		}
		return (0);
	}
	return (1);
}

void	*monitor(void *data)
{
	t_data	*codex;

	codex = (t_data *)data;
	while (1)
	{
		if (!is_running(codex))
			break ;
		if (continue_coders(codex) == 0)
			break ;
		usleep(500);
	}
	return (NULL);
}
