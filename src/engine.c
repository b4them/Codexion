/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 14:55:15 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/30 16:34:20 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	start_sim(t_hub *hub, t_coder *coders)
{
	int	i;

	hub->start_time = get_time_ms();
	i = -1;
	while (++i < hub->n_coders)
	{
		coders[i].last_compile = hub->start_time;
		if (hub->n_coders == 1)
			pthread_create(&coders[i].thread_id, NULL, lone_coder, &coders[i]);
		else
			pthread_create(&coders[i].thread_id, NULL, coder_routine, &coders[i]);
	}
	if (pthread_create(&hub->monitor_id, NULL, monitor_routine, hub) != 0)
		return (1);
	i = -1;
	while (++i < hub->n_coders)
		pthread_join(coders[i].thread_id, NULL);
	pthread_join(hub->monitor_id, NULL);
	return (0);
}

void	cleanup_sim(t_hub *hub, t_coder *coders)
{
	int	i;

	i = -1;
	while (++i < hub->n_coders)
		pthread_mutex_destroy(&hub->dongle_locks[i]);
	pthread_mutex_destroy(&hub->print_lock);
	pthread_mutex_destroy(&hub->queue_lock);
	pthread_mutex_destroy(&hub->sim_lock);
	pthread_cond_destroy(&hub->arbitrator_cond);
	free(hub->dongle_locks);
	free(hub->queue);
	free(hub->dongle_ready);
	free(coders);
}

void	*lone_coder(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->l_dongle]);
	print_log(coder, "has taken a dongle");
	while (check_sim_running(coder->hub))
		ft_usleep(1);
	pthread_mutex_unlock(&coder->hub->dongle_locks[coder->l_dongle]);
	return (NULL);
}
