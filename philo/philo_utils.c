/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 19:31:09 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 23:41:33 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	dpp_send_message(time_t timestamp_in_mili, size_t x, const char *str)
{
	printf("%ld %lu %s\n", timestamp_in_mili, x, str);
}

time_t	dpp_get_timestamp(struct timeval *tv_base)
{
	struct timeval	tv;
	time_t			dsec;
	suseconds_t		dusec;

	gettimeofday(&tv, NULL);
	dsec = tv.tv_sec - tv_base->tv_sec;
	dusec = tv.tv_usec - tv_base->tv_usec;
	return (dsec * 1000 + dusec / 1000);
}

int	dpp_delay(time_t time, int *token)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	while (dpp_get_timestamp(&tv) < time)
	{
		if (token && *token)
			return (1);
		if (usleep(DPP_DELAY_LIMIT / 2) != 0)
			return (-1);
	}
	return (0);
}
