#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
struct metadata_tag{
  size_t byte_num;
  bool isfree;
  void * next;
  void * data;
  
};
typedef struct metadata_tag metadata_t;

void * ff_malloc(size_t size);

void ff_free(void * ptr);

void * bf_malloc(size_t size);

void bf_free(void * ptr);
unsigned long get_data_segment_free_space_size();
  unsigned long get_data_segment_size();
