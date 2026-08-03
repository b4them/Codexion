/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:47:57 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/08/03 16:04:03 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	can_take_dongles(t_coder *coder)
{
	long	curr;
	int		i;
	t_coder	*other;

	if (coder->hub->dongle_ready[coder->l_dongle] == -1
		|| coder->hub->dongle_ready[coder->r_dongle] == -1)
		return (0);
	curr = get_time_ms();
	if (curr < coder->hub->dongle_ready[coder->l_dongle]
		|| curr < coder->hub->dongle_ready[coder->r_dongle])
		return (0);
	i = -1;
	while (++i < coder->hub->q_size)
	{
		other = coder->hub->queue[i];
		if (other != coder && compare_coders(other, coder, coder->hub) == 1)
		{
			if (other->l_dongle == coder->l_dongle
				|| other->r_dongle == coder->l_dongle
				|| other->l_dongle == coder->r_dongle
				|| other->r_dongle == coder->r_dongle)
				return (0);
		}
	}
	return (1);
}

static int	check_all_compiled(t_hub *hub)
{
	int	i;
	int	finished;

	if (hub->n_compiles == 0)
		return (0);
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

static void	trigger_burnout(t_hub *hub, int id)
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
		trigger_burnout(hub, hub->coders[i].coder_id);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_hub	*hub;
	int		i;

	hub = (t_hub *)arg;
	ft_usleep(10);
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
			trigger_burnout(hub, -1);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
