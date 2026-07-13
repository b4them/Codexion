/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:48:25 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/07/13 16:32:39 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>

typedef struct s_coder	t_coder;
typedef struct s_hub	t_hub;

struct s_hub
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
	t_coder			*coders;
	t_coder			**queue;
	int				q_size;
	pthread_mutex_t	queue_lock;
	int				ticket_counter;
};

struct s_coder
{
	int				coder_id;
	int				last_compile;
	int				compiles;
	int				l_dongle;
	int				r_dongle;
	t_hub			*hub;
	int				request_ticket;
};

int		ft_satoi(const char *nptr);

int		compare_coders(t_coder *a, t_coder *b, t_hub *hub);
void	push_queue(t_hub *hub, t_coder *coder);
void	heapify_up(t_hub *hub, int index);
t_coder	*pop_queue(t_hub *hub);
void	heapify_down(t_hub *hub, int index);
long	get_time_ms(void);
void	ft_usleep(long time_to_sleep);

#endif
