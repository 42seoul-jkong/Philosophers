/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 13:26:09 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

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

typedef struct s_philo_arg
{
	size_t			philosopher_number;
	size_t			eat_counter;
	pthread_mutex_t	*lock;
	pthread_mutex_t	*fork[2];
}	t_philo_arg;

typedef struct s_philo
{
	pthread_t	thread_id;
	t_philo_arg	thread_arg;
	void		*thread_res;
}	t_philo;

int	philo_getopt(int argc, char *argv[], t_philo_option *out);

#endif
