/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus_semaphore.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 01:32:31 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 02:01:30 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

int	dpp_sem_init(const char *name, int value, sem_t **sem_ptr)
{
	sem_t	*sem;

	sem = sem_open(name, O_CREAT, S_IRWXU, value);
	if (sem == SEM_FAILED)
		return (0);
	*sem_ptr = sem;
	return (1);
}

int	dpp_sem_open(const char *name, sem_t **sem_ptr)
{
	sem_t	*sem;

	sem = sem_open(name, O_RDWR);
	if (sem == SEM_FAILED)
		return (0);
	*sem_ptr = sem;
	return (1);
}

void	dpp_sem_close3(sem_t *s1, sem_t *s2, sem_t *s3)
{
	sem_close(s1);
	sem_close(s2);
	sem_close(s3);
}

void	dpp_sem_unlink3(const char *s1, const char *s2, const char *s3)
{
	sem_unlink(s1);
	sem_unlink(s2);
	sem_unlink(s3);
}
