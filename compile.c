/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compile.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:53:48 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 19:54:41 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	acquire_dongle(t_coder *coder, int index)
{
	t_dongle		*dongle;
	struct timespec	ts;
	long			key;

	key = priority_key(coder);
	dongle = &coder->data->dongles[index];
	pthread_mutex_lock(&dongle->state_mtx);
	push(&dongle->heap, coder->id, key);
	while (is_running(coder->data)
		&& (!dongle->available || peek_top(&dongle->heap).id != coder->id
			|| get_time_ms() - dongle->released_at < coder->data->cooldown))
	{
		deadline_in_us(&ts, 500);
		pthread_cond_timedwait(&dongle->state_cond, &dongle->state_mtx, &ts);
	}
	if (!is_running(coder->data))
	{
		pthread_mutex_unlock(&dongle->state_mtx);
		return (0);
	}
	pop(&dongle->heap);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->state_mtx);
	return (1);
}

static void	release_dongle(t_data *data, int index)
{
	t_dongle	*dongle;

	dongle = &data->dongles[index];
	pthread_mutex_lock(&dongle->state_mtx);
	dongle->available = 1;
	dongle->released_at = get_time_ms();
	pthread_cond_broadcast(&dongle->state_cond);
	pthread_mutex_unlock(&dongle->state_mtx);
}

static void	update_compile_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mtx);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mtx);
}

void	compile(t_coder *coder, int first, int second)
{
	if (!is_running(coder->data))
		return ;
	if (!acquire_dongle(coder, first))
		return ;
	safe_print(coder->data, coder->id, "has taken a dongle");
	if (!acquire_dongle(coder, second))
	{
		release_dongle(coder->data, first);
		return ;
	}
	safe_print(coder->data, coder->id, "has taken a dongle");
	if (!is_running(coder->data)) 
	{
		release_dongle(coder->data, second);
		release_dongle(coder->data, first);
		return ;
	}
	safe_print(coder->data, coder->id, "is compiling");
	update_compile_start(coder);
	usleep(coder->data->compile_time * 1000);
	pthread_mutex_lock(&coder->state_mtx);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mtx);
	release_dongle(coder->data, second);
	release_dongle(coder->data, first);
}
