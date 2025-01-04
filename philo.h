/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 14:30:24 by falhaimo          #+#    #+#             */
/*   Updated: 2025/01/04 13:49:27 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <sys/time.h>
# include <pthread.h>
# include <stdlib.h>
# include <stddef.h>

typedef struct s_philo
{
	int	id;
	int	meal_eaten;
	long	last_meal_time;
	pthread_t		thread;
	struct s_data		*data;
}	t_philo;

typedef struct s_data
{
	int	num_philo;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	num_meals;
	int	stop;
	long	start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	stop_mutex;
	t_philo	*philos; 
}	t_data;

int		ft_atoi(char *nptr);
long		get_time();
void	sleep_time(long ms);
int		parse_args(int argc, char **argv, t_data *data);
void	init_data(t_data *data);
void	*routine(void *arg);
void	*monitor(void *arg);
void	create_thraeds(t_data *data);

#endif
