/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:38 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/31 12:18:04 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (argc != 9)
	{
		fprintf(stderr, "Eroor: invalid number of arguments\n");
		return (1);
	}
	if (!parser_args(&sim.config, argv))
	{
		fprintf(stderr, "Eroor: invalid argments\n");
		return (1);
	}
	sim.coders = NULL;
	sim.dongles = NULL;
	sim.start_time = 0;
	sim.stopped = 0;
	if (pthread_mutex_init(&sim.state_mutex, NULL) != 0)
	{
		fprintf(stderr, "Error: mutex initialization failed\n");
		return (1);
	}
	if (!init_coders(&sim))
	{
		fprintf(stderr, "Error: faild to initialize coders\n");
		return (1);
	}
	if (!init_dongles(&sim))
	{
		fprintf(stderr, "Error: faild to initalize dongles\n");
		free(sim.coders);
		return (1);
	}
	if (!run_simulation(&sim))
	{
		fprintf(stderr, "Error: simulation faild\n");
		destroy_simulation(&sim);
		return (1);
	}
	destroy_simulation(&sim);
	return (0);
}
