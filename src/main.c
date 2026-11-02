/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 16:05:27 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:14 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	init_coders(t_hub *hub, t_coder **coders)
{
	int	i;

	*coders = malloc(sizeof(t_coder) * hub->n_coders);
	if (!(*coders))
		return (1);
	i = -1;
	while (++i < hub->n_coders)
	{
		if (pthread_mutex_init(&hub->dongle_locks[i], NULL) != 0)
			return (rollback_coders(hub, *coders, i));
		(*coders)[i].coder_id = i + 1;
		(*coders)[i].compiles = 0;
		(*coders)[i].hub = hub;
		(*coders)[i].l_dongle = i;
		(*coders)[i].r_dongle = (i + 1) % hub->n_coders;
		if (i == hub->n_coders - 1)
		{
			(*coders)[i].l_dongle = 0;
			(*coders)[i].r_dongle = i;
		}
	}
	hub->coders = *coders;
	return (0);
}

static int	allocate_hub(t_hub *hub)
{
	hub->dongle_locks = malloc(sizeof(pthread_mutex_t) * hub->n_coders);
	hub->queues = malloc(sizeof(t_queue) * hub->n_coders);
	hub->slots = malloc(sizeof(t_coder *) * 2 * hub->n_coders);
	hub->dongle_ready = malloc(sizeof(long) * hub->n_coders);
	if (!hub->dongle_locks || !hub->queues || !hub->slots
		|| !hub->dongle_ready)
	{
		free(hub->dongle_locks);
		free(hub->queues);
		free(hub->slots);
		free(hub->dongle_ready);
		pthread_mutex_destroy(&hub->print_lock);
		pthread_mutex_destroy(&hub->queue_lock);
		pthread_mutex_destroy(&hub->sim_lock);
		pthread_cond_destroy(&hub->arbitrator_cond);
		return (1);
	}
	return (0);
}

int	init_hub(t_hub *hub, char **av)
{
	if (parse_args(hub, av) != 0)
		return (1);
	if (pthread_mutex_init(&hub->print_lock, NULL) != 0
		|| pthread_mutex_init(&hub->queue_lock, NULL) != 0
		|| pthread_mutex_init(&hub->sim_lock, NULL) != 0
		|| pthread_cond_init(&hub->arbitrator_cond, NULL) != 0)
		return (1);
	if (allocate_hub(hub) != 0)
		return (1);
	memset(hub->dongle_ready, 0, sizeof(long) * hub->n_coders);
	init_queues(hub);
	hub->ticket_counter = 0;
	hub->sim_running = 1;
	return (0);
}

static int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

int	main(int ac, char **av)
{
	t_coder	*coders;
	t_hub	hub;
	int		ret;

	if (ac != 9)
		return (print_error("expected 8 arguments: number_of_coders "
				"time_to_burnout time_to_compile time_to_debug "
				"time_to_refactor number_of_compiles_required "
				"dongle_cooldown scheduler(fifo|edf)"));
	if (init_hub(&hub, av) != 0)
		return (print_error("invalid arguments (non-negative integers, "
				"at least 1 coder, scheduler must be fifo or edf)"));
	if (init_coders(&hub, &coders) != 0)
		return (print_error("failed to initialize coders"));
	ret = start_sim(&hub, coders);
	if (ret != 0)
		print_error("simulation failed to start");
	cleanup_sim(&hub, coders);
	return (ret);
}
