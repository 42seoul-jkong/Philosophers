/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 19:31:09 by jkong             #+#    #+#             */
/*   Updated: 2022/05/09 02:52:44 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static time_t	_get_timestamp(struct timeval *tv_base)
{
	struct timeval	tv;
	time_t			dsec;
	suseconds_t		dusec;

	gettimeofday(&tv, NULL);
	dsec = tv.tv_sec - tv_base->tv_sec;
	dusec = tv.tv_usec - tv_base->tv_usec;
	return (dsec * 1000 + dusec / 1000);
}

int	dpp_fork_try_take(t_fork *fork, struct timeval *time, time_t timeout)
{
	int	acquire;

	acquire = 0;
	while (!(_get_timestamp(time) > timeout))
	{
		pthread_mutex_lock(&fork->lock);
		if (!fork->taken)
		{
			fork->taken++;
			acquire = 1;
		}
		pthread_mutex_unlock(&fork->lock);
		if (acquire)
			return (0);
		else if (usleep(DPP_MEAL_LIMIT / DPP_DIVIDER) != 0)
			return (-1);
	}
	return (1);
}

void	dpp_fork_put_down(t_fork *fork)
{
	pthread_mutex_lock(&fork->lock);
	fork->taken--;
	pthread_mutex_unlock(&fork->lock);
}

void	dpp_send_message(t_problem *problem, size_t i, const char *str)
{
	const size_t	x = i + 1;
	time_t			timestamp_in_mili;

//	pthread_mutex_lock(&problem->lock);
	timestamp_in_mili = _get_timestamp(&problem->begin);
	printf("%06ld %03lu %s\n", timestamp_in_mili, x, str);
//	pthread_mutex_unlock(&problem->lock);
}

int	dpp_delay(t_problem *problem, time_t time)
{
	int *const		token = &problem->cancel;
	struct timeval	tv;
	int				interrupted;

	gettimeofday(&tv, NULL);
	while (_get_timestamp(&tv) < time)
	{
		pthread_mutex_lock(&problem->lock);
		interrupted = *token;
		pthread_mutex_unlock(&problem->lock);
		if (interrupted)
			return (1);
		if (usleep(DPP_MEAL_LIMIT / DPP_DIVIDER) != 0)
			return (-1);
	}
	return (0);
}
