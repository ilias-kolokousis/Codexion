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
#include <errno.h>
#include <limits.h>

/* Accepts only a plain non-negative decimal integer that fits in an int. */
static int	parse_number(const char *str, long *out)
{
	char	*end;
	long	value;

	if (str == NULL || *str == '\0')
		return (0);
	errno = 0;
	value = strtol(str, &end, 10);
	if (errno != 0 || *end != '\0' || value < 0 || value > INT_MAX)
		return (0);
	*out = value;
	return (1);
}

static int	parse_args(char **argv, t_data *data)
{
	if (!parse_number(argv[1], &data->n_coders)
		|| !parse_number(argv[2], &data->burnout_time)
		|| !parse_number(argv[3], &data->compile_time)
		|| !parse_number(argv[4], &data->debug_time)
		|| !parse_number(argv[5], &data->refactor_time)
		|| !parse_number(argv[6], &data->compiles_needed)
		|| !parse_number(argv[7], &data->cooldown)
		|| data->n_coders < 1)
		return (0);
	if (strcmp(argv[8], "fifo") == 0)
		data->scheduler = FIFO_SCHED;
	else if (strcmp(argv[8], "edf") == 0)
		data->scheduler = EDF_SCHED;
	else
		return (0);
	return (1);
}

static void	print_usage(const char *prog)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown <fifo|edf>\n"
		"  number_of_coders must be >= 1, every other number >= 0 "
		"(times in ms).\n", prog);
}

int	main(int argc, char **argv)
{
	t_data	data;
	int		ok;

	if (argc != 9 || !parse_args(argv, &data))
	{
		print_usage(argv[0]);
		return (EXIT_FAILURE);
	}
	if (!init_simulation(&data))
	{
		fprintf(stderr, "Error: failed to initialise the simulation\n");
		return (EXIT_FAILURE);
	}
	ok = run_simulation(&data);
	cleanup_simulation(&data);
	if (!ok)
	{
		fprintf(stderr, "Error: failed to start the simulation threads\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
