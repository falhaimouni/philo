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
	struct timeval tv;

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

void	print_action(t_data *data, int id, const char *action)
{
	pthread_mutex_lock(&data->print_mutex);
	printf("%ld %d %s\n", get_time(), id, action);
	pthread_mutex_unlock(&data->print_mutex);
}

void	*monitor_routine(void *arg)
{
	t_data *data;
	int all_finished;

	data = (t_data *)arg;
	while (1)
	{
		all_finished = 1;
		for (int i = 0; i < data->num_philo; i++) {
			pthread_mutex_lock(&data->stop_mutex);
			if (data->stop) {
				pthread_mutex_unlock(&data->stop_mutex);
				return NULL;
			}
			pthread_mutex_unlock(&data->stop_mutex);

			// Use meals_mutex instead of meal_mutexes[i]
			pthread_mutex_lock(&data->meals_mutex);
			if ((get_time() - data->philos[i].last_meal_time) > data->time_to_die) {
				print_action(data, data->philos[i].id, "died");

				pthread_mutex_lock(&data->stop_mutex);
				data->stop = 1;
				pthread_mutex_unlock(&data->stop_mutex);

				pthread_mutex_unlock(&data->meals_mutex);
				return NULL;
			}
			pthread_mutex_unlock(&data->meals_mutex);

			// Check if all philosophers finished eating
			pthread_mutex_lock(&data->meals_mutex);
			if (data->num_meals > 0 && data->philos[i].meals_eaten < data->num_meals) {
				all_finished = 0;
			}
			pthread_mutex_unlock(&data->meals_mutex);
		}

		if (data->num_meals > 0 && all_finished) {
			pthread_mutex_lock(&data->stop_mutex);
			data->stop = 1;
			pthread_mutex_unlock(&data->stop_mutex);
			return NULL;
		}

		usleep(1000);
	}
	return NULL;
}

void *routine(void *arg) {
	t_philo *philo = (t_philo *)arg;
	t_data *data = philo->data;

	if (philo->id % 2 == 0)
		usleep(500);

	while (1) {
		pthread_mutex_lock(&data->stop_mutex);
		if (data->stop) {
			pthread_mutex_unlock(&data->stop_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&data->stop_mutex);

		pthread_mutex_lock(&data->forks[philo->id - 1]);
		print_action(data, philo->id, "has taken a fork");
		pthread_mutex_lock(&data->forks[philo->id % data->num_philo]);
		print_action(data, philo->id, "has taken a fork");

		pthread_mutex_lock(&data->stop_mutex);
		philo->last_meal_time = get_time();
		pthread_mutex_unlock(&data->stop_mutex);

		print_action(data, philo->id, "is eating");
		sleep_time(data->time_to_eat);

		pthread_mutex_lock(&data->meals_mutex);
		philo->meals_eaten++;
		pthread_mutex_unlock(&data->meals_mutex);

		pthread_mutex_unlock(&data->forks[philo->id - 1]);
		pthread_mutex_unlock(&data->forks[philo->id % data->num_philo]);

		print_action(data, philo->id, "is sleeping");
		sleep_time(data->time_to_sleep);

		print_action(data, philo->id, "is thinking");
	}
	return NULL;
}

void init_data(t_data *data) {
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philo);
	data->philos = malloc(sizeof(t_philo) * data->num_philo);
	pthread_mutex_init(&data->stop_mutex, NULL);
	pthread_mutex_init(&data->meals_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);

	for (int i = 0; i < data->num_philo; i++) {
		pthread_mutex_init(&data->forks[i], NULL);
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal_time = get_time();
		data->philos[i].meals_eaten = 0;
	}
	data->stop = 0;
	data->meals_finished = 0;
}

void free_data(t_data *data) {
	for (int i = 0; i < data->num_philo; i++)
		pthread_mutex_destroy(&data->forks[i]);
	free(data->forks);
	free(data->philos);
	pthread_mutex_destroy(&data->stop_mutex);
	pthread_mutex_destroy(&data->meals_mutex);
	pthread_mutex_destroy(&data->print_mutex);
}

int parse_args(int argc, char **argv, t_data *data) {
	if (argc != 5 && argc != 6)
		return 1;
	data->num_philo = atoi(argv[1]);
	data->time_to_die = atoi(argv[2]);
	data->time_to_eat = atoi(argv[3]);
	data->time_to_sleep = atoi(argv[4]);
	data->num_meals = (argc == 6) ? atoi(argv[5]) : -1;
	return 0;
}

int main(int argc, char **argv) {
	t_data data;
	pthread_t monitor_thread;

	if (parse_args(argc, argv, &data)) {
		printf("Invalid arguments\n");
		return 1;
	}

	init_data(&data);
	pthread_create(&monitor_thread, NULL, monitor_routine, &data);

	for (int i = 0; i < data.num_philo; i++)
		pthread_create(&data.philos[i].thread, NULL, routine, &data.philos[i]);

	for (int i = 0; i < data.num_philo; i++)
		pthread_join(data.philos[i].thread, NULL);

	pthread_join(monitor_thread, NULL);
	free_data(&data);
	return 0;
}
