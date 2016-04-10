/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
  q->head = NULL;
  q->comparerfunc = comparer;
  

}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  /*
	if (q->head == NULL){
    struct node *tmp = malloc(sizeof(struct node));

    tmp->data = ptr;
    tmp->prev = NULL;
    tmp->next = NULL;
    q->head = tmp;
    return 0;
  }else{//need to compare
    int i = 0;
    struct node *tmp = q->head;
    struct node *nxt = q->head;

    while (tmp->next != NULL || q->comparerfunc(tmp->data,ptr)){
      tmp = tmp->next;
      i++;
    }
    nxt = tmp->next;
    tmp->next = malloc(sizeof(struct node));
    tmp->next->data = ptr;
    tmp->next->prev = tmp;
    tmp->next->next = nxt;
    return i;
  }

  return -1;
*/
//TODO: not working yet...
  
  int i = 0;
  int set = 0;
  struct node *tmp = q->head;
  struct node *prev = NULL;
  while(tmp != NULL){
    if(q->comparerfunc(tmp->data,ptr)>0){
        struct node *newnode = malloc(sizeof(struct node));
        newnode->data = ptr;
        newnode->next = tmp;
        if (i != 0){
        newnode->prev = tmp->prev;//
        newnode->prev->next = newnode;
        }else{
          q->head = newnode;
          newnode->prev = NULL;
        }
        tmp->prev = newnode;
        set = 1;
      break;
    }
    prev = tmp;
    tmp = tmp->next;
    i++;
  }
  if (set == 0){
    struct node *newnode = malloc(sizeof(struct node));
    newnode->data = ptr;
    newnode->next = NULL;
    if (i != 0){
    prev->next = newnode;
    newnode->prev = prev;
    }else{
      q->head = newnode;
      newnode->prev = NULL;
    }
  }
return i;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
  if(q->head == NULL){
    return NULL;
  }
  else{
    return q->head->data;
  }
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
  if (q->head == NULL){
    return NULL;
  }
  return priqueue_remove_at(q,0);

	
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  if (q->head == NULL){
    return NULL;
  }
  int i = index;
  struct node *tmp = q->head;
  while(i > 0 && tmp->next != NULL){
    tmp = tmp->next;
    i--;
  }
  if(i > 0){
    return NULL;
  }
  return tmp->data;

}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
  struct node *tmp = q->head;
  int i = 0;
  int num = 0;
  while(tmp != NULL){
    if(tmp->data == ptr){
      //tmp=tmp->next;
      priqueue_remove_at(q,i);
      num++;
    }else{
      i++;
      tmp=tmp->next;
    }

  }
  return num;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{

  struct node *tmp = q->head;
  if (tmp == NULL){
    return NULL;
  }
  if (index != 0){
  for(int i = index; i>0; i--){
    if(tmp->next == NULL){
      printf("No element at index!\n");
      return NULL;
    }
    tmp = tmp->next;
  }
  if (tmp->prev != NULL){
    tmp->prev->next = tmp->next;
  }
  if (tmp->next != NULL){
    tmp->next->prev = tmp->prev;
  }
  }else{
    q->head = tmp->next;
    if(q->head != NULL){
    q->head->prev = NULL;
    }
  }
  void * dta = tmp->data;
  free(tmp);

	return dta;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
  int i = 0;

  if(q->head == NULL){
    return i;
  }
  else{
    struct node *tmp = q->head;
    i++;
    while(tmp->next != NULL){
      i++;
      tmp = tmp->next;
    }
    return i;
  }

}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
  while(priqueue_remove_at(q,0) != NULL){
  }
  //free(q);
}
