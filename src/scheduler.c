/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:46:35 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 20:19:22 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** FIFO: requests are served in arrival order.
** EDF: the coder closest to burning out is served first.
*/
long	priority_key(t_coder *coder)
{
	long	deadline;

	if (coder->data->scheduler == FIFO_SCHED)
		return (get_time_ms());
	pthread_mutex_lock(&coder->mtx);
	deadline = coder->last_compile_start + coder->data->burnout_time;
	pthread_mutex_unlock(&coder->mtx);
	return (deadline);
}

/* On equal keys the earlier request stays in front. */
void	queue_push(t_queue *queue, int id, long key)
{
	t_item	tmp;

	if (queue->size >= QUEUE_CAPACITY)
		return ;
	queue->items[queue->size].id = id;
	queue->items[queue->size].key = key;
	queue->size++;
	if (queue->size == 2 && queue->items[0].key > queue->items[1].key)
	{
		tmp = queue->items[0];
		queue->items[0] = queue->items[1];
		queue->items[1] = tmp;
	}
}

t_item	queue_peek(t_queue *queue)
{
	return (queue->items[0]);
}

void	queue_pop(t_queue *queue)
{
	int	i;

	if (queue->size == 0)
		return ;
	i = 0;
	while (i < queue->size - 1)
	{
		queue->items[i] = queue->items[i + 1];
		i++;
	}
	queue->size--;
}
