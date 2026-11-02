/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:55:21 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:34:02 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_coders(t_data *data, int count)
{
	while (count-- > 0)
		pthread_mutex_destroy(&data->coders[count].mtx);
	free(data->coders);
	data->coders = NULL;
}

void	destroy_dongles(t_data *data, int count)
{
	while (count-- > 0)
	{
		pthread_mutex_destroy(&data->dongles[count].mtx);
		pthread_cond_destroy(&data->dongles[count].cond);
	}
	free(data->dongles);
	data->dongles = NULL;
}

static int	init_coders(t_data *data)
{
	int	i;

	data->coders = calloc(data->n_coders, sizeof(t_coder));
	if (data->coders == NULL)
		return (0);
	i = 0;
	while (i < data->n_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].data = data;
		if (pthread_mutex_init(&data->coders[i].mtx, NULL) != 0)
		{
			destroy_coders(data, i);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	init_dongles(t_data *data)
{
	int	i;

	data->dongles = calloc(data->n_coders, sizeof(t_dongle));
	if (data->dongles == NULL)
		return (0);
	i = 0;
	while (i < data->n_coders)
	{
		data->dongles[i].available = 1;
		if (pthread_mutex_init(&data->dongles[i].mtx, NULL) != 0)
		{
			destroy_dongles(data, i);
			return (0);
		}
		if (pthread_cond_init(&data->dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&data->dongles[i].mtx);
			destroy_dongles(data, i);
			return (0);
		}
		i++;
	}
	return (1);
}

/* On failure everything that was already set up is released again. */
int	init_simulation(t_data *data)
{
	data->coders = NULL;
	data->dongles = NULL;
	data->sim_running = 0;
	if (pthread_mutex_init(&data->sim_mtx, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mtx, NULL) != 0)
	{
		pthread_mutex_destroy(&data->sim_mtx);
		return (0);
	}
	if (!init_coders(data) || !init_dongles(data))
	{
		if (data->coders != NULL)
			destroy_coders(data, data->n_coders);
		pthread_mutex_destroy(&data->sim_mtx);
		pthread_mutex_destroy(&data->print_mtx);
		return (0);
	}
	return (1);
}
