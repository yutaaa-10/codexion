/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 08:06:38 by yukurosa          #+#    #+#             */
/*   Updated: 2026/11/06 12:23:35 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	print_error(const char *message)
{
	fprintf(stderr, "Error: %s\n", message);
	return (1);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (argc != 9)
		return (print_error("invalid number of arguments"));
	if (!parser_args(&sim.config, argv))
		return (print_error("invalid arguments"));
	if (!init_simulation(&sim))
		return (print_error("initialization failed"));
	if (!run_simulation(&sim))
	{
		destroy_simulation(&sim);
		return (print_error("simulation failed"));
	}
	destroy_simulation(&sim);
	return (0);
}
