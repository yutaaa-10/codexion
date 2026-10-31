/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:38 by yukurosa          #+#    #+#             */
/*   Updated: 2026/10/30 11:02:42 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (argc != 9)
		return (1);
	if (!parser_args(&sim.config, argv))
		return (1);
	sim.coders = NULL;
	sim.dongles = NULL;
	sim.start_time = 0;
	sim.stopped = 0;
	if (!init_coders(&sim))
		return (1);
	if (!init_dongles(&sim))
	{
		free(sim.coders);
		return (1);
	}
	if (!run_simulation(&sim))
	{
		destroy_simulation(&sim);
		return (1);
	}
	destroy_simulation(&sim);
	return (0);
}
