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

// This pointer is a const because if it does not stay at the same address 
// through the life of the program, could lead to memory leak.
static arena_node * const g_head = {0};
static ALGORITHM g_algo = NO_ALGO;
    

int mavalloc_init(size_t size, ALGORITHM algorithm)
{
    if(size < 0)
    {
        return -1;
    }
    
    ALIGN4(size);
    g_algo = algorithm;
    *g_head = (arena_node){
        .type = HOLE, 
        .prev = NULL, 
        .next = NULL, 
        .data = malloc(size), 
        .size = size
    };
    
    if(g_head->data == NULL)
    {
        return -1;
    }
    
    return 0;
}

void mavalloc_destroy( )
{
    free(g_head->data);
    return;
}

void * mavalloc_alloc(size_t size)
{
    // only return NULL on failure
    return NULL;
}

void mavalloc_free(void * ptr)
{
    
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
