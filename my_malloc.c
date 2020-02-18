#include<unistd.h>
#include<stdbool.h>
#include "my_malloc.h"
#include <stdio.h>
#include<limits.h>
static metadata_t * startptr = NULL;
static metadata_t * endptr = NULL;
static metadata_t * memorystart = NULL;
int allocate_num=1;

//to see if somewhere could fit in new entering size
void * check_bf(size_t size){
  metadata_t * curr = startptr;
  size_t min = INT_MAX;
  metadata_t * nextisbest = NULL;
  metadata_t * new =NULL;
  if(startptr==NULL){
    return NULL;
  }
  
  //start search from the next of startptr,the startptr will not be reallocate
  while(curr->next!=endptr&&curr->next!=NULL){
       if(((metadata_t *)(curr->next))->byte_num>=size && ((metadata_t *)(curr->next))->byte_num-size<min ){
        if(((metadata_t *)(curr->next))->byte_num==size){
	 min = 0;
	 nextisbest = curr;
	 break;
       }
       
      min = ((metadata_t *)(curr->next))->byte_num-size;
      nextisbest = curr;
    }
    curr = curr->next;
  }
    
  if(nextisbest!=NULL){  
   //current bytenum can only fit in current data
    metadata_t * best = nextisbest->next;
    if(best->byte_num<= (size+sizeof(metadata_t))){     
         best->isfree = false;
	 nextisbest->next = best->next; 
         return best->data;
	   }
    
       //can split the free space
    else if(best->byte_num> (size+sizeof(metadata_t))){
       
        new = (char*)best+sizeof(metadata_t)+best->byte_num-size-sizeof(metadata_t);
      	new->byte_num = size;
      	new->isfree = false;
      	new->data =(char *)new + sizeof(metadata_t);
      	best->isfree= true;
        best->byte_num = best->byte_num - sizeof(metadata_t) - size;
	
        return new->data;
      }
  }
  return NULL;
}

void bf_free(void * ptr){
  
  metadata_t * curr = (char *)ptr - sizeof(metadata_t);
  curr->isfree = true;
 
  //when there is no node in the free list
  if(startptr==NULL){
    startptr = curr;
    startptr->next = NULL;
    endptr = startptr;
  }
 
  //add to the end of freelist
  else{    

    //if the freed node is the next node of startptr
  if(startptr->next==NULL){
    startptr->next = curr;
  }
    endptr->next = curr;
    endptr = curr;
    endptr->next = NULL;
  }
 
  // merge when two nodes are physically adjacent   
  if(((metadata_t*)((char*)ptr+curr->byte_num))->isfree==true){
  metadata_t * traverse = startptr;
  while(traverse->next!=endptr&&traverse->next!=NULL){
    if((traverse->next)==((char*)ptr+curr->byte_num)){
      curr->byte_num = curr->byte_num+ ((metadata_t *)(traverse->next))->byte_num+sizeof(metadata_t);
     traverse->next =((metadata_t *)(traverse->next))->next;
     break;
    }
    traverse = traverse->next;
  }
}
  
}

//use sbrk() to allocate memory
metadata_t * allocate(size_t size){

  allocate_num++;
  metadata_t * ptr = sbrk(sizeof(metadata_t)+size);	
	ptr->byte_num = size;
	ptr->isfree = false;
	ptr->data = (char*)ptr+sizeof(metadata_t);
	ptr->next = NULL;
	if(allocate_num==2){
           memorystart = ptr;
       }
	return ptr->data;
}

void * bf_malloc(size_t size){

  void * insert_place = check_bf(size);
  if(insert_place == NULL){
  insert_place = allocate(size);
  }
   return insert_place;
 
}


void * check_ff(size_t size){
 metadata_t * curr = startptr;
  metadata_t * nextisbest = NULL;
  metadata_t * new =NULL;
  
  if(startptr==NULL){
    return NULL;
  }
  
  //start search from the next of startptr,the startptr will not be reallocate
  while(curr->next!=endptr&&curr->next!=NULL){   
    if(((metadata_t *)(curr->next))->byte_num>=size){
      nextisbest = curr;
      break;
    }
    curr = curr->next;
  }
    
  if(nextisbest!=NULL){  
   //current bytenum can only fit in current data
    metadata_t * best = nextisbest->next;
    if(best->byte_num<= (size+sizeof(metadata_t))){     
         best->isfree = false;
	 nextisbest->next = best->next; 
         return best->data;
	   }
    
       //can split the free space
    else if(best->byte_num> (size+sizeof(metadata_t))){
       
        new = (char*)best+sizeof(metadata_t)+best->byte_num-size-sizeof(metadata_t);
      	new->byte_num = size;
      	new->isfree = false;
      	new->data =(char *)new + sizeof(metadata_t);
       	//best is still free and best's next unchange
      	best->isfree= true;
        best->byte_num = best->byte_num - sizeof(metadata_t) - size;
	
        return new->data;
      }
  }
  return NULL;
}



void * ff_malloc(size_t size){
 
  void * insert_place = check_ff(size);
  if(insert_place == NULL){
  insert_place = allocate(size);
  }
   return insert_place;

}


void ff_free(void * ptr){
  bf_free(ptr);
}

unsigned long get_data_segment_size(){
  unsigned long start = (char*)memorystart;
  unsigned long sum = (unsigned long)sbrk(0)- start;
  return sum;
}


unsigned long get_data_segment_free_space_size(){
  unsigned long sum = 0;
  metadata_t * curr = startptr;
  while(curr!=endptr->next){
    sum += sizeof(metadata_t)+curr->byte_num;
    curr = curr->next;
  }
  return sum;
}
  
