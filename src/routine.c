/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 11:21:49 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/14 16:43:17 by bmoura-s         ###   ########.fr       */
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
	while (check_sim_running(coder->hub))
	{

	}
	return (NULL);
}

void	print_log(t_coder *coder, char *status)
{
	long	current_time;

	pthread_mutex_lock(&coder->hub->print_lock);
	if (check_sim_running(coder->hub))
	{
		current_time = get_time_ms();
	}
}
