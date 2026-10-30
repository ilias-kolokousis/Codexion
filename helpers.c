/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 12:04:53 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:35:11 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	debug_n_refactor(t_coder *coder)
{
	if (!is_running(coder->data))
		return ;
	safe_print(coder->data, coder->id, "is debugging");
	usleep(coder->data->debug_time * 1000);
	if (!is_running(coder->data))
		return ;
	safe_print(coder->data, coder->id, "is refactoring");
	usleep(coder->data->refactor_time * 1000);
}

void	start_coder_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i++ < data->n_coders)
	{
		if (pthread_create(&data->coders[i - 1].thread,
				NULL, &coder_thread, &data->coders[i - 1]) != 0)
			return ;
	}
	i = 0;
	while (i++ < data->n_coders)
	{
		if (pthread_join(data->coders[i - 1].thread, NULL) != 0)
			return ;
		pthread_mutex_destroy(&data->coders[i - 1].state_mtx);
	}
}
