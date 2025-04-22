/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:04:04 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/22 11:45:09 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	take_forks_even(t_philo *philo, int left, int right)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->forks[left]);
	log_status(data, philo->id + 1, "has taken a fork");
	if (get_stop_simulation(data))
	{
		pthread_mutex_unlock(&data->forks[left]);
		return (0);
	}
	pthread_mutex_lock(&data->forks[right]);
	log_status(data, philo->id + 1, "has taken a fork");
	if (get_stop_simulation(data))
	{
		pthread_mutex_unlock(&data->forks[left]);
		pthread_mutex_unlock(&data->forks[right]);
		return (0);
	}
	return (1);
}

int	take_forks_odd(t_philo *philo, int left, int right)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->forks[right]);
	log_status(data, philo->id + 1, "has taken a fork");
	if (get_stop_simulation(data))
	{
		pthread_mutex_unlock(&data->forks[right]);
		return (0);
	}
	pthread_mutex_lock(&data->forks[left]);
	log_status(data, philo->id + 1, "has taken a fork");
	if (get_stop_simulation(data))
	{
		pthread_mutex_unlock(&data->forks[right]);
		pthread_mutex_unlock(&data->forks[left]);
		return (0);
	}
	return (1);
}

void	update_meal_time(t_philo *philo)
{
	pthread_mutex_lock(&philo->mutex);
	philo->last_meal = get_time_in_ms();
	pthread_mutex_unlock(&philo->mutex);
}

int	handle_eating(t_philo *philo, int left, int right)
{
	t_data	*data;
	int		should_continue;

	data = philo->data;
	if (get_stop_simulation(data))
	{
		if (pthread_mutex_unlock(&data->forks[left]) == 0)
			;
		if (pthread_mutex_unlock(&data->forks[right]) == 0)
			;
		return (0);
	}
	update_meal_time(philo);
	log_status(data, philo->id + 1, "is eating");
	smart_sleep(data->time_to_eat, data);
	pthread_mutex_lock(&philo->mutex);
	philo->meals_eaten++;
	should_continue = 1;
	if (data->meals != -1 && philo->meals_eaten >= data->meals)
		should_continue = 0;
	pthread_mutex_unlock(&philo->mutex);
	pthread_mutex_unlock(&data->forks[left]);
	pthread_mutex_unlock(&data->forks[right]);
	return (should_continue);
}

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
		handle_single_philosopher(philo, left);
		return (NULL);
	}
	philo_lifecycle(philo, left, right);
	return (NULL);
}
