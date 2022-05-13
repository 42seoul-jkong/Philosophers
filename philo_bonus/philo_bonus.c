/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 02:08:35 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

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

static int	_init(t_problem *problem, t_philo *philos)
{
	const size_t	n = problem->opt.number_of_philosophers;
	const size_t	m = problem->opt.number_of_times_each_philosopher_must_eat;
	size_t			i;

	problem->name = DPP_MAIN_SEM_NAME;
	if (!dpp_sem_init(problem->name, 1, &problem->lock))
		return (0);
	problem->sub_name = DPP_SUB_SEM_NAME;
	if (!dpp_sem_init(problem->sub_name, 1, &problem->sub_lock))
		return (0);
	problem->forks.name = DPP_FORKS_SEM_NAME;
	if (!dpp_sem_init(problem->forks.name, n, &problem->forks.lock))
		return (0);
	i = 0;
	while (i < n)
	{
		philos[i].process_arg.philosopher_number = i;
		philos[i].process_arg.eat_counter = m;
		philos[i].process_arg.problem = problem;
		i++;
	}
	return (1);
}

static int	_init_child(t_problem *problem, int *process_res)
{
	int	s;

	problem->lock = NULL;
	problem->sub_lock = NULL;
	problem->forks.lock = NULL;
	*process_res = EXIT_FAILURE;
	s = dpp_sem_open(problem->name, &problem->lock);
	if (_proc_error(problem, s, 1, "sem_open"))
		return (0);
	s = dpp_sem_open(problem->sub_name, &problem->sub_lock);
	if (_proc_error(problem, s, 1, "sem_open"))
		return (0);
	s = dpp_sem_open(problem->forks.name, &problem->forks.lock);
	if (_proc_error(problem, s, 1, "sem_open"))
		return (0);
	*process_res = EXIT_SUCCESS;
	return (1);
}

static void	_final(t_problem *problem, t_philo *philos)
{
	const size_t	n = problem->opt.number_of_philosophers;
	size_t			i;
	pid_t			pid;

	gettimeofday(&problem->begin, NULL);
	i = 0;
	while (i < n)
	{
		pid = fork();
		if (_proc_error(problem, pid == -1, 0, "fork"))
			break ;
		if (pid)
		{
			philos[i++].process_id = pid;
			continue ;
		}
		if (_init_child(problem, &philos[i].process_res))
			philos[i].process_res = philo_dine(&philos[i].process_arg);
		dpp_sem_close3(problem->lock, problem->sub_lock, problem->forks.lock);
		exit(philos[i].process_res);
	}
	dpp_wait(philos, n);
}

int	main(int argc, char *argv[])
{
	t_problem	problem;
	t_philo		*philos;
	size_t		n;

	memset(&problem, 0, sizeof(problem));
	if (!philo_getopt(argc, argv, &problem.opt))
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat \
time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (EXIT_FAILURE);
	}
	n = problem.opt.number_of_philosophers;
	dpp_sem_unlink3(DPP_MAIN_SEM_NAME, DPP_SUB_SEM_NAME, DPP_FORKS_SEM_NAME);
	philos = malloc(n * sizeof(*philos));
	if (philos)
	{
		memset(philos, 0, n * sizeof(*philos));
		if (_init(&problem, philos))
			_final(&problem, philos);
	}
	free(philos);
	dpp_sem_unlink3(DPP_MAIN_SEM_NAME, DPP_SUB_SEM_NAME, DPP_FORKS_SEM_NAME);
	return (problem.exit);
}
