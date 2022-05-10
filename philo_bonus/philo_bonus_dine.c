/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus_dine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/10 02:04:19 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static void	_philo_on_dead(t_philo_arg *p)
{
	const size_t	x = 1 + p->philosopher_number;

	dpp_send_message(p->problem, x, "died");
	pthread_mutex_lock(&p->problem->lock);
	p->problem->cancel = 1;
	pthread_mutex_unlock(&p->problem->lock);
}

static int	_philo_eat(t_philo_arg *p, struct timeval *last_meal)
{
	int				result;
	const size_t	x = 1 + p->philosopher_number;
	const time_t	timeout = p->problem->opt.time_to_die;
	time_t			delay;

	result = 0;
	if (dpp_fork_try_take(p->fork[0], x, last_meal, timeout) == 0)
	{
		dpp_send_message(p->problem, x, "has taken a fork");
		if (dpp_fork_try_take(p->fork[1], x, last_meal, timeout) == 0)
		{
			dpp_send_message(p->problem, x, "is eating");
			gettimeofday(last_meal, NULL);
			delay = p->problem->opt.time_to_eat;
			if (dpp_delay(p->problem, delay, last_meal, timeout) == 0)
				result = 1;
			dpp_fork_put_down(p->fork[1], x);
		}
		else
			_philo_on_dead(p);
		dpp_fork_put_down(p->fork[0], x);
	}
	else
		_philo_on_dead(p);
	return (result);
}

void	*philo_dine(void *arg)
{
	t_philo_arg *const	p = arg;
	const size_t		x = 1 + p->philosopher_number;
	const time_t		timeout = p->problem->opt.time_to_die;
	struct timeval		last_meal;
	time_t				delay;

	pthread_mutex_lock(&p->problem->lock);
	last_meal = p->problem->begin;
	pthread_mutex_unlock(&p->problem->lock);
	if (p->philosopher_number & 1)
		usleep(DPP_YIELD);
	while (p->eat_counter-- > 0)
	{
		if (!_philo_eat(p, &last_meal))
			break ;
		dpp_send_message(p->problem, x, "is sleeping");
		delay = p->problem->opt.time_to_sleep;
		if (dpp_delay(p->problem, delay, &last_meal, timeout))
			break ;
		dpp_send_message(p->problem, x, "is thinking");
		usleep(DPP_YIELD);
	}
	return (NULL);
}
