/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:02:36 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:32:03 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	prepare_start(t_data *data)
{
	int	i;

	data->start_time = get_time_ms();
	i = 0;
	while (i < data->n_coders)
		data->coders[i++].last_compile_start = data->start_time;
	data->sim_running = 1;
}

/* Returns how many coder threads were actually created. */
static int	start_coders(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_coders)
	{
		if (pthread_create(&data->coders[i].thread, NULL,
				&coder_routine, &data->coders[i]) != 0)
			break ;
		i++;
	}
	return (i);
}

/*
** Runs the simulation to completion. If any thread fails to start, the
** simulation is stopped and every thread that did start is still joined,
** so no thread outlives the data it points to.
*/
int	run_simulation(t_data *data)
{
	int	monitor_ok;
	int	started;
	int	all_started;

	prepare_start(data);
	monitor_ok = (pthread_create(&data->monitor, NULL,
				&monitor_routine, data) == 0);
	started = 0;
	if (monitor_ok)
		started = start_coders(data);
	all_started = (monitor_ok && started == data->n_coders);
	if (!all_started)
		stop_simulation(data, 0, NULL);
	if (monitor_ok)
		pthread_join(data->monitor, NULL);
	while (started-- > 0)
		pthread_join(data->coders[started].thread, NULL);
	return (all_started);
}

void	cleanup_simulation(t_data *data)
{
	destroy_dongles(data, data->n_coders);
	destroy_coders(data, data->n_coders);
	pthread_mutex_destroy(&data->sim_mtx);
	pthread_mutex_destroy(&data->print_mtx);
}
