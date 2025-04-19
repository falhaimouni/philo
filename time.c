/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 16:27:43 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/19 16:28:08 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	get_stop_simulation(t_data *data)
{
	int	stop;

	pthread_mutex_lock(&data->stop_mutex);
	stop = data->stop_simulation;
	pthread_mutex_unlock(&data->stop_mutex);
	return (stop);
}

void	set_stop_simulation(t_data *data, int value)
{
	pthread_mutex_lock(&data->stop_mutex);
	data->stop_simulation = value;
	pthread_mutex_unlock(&data->stop_mutex);
}

long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (0);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

void	smart_sleep(long duration_ms, t_data *data)
{
	long	start;

	start = get_time_in_ms();
	while (!get_stop_simulation(data)
		&& (get_time_in_ms() - start) < duration_ms)
		usleep(50);
}

void	log_status(t_data *data, int id, const char *status)
{
	long	timestamp;

	pthread_mutex_lock(&data->print_mutex);
	if (get_stop_simulation(data) && strcmp(status, "died") != 0)
	{
		pthread_mutex_unlock(&data->print_mutex);
		return ;
	}
	timestamp = get_time_in_ms() - data->start_time;
	printf("%ld %d %s\n", timestamp, id, status);
	pthread_mutex_unlock(&data->print_mutex);
}
