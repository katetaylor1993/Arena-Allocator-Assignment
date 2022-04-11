// The MIT License (MIT)
//
// Copyright (c) 2022 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include "mavalloc.h"

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

// setting up a double-linked list
typedef struct arena_node_struct
{
    NODE_TYPE type;
    struct arena_node_struct *prev;
    struct arena_node_struct *next;
    void *data;
    uint64_t size;
} arena_node;

static arena_node *g_head;
static arena_node *g_tail;
static ALGORITHM g_algo = NO_ALGO;

int mavalloc_init(size_t size, ALGORITHM algorithm)
{
    if (size < 0)
    {
        return -1;
    }

    ALIGN4(size);
    g_algo = algorithm;
    g_head = (arena_node *)malloc(sizeof(arena_node));
    g_head->type = HOLE;
    g_head->prev = NULL;
    g_head->next = NULL;
    g_head->data = malloc(size);
    g_head->size = size;
    g_tail = g_head;

    if (g_head->data == NULL)
    {
        return -1;
    }

    g_algo = algorithm;
    if (g_algo == NO_ALGO)
    {
        return -1;
    }

    return 0;
}

// Note: this is not done yet
void mavalloc_destroy()
{
    arena_node *current = g_head;
    arena_node *current_copy;
    while (current != NULL)
    {
        current_copy = current;
        current = current->next;

        free(current_copy);
    }

    return;
}

// Note: First Fit/Next Fit are essentially similar, just the pointer for 'node' variable, will be pointing in different spots. First fit will be the first part
// free in the heap, next fit will be right after, which is initialized in the first if and else ifs.
// Best/Worst Fit are also similar in a sense, worst fit will be just the opposite of best fit, as in the smallest portion will be used for best
// biggest portion willl be used for worst
void *mavalloc_alloc(size_t size)
{
    if (!g_head)
    {
        return NULL;
    }
    arena_node *node;

    if (g_algo != NEXT_FIT)
    {
        node = g_head;
    }
    else if (g_algo == NEXT_FIT)
    {
        node = g_tail;
    }
    else
    {
        printf("ERROR: Unknown allocation algorithm!\n");
        exit(0);
    }

    size_t aligned_size = ALIGN4(size);

    if (g_algo == FIRST_FIT || g_algo == NEXT_FIT)
    {
        while (node)
        {
            if (node->size >= aligned_size && node->type == HOLE)
            {

                int leftover_size = 0;

                node->type = PALLOC;
                leftover_size = node->size - aligned_size;
                node->size = aligned_size;

                if (leftover_size > 0)
                {
                    arena_node *previous_next = node->next;
                    arena_node *leftover_node = (arena_node *)malloc(sizeof(arena_node));

                    leftover_node->data = node->data + size;
                    leftover_node->type = HOLE;
                    leftover_node->size = leftover_size;
                    leftover_node->next = previous_next;
                    leftover_node->prev = node;

                    node->next = leftover_node;
                }

                g_tail = node;
                return (void *)node->data;
            }
            node = node->next;
        }
    }
    else if (g_algo == BEST_FIT || g_algo == WORST_FIT)
    {
        // Set the worst and best nodes first, largest leftover is going for worst
        // smallest leftover will be for best
        arena_node *worst = (arena_node *)malloc(sizeof(arena_node));
        arena_node *best = (arena_node *)malloc(sizeof(arena_node));
        int current_best = INT_MAX;
        int current_worst = INT_MIN;
        int leftover = 0;
        while (node)
        {
            // iteratate until empty
            if (node->size >= aligned_size && node->type == HOLE)
            {
                int leftover_size = 0;

                leftover_size = node->size - aligned_size;

                if (leftover_size < current_best)
                {
                    current_best = leftover_size;
                    best = node;
                }
                if (leftover_size > current_worst)
                {
                    current_worst = leftover_size;
                    worst = node;
                }
            }
            node = node->next;
        }
        if (g_algo == BEST_FIT)
        {
            best->type = PALLOC;
            best->size = current_best - aligned_size;
            leftover = current_best - aligned_size;
            if (leftover > 0)
            {
                arena_node *previous_next = best->next;
                arena_node *leftover_node = (arena_node *)malloc(sizeof(arena_node));

                leftover_node->data = best->data + size;
                leftover_node->type = HOLE;
                leftover_node->size = leftover;
                leftover_node->next = previous_next;
                leftover_node->prev = best;

                best->next = leftover_node;
            }
            g_tail = best;
            return (void *)best->data;
        }
        if (g_algo == WORST_FIT)
        {
            worst->type = PALLOC;
            worst->size = current_worst - aligned_size;
            leftover = current_worst - aligned_size;
            if (leftover > 0)
            {
                arena_node *previous_next = worst->next;
                arena_node *leftover_node = (arena_node *)malloc(sizeof(arena_node));

                leftover_node->data = worst->data + size;
                leftover_node->type = HOLE;
                leftover_node->size = leftover;
                leftover_node->next = previous_next;
                leftover_node->prev = worst;

                worst->next = leftover_node;
            }
            g_tail = worst;
            return (void *)worst->data;
        }
    }
    // only return NULL on failure
    g_head->prev = NULL;

    return NULL;
}

void mavalloc_free(void *ptr)
{
    if (ptr == NULL)
        return;
    arena_node *current = g_head;
    arena_node *hit = NULL;
    do
    {
        if (ptr == current->data)
        {
            hit = current;
            break;
        }
        else
        {
            current = current->next;
        }
    } while (current != NULL);

    // coalesce
    arena_node *hit_copy = hit;
    int prevHole = 0;
    int keepHit = 1;
    if (hit->prev != NULL)
    {
        if (hit->prev->type == HOLE)
        {
            prevHole = 1;
            hit->prev->next = hit->next;
            hit->prev->size += hit->size;
            hit = hit->prev;
            free(hit_copy);
            keepHit = 0;
        }
    }
    if (hit->next != NULL)
    {
        if (hit->next->type == HOLE)
        {
            if (prevHole)
            {
                arena_node * hit_copy_2 = hit->next;
                hit->size += hit->next->size;
                hit->next = hit->next->next;
                free(hit_copy_2);
            }
            else
            {
                hit->next->prev = hit->prev;
                hit->next->size += hit->size;
            }
            keepHit = 0;
        }
        if(keepHit)
        {
            hit->type = HOLE;
        }
    }
    //TODO: remove this
    arena_node * test = g_head;
    //printf("%p: size %ld next %p prev %p\n",g_head->prev,g_head->prev->size,g_head->prev->next,g_head->prev->prev);
    while(test!=NULL)
    {
      printf("%p: size %ld next %p prev %p\n",test,test->size,test->next,test->prev);
      test = test->next;
    }
    return;
}

int mavalloc_size()
{
    int number_of_nodes = 0;
    if (g_head->data != NULL)
    {
        arena_node * current = g_head->next;
        number_of_nodes = 1;
        while (current != NULL)
        {
            number_of_nodes++;
            current = current->next;
        }
    }

    return number_of_nodes;
}
