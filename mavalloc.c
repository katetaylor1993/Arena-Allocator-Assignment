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

// setting up a double-linked list
typedef struct arena_node_struct
{
    NODE_TYPE type;
    struct arena_node_struct * prev;
    struct arena_node_struct * next;
    void * data;
    uint64_t size;
} arena_node;

static arena_node * g_head;
static ALGORITHM g_algo = NO_ALGO;


int mavalloc_init(size_t size, ALGORITHM algorithm)
{
    if(size < 0)
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

    if(g_head->data == NULL)
    {
        return -1;
    }

    g_algo = algorithm;
    if(g_algo == NO_ALGO)
    {
        return -1;
    }

    return 0;
}

// Note: this is not done yet
void mavalloc_destroy( )
{
    arena_node * current = g_head;
    arena_node * current_copy = NULL;
    while(current != NULL)
    {
        current_copy = current;
        current = current->next;
        free(current_copy);
    }
    free(g_head->data);
    return;
}

// Note: According to professor, you must use malloc here to create nodes
void * mavalloc_alloc(size_t size)
{
    // only return NULL on failure

    return NULL;
}

void mavalloc_free(void * ptr)
{
    if(ptr == NULL)
        return;
    arena_node * current = g_head;
    arena_node * hit = NULL;
    do
    {
        if(ptr == current->data)
        {
            hit = current;
            break;
        }
    } while(current != NULL);

    // coalesce
    arena_node * hit_copy = hit;
    if(hit->prev != NULL)
    {
        if(hit->prev->type == HOLE)
        {
            hit->prev->next = hit->next;
            hit->prev->size += hit->size;
            hit = hit->prev;
        }
    }
    if(hit->next != NULL)
    {
        if(hit->next->type == HOLE)
        {
            hit->next->prev = hit->prev;
            hit->next->size += hit->size;
        }
    }
    free(hit_copy);
    return;
}

int mavalloc_size()
{
    int number_of_nodes = 0;
    if(g_head->data != NULL)
    {
        number_of_nodes = 1;
        while(g_head->next != NULL)
        {
            number_of_nodes++;
        }
    }

    return number_of_nodes;
}
