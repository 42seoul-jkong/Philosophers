/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 23:43:18 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# ifndef DPP_DELAY_LIMIT
#  define DPP_DELAY_LIMIT 10000L
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
	int				interrupted;
	t_philo_option	opt;
	pthread_mutex_t	lock;
	struct timeval	begin;
}	t_problem;

typedef struct s_fork
{
	pthread_mutex_t	lock;
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

int		philo_getopt(int argc, char *argv[], t_philo_option *out);

void	dpp_send_message(time_t timestamp_in_mili, size_t x, const char *str);
time_t	dpp_get_timestamp(struct timeval *tv_base);
int		dpp_delay(time_t time, int *token);

#endif
