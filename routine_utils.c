/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:49:47 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/22 11:45:56 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	handle_single_philosopher(t_philo *philo, int left)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->forks[left]);
	log_status(data, philo->id + 1, "has taken a fork");
	while (!get_stop_simulation(data))
		usleep(50);
	pthread_mutex_unlock(&data->forks[left]);
}

void	philo_lifecycle(t_philo *philo, int left, int right)
{
	t_data	*data;
	int		forks_taken;

	data = philo->data;
	if (philo->id % 2 == 0)
		usleep(2000);
	while (!get_stop_simulation(data))
	{
		if (philo->id % 2 == 0)
			forks_taken = take_forks_even(philo, left, right);
		else
			forks_taken = take_forks_odd(philo, left, right);
		if (forks_taken && !handle_eating(philo, left, right))
			break ;
		if (get_stop_simulation(data))
			break ;
		log_status(data, philo->id + 1, "is sleeping");
		smart_sleep(data->time_to_sleep, data);
		if (get_stop_simulation(data))
			break ;
		log_status(data, philo->id + 1, "is thinking");
	}
}
