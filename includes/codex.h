/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:48:25 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/09 14:55:02 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

typedef struct s_hub
{
	int				n_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				n_compiles;
	int				dongle_cooldown;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	*dongle_locks;
	int				scheduler;
	struct s_coder	**coders;
}	t_hub;

typedef struct s_coder
{
	int				coder_id;
	int				last_compile;
	int				compiles;
	struct s_hub	*hub;
}	t_coder;

int	ft_satoi(const char *nptr);

#endif
