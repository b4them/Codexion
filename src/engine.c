/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 14:55:15 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 00:12:44 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

static int	spawn_coders(t_hub *hub, t_coder *coders)
{
	int	i;
	int	err;

	i = -1;
	while (++i < hub->n_coders)
	{
		coders[i].last_compile = hub->start_time;
		if (hub->n_coders == 1)
			err = pthread_create(&coders[i].thread_id, NULL,
					lone_coder, &coders[i]);
		else
			err = pthread_create(&coders[i].thread_id, NULL,
					coder_routine, &coders[i]);
		if (err != 0)
			return (i);
	}
	return (i);
}

int	start_sim(t_hub *hub, t_coder *coders)
{
	int	i;
	int	started;
	int	failed;

	if (hub->n_compiles == 0)
		return (0);
	hub->start_time = get_time_ms();
	started = spawn_coders(hub, coders);
	failed = (started != hub->n_coders);
	if (!failed && pthread_create(
			&hub->monitor_id, NULL, monitor_routine, hub) != 0)
		failed = 1;
	if (failed)
		stop_sim(hub, -1);
	i = -1;
	while (++i < started)
		pthread_join(coders[i].thread_id, NULL);
	if (!failed)
		pthread_join(hub->monitor_id, NULL);
	return (failed);
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
	free(hub->queues);
	free(hub->slots);
	free(hub->dongle_ready);
	free(coders);
}

void	*lone_coder(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->l_dongle]);
	print_log(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->hub->queue_lock);
	while (check_sim_running(coder->hub))
		pthread_cond_wait(&coder->hub->arbitrator_cond,
			&coder->hub->queue_lock);
	pthread_mutex_unlock(&coder->hub->queue_lock);
	pthread_mutex_unlock(&coder->hub->dongle_locks[coder->l_dongle]);
	return (NULL);
}

int	rollback_coders(t_hub *hub, t_coder *coders, int i)
{
	while (--i >= 0)
		pthread_mutex_destroy(&hub->dongle_locks[i]);
	free(coders);
	return (1);
}
