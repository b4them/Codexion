/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arbiter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 00:14:58 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/05 12:35:12 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

void	init_queues(t_hub *hub)
{
	int	i;

	i = -1;
	while (++i < hub->n_coders)
	{
		hub->queues[i].items = &hub->slots[2 * i];
		hub->queues[i].size = 0;
	}
}

void	push_queue(t_hub *hub, t_coder *coder)
{
	coder->request_ticket = hub->ticket_counter++;
	heap_push(&hub->queues[coder->l_dongle], coder);
	heap_push(&hub->queues[coder->r_dongle], coder);
}

void	remove_queue(t_hub *hub, t_coder *coder)
{
	heap_remove(&hub->queues[coder->l_dongle], coder);
	heap_remove(&hub->queues[coder->r_dongle], coder);
}

int	can_take_dongles(t_coder *coder)
{
	t_hub	*hub;
	long	curr;

	hub = coder->hub;
	if (hub->dongle_ready[coder->l_dongle] == -1
		|| hub->dongle_ready[coder->r_dongle] == -1)
		return (0);
	curr = get_time_ms();
	if (curr < hub->dongle_ready[coder->l_dongle]
		|| curr < hub->dongle_ready[coder->r_dongle])
		return (0);
	return (hub->queues[coder->l_dongle].items[0] == coder
		&& hub->queues[coder->r_dongle].items[0] == coder);
}

long	next_ready_time(t_coder *coder)
{
	t_hub	*hub;
	long	left;
	long	right;

	hub = coder->hub;
	left = hub->dongle_ready[coder->l_dongle];
	right = hub->dongle_ready[coder->r_dongle];
	if (left == -1 || right == -1)
		return (-1);
	if (hub->queues[coder->l_dongle].items[0] != coder
		|| hub->queues[coder->r_dongle].items[0] != coder)
		return (-1);
	if (left > right)
		return (left);
	return (right);
}
