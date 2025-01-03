/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: farah <farah@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 11:11:05 by falhaimo          #+#    #+#             */
/*   Updated: 2025/01/02 23:34:00 by farah            ###   ########.fr       */
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

long	get_time(void)
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
	data->philos = malloc(sizeof(t_philo) * data->num_philo);
	while (i < data->num_philo)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}  
}

void	*routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;

	philo = (t_philo *)arg;
	data = philo->data;
	while (1)
	{
		pthread_mutex_lock(&data->forks[philo->id - 1]);
		printf("%ld %d has taken a fork\n", get_time(), philo->id);
		pthread_mutex_lock(&data->forks[philo->id % data->num_philo]);
		printf("%ld %d has taken a fork\n", get_time(), philo->id);
		printf("%ld %d is eating\n", get_time(), philo->id);
		sleep_time(data->time_to_eat);
		pthread_mutex_unlock(&data->forks[philo->id - 1]);\
			pthread_mutex_unlock(&data->forks[philo->id % data->num_philo]);
		printf("%ld %d is sleeping\n", get_time(), philo->id);
		sleep_time(data->time_to_eat);
		printf("%ld %d is thinking\n", get_time(), philo->id);
	}
	return (NULL);
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

int	main(int argc, char **argv) 
{

	t_data	data;

	if (parse_args(argc, argv, &data) != 0)
	{
		printf("Usage: %s num_philo time_to_die time_to_eat time_to_sleep [num_meals]\n", argv[0]);
		return 1;
	}
	init_data(&data);

	create_thraeds(&data);
	for (int i = 0; i < data.num_philo; i++)
		pthread_mutex_destroy(&data.forks[i]);
	free(data.forks);
	free(data.philos);
	return 0;
}
