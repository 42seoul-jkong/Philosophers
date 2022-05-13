/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 19:31:09 by jkong             #+#    #+#             */
/*   Updated: 2022/05/13 16:17:00 by jkong            ###   ########.fr       */
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
	return (dsec * 1000L + dusec / 1000L);
}

int	dpp_fork_try_take(t_fork *fork, size_t x,
		struct timeval *time, time_t timeout)
{
	int	acquire;

	acquire = 0;
	while (!(_get_timestamp(time) > timeout))
	{
		pthread_mutex_lock(&fork->lock);
		if (!fork->taken)
		{
			fork->taken = x;
			acquire = 1;
		}
		pthread_mutex_unlock(&fork->lock);
		if (acquire)
			return (0);
		if (usleep(DPP_YIELD / 5) != 0)
			return (-1);
	}
	return (1);
}

void	dpp_fork_put_down(t_fork *fork, size_t x)
{
	pthread_mutex_lock(&fork->lock);
	if (fork->taken == x)
		fork->taken = 0;
	pthread_mutex_unlock(&fork->lock);
	usleep(DPP_YIELD);
}

void	dpp_send_message(t_problem *problem, size_t x, const char *str,
			int reentrant)
{
	time_t			timestamp_in_mili;

	if (!reentrant)
		pthread_mutex_lock(&problem->lock);
	if (!problem->cancel)
	{
		timestamp_in_mili = _get_timestamp(&problem->begin);
		printf("%06ld %03lu %s\n", timestamp_in_mili, x, str);
	}
	if (!reentrant)
		pthread_mutex_unlock(&problem->lock);
}

int	dpp_delay(t_problem *problem, time_t timespan,
		struct timeval *time, time_t timeout)
{
	int *const		token = &problem->cancel;
	struct timeval	tv;
	int				interrupted;

	gettimeofday(&tv, NULL);
	while (_get_timestamp(&tv) < timespan && !(_get_timestamp(time) > timeout))
	{
		pthread_mutex_lock(&problem->lock);
		interrupted = *token;
		pthread_mutex_unlock(&problem->lock);
		if (interrupted)
			return (1);
		if (usleep(DPP_YIELD) != 0)
			return (-1);
	}
	return (0);
}
