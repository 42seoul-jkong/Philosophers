/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/12 23:09:12 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# ifndef DPP_YIELD
#  define DPP_YIELD 500L
# endif

# include <sys/time.h>
# include <pthread.h>
# include <memory.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_philo_option
{
	size_t	number_of_philosophers;
	time_t	time_to_die;
	time_t	time_to_eat;
	time_t	time_to_sleep;
	size_t	number_of_times_each_philosopher_must_eat;
}	t_philo_option;

typedef struct s_problem
{
	int				exit;
	int				cancel;
	t_philo_option	opt;
	pthread_mutex_t	lock;
	struct timeval	begin;
}	t_problem;

typedef struct s_fork
{
	pthread_mutex_t	lock;
	size_t			taken;
}	t_fork;

typedef struct s_philo_arg
{
	size_t		philosopher_number;
	size_t		eat_counter;
	t_problem	*problem;
	t_fork		*fork[2];
}	t_philo_arg;

typedef struct s_philo
{
	pthread_t	thread_id;
	t_philo_arg	thread_arg;
	void		*thread_res;
}	t_philo;

void	*philo_dine(void *arg);
int		philo_getopt(int argc, char *argv[], t_philo_option *out);

int		dpp_fork_try_take(t_fork *fork, size_t x,
			struct timeval *time, time_t timeout);
void	dpp_fork_put_down(t_fork *fork, size_t x);
void	dpp_send_message(t_problem *problem, size_t x, const char *str,
			int reentrant);
int		dpp_delay(t_problem *problem, time_t timespan,
			struct timeval *time, time_t timeout);

#endif
