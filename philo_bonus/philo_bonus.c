/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 01:14:45 by jkong            ###   ########.fr       */
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
	sem_t			*sem;

	problem->name = DPP_MAIN_SEM_NAME;
	sem = sem_open(problem->name, O_CREAT, S_IRWXU, 1);
	if (sem == SEM_FAILED)
		return (0);
	problem->lock = sem;
	problem->sub_name = DPP_SUB_SEM_NAME;
	sem = sem_open(problem->sub_name, O_CREAT, S_IRWXU, 1);
	if (sem == SEM_FAILED)
		return (0);
	problem->sub_lock = sem;
	problem->forks.name = DPP_FORKS_SEM_NAME;
	sem = sem_open(problem->forks.name, O_CREAT, S_IRWXU, n);
	if (sem == SEM_FAILED)
		return (0);
	problem->forks.lock = sem;
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
	sem_t	*sem;

	problem->lock = NULL;
	problem->sub_lock = NULL;
	problem->forks.lock = NULL;
	sem = sem_open(problem->name, O_RDWR);
	if (_proc_error(problem, sem == SEM_FAILED, 0, "sem_open"))
	{
		*process_res = EXIT_FAILURE;
		return (0);
	}
	problem->lock = sem;
	sem = sem_open(problem->sub_name, O_RDWR);
	if (_proc_error(problem, sem == SEM_FAILED, 0, "sem_open"))
	{
		*process_res = EXIT_FAILURE;
		return (0);
	}
	problem->sub_lock = sem;
	sem = sem_open(problem->forks.name, O_RDWR);
	if (_proc_error(problem, sem == SEM_FAILED, 0, "sem_open"))
	{
		*process_res = EXIT_FAILURE;
		return (0);
	}
	problem->forks.lock = sem;
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
		if (pid == 0)
		{
			if (_init_child(problem, &philos[i].process_res))
				philos[i].process_res = philo_dine(&philos[i].process_arg);
			sem_close(problem->lock);
			sem_close(problem->sub_lock);
			sem_close(problem->forks.lock);
			exit(philos[i].process_res);
		}
		philos[i].process_id = pid;
		i++;
	}
	i = n;
	while (i-- > 0)
	{
		pid = waitpid(-1, &philos[i].process_res, 0);
		if (WEXITSTATUS(philos[i].process_res) == EXIT_FAILURE)
		{
			i = 0;
			while (i < n)
				kill(philos[i++].process_id, SIGTERM);
			return ;
		}
	}
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
	sem_unlink(DPP_MAIN_SEM_NAME);
	sem_unlink(DPP_SUB_SEM_NAME);
	sem_unlink(DPP_FORKS_SEM_NAME);
	philos = malloc(n * sizeof(*philos));
	if (philos)
	{
		memset(philos, 0, n * sizeof(*philos));
		if (_init(&problem, philos))
			_final(&problem, philos);
	}
	free(philos);
	sem_unlink(DPP_MAIN_SEM_NAME);
	sem_unlink(DPP_SUB_SEM_NAME);
	sem_unlink(DPP_FORKS_SEM_NAME);
	return (problem.exit);
}
