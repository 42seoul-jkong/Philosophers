/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 03:56:18 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	*philo_start(void *arg)
{
	t_philo_arg *const	args = arg;

	for (int i = 0; i < 500; i++)
	{
		pthread_mutex_lock(&args->fork[0]);
		pthread_mutex_lock(&args->fork[1]);
		struct timeval tv;
		gettimeofday(&tv, NULL);
		printf("%ld %zu : %d\n", tv.tv_usec, args->philosopher_number, i);
		usleep(3500);
		pthread_mutex_unlock(&args->fork[1]);
		pthread_mutex_unlock(&args->fork[0]);
	}
	return (args);
}

int	main(int argc, char *argv[])
{
	t_philo_option	opt;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	lock;
	size_t			i;

	if (!philo_getopt(argc, argv, &opt))
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat \
time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (1);
	}
	philos = malloc(opt.number_of_philosophers * sizeof(t_philo));
	forks = malloc(opt.number_of_philosophers * sizeof(pthread_mutex_t));
	if (philos && forks)
	{
		i = 0;
		while (i < opt.number_of_philosophers)
			pthread_mutex_init(&forks[i++], NULL);
		{
			i = 0;
			while (i < opt.number_of_philosophers)
			{
				philos[i].thread_arg.philosopher_number = i;
				philos[i].thread_arg.eat_count = 0;
				philos[i].thread_arg.lock = &lock;
				philos[i].thread_arg.fork[0] = forks[i];
				philos[i].thread_arg.fork[1] = forks[(i + 1) % opt.number_of_philosophers];
				i++;
			}
			{
				i = 0;
				while (i < opt.number_of_philosophers)
				{
					pthread_create(&philos[i].thread_id, NULL, philo_start, &philos[i].thread_arg);
					i++;
				}
				while (i-- > 0)
					pthread_join(philos[i].thread_id, &philos[i].thread_res);
			}
		}
		i = opt.number_of_philosophers;
		while (i-- > 0)
			pthread_mutex_destroy(&forks[i]);
	}
	free(philos);
	free(forks);
	return (0);
}
