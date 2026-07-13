/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 14:45:19 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/13 15:29:12 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	compare_coders(t_coder *a, t_coder *b, t_hub *hub)
{
	long	deadline_a;
	long	deadline_b;

	if (hub->scheduler == 1)
	{
		deadline_a = a->last_compile + hub->time_to_burnout;
		deadline_b = b->last_compile + hub->time_to_burnout;
		if (deadline_a != deadline_b)
			return (deadline_a < deadline_b);
		return (a->request_ticket < b->request_ticket);
	}
	return (a->request_ticket < b->request_ticket);
}

void	push_queue(t_hub *hub, t_coder *coder)
{
	coder->request_ticket = hub->ticket_counter++;
	hub->queue[hub->q_size] = coder;
	heapify_up(hub, hub->q_size);
	hub->q_size++;
}

void	heapify_up(t_hub *hub, int index)
{
	int		parent;
	t_coder	*temp;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (compare_coders(hub->queue[index], hub->queue[parent], hub) == 1)
		{
			temp = hub->queue[index];
			hub->queue[index] = hub->queue[parent];
			hub->queue[parent] = temp;
			index = parent;
		}
		else
			break ;
	}
}

t_coder	*pop_queue(t_hub *hub)
{
	t_coder	*popped;

	if (hub->q_size == 0)
		return (NULL);
	popped = hub->queue[0];
	hub->q_size--;
	if (hub->q_size > 0)
	{
		hub->queue[0] = hub->queue[hub->q_size];
		heapify_down(hub, 0);
	}
	return (popped);
}

void	heapify_down(t_hub *hub, int index)
{
	int		left;
	int		right;
	int		highest;
	t_coder	*temp;

	while ((2 * index + 1) < hub->q_size)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		highest = left;
		if (right < hub->q_size && compare_coders(
				hub->queue[right], hub->queue[left], hub) == 1)
			highest = right;
		if (compare_coders(hub->queue[highest], hub->queue[index], hub) == 1)
		{
			temp = hub->queue[index];
			hub->queue[index] = hub->queue[highest];
			hub->queue[highest] = temp;
			index = highest;
		}
		else
			break ;
	}
}
