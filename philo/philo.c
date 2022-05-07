/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 23:48:49 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	*philo_dine(void *arg)
{
	t_philo_arg *const	args = arg;
	const size_t		x = args->philosopher_number;
	time_t				timestamp;

	pthread_mutex_lock(&args->problem->lock);
	pthread_mutex_unlock(&args->problem->lock);
	while (args->eat_counter-- > 0)
	{
		timestamp = dpp_get_timestamp(&args->problem->begin);
		dpp_send_message(timestamp, x, "has taken a fork");
		pthread_mutex_lock(&args->fork[0]->lock);
		pthread_mutex_lock(&args->fork[1]->lock);
		timestamp = dpp_get_timestamp(&args->problem->begin);
		dpp_send_message(timestamp, x, "is eating");
		dpp_delay(args->problem->opt.time_to_eat, &args->problem->interrupted);
		timestamp = dpp_get_timestamp(&args->problem->begin);
		dpp_send_message(timestamp, x, "is sleeping");
		dpp_delay(args->problem->opt.time_to_sleep, &args->problem->interrupted);
		timestamp = dpp_get_timestamp(&args->problem->begin);
		dpp_send_message(timestamp, x, "is thinking");
		pthread_mutex_unlock(&args->fork[1]->lock);
		pthread_mutex_unlock(&args->fork[0]->lock);
		timestamp = dpp_get_timestamp(&args->problem->begin);
		dpp_send_message(timestamp, x, "died");
	}
	return (args);
}

static int	_on_error(t_problem *problem, int s, const char *msg)
{
	problem->exit = EXIT_FAILURE;
	problem->interrupted = 1;
	(void)s;
	(void)msg;
	return (0);
}

static int	_init(t_problem *problem, t_philo *philos, t_fork *forks)
{
	const size_t	n = problem->opt.number_of_philosophers;
	const size_t	m = problem->opt.number_of_times_each_philosopher_must_eat;
	size_t			i;
	int				s;

	s = pthread_mutex_init(&problem->lock, NULL);
	if (s != 0)
		return (_on_error(problem, s, "pthread_mutex_init"));
	i = 0;
	while (i < n)
	{
		philos[i].thread_arg.philosopher_number = i;
		philos[i].thread_arg.eat_counter = m;
		philos[i].thread_arg.problem = problem;
		philos[i].thread_arg.fork[!(i & 1)] = &forks[i];
		philos[i].thread_arg.fork[i & 1] = &forks[(i + 1) % n];
		s = pthread_mutex_init(&forks[i].lock, NULL);
		if (s != 0)
			return (_on_error(problem, s, "pthread_mutex_init"));
		i++;
	}
	return (1);
}

static void	_final(t_problem *problem, t_philo *philos, t_fork *forks)
{
	const size_t	n = problem->opt.number_of_philosophers;
	size_t			i;
	int				s;

	pthread_mutex_lock(&problem->lock);
	i = 0;
	while (i < n)
	{
		s = pthread_create(&philos[i].thread_id, NULL,
				philo_dine, &philos[i].thread_arg);
		if (s != 0)
		{
			_on_error(problem, s, "pthread_create");
			break ;
		}
		i++;
	}
	gettimeofday(&problem->begin, NULL);
	pthread_mutex_unlock(&problem->lock);
	while (i-- > 0)
		pthread_join(philos[i].thread_id, &philos[i].thread_res);
	i = 0;
	while (i < n)
		pthread_mutex_destroy(&forks[i++].lock);
	pthread_mutex_destroy(&problem->lock);
}

int	main(int argc, char *argv[])
{
	t_problem	problem;
	t_philo		*philos;
	t_fork		*forks;
	size_t		n;

	memset(&problem, 0, sizeof(problem));
	if (!philo_getopt(argc, argv, &problem.opt))
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat \
time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (1);
	}
	n = problem.opt.number_of_philosophers;
	philos = malloc(n * sizeof(*philos));
	forks = malloc(n * sizeof(*forks));
	if (philos && forks)
	{
		memset(philos, 0, n * sizeof(*philos));
		memset(forks, 0, n * sizeof(*forks));
		if (_init(&problem, philos, forks))
			_final(&problem, philos, forks);
	}
	free(philos);
	free(forks);
	return (problem.exit);
}
