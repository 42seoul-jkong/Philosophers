/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 19:31:09 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 14:33:50 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

time_t	get_timestamp(struct timeval *tv_base)
{
	struct timeval	tv;
	time_t			dsec;
	suseconds_t		dusec;

	gettimeofday(&tv, NULL);
	dsec = tv.tv_sec - tv_base->tv_sec;
	dusec = tv.tv_usec - tv_base->tv_usec;
	return (dsec * 1000L + dusec / 1000L);
}

void	dpp_send_message(t_problem *problem, size_t x, const char *str,
			int reentrant)
{
	time_t			timestamp_in_mili;

	if (!reentrant)
		sem_wait(problem->lock);
	timestamp_in_mili = get_timestamp(&problem->begin);
	printf("%06ld %03lu %s\n", timestamp_in_mili, x, str);
	if (!reentrant)
		sem_post(problem->lock);
}

int	dpp_delay(t_problem *problem, time_t timespan)
{
	struct timeval	tv;

	(void)problem;
	gettimeofday(&tv, NULL);
	while (get_timestamp(&tv) < timespan)
	{
		if (usleep(DPP_YIELD) != 0)
			return (-1);
	}
	return (0);
}

void	dpp_wait(t_problem *problem, t_philo *philos)
{
	const size_t	n = problem->opt.number_of_philosophers;
	size_t			i;
	pid_t			pid;
	int				process_res;

	if (!problem->cancel)
		problem->exit = EXIT_SUCCESS;
	problem->cancel = 0;
	i = n;
	while (i-- > 0)
	{
		pid = waitpid(-1, &process_res, 0);
		(void)pid;
		if (WEXITSTATUS(process_res) == EXIT_FAILURE)
		{
			if (!problem->cancel)
			{
				i = 0;
				while (i < n)
					kill(philos[i++].process_id, SIGTERM);
				problem->exit = EXIT_FAILURE;
				problem->cancel = 1;
			}
		}
	}
}
