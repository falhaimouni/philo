/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:52:12 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/20 11:38:45 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	int		left;
	int		right;

	philo = (t_philo *)arg;
	data = philo->data;
	left = philo->id;
	right = (philo->id + 1) % data->num_philos;
	if (data->num_philos == 1)
	{
		pthread_mutex_lock(&data->forks[left]);
		log_status(data, philo->id + 1, "has taken a fork");
		while (!get_stop_simulation(data))
			usleep(50);
		pthread_mutex_unlock(&data->forks[left]);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		usleep(2000);
	while (!get_stop_simulation(data))
	{
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(&data->forks[left]);
			log_status(data, philo->id + 1, "has taken a fork");
			if (get_stop_simulation(data))
			{
				pthread_mutex_unlock(&data->forks[left]);
				break ;
			}
			pthread_mutex_lock(&data->forks[right]);
			log_status(data, philo->id + 1, "has taken a fork");
		}
		else
		{
			pthread_mutex_lock(&data->forks[right]);
			log_status(data, philo->id + 1, "has taken a fork");
			if (get_stop_simulation(data))
			{
				pthread_mutex_unlock(&data->forks[right]);
				break ;
			}
			pthread_mutex_lock(&data->forks[left]);
			log_status(data, philo->id + 1, "has taken a fork");
		}
		if (get_stop_simulation(data))
		{
			pthread_mutex_unlock(&data->forks[left]);
			pthread_mutex_unlock(&data->forks[right]);
			break ;
		}
		pthread_mutex_lock(&philo->mutex);
		philo->last_meal = get_time_in_ms();
		pthread_mutex_unlock(&philo->mutex);
		log_status(data, philo->id + 1, "is eating");
		smart_sleep(data->time_to_eat, data);
		pthread_mutex_lock(&philo->mutex);
		philo->meals_eaten++;
		if (data->meals_required != -1
			&& philo->meals_eaten >= data->meals_required)
		{
			pthread_mutex_unlock(&philo->mutex);
			pthread_mutex_unlock(&data->forks[left]);
			pthread_mutex_unlock(&data->forks[right]);
			break ;
		}
		pthread_mutex_unlock(&philo->mutex);
		pthread_mutex_unlock(&data->forks[left]);
		pthread_mutex_unlock(&data->forks[right]);
		if (get_stop_simulation(data))
			break ;
		log_status(data, philo->id + 1, "is sleeping");
		smart_sleep(data->time_to_sleep, data);
		if (get_stop_simulation(data))
			break ;
		log_status(data, philo->id + 1, "is thinking");
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	long	last_meal;
	int		i;
	int		all_done;
	int		meals;

	data = (t_data *)arg;
	while (!get_stop_simulation(data))
	{
		all_done = 1;
		i = 0;
		while (i < data->num_philos)
		{
			pthread_mutex_lock(&data->philos[i].mutex);
			last_meal = data->philos[i].last_meal;
			meals = data->philos[i].meals_eaten;
			pthread_mutex_unlock(&data->philos[i].mutex);
			if (data->meals_required == -1 || meals < data->meals_required)
			{
				if (get_time_in_ms() - last_meal > data->time_to_die)
				{
					log_status(data, data->philos[i].id + 1, "died");
					set_stop_simulation(data, 1);
					return (NULL);
				}
			}
			if (data->meals_required != -1 && meals < data->meals_required)
				all_done = 0;
			i++;
		}
		if (data->meals_required != -1 && all_done)
		{
			set_stop_simulation(data, 1);
			break ;
		}
		usleep(10);
	}
	return (NULL);
}

int	init_mutex(t_data *data)
{
	int	i;

	i = 0;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
		return (1);
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			free(data->forks);
			return (1);
		}
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		free(data->forks);
		return (1);
	}
	if (pthread_mutex_init(&data->stop_mutex, NULL) != 0)
	{
		free(data->forks);
		pthread_mutex_destroy(&data->print_mutex);
		return (1);
	}
	return (0);
}

int	init_philo(t_data *data)
{
	int	i;

	i = 0;
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		free(data->forks);
		pthread_mutex_destroy(&data->print_mutex);
		pthread_mutex_destroy(&data->stop_mutex);
		return (1);
	}
	i = 0;
	while (i < data->num_philos)
	{
		data->philos[i].id = i;
		data->philos[i].meals_eaten = 0;
		data->philos[i].last_meal = data->start_time;
		data->philos[i].data = data;
		if (pthread_mutex_init(&data->philos[i].mutex, NULL) != 0)
		{
			while (i > 0)
			{
				i--;
				pthread_mutex_destroy(&data->philos[i].mutex);
			}
			free(data->philos);
			free(data->forks);
			pthread_mutex_destroy(&data->print_mutex);
			pthread_mutex_destroy(&data->stop_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

int	create_threads(t_data *data, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&data->philos[i].t_id, NULL,
				philosopher_routine, &data->philos[i]) != 0)
		{
			set_stop_simulation(data, 1);
			return (1);
		}
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, data) != 0)
	{
		set_stop_simulation(data, 1);
		return (1);
	}
	return (0);
}

void	join_threads(t_data *data, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		pthread_join(data->philos[i].t_id, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_data		data;
	pthread_t	monitor;

	if (parse_args(argc, argv, &data) != 0)
	{
		printf("Invalid arguments\n");
		return (1);
	}
	if (init_mutex(&data) || init_philo(&data))
	{
		printf("Initialization error\n");
		cleanup(&data);
		return (1);
	}
	if (create_threads(&data, &monitor) != 0)
	{
		cleanup(&data);
		return (1);
	}
	join_threads(&data, monitor);
	cleanup(&data);
	return (0);
}
