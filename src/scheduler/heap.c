/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukurosa <yukurosa@student.42tokyo.jp>     #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-11-02 05:05:20 by yukurosa          #+#    #+#             */
/*   Updated: 2026-11-02 05:05:20 by yukurosa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_requests(t_request **first, t_request **second)
{
	t_request	*temporary;

	temporary = *first;
	*first = *second;
	*second = temporary;
}

static void	sift_up(t_heap *heap, int index, t_scheduler policy)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (request_has_priority(heap->items[parent], heap->items[index],
				policy))
			break ;
		swap_requests(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
}

static void	sift_down(t_heap *heap, int index, t_scheduler policy)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = index * 2 + 1;
		if (left >= heap->size)
			break ;
		right = left + 1;
		best = left;
		if (right < heap->size && request_has_priority(heap->items[right],
				heap->items[left], policy))
			best = right;
		if (request_has_priority(heap->items[index], heap->items[best], policy))
			break ;
		swap_requests(&heap->items[index], &heap->items[best]);
		index = best;
	}
}

int	heap_push(t_heap *heap, t_request *request, t_scheduler policy)
{
	int	index;

	if (!heap || !heap->items || !request)
		return (0);
	if (heap->size >= heap->capacity)
		return (0);
	index = heap->size;
	heap->items[index] = request;
	heap->size++;
	sift_up(heap, index, policy);
	return (1);
}

t_request	*heap_pop(t_heap *heap, t_scheduler policy)
{
	t_request	*result;

	if (!heap || !heap->items || heap->size == 0)
		return (NULL);
	result = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		sift_down(heap, 0, policy);
	}
	heap->items[heap->size] = NULL;
	return (result);
}
