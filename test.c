#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int				counter = 0;
int				turn = 0;
pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	cond = PTHREAD_COND_INITIALIZER;

void	*yuta_handling(void *arg)
{
	char	*name;

	name = (char *)arg;
	for (int i = 0; i < 50; i++)
	{
		pthread_mutex_lock(&mutex);
		while (turn != 0)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		counter++;
		printf("Yuta:%d\n", counter);
		turn = 1;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
	printf("%s finished\n", name);
	return (NULL);
}

void	*ayano_handling(void *arg)
{
	char	*name;

	name = (char *)arg;
	for (int i = 0; i < 50; i++)
	{
		pthread_mutex_lock(&mutex);
		while (turn != 1)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		counter++;
		printf("Ayano:%d\n", counter);
		turn = 2;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
	printf("%s finished\n", name);
	return (NULL);
}

int	main(void)
{
	pthread_t	a1;
	pthread_t	a2;
	char		name1[5];
	char		name2[6];

	strcpy(name1, "Yuta");
	strcpy(name2, "Ayano");
	pthread_create(&a1, NULL, yuta_handling, name1);
	pthread_create(&a2, NULL, ayano_handling, name2);
	for (int i = 0; i < 50; i++)
	{
		pthread_mutex_lock(&mutex);
		while (turn != 2)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		counter++;
		printf("Tokura:%d\n", counter);
		turn = 0;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
	pthread_join(a1, NULL);
	pthread_join(a2, NULL);
	printf("main procces");
	return (0);
}
