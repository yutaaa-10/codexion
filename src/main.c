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

static int	print_error(const char *message)
{
	fprintf(stderr, "Error: %s\n", message);
	return (1);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	int				status;

	if (argc != 9)
		return (print_error("invalid number of arguments"));
	memset(&sim, 0, sizeof(t_simulation));
	if (!parser_args(&sim.config, argv))
		return (print_error("invalid arguments"));
	if (!init_simulation(&sim))
		return (print_error("initialization failed"));
	status = 0;
	if (!run_simulation(&sim))
		status = print_error("simulation failed");
	destroy_simulation(&sim);
	return (status);
}
