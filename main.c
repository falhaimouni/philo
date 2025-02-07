/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: farah <farah@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 19:42:08 by farah             #+#    #+#             */
/*   Updated: 2025/02/07 22:24:23 by farah            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(char *nptr)
{
	int	i;
	int	res;
	int	neg;

	i = 0;
	res = 0;
	neg = 1;
	while (nptr[i] == ' ' || (nptr[i] >= '\t' && nptr[i] <= '\r'))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			neg = -1;
		i++;
	}
	while (nptr[i] && (nptr[i] >= '0' && nptr[i] <= '9'))
	{
		res = res * 10 + (nptr[i] - '0');
		i++;
	}
	return (res * neg);
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
	while (!data->stop_simulation && (get_time_in_ms() - start) < duration_ms)
		usleep(50);
}

void	log_status(t_data *data, int id, const char *status)
{
	long	timestamp;

	pthread_mutex_lock(&data->print_mutex);
	if (data->stop_simulation && strcmp(status, "died") != 0)
	{
		pthread_mutex_unlock(&data->print_mutex);
		return ;
	}
	timestamp = get_time_in_ms() - data->start_time;
	printf("%ld %d %s\n", timestamp, id, status);
	pthread_mutex_unlock(&data->print_mutex);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	int	left;
	int	right;

	philo = (t_philo *)arg;
	data = philo->data;
	left = philo->id;
	right = (philo->id + 1) % data->num_philos;
	if (data->num_philos == 1)
	{
		pthread_mutex_lock(&data->forks[left]);
		log_status(data, philo->id + 1, "has taken a fork");
		while (!data->stop_simulation)
			usleep(50);
		pthread_mutex_unlock(&data->forks[left]);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		usleep(1000);
	while (!data->stop_simulation)
	{
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(&data->forks[left]);
			log_status(data, philo->id + 1, "has taken a fork");
			if (data->stop_simulation)
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
			if (data->stop_simulation)
			{
				pthread_mutex_unlock(&data->forks[right]);
				break ;
			}
			pthread_mutex_lock(&data->forks[left]);
			log_status(data, philo->id + 1, "has taken a fork");
		}
		if (data->stop_simulation)
		{
			pthread_mutex_unlock(&data->forks[left]);
			pthread_mutex_unlock(&data->forks[right]);
			break ;
		}
		philo->last_meal = get_time_in_ms();
		log_status(data, philo->id + 1, "is eating");
		smart_sleep(data->time_to_eat, data);
		philo->meals_eaten++;
		pthread_mutex_unlock(&data->forks[left]);
		pthread_mutex_unlock(&data->forks[right]);
		if (data->meals_required != -1
			&& philo->meals_eaten >= data->meals_required)
			break ;
		if (data->stop_simulation)
			break ;
		log_status(data, philo->id + 1, "is sleeping");
		smart_sleep(data->time_to_sleep, data);
		if (data->stop_simulation)
			break ;
		log_status(data, philo->id + 1, "is thinking");
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int	i;
	int	all_done;

	data = (t_data *)arg;
	while (!data->stop_simulation)
	{
		all_done = 1;
		i = 0;
		while (i < data->num_philos)
		{
			if (data->meals_required != -1
				&& data->philos[i].meals_eaten < data->meals_required)
				all_done = 0;
			if (!data->stop_simulation
				&& (get_time_in_ms() - data->philos[i].last_meal > data->time_to_die))
			{
				log_status(data, data->philos[i].id + 1, "died");
				data->stop_simulation = 1;
				return (NULL);
			}
			i++;
		}
		if (data->meals_required != -1 && all_done)
		{
			data->stop_simulation = 1;
			break ;
		}
		usleep(10);
	}
	return (NULL);
}

int	parse_args(int argc, char **argv, t_data *data)
{
	if (argc != 5 && argc != 6)
		return (1);
	data->num_philos = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->meals_required = ft_atoi(argv[5]);
	else
		data->meals_required = -1;
	data->stop_simulation = 0;
	data->start_time = get_time_in_ms();
	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;
	int	i;
	pthread_t	monitor;

	if (parse_args(argc, argv, &data) != 0)
	{
		printf("Invalid arguments\n");
		return (1);
	}
	data.forks = malloc(sizeof(pthread_mutex_t) * data.num_philos);
	if (!data.forks)
		return (1);
	i = 0;
	while (i < data.num_philos)
	{
		if (pthread_mutex_init(&data.forks[i], NULL) != 0)
		{
			free(data.forks);
			return (1);
		}
		i++;
	}
	if (pthread_mutex_init(&data.print_mutex, NULL) != 0)
	{
		free(data.forks);
		return (1);
	}
	data.philos = malloc(sizeof(t_philo) * data.num_philos);
	if (!data.philos)
	{
		free(data.forks);
		return (1);
	}
	i = 0;
	while (i < data.num_philos)
	{
		data.philos[i].id = i;
		data.philos[i].meals_eaten = 0;
		data.philos[i].last_meal = data.start_time;
		data.philos[i].data = &data;
		i++;
	}
	i = 0;
	while (i < data.num_philos)
	{
		if (pthread_create(&data.philos[i].thread, NULL,
				philosopher_routine, &data.philos[i]) != 0)
		{
			data.stop_simulation = 1;
			break ;
		}
		i++;
	}
	if (pthread_create(&monitor, NULL, monitor_routine, &data) != 0)
		data.stop_simulation = 1;
	i = 0;
	while (i < data.num_philos)
	{
		pthread_join(data.philos[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	i = 0;
	while (i < data.num_philos)
	{
		pthread_mutex_destroy(&data.forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data.print_mutex);
	free(data.forks);
	free(data.philos);
	return (0);
}
