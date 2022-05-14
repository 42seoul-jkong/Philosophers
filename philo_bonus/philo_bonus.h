/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 22:11:46 by jkong             #+#    #+#             */
/*   Updated: 2022/05/14 14:32:41 by jkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# ifndef DPP_YIELD
#  define DPP_YIELD 500L
# endif

# ifndef DPP_MAIN_SEM_NAME
#  define DPP_MAIN_SEM_NAME "sem_dpp_main"
# endif

# ifndef DPP_SUB_SEM_NAME
#  define DPP_SUB_SEM_NAME "sem_dpp_sub"
# endif

# ifndef DPP_FORKS_SEM_NAME
#  define DPP_FORKS_SEM_NAME "sem_dpp_forks"
# endif

# include <fcntl.h>
# include <sys/stat.h>
# include <semaphore.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/time.h>
# include <pthread.h>
# include <memory.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_philo_option
{
	size_t	number_of_philosophers;
	time_t	time_to_die;
	time_t	time_to_eat;
	time_t	time_to_sleep;
	size_t	number_of_times_each_philosopher_must_eat;
}	t_philo_option;

typedef struct s_forks
{
	const char	*name;
	sem_t		*lock;
}	t_forks;

typedef struct s_problem
{
	int				exit;
	int				cancel;
	t_philo_option	opt;
	const char		*name;
	sem_t			*lock;
	const char		*sub_name;
	sem_t			*sub_lock;
	t_forks			forks;
	struct timeval	begin;
}	t_problem;

typedef struct s_philo_arg
{
	size_t			philosopher_number;
	size_t			eat_counter;
	t_problem		*problem;
	struct timeval	last_meal;
}	t_philo_arg;

typedef struct s_philo
{
	pid_t		process_id;
	t_philo_arg	process_arg;
}	t_philo;

int		philo_dine(void *arg);
int		philo_getopt(int argc, char *argv[], t_philo_option *out);

time_t	get_timestamp(struct timeval *tv_base);
void	dpp_send_message(t_problem *problem, size_t x, const char *str,
			int reentrant);
int		dpp_delay(t_problem *problem, time_t timespan);
void	dpp_wait(t_problem *problem, t_philo *philos);

int		dpp_sem_init(const char *name, int value, sem_t **sem_ptr);
int		dpp_sem_open(const char *name, sem_t **sem_ptr);
void	dpp_sem_close3(sem_t *s1, sem_t *s2, sem_t *s3);
void	dpp_sem_unlink3(const char *s1, const char *s2, const char *s3);

#endif
