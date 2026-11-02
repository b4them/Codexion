/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:47:57 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:00 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

static int	check_all_compiled(t_hub *hub)
{
	int	i;
	int	finished;

	finished = 0;
	i = -1;
	while (++i < hub->n_coders)
	{
		pthread_mutex_lock(&hub->sim_lock);
		if (hub->coders[i].compiles >= hub->n_compiles)
			finished++;
		pthread_mutex_unlock(&hub->sim_lock);
	}
	return (finished == hub->n_coders);
}

void	stop_sim(t_hub *hub, int id)
{
	pthread_mutex_lock(&hub->sim_lock);
	hub->sim_running = 0;
	pthread_mutex_unlock(&hub->sim_lock);
	if (id != -1)
	{
		pthread_mutex_lock(&hub->print_lock);
		printf("%ld %d burned out\n", get_time_ms() - hub->start_time, id);
		pthread_mutex_unlock(&hub->print_lock);
	}
	pthread_mutex_lock(&hub->queue_lock);
	pthread_cond_broadcast(&hub->arbitrator_cond);
	pthread_mutex_unlock(&hub->queue_lock);
}

static int	check_coder_burnout(t_hub *hub, int i)
{
	long	lc;

	pthread_mutex_lock(&hub->sim_lock);
	lc = hub->coders[i].last_compile;
	pthread_mutex_unlock(&hub->sim_lock);
	if (get_time_ms() - lc > hub->time_to_burnout)
	{
		stop_sim(hub, hub->coders[i].coder_id);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_hub	*hub;
	int		i;

	hub = (t_hub *)arg;
	while (check_sim_running(hub))
	{
		i = -1;
		while (++i < hub->n_coders && check_sim_running(hub))
		{
			if (check_coder_burnout(hub, i))
				return (NULL);
		}
		if (check_all_compiled(hub))
		{
			stop_sim(hub, -1);
			return (NULL);
		}
		pthread_mutex_lock(&hub->queue_lock);
		pthread_cond_broadcast(&hub->arbitrator_cond);
		pthread_mutex_unlock(&hub->queue_lock);
		usleep(1000);
	}
	return (NULL);
}
