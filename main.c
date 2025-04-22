/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: falhaimo <falhaimo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:52:12 by falhaimo          #+#    #+#             */
/*   Updated: 2025/04/22 11:57:16 by falhaimo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	check_philos(t_data *data, int *all_done)
{
	int		i;
	long	last_meal;
	int		meals;

	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_lock(&data->philos[i].mutex);
		last_meal = data->philos[i].last_meal;
		meals = data->philos[i].meals_eaten;
		pthread_mutex_unlock(&data->philos[i].mutex);
		if (data->meals == -1 || meals < data->meals)
		{
			if (get_time_in_ms() - last_meal > data->time_to_die)
			{
				log_status(data, data->philos[i].id + 1, "died");
				set_stop_simulation(data, 1);
				return ;
			}
		}
		if (data->meals != -1 && meals < data->meals)
			*all_done = 0;
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		all_done;

	data = (t_data *)arg;
	while (!get_stop_simulation(data))
	{
		all_done = 1;
		check_philos(data, &all_done);
		if (data->meals != -1 && all_done)
		{
			set_stop_simulation(data, 1);
			break ;
		}
		usleep(10);
	}
	return (NULL);
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
