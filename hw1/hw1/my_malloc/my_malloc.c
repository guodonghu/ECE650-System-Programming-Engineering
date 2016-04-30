#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include"my_malloc.h"
#include<pthread.h>

void *first_block = NULL;
pthread_mutex_t malloc_lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t malloc_lock2 = PTHREAD_MUTEX_INITIALIZER;


size_t align8(size_t s) {
    if (s % 8 == 0) {
        return s;
    }
    return ((s >> 3) + 1) << 3;
}



block *ff_find_block(block **curr, size_t size) {
    block *b = first_block;
    while(b && !(b->free && b->size >= size)) {
        *curr = b;
        b = b->next;
    }
    return b;
}

block *bf_find_block(block **curr, size_t size) {
    block *b = first_block;
    block *bf_block = NULL;
    while(b) {
        if (b->size >= size && b->free) {
            if (bf_block == NULL || b->size < bf_block->size) {
                bf_block = b;
            }
        }
        *curr = b;
        b = b->next;
    }
    return bf_block;
}

block *wf_find_block(block **curr, size_t size) {
    block *b = first_block;
    block *wf_block = NULL;
    while(b) {
        if (b->size >= size && b->free) {
            if (wf_block == NULL || b->size > wf_block->size) {
                wf_block = b;
            }
        }
        *curr = b;
        b = b->next;
    }
    return wf_block;
}



block *extend_heap(block *curr, size_t s) {
    block *b = sbrk(s + BLOCK_SIZE);
    if (b == (void *)-1) {
        return NULL;
    }
    
    if(curr) {
        curr->next = b;
    }
    b->size = s;
    b->next = NULL;
    b->free = 0;
    b->prev = curr;
    //b->data = b + BLOCK_SIZE;
    return b;
}

void split_block(block *b, size_t s) {
    block *new_block;
    block *block_data = b + 1;
    new_block =(block *) ((char *)block_data + s);
    new_block->size = b->size - s - BLOCK_SIZE;
    new_block->next = b->next;
    new_block->free = 1;
    b->size = s;
    b->next = new_block;
    new_block->prev = b;
}
    


void *ff_malloc(size_t size) {
    block *b, *curr;
    size_t s;
    if (size <= 0) {
        return NULL;
    }
    s = align8(size);
    if (first_block) {
        curr = first_block;
        b = ff_find_block(&curr, s);
        if (b) {
            if ((b->size - s) >= (BLOCK_SIZE + 8)) {
                split_block(b, s);
            }
            b->free = 0;
        }
        else {
            b = extend_heap(curr, s);
            if (!b) {
                return NULL;
            }
        }
    }
    else {
        b = extend_heap(NULL, s);
        if (!b) {
            return NULL;
        }
        first_block = b;
    }
    return (b + 1);
}

void *bf_malloc(size_t size) {
    block *b, *curr;
    size_t s;
    if (size <= 0) {
        return NULL;
    }
    s = align8(size);
    if (first_block) {
        curr = first_block;
        b = bf_find_block(&curr, s);
        if (b) {
            if ((b->size - s) >= (BLOCK_SIZE + 8)) {
                split_block(b, s);
            }
            b->free = 0;
        }
        else {
            b = extend_heap(curr, s);
            if (!b) {
                return NULL;
            }
        }
    }
    else {
        b = extend_heap(NULL, s);
        if (!b) {
            return NULL;
        }
        first_block = b;
    }
    return (b + 1);
}

void *wf_malloc(size_t size) {
    block *b, *curr;
    size_t s;
    if (size <= 0) {
        return NULL;
    }
    s = align8(size);
    if (first_block) {
        curr = first_block;
        b = wf_find_block(&curr, s);
        if (b) {
            if ((b->size - s) >= (BLOCK_SIZE + 8)) {
                split_block(b, s);
            }
            b->free = 0;
        }
        else {
            b = extend_heap(curr, s);
            if (!b) {
                return NULL;
            }
        }
    }
    else {
        b = extend_heap(NULL, s);
        if (!b) {
            return NULL;
        }
        first_block = b;
    }
    return (b + 1);
}


block *get_block(void *p) {
    return (block*)p - 1;
}
       
