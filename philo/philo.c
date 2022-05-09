/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/09 12:30:36 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	*philo_dine(void *arg)
{
	t_philo_arg *const	args = arg;
	const size_t		x = args->philosopher_number;
	const time_t		timeout = args->problem->opt.time_to_die;
	struct timeval		last_meal;
	time_t				delay;

	pthread_mutex_lock(&args->problem->lock);
	last_meal = args->problem->begin;
	pthread_mutex_unlock(&args->problem->lock);
	if (x & 1)
		usleep(DPP_YIELD_FIRST);
	while (args->eat_counter-- > 0)
	{
		if (dpp_fork_try_take(args->fork[0], &last_meal, timeout))
		{
			dpp_send_message(args->problem, x, "died");
			//...
			pthread_mutex_lock(&args->problem->lock);
			args->problem->cancel = 1;
			pthread_mutex_unlock(&args->problem->lock);
			break ;
		}
		dpp_send_message(args->problem, x, "has taken a fork");
		if (dpp_fork_try_take(args->fork[1], &last_meal, timeout))
		{
			dpp_send_message(args->problem, x, "died");
			//...
			pthread_mutex_lock(&args->problem->lock);
			args->problem->cancel = 1;
			pthread_mutex_unlock(&args->problem->lock);
			//...
			dpp_fork_put_down(args->fork[0]);
			break ;
		}
		dpp_send_message(args->problem, x, "is eating");
		gettimeofday(&last_meal, NULL);
		delay = args->problem->opt.time_to_eat;
		if (dpp_delay(args->problem, delay, &last_meal, timeout))
		{
			//...
			dpp_fork_put_down(args->fork[1]);
			dpp_fork_put_down(args->fork[0]);
			break ;
		}
		dpp_fork_put_down(args->fork[1]);
		dpp_fork_put_down(args->fork[0]);
		dpp_send_message(args->problem, x, "is sleeping");
		delay = args->problem->opt.time_to_sleep;
		if (dpp_delay(args->problem, delay, &last_meal, timeout))
			break ;
		dpp_send_message(args->problem, x, "is thinking");
		usleep(DPP_YIELD);
	}
	return (NULL);
}

static int	_proc_error(t_problem *problem, int s, int err, const char *msg)
{
	if (s != err)
	{
		problem->exit = EXIT_FAILURE;
		problem->cancel = 1;
		(void)msg;
		return (1);
	}
	return (0);
}

static int	_init(t_problem *problem, t_philo *philos, t_fork *forks)
{
	const size_t	n = problem->opt.number_of_philosophers;
	const size_t	m = problem->opt.number_of_times_each_philosopher_must_eat;
	size_t			i;
	int				s;

	s = pthread_mutex_init(&problem->lock, NULL);
	if (_proc_error(problem, s, 0, "pthread_mutex_init"))
		return (0);
	i = 0;
	while (i < n)
	{
		philos[i].thread_arg.philosopher_number = i;
		philos[i].thread_arg.eat_counter = m;
		philos[i].thread_arg.problem = problem;
		philos[i].thread_arg.fork[!!(i & 1)] = &forks[i];
		philos[i].thread_arg.fork[!(i & 1)] = &forks[(i + 1) % n];
		s = pthread_mutex_init(&forks[i].lock, NULL);
		if (_proc_error(problem, s, 0, "pthread_mutex_init"))
			return (0);
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
		if (_proc_error(problem, s, 0, "pthread_create"))
			break ;
		i++;
	}
	gettimeofday(&problem->begin, NULL);
	pthread_mutex_unlock(&problem->lock);
	i = n;
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
