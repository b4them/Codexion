/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:47:57 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/29 17:05:07 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	ft_satoi(const char *nptr)
{
	long	sum;
	int		sign;

	sum = 0;
	sign = 1;
	while (*nptr == ' ' || (*nptr >= 9 && *nptr <= 13))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
	{
		if (*nptr == '-')
			sign *= -1;
		nptr++;
	}
	while (*nptr >= '0' && *nptr <= '9')
	{
		sum = (sum * 10) + (*nptr - '0');
		nptr++;
	}
	if (*nptr != '\0')
		return (-1);
	return (sign * sum);
}

int	can_take_dongles(t_coder *coder)
{
	long	current_time;

	if (coder->hub->queue[0] != coder)
		return (0);
	current_time = get_time_ms();
	if (current_time < coder->hub->dongle_ready[coder->l_dongle]
		|| current_time < coder->hub->dongle_ready[coder->r_dongle])
		return (0);
	return (1);
}

static int	check_all_compiled(t_hub *hub)
{
	int	i;
	int	finished_coders;

	finished_coders = 0;
	i = -1;
	while (++i < hub->n_coders)
	{
		if (hub->coders[i].compiles >= hub->n_compiles)
			finished_coders++;
	}
	return (finished_coders == hub->n_coders);
}

static void	trigger_burnout(t_hub *hub, int id)
{
	pthread_mutex_lock(&hub->sim_lock);
	hub->sim_running = 0;
	pthread_mutex_unlock(&hub->sim_lock);
	pthread_mutex_lock(&hub->print_lock);
	printf("%ld %d burned out\n", get_time_ms() - hub->start_time, id);
	pthread_mutex_unlock(&hub->print_lock);
	pthread_mutex_lock(&hub->queue_lock);
	pthread_cond_broadcast(&hub->arbitrator_cond);
	pthread_mutex_unlock(&hub->queue_lock);
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
			if (get_time_ms() - hub->coders[
					i].last_compile > hub->time_to_burnout)
			{
				trigger_burnout(hub, hub->coders[i].coder_id);
				return (NULL);
			}
			if (check_all_compiled(hub))
			{
				trigger_burnout(hub, -1);
				return (NULL);
			}
		}
		usleep(1000);
	}
	return (NULL);
}
