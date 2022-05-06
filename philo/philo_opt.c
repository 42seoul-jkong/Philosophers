/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_opt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/07 03:41:04 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	_isspace(int c)
{
	return ((011 <= c && c <= 015) || c == ' ');
}

static int	_isdigit(int c)
{
	return ('0' <= c && c <= '9');
}

static long	_atol(const char *str)
{
	int		sign;
	long	number;

	while (_isspace(*str))
		str++;
	sign = 0;
	if (*str == '+' || *str == '-')
		sign = *str++ == '-';
	number = 0;
	while (*str && _isdigit(*str))
		number = 10L * number + (1 - (sign << 1)) * (*str++ - '0');
	return (number);
}

static unsigned long	_atoul(const char *str)
{
	unsigned long	number;

	while (_isspace(*str))
		str++;
	number = 0;
	while (*str && _isdigit(*str))
		number = 10UL * number + (*str++ - '0');
	return (number);
}

int	philo_getopt(int argc, char *argv[], t_philo_option *out)
{
	size_t	tmp;

	memset(out, 0, sizeof(t_philo_option));
	if (argc > 4)
	{
		out->number_of_philosophers = _atoul(argv[1]);
		out->time_to_die = _atol(argv[2]);
		out->time_to_eat = _atol(argv[3]);
		out->time_to_sleep = _atol(argv[4]);
		if (argc > 5)
			tmp = _atoul(argv[5]);
		else
			tmp = ~0UL;
		out->number_of_times_each_philosopher_must_eat = tmp;
		return (1);
	}
	return (0);
}
