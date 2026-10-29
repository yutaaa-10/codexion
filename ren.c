#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void	*thread_function(void *arg)
{
	int	id;

	id = *(int *)arg;
	printf("Thread %d: 開始\n", id);
	printf("Thread %d: 終了\n", id);
	return (NULL);
}

int	main(void)
{
	pthread_t	threads[3];
	int			ids[3] = {1, 2, 3};
	int			i;

	i = 0;
	while (i < 3)
	{
		pthread_create(&threads[i], NULL, thread_function, &ids[i]);
		i++;
	}
	i = 0;
	while (i < 3)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	printf("すべてのスレッドが終了しました\n");
	return (0);
}
