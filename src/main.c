/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 16:05:27 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/13 16:28:20 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	init_coders(t_hub *hub, t_coder **coders)
{
	int	i;

	*coders = malloc(sizeof(t_coder) * hub->n_coders);
	if (!(*coders))
		return (1);
	i = 0;
	while (i < hub->n_coders)
	{
		if (pthread_mutex_init(&hub->dongle_locks[i], NULL) != 0)
			return (1);
		(*coders)[i].coder_id = i + 1;
		(*coders)[i].compiles = 0;
		(*coders)[i].hub = hub;
		(*coders)[i].l_dongle = i;
		(*coders)[i].r_dongle = (i + 1) % hub->n_coders;
		i++;
	}
	hub->coders = *coders;
	return (0);
}

static int	parse_args(t_hub *hub, char **av)
{
	hub->n_coders = ft_satoi(av[1]);
	hub->time_to_burnout = ft_satoi(av[2]);
	hub->time_to_compile = ft_satoi(av[3]);
	hub->time_to_debug = ft_satoi(av[4]);
	hub->time_to_refactor = ft_satoi(av[5]);
	hub->n_compiles = ft_satoi(av[6]);
	hub->dongle_cooldown = ft_satoi(av[7]);
	if (hub->n_coders <= 0 || hub->time_to_burnout < 0
		|| hub->time_to_compile < 0 || hub->time_to_debug < 0
		|| hub->time_to_refactor < 0 || hub->n_compiles < 0
		|| hub->dongle_cooldown < 0)
		return (1);
	if (strcmp(av[8], "fifo") == 0)
		hub->scheduler = 0;
	else if (strcmp(av[8], "edf") == 0)
		hub->scheduler = 1;
	else
		return (1);
	return (0);
}

int	init_hub(t_hub *hub, char **av)
{
	if (parse_args(hub, av) != 0)
		return (1);
	if (pthread_mutex_init(&hub->print_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->queue_lock, NULL) != 0)
		return (1);
	hub->dongle_locks = malloc(sizeof(pthread_mutex_t) * hub->n_coders);
	hub->queue = malloc(sizeof(t_coder *) * hub->n_coders);
	if (!hub->dongle_locks || !hub->queue)
		return (1);
	hub->q_size = 0;
	hub->ticket_counter = 0;
	return (0);
}

int	main(int ac, char **av)
{
	t_coder	*coders;
	t_hub	hub;

	if (ac != 9)
	{
		printf("Error: Invalid number of arguments.\n");
		return (1);
	}
	if (init_hub(&hub, av) != 0)
	{
		printf("Error: Invalid arguments provided.\n");
		return (1);
	}
	if (init_coders(&hub, &coders) != 0)
	{
		printf("Error: Failed to initialize coders.\n");
		return (1);
	}
	return (0);
}
