/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: farah <farah@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 11:11:05 by falhaimo          #+#    #+#             */
/*   Updated: 2025/01/17 13:15:24 by farah            ###   ########.fr       */
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
	while (nptr[i] == 32 || (nptr[i] >= '\t' && nptr[i] <= '\r'))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			neg = -1;
		i++;
	}
	while (nptr[i] != '\0' && nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = (res * 10) + (nptr[i] - '0');
		i++;
	}
	return (res * neg);
}

long	get_time()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	sleep_time(long ms)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < ms)
		usleep(100);
}

int	parse_args(int argc, char **argv, t_data *data)
{
	if (argc != 5 && argc != 6)
		return (1);
	data->num_philo = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->num_meals = ft_atoi(argv[5]);
	else
		data->num_meals = -1;
	return (0);
}

void	init_data(t_data *data)
{
	int	i;

	i = 0;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	if (!data->forks)
		return ;
	data->philos = malloc(sizeof(t_philo) * data->num_philo);
	if (!data->philos)
		return ;
	data->meal_mutexes = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	if (!data->meal_mutexes)
		return ;
	data->stop = 0;
	pthread_mutex_init(&data->stop_mutex, NULL);
	while (i < data->num_philo)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		pthread_mutex_init(&data->meal_mutexes[i], NULL);
		i++;
	}
}

void *monitor_routine(void *arg)
{
    t_data *data = (t_data *)arg;
    int i;

    while (1)
    {
        i = 0;
        while (i < data->num_philo)
        {
            pthread_mutex_lock(&data->meal_mutexes[i]);
            if ((get_time() - data->philos[i].last_meal_time) > data->time_to_die)
            {
                printf("%ld %d died\n", get_time(), data->philos[i].id);
                pthread_mutex_unlock(&data->meal_mutexes[i]);
                pthread_mutex_lock(&data->stop_mutex);
                data->stop = 1;
                pthread_mutex_unlock(&data->stop_mutex);
                return (NULL);
            }
            pthread_mutex_unlock(&data->meal_mutexes[i]);
            i++;
        }
        usleep(1000); // Check every millisecond
    }
    return (NULL);
}

void	*routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;

	philo = (t_philo *)arg;
	data = philo->data;
	philo->last_meal_time = get_time();
	while (1)
	{
		pthread_mutex_lock(&data->stop_mutex);
		if (data->stop)
		{
			pthread_mutex_unlock(&data->stop_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&data->stop_mutex);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(&data->forks[philo->id - 1]);
		printf("%ld %d has taken first fork\n", get_time(), philo->id);
		pthread_mutex_lock(&data->forks[philo->id % data->num_philo]);
		printf("%ld %d has taken second fork\n", get_time(), philo->id);
	}
	else
	{
	pthread_mutex_lock(&data->forks[philo->id % data->num_philo]);
	printf("%ld %d has taken first fork\n", get_time(), philo->id);
	pthread_mutex_lock(&data->forks[philo->id - 1]);
	printf("%ld %d has taken second fork\n", get_time(), philo->id);
	}
		printf("%ld %d is eating\n", get_time(), philo->id);
		pthread_mutex_lock(&data->meal_mutexes[philo->id - 1]);
		philo->last_meal_time = get_time();
		pthread_mutex_unlock(&data->meal_mutexes[philo->id - 1]);
		sleep_time(data->time_to_eat);
		pthread_mutex_unlock(&data->forks[philo->id - 1]);
		pthread_mutex_unlock(&data->forks[philo->id % data->num_philo]);
		printf("%ld %d is sleeping\n", get_time(), philo->id);
		sleep_time(data->time_to_sleep);
		printf("%ld %d is thinking\n", get_time(), philo->id);
	}
	return (NULL);
}

void	*monitor(void *arg)
{
	int		i;
	long	cur_time;
	t_data	*data;

	data = (t_data *)arg;
	usleep(10000);
	while (1)
	{
		i = 0;
		while (i < data->num_philo)
		{
			cur_time = get_time();
			pthread_mutex_lock(&data->stop_mutex);
			if (data->stop)
			{
				pthread_mutex_unlock(&data->stop_mutex);
				pthread_mutex_unlock(&data->forks[data->philos->id - 1]);
				return (NULL);
			}
			pthread_mutex_unlock(&data->stop_mutex);
			pthread_mutex_lock(&data->meal_mutexes[i]);
			if (cur_time - data->philos[i].last_meal_time > data->time_to_die)
			{
				printf("%ld %d died\n", cur_time, data->philos[i].id);
				data->stop = 1;
				pthread_mutex_unlock(&data->meal_mutexes[i]);
				pthread_mutex_unlock(&data->stop_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&data->meal_mutexes[i]);
			pthread_mutex_unlock(&data->stop_mutex);
			i++;
		}
		usleep(5000);
	}
}

void	create_thraeds(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		pthread_create(&data->philos[i].thread, NULL, routine, &data->philos[i]);
		i++;
	}
	i = 0;
	while (i < data->num_philo)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}

}

void	free_data(t_data *data)
{
	//int	i;

	free(data->forks);
	free(data->philos);
	free(data->meal_mutexes);
	// i = 0;
	// while (i < data->num_philo)
	// {
	//  	pthread_mutex_destroy(&data->forks[i]);
	//  	pthread_mutex_destroy(&data->meal_mutexes[i]);
	// 	i++;
	// }
	pthread_mutex_destroy(&data->stop_mutex);
}

int	main(int argc, char **argv)
{
	t_data	data;
	pthread_t monitor_thread;
	
	if (parse_args(argc, argv, &data) != 0)
	{
		printf("Invalid arguments\n");
		return (1);
	}
	init_data(&data);
	pthread_create(&monitor_thread, NULL, monitor, &data);
	create_thraeds(&data);
	pthread_join(monitor_thread, NULL);
	free_data(&data);
	return (0);
}
