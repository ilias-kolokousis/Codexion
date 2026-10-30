/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:17:34 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 19:53:11 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((long)tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	deadline_in_us(struct timespec *ts, long us)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + (tv.tv_usec + us) / 1000000;
	ts->tv_nsec = ((tv.tv_usec + us) % 1000000) * 1000;
}

void	safe_print(t_data *data, int id, char *msg)
{
	pthread_mutex_lock(&data->print_mtx);
	if (id == 0)
		printf("%li %s\n", get_time_ms() - data->start_time, msg);
	else
		printf("%li %i %s\n", get_time_ms() - data->start_time, id, msg);
	pthread_mutex_unlock(&data->print_mtx);
}

int	is_running(t_data *data)
{
	int	running;

	pthread_mutex_lock(&data->sim_mtx);
	running = data->sim_running;
	pthread_mutex_unlock(&data->sim_mtx);
	return (running);
}
