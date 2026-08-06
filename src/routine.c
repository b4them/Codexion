/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 11:21:49 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/08/05 11:32:30 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	check_sim_running(t_hub *hub)
{
	int	status;

	pthread_mutex_lock(&hub->sim_lock);
	status = hub->sim_running;
	pthread_mutex_unlock(&hub->sim_lock);
	return (status);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->coder_id % 2 == 0)
		ft_usleep(10);
	while (check_sim_running(coder->hub))
	{
		take_dongles(coder);
		if (!check_sim_running(coder->hub))
			break ;
		print_log(coder, "has taken a dongle");
		print_log(coder, "has taken a dongle");
		print_log(coder, "is compiling");
		pthread_mutex_lock(&coder->hub->sim_lock);
		coder->last_compile = get_time_ms();
		coder->compiles++;
		pthread_mutex_unlock(&coder->hub->sim_lock);
		ft_usleep(coder->hub->time_to_compile);
		drop_dongles(coder);
		print_log(coder, "is debugging");
		ft_usleep(coder->hub->time_to_debug);
		print_log(coder, "is refactoring");
		ft_usleep(coder->hub->time_to_refactor);
	}
	return (NULL);
}

void	print_log(t_coder *coder, char *status)
{
	long	current_time;

	pthread_mutex_lock(&coder->hub->print_lock);
	if (check_sim_running(coder->hub))
	{
		current_time = get_time_ms() - coder->hub->start_time;
		printf("%ld %d %s\n", current_time, coder->coder_id, status);
	}
	pthread_mutex_unlock(&coder->hub->print_lock);
}

void	drop_dongles(t_coder *coder)
{
	long	curr;

	pthread_mutex_lock(&coder->hub->queue_lock);
	curr = get_time_ms();
	coder->hub->dongle_ready[coder->l_dongle] = (curr
			+ coder->hub->dongle_cooldown);
	coder->hub->dongle_ready[coder->r_dongle] = (curr
			+ coder->hub->dongle_cooldown);
	pthread_cond_broadcast(&coder->hub->arbitrator_cond);
	pthread_mutex_unlock(&coder->hub->queue_lock);
	pthread_mutex_unlock(&coder->hub->dongle_locks[coder->r_dongle]);
	pthread_mutex_unlock(&coder->hub->dongle_locks[coder->l_dongle]);
}

void	take_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->queue_lock);
	push_queue(coder->hub, coder);
	while (check_sim_running(coder->hub) && !can_take_dongles(coder))
		pthread_cond_wait(&coder->hub->arbitrator_cond,
			&coder->hub->queue_lock);
	if (!check_sim_running(coder->hub))
	{
		pthread_mutex_unlock(&coder->hub->queue_lock);
		return ;
	}
	remove_queue(coder->hub, coder);
	coder->hub->dongle_ready[coder->l_dongle] = -1;
	coder->hub->dongle_ready[coder->r_dongle] = -1;
	pthread_mutex_unlock(&coder->hub->queue_lock);
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->l_dongle]);
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->r_dongle]);
}
