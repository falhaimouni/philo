/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 14:30:24 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/22 11:58:38 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

typedef struct s_data	t_data;

typedef struct s_philo
{
	long			last_meal;
	int				id;
	int				meals_eaten;
	t_data			*data;
	pthread_t		t_id;
	pthread_mutex_t	mutex;
}	t_philo;

struct s_data
{
	int				flag;
	int				num_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meals;
	int				stop_simulation;
	long			start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	stop_mutex;
	t_philo			*philos;
};

long	get_time_in_ms(void);
int		ft_atoi(char *nptr);
int		init_mutex(t_data *data);
int		init_philo(t_data *data);
int		check_valid(t_data *data);
int		init_forks_mutex(t_data *data);
int		init_other_mutexes(t_data *data);
int		get_stop_simulation(t_data *data);
int		init_philo_mutexes(t_data *data, int i);
int		handle_eating(t_philo *philo, int left, int right);
int		parse_args(int argc, char **argv, t_data *data);
int		take_forks_even(t_philo *philo, int left, int right);
int		take_forks_odd(t_philo *philo, int left, int right);
void	smart_sleep(long duration_ms, t_data *data);
void	log_status(t_data *data, int id, const char *status);
void	cleanup(t_data *data);
void	*philosopher_routine(void *arg);
void	set_stop_simulation(t_data *data, int value);
void	handle_single_philosopher(t_philo *philo, int left);
void	philo_lifecycle(t_philo *philo, int left, int right);

#endif
