/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 00:14:32 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 01:13:02 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

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

void	do_compile(t_coder *coder)
{
	print_log(coder, "has taken a dongle");
	print_log(coder, "has taken a dongle");
	print_log(coder, "is compiling");
	pthread_mutex_lock(&coder->hub->sim_lock);
	coder->last_compile = get_time_ms();
	pthread_mutex_unlock(&coder->hub->sim_lock);
	ft_usleep(coder->hub->time_to_compile, coder->hub);
	pthread_mutex_lock(&coder->hub->sim_lock);
	coder->compiles++;
	pthread_mutex_unlock(&coder->hub->sim_lock);
	drop_dongles(coder);
}
