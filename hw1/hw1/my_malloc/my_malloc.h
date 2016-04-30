#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__
#include<stdio.h>
#include<unistd.h>
#include<assert.h>

struct _block {
    size_t size;
    struct _block *prev;
    struct _block *next;
    long free;
    //void *data;
};
typedef struct _block block;

#define BLOCK_SIZE sizeof(block)           //meta block size

size_t align8(size_t s);

block *ff_find_block(block **curr, size_t size);

block *extend_heap(block *curr, size_t s);

void split_block(block *b, size_t s);

void *ff_malloc(size_t size);

void *bf_malloc(size_t size);

void *wf_malloc(size_t size); 

block *get_block(void *p);

block *merge(block *b) ;

void ff_free(void *ptr);

void bf_free(void *ptr);

void wf_free(void *ptr);

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space_size();

void *ts_malloc(size_t size);

void ts_free(void *ptr);


#endif
