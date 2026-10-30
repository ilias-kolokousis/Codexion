/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:02:36 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:32:03 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coding_process(t_coder *coder, int first, int second)
{
	compile(coder, first, second);
	debug_n_refactor(coder);
}

void	*coder_thread(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	while (is_running(coder->data))
	{
		if (coder->data->n_coders == 1)
		{
			pthread_mutex_lock(&coder->data->dongles[0].state_mtx);
			safe_print(coder->data, coder->id, "has taken a dongle");
			while (is_running(coder->data))
				usleep(500);
			pthread_mutex_unlock(&coder->data->dongles[0].state_mtx);
		}
		else if (coder->id % 2 != 0)
			coding_process(coder, coder->id - 1, coder->id
				% coder->data->n_coders);
		else
			coding_process(coder, coder->id % coder->data->n_coders,
				coder->id - 1);
	}
	return (NULL);
}

static void	*coders_init(t_data *codexion)
{
	int	i;

	i = 0;
	codexion->coders = malloc(sizeof(t_coder) * codexion->n_coders);
	if (codexion->coders == NULL)
		return (NULL);
	while (i < codexion->n_coders)
	{
		codexion->coders[i].id = i + 1;
		codexion->coders[i].last_compile_start = get_time_ms();
		codexion->coders[i].compile_count = 0;
		codexion->coders[i].data = codexion;
		if (pthread_mutex_init(&codexion->coders[i].state_mtx, NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&codexion->coders[i].state_mtx);
			free(codexion->coders);
			codexion->coders = NULL;
			return (NULL);
		}
		i++;
	}
	return (codexion->coders);
}

void	destroy_n_free(t_data *data)
{
	pthread_mutex_destroy(&data->sim_mtx);
	pthread_mutex_destroy(&data->print_mtx);
	free(data->coders);
	data->coders = NULL;
	if (data->dongles != NULL)
		destroy(data, data->n_coders - 1);
}

void	start_codexion(t_data *data)
{
	coders_init(data);
	dongles_init(data);
	pthread_mutex_lock(&data->sim_mtx);
	data->sim_running = 1;
	pthread_mutex_unlock(&data->sim_mtx);
	if (data->coders != NULL && data->dongles != NULL)
	{
		if (pthread_create(&data->monitor, NULL, &monitor, data) != 0)
			return (destroy_n_free(data));
		start_coder_threads(data);
		if (pthread_join(data->monitor, NULL) != 0)
			return (destroy_n_free(data));
	}
	destroy_n_free(data);
}
