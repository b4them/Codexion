/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 14:45:19 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:33 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	compare_coders(t_coder *a, t_coder *b)
{
	long	deadline_a;
	long	deadline_b;

	if (a->hub->scheduler == 1)
	{
		deadline_a = a->last_compile + a->hub->time_to_burnout;
		deadline_b = b->last_compile + b->hub->time_to_burnout;
		if (deadline_a != deadline_b)
			return (deadline_a < deadline_b);
		return (a->request_ticket < b->request_ticket);
	}
	return (a->request_ticket < b->request_ticket);
}

void	heap_push(t_queue *q, t_coder *coder)
{
	q->items[q->size] = coder;
	heapify_up(q, q->size);
	q->size++;
}

void	heapify_up(t_queue *q, int index)
{
	int		parent;
	t_coder	*temp;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (compare_coders(q->items[index], q->items[parent]) == 1)
		{
			temp = q->items[index];
			q->items[index] = q->items[parent];
			q->items[parent] = temp;
			index = parent;
		}
		else
			break ;
	}
}

void	heap_remove(t_queue *q, t_coder *coder)
{
	int	i;

	i = -1;
	while (++i < q->size)
	{
		if (q->items[i] == coder)
		{
			q->size--;
			q->items[i] = q->items[q->size];
			if (i < q->size)
			{
				heapify_up(q, i);
				heapify_down(q, i);
			}
			return ;
		}
	}
}

void	heapify_down(t_queue *q, int index)
{
	int		left;
	int		right;
	int		highest;
	t_coder	*temp;

	while ((2 * index + 1) < q->size)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		highest = left;
		if (right < q->size
			&& compare_coders(q->items[right], q->items[left]) == 1)
			highest = right;
		if (compare_coders(q->items[highest], q->items[index]) == 1)
		{
			temp = q->items[index];
			q->items[index] = q->items[highest];
			q->items[highest] = temp;
			index = highest;
		}
		else
			break ;
	}
}
