/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikolokou <ikolokou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 18:49:31 by ikolokou          #+#    #+#             */
/*   Updated: 2026/08/28 12:26:42 by ikolokou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
static long	max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

static void	check_feasibility(t_data *codexion)
{
	if (codexion->burnout_time
		< max(2 * (codexion->compile_time + codexion->cooldown),
			codexion->compile_time
			+ max(codexion->debug_time
				+ codexion->refactor_time, codexion->cooldown)))
		printf("Burnout is expected to end the simulation\n");
	else
		printf("Every coder is expected to do all compiles needed\n");
}
*/

static t_data	*assign_args(char *argv[], t_data *codexion)
{
	if (atoi(argv[1]) <= 0 || atoi(argv[2]) < 0 || atoi(argv[3]) < 0
		|| atoi(argv[4]) < 0 || atoi(argv[5]) < 0 || atoi(argv[6]) < 0
		|| atoi(argv[7]) < 0 || (strcmp(argv[8], "fifo") != 0
			&& strcmp(argv[8], "edf") != 0)
		|| pthread_mutex_init(&codexion->sim_mtx, NULL) != 0 
		|| pthread_mutex_init(&codexion->print_mtx, NULL) != 0
	)
	{
		codexion->n_coders = 0;
		return (codexion);
	}
	codexion->n_coders = atoi(argv[1]);
	codexion->burnout_time = atoi(argv[2]);
	codexion->compile_time = atoi(argv[3]);
	codexion->debug_time = atoi(argv[4]);
	codexion->refactor_time = atoi(argv[5]);
	codexion->compiles_needed = atoi(argv[6]);
	codexion->cooldown = atoi(argv[7]);
	if ((strcmp(argv[8], "fifo") == 0))
		codexion->scheduler = FIFO_SCHED;
	else
		codexion->scheduler = EDF_SCHED;
	codexion->start_time = get_time_ms();
	return (codexion);
}

int	main(int argc, char *argv[])
{
	t_data	*codexion;

	if (argc != 9)
	{
		fprintf(stderr, "You've provided %d args instead of 8.\n", argc - 1);
		return (-1);
	}
	codexion = (t_data *) malloc(sizeof(t_data));
	if (codexion == NULL)
	{
		fprintf(stderr, "Malloc failed.");
		return (-1);
	}
	codexion = assign_args(argv, codexion);
	if (codexion->n_coders == 0)
	{
		fprintf(stderr, "Check if the arguments are in the correct format.\n");
		free(codexion);
		return (-1);
	}
	start_codexion(codexion);
	free(codexion);
	return (0);
}