block *merge(block *b) {
    if (b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
    return b;
}

void ff_free(void *ptr) {
    if (!ptr) {
        return;
    }
    block *b;
    b = get_block(ptr);
    // assert(b->free == 0);
    if (b->free == 1) {
        perror("double free\n");
        return;
    }
    b->free = 1;
    if (b->prev && b->prev->free) {
            b = merge(b->prev);
    }
    if (b->next) {
        b = merge(b);
    }
    /*if (b->prev == NULL) {
        assert(b = first_block);
        first_block = NULL;
        // brk(sbrk(0)-(b->size + BLOCK_SIZE));
        } */
}

void bf_free(void *ptr) {
    if (!ptr) {
        return;
    }
    block *b;
    b = get_block(ptr);
    // assert(b->free == 0);
    if (b->free == 1) {
        perror("double free\n");
        return;
    }
    b->free = 1;
    if (b->prev && b->prev->free) {
            b = merge(b->prev);
    }
    if (b->next) {
        b = merge(b);
    }
    /*if (b->prev == NULL) {
        assert(b = first_block);
        first_block = NULL;
        // brk(sbrk(0)-(b->size + BLOCK_SIZE));
        }*/
}

void wf_free(void *ptr) {
    if (!ptr) {
        return;
    }
    block *b;
    b = get_block(ptr);
    //assert(b->free == 0);
    if (b->free == 1) {
        perror("double free\n");
        return;
    }
    b->free = 1;
    if (b->prev && b->prev->free) {
            b = merge(b->prev);
    }
    if (b->next) {
        b = merge(b);
    }
    /*if (b->prev == NULL) {
        assert(b = first_block);
        first_block = NULL;
        // brk(sbrk(0)-(b->size + BLOCK_SIZE));
        }*/
}


unsigned long get_data_segment_size() {
    if (first_block == NULL) {
        return 0;
    }
    return (long)((char *)sbrk(0) - (char *)first_block); 
}

unsigned long get_data_segment_free_space_size() {
    long free_space = 0;
    block *b = first_block;
    while(b) {
        if (b->free) {
            free_space += b->size + BLOCK_SIZE;
        }
        b = b->next;
    }
    return free_space;
}



void *ts_malloc(size_t size) {
    block *b, *curr;
    size_t s;
    if (size <= 0) {
        return NULL;
    }
    s = align8(size);
    if (first_block) {
        curr = first_block;
        b = bf_find_block(&curr, s);
        if (b) {
            pthread_mutex_lock(&malloc_lock1);
            if (b->free) {
                /* if ((b->size - s) >= (BLOCK_SIZE + 8)) {
                    split_block(b, s);
                    }*/
                b->free = 0;
                pthread_mutex_unlock(&malloc_lock1);
            }
            else {
                pthread_mutex_unlock(&malloc_lock1);
                return ts_malloc(size);
            }
            
        }
        else {
            pthread_mutex_lock(&malloc_lock2);
            if (curr->next == NULL) {
                b = extend_heap(curr, s);
                if (!b) {
                    return NULL;
                }
            }
            else {
                while (curr->next != NULL) {
                    curr = curr->next;
                }
                b = extend_heap(curr, s);
                if (!b) {
                    pthread_mutex_unlock(&malloc_lock2);
                    return NULL;
                }
            }
            pthread_mutex_unlock(&malloc_lock2);
        }
    }
    else {
        pthread_mutex_lock(&malloc_lock2);
        if (!first_block) {
            b = extend_heap(NULL, s);
            if (!b) {
                return NULL;
            }
            first_block = b;
            pthread_mutex_unlock(&malloc_lock2);
        }
        
        else {
            pthread_mutex_unlock(&malloc_lock2);
            return ts_malloc(size);
        }
        
        
    }
    
    return (b + 1);
}


    
void ts_free(void *ptr) {
    if (!ptr) {
        return;
    }
    block *b;
    b = get_block(ptr);
    pthread_mutex_lock(&malloc_lock1);
    // assert(b->free == 0);
    if (b->free == 1) {
        perror("double free\n");
        return;
    }
    b->free = 1;
    
    /* if (b->prev && b->prev->free) {
            b = merge(b->prev);
    }
    if (b->next) {
        b = merge(b);
        }*/
    pthread_mutex_unlock(&malloc_lock1);
    
}


