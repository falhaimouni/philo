/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: farah <farah@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 14:30:24 by falhaimo          #+#    #+#             */
/*   Updated: 2025/01/08 19:03:12 by farah            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

typedef struct s_data	t_data;

typedef struct s_philo
{
	long	last_meal;
	int		id;
	int		meals_eaten;
	pthread_t		thread;
	t_data		*data;
}	t_philo;

struct s_data
{
	int	num_philos;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	int	meals_required;
	int	stop_simulation;
	long	start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	t_philo	*philos;
};

#endif
