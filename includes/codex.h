/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:48:25 by bmoura-s          #+#    #+#             */
/*   Updated: 2026/11/05 12:27:46 by bmoura-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <string.h>
# include <limits.h>
# include <sys/time.h>

typedef struct s_coder	t_coder;
typedef struct s_hub	t_hub;
typedef struct s_queue	t_queue;

struct s_hub
{
	int				n_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				n_compiles;
	int				dongle_cooldown;
	int				scheduler;
	long			start_time;

	t_coder			*coders;
	t_queue			*queues;
	t_coder			**slots;
	long			*dongle_ready;
	pthread_mutex_t	*dongle_locks;

	pthread_t		monitor_id;
	int				ticket_counter;
	int				sim_running;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	queue_lock;
	pthread_mutex_t	sim_lock;
	pthread_cond_t	arbitrator_cond;
};

struct s_queue
{
	t_coder			**items;
	int				size;
};

struct s_coder
{
	int				coder_id;
	int				l_dongle;
	int				r_dongle;
	t_hub			*hub;

	pthread_t		thread_id;
	long			last_compile;
	int				compiles;
	int				request_ticket;
};

int		parse_int(const char *s, int *out);
int		parse_args(t_hub *hub, char **av);
int		compare_coders(t_coder *a, t_coder *b);
void	heap_push(t_queue *q, t_coder *coder);
void	heap_remove(t_queue *q, t_coder *coder);
void	heapify_up(t_queue *q, int index);
void	heapify_down(t_queue *q, int index);
void	init_queues(t_hub *hub);
void	push_queue(t_hub *hub, t_coder *coder);
void	remove_queue(t_hub *hub, t_coder *coder);
long	get_time_ms(void);
void	ft_usleep(long time_to_sleep, t_hub *hub);
int		check_sim_running(t_hub *hub);
void	*coder_routine(void *arg);
int		start_sim(t_hub *hub, t_coder *coders);
void	cleanup_sim(t_hub *hub, t_coder *coders);
void	*lone_coder(void *arg);
void	print_log(t_coder *coder, char *status);
void	drop_dongles(t_coder *coder);
void	do_compile(t_coder *coder);
int		can_take_dongles(t_coder *coder);
long	next_ready_time(t_coder *coder);
void	wait_turn(t_coder *coder);
int		take_dongles(t_coder *coder);
void	*monitor_routine(void *arg);
void	stop_sim(t_hub *hub, int id);
int		rollback_coders(t_hub *hub, t_coder *coders, int i);

#endif
