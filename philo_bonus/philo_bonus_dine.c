/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus_dine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 01:15:33 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static int	_philo_eat(t_philo_arg *p, struct timeval *last_meal)
{
	int				result;
	const size_t	x = 1 + p->philosopher_number;

	result = 0;
	sem_wait(p->problem->sub_lock);
	sem_wait(p->problem->forks.lock);
	dpp_send_message(p->problem, x, "has taken a fork", 0);
	sem_wait(p->problem->forks.lock);
	sem_post(p->problem->sub_lock);
	sem_wait(p->problem->lock);
	dpp_send_message(p->problem, x, "is eating", 1);
	gettimeofday(last_meal, NULL);
	sem_post(p->problem->lock);
	if (dpp_delay(p->problem, p->problem->opt.time_to_eat) == 0)
		result = 1;
	sem_post(p->problem->forks.lock);
	sem_post(p->problem->forks.lock);
	return (result);
}

static void	*philo_inspector(void *arg)
{
	t_philo_arg *const	p = arg;
	const size_t		x = 1 + p->philosopher_number;
	const time_t		timeout = p->problem->opt.time_to_die;
	int					finished;
	int					died;

	while (1)
	{
		sem_wait(p->problem->lock);
		died = get_timestamp(&p->last_meal) > timeout;
		finished = p->problem->cancel;
		sem_post(p->problem->lock);
		if (finished)
			break ;
		if (died)
		{
			dpp_send_message(p->problem, x, "died", 0);
			exit(EXIT_FAILURE);
		}
		usleep(DPP_YIELD);
	}
	return (NULL);
}

int	philo_dine(void *arg)
{
	t_philo_arg *const	p = arg;
	const size_t		x = 1 + p->philosopher_number;
	pthread_t			inspector_id;

	p->last_meal = p->problem->begin;
	if (pthread_create(&inspector_id, NULL, philo_inspector, arg) != 0)
		return (EXIT_FAILURE);
	while (p->eat_counter-- > 0)
	{
		if (!_philo_eat(p, &p->last_meal))
			return (EXIT_FAILURE);
		dpp_send_message(p->problem, x, "is sleeping", 0);
		if (dpp_delay(p->problem, p->problem->opt.time_to_sleep))
			return (EXIT_FAILURE);
		dpp_send_message(p->problem, x, "is thinking", 0);
		usleep(DPP_YIELD);
	}
	sem_wait(p->problem->lock);
	p->problem->cancel = 1;
	sem_post(p->problem->lock);
	pthread_join(inspector_id, NULL);
	return (EXIT_SUCCESS);
}
