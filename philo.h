/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 14:30:24 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/19 16:31:14 by falhaimo         ###   ########.fr       */
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
	int				meals_required;
	int				stop_simulation;
	long			start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	stop_mutex;
	t_philo			*philos;
};

int		get_stop_simulation(t_data *data);
void	set_stop_simulation(t_data *data, int value);
long	get_time_in_ms(void);
void	smart_sleep(long duration_ms, t_data *data);
void	log_status(t_data *data, int id, const char *status);
int		ft_atoi(char *nptr);
int		check_valid(t_data *data);
int		parse_args(int argc, char **argv, t_data *data);
void	cleanup(t_data *data);

#endif
