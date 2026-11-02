/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 00:15:25 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/02 01:13:14 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codex.h"

int	parse_int(const char *s, int *out)
{
	long	sum;

	if (*s == '+')
		s++;
	if (*s < '0' || *s > '9')
		return (1);
	sum = 0;
	while (*s >= '0' && *s <= '9')
	{
		sum = (sum * 10) + (*s - '0');
		if (sum > INT_MAX)
			return (1);
		s++;
	}
	if (*s != '\0')
		return (1);
	*out = (int)sum;
	return (0);
}

static int	parse_scheduler(const char *s, int *out)
{
	if (strcmp(s, "fifo") == 0)
		*out = 0;
	else if (strcmp(s, "edf") == 0)
		*out = 1;
	else
		return (1);
	return (0);
}

int	parse_args(t_hub *hub, char **av)
{
	if (parse_int(av[1], &hub->n_coders) != 0 || hub->n_coders < 1)
		return (1);
	if (parse_int(av[2], &hub->time_to_burnout) != 0
		|| parse_int(av[3], &hub->time_to_compile) != 0
		|| parse_int(av[4], &hub->time_to_debug) != 0
		|| parse_int(av[5], &hub->time_to_refactor) != 0
		|| parse_int(av[6], &hub->n_compiles) != 0
		|| parse_int(av[7], &hub->dongle_cooldown) != 0)
		return (1);
	return (parse_scheduler(av[8], &hub->scheduler));
}
