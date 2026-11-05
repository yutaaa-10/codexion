/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 04:53:29 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 04:53:29 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_has_priority(t_request *first, t_request *second,
		t_scheduler policy)
{
	if (policy == FIFO)
	{
		if (first->arrival_order != second->arrival_order)
			return (first->arrival_order < second->arrival_order);
	}
	else if (first->deadline != second->deadline)
		return (first->deadline < second->deadline);
	if (first->arrival_order != second->arrival_order)
		return (first->arrival_order < second->arrival_order);
	return (first->customer->id < second->customer->id);
}

int	heap_init(t_heap *heap, int capacity)
{
	if (!heap || capacity <= 0)
		return (0);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
	heap->items = malloc(sizeof(t_request *) * capacity);
	if (!heap->items)
		return (0);
	heap->capacity = capacity;
	return (1);
}

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

t_request	*heap_peek(t_heap *heap)
{
	if (!heap || !heap->items || heap->size == 0)
		return (NULL);
	return (heap->items[0]);
}
