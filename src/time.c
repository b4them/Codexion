/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:11:47 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 00:14:12 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_usleep(long time_to_sleep, t_hub *hub)
{
	long	start_time;

	start_time = get_time_ms();
	while (check_sim_running(hub)
		&& (get_time_ms() - start_time) < time_to_sleep)
		usleep(500);
}
