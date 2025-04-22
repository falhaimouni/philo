/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:56:47 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/22 11:57:21 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_forks_mutex(t_data *data)
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
	return (0);
}

int	init_other_mutexes(t_data *data)
{
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

int	init_mutex(t_data *data)
{
	if (init_forks_mutex(data) != 0)
		return (1);
	if (init_other_mutexes(data) != 0)
		return (1);
	return (0);
}

int	init_philo_mutexes(t_data *data, int i)
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

int	init_philo(t_data *data)
{
	int	i;

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
			return (init_philo_mutexes(data, i));
		i++;
	}
	return (0);
}
