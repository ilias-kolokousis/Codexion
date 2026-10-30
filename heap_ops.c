/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:46:35 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/27 20:19:22 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	priority_key(t_coder *coder)
{
	long	edf_time;

	pthread_mutex_lock(&coder->state_mtx);
	edf_time = coder->last_compile_start + coder->data->burnout_time;
	pthread_mutex_unlock(&coder->state_mtx);
	if (coder->data->scheduler == FIFO_SCHED)
		return (get_time_ms());
	else
		return (edf_time);
}

void	push(t_heap *heap, int id, long key)
{
	t_item	tmp;

	heap->items[heap->size].id = id;
	heap->items[heap->size].key = key;
	heap->size++;
	if (heap->size == 2 && heap->items[0].key > heap->items[1].key)
	{
		tmp = heap->items[0];
		heap->items[0] = heap->items[1];
		heap->items[1] = tmp;
	}
}

t_item	peek_top(t_heap *heap)
{
	return (heap->items[0]);
}

t_item	pop(t_heap *heap)
{
	int		i;
	t_item	smallest_key;

	smallest_key = heap->items[0];
	i = 0;
	while (i < heap->size - 1)
	{
		heap->items[i] = heap->items[i + 1];
		i++;
	}
	heap->items[heap->size - 1].id = 0;
	heap->items[heap->size - 1].key = 0;
	heap->size--;
	return (smallest_key);
}
