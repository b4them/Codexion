/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 11:21:49 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/05 12:32:24 by bmoura-s         ###   ########.fr       */
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
		ft_usleep(10, coder->hub);
	while (check_sim_running(coder->hub))
	{
		if (!take_dongles(coder))
			break ;
		do_compile(coder);
		print_log(coder, "is debugging");
		ft_usleep(coder->hub->time_to_debug, coder->hub);
		print_log(coder, "is refactoring");
		ft_usleep(coder->hub->time_to_refactor, coder->hub);
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

int	take_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->queue_lock);
	push_queue(coder->hub, coder);
	while (check_sim_running(coder->hub) && !can_take_dongles(coder))
		wait_turn(coder);
	if (!check_sim_running(coder->hub))
	{
		pthread_mutex_unlock(&coder->hub->queue_lock);
		return (0);
	}
	remove_queue(coder->hub, coder);
	coder->hub->dongle_ready[coder->l_dongle] = -1;
	coder->hub->dongle_ready[coder->r_dongle] = -1;
	pthread_mutex_unlock(&coder->hub->queue_lock);
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->l_dongle]);
	pthread_mutex_lock(&coder->hub->dongle_locks[coder->r_dongle]);
	return (1);
}
