/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:53:48 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 19:54:41 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	compile(t_coder *coder, int first, int second)
{
	if (!acquire_dongle(coder, first))
		return ;
	log_state(coder->data, coder->id, "has taken a dongle");
	if (!acquire_dongle(coder, second))
	{
		release_dongle(coder->data, first);
		return ;
	}
	log_state(coder->data, coder->id, "has taken a dongle");
	pthread_mutex_lock(&coder->mtx);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->mtx);
	log_state(coder->data, coder->id, "is compiling");
	sleep_ms(coder->data, coder->data->compile_time);
	pthread_mutex_lock(&coder->mtx);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mtx);
	release_dongle(coder->data, second);
	release_dongle(coder->data, first);
}

static void	debug_and_refactor(t_coder *coder)
{
	log_state(coder->data, coder->id, "is debugging");
	sleep_ms(coder->data, coder->data->debug_time);
	log_state(coder->data, coder->id, "is refactoring");
	sleep_ms(coder->data, coder->data->refactor_time);
}

/* A lone coder only ever has one dongle, so it can never compile. */
static void	lone_coder(t_coder *coder)
{
	if (!acquire_dongle(coder, 0))
		return ;
	log_state(coder->data, coder->id, "has taken a dongle");
	while (is_running(coder->data))
		usleep(POLL_US);
	release_dongle(coder->data, 0);
}

/*
** Coder N sits between dongles N-1 (left) and N % n_coders (right).
** Odd coders pick up left first and even coders right first, which breaks
** the circular wait and therefore prevents deadlock.
*/
void	*coder_routine(void *arg)
{
	t_coder	*coder;
	int		left;
	int		right;

	coder = (t_coder *)arg;
	if (coder->data->n_coders == 1)
	{
		lone_coder(coder);
		return (NULL);
	}
	left = coder->id - 1;
	right = coder->id % coder->data->n_coders;
	while (is_running(coder->data))
	{
		if (coder->id % 2 != 0)
			compile(coder, left, right);
		else
			compile(coder, right, left);
		debug_and_refactor(coder);
	}
	return (NULL);
}
