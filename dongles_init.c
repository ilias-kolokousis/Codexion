/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 19:55:21 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:34:02 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*destroy(t_data *codexion, int size)
{
	int	i;

	i = 0;
	while (i <= size)
	{
		pthread_mutex_destroy(&codexion->dongles[i].state_mtx);
		pthread_cond_destroy(&codexion->dongles[i++].state_cond);
	}
	free(codexion->dongles);
	codexion->dongles = NULL;
	return (NULL);
}

void	*dongles_init(t_data *codexion)
{
	int	i;

	codexion->dongles = malloc(sizeof(t_dongle) * codexion->n_coders);
	if (codexion->dongles == NULL)
		return (NULL);
	i = 0;
	while (i < codexion->n_coders)
	{
		if (pthread_mutex_init(&codexion->dongles[i].state_mtx, NULL) != 0
			|| pthread_cond_init(&codexion->dongles[i].state_cond, NULL) != 0)
			return (destroy(codexion, i - 1));
		codexion->dongles[i].available = 1;
		codexion->dongles[i].released_at = 0;
		codexion->dongles[i].heap.items[0].id = 0; 
		codexion->dongles[i].heap.items[1].id = 0;
		codexion->dongles[i].heap.items[0].key = 0; 
		codexion->dongles[i].heap.items[1].key = 0; 
		codexion->dongles[i++].heap.size = 0; 
	}
	return (codexion->dongles);
}
