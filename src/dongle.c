/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 12:04:53 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:35:11 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Called with dongle->mtx held. */
static int	dongle_ready(t_coder *coder, t_dongle *dongle)
{
	return (dongle->available
		&& queue_peek(&dongle->queue).id == coder->id
		&& get_time_ms() - dongle->released_at >= coder->data->cooldown);
}

/*
** Queues the coder's request and blocks until the dongle is free, its
** cooldown has elapsed and the coder is first in line according to the
** scheduler. Returns 0 if the simulation stopped while waiting.
*/
int	acquire_dongle(t_coder *coder, int index)
{
	t_dongle		*dongle;
	struct timespec	deadline;
	long			key;
	int				acquired;

	key = priority_key(coder);
	dongle = &coder->data->dongles[index];
	pthread_mutex_lock(&dongle->mtx);
	queue_push(&dongle->queue, coder->id, key);
	while (is_running(coder->data) && !dongle_ready(coder, dongle))
	{
		deadline_in_us(&deadline, POLL_US);
		pthread_cond_timedwait(&dongle->cond, &dongle->mtx, &deadline);
	}
	acquired = is_running(coder->data);
	if (acquired)
	{
		queue_pop(&dongle->queue);
		dongle->available = 0;
	}
	pthread_mutex_unlock(&dongle->mtx);
	return (acquired);
}

void	release_dongle(t_data *data, int index)
{
	t_dongle	*dongle;

	dongle = &data->dongles[index];
	pthread_mutex_lock(&dongle->mtx);
	dongle->available = 1;
	dongle->released_at = get_time_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mtx);
}
