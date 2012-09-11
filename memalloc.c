#include "memalloc.h"
#include <stdio.h>
#include <pthread.h>

struct list freelist;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

bool compare_elems(const struct list_elem *a,const struct list_elem *b, void *aux){
    return a <  b;
}

void mem_init(uint8_t *base, size_t length){
    list_init(&freelist);
    ((struct free_block *) base)->length = length;
    list_push_front(&freelist, &(((struct free_block *) base)->elem));
    list_sort(&freelist, &compare_elems, NULL);
}

void * mem_alloc(size_t reqSize){
	pthread_mutex_lock(&mutex1);
		
	size_t reqTotal = reqSize + sizeof(size_t);
	
	//check to see if the requested size is less than an allocatable block
	if (reqTotal < sizeof(struct free_block)){
		reqTotal = sizeof(struct free_block);
	}

	struct list_elem *curElem;
	
	for (curElem = list_begin (&freelist); curElem != list_end (&freelist);
         curElem = list_next (curElem)){

		struct free_block *f = list_entry (curElem, struct free_block, elem);
		size_t curSize = f->length;
		
		if (curSize >= reqTotal){
			if ((curSize - reqTotal) > sizeof(struct free_block)){//if the requested total would leave enough
				//for another free block, chip off a piece of free memory
				
				struct used_block *retMemory = (struct used_block *) ((void *) f + (curSize - reqTotal));
				//memory length INCLUDES length header
				retMemory->length = reqTotal;
				f->length = curSize - reqTotal;
								
				pthread_mutex_unlock(&mutex1);
				return (void *) (retMemory->data);
			} else{ //give the whole block
				struct used_block *retMemory = (struct used_block*) f;
				retMemory->length = curSize;
				list_remove(&f->elem);

				//release lock
				pthread_mutex_unlock(&mutex1);

				return (void *) (retMemory->data);
			}
		}
      }
	//release lock
	pthread_mutex_unlock(&mutex1);

	return NULL;
}

void mem_free(void *ptr){
	
	pthread_mutex_lock(&mutex1);
	struct used_block *used_block = (struct used_block *)(ptr - offsetof(struct used_block, data));
	void *end_of_used_block = (void*) used_block + used_block->length;
	
	struct list_elem *curElem;
	struct list_elem *prevElem = NULL;
	struct free_block *f = NULL;
	struct free_block *p = NULL;
	bool preMerge = false;
	
	if(list_empty(&freelist)){
		f = (struct free_block *) used_block;
		list_insert_ordered(&freelist, &f->elem, &compare_elems, NULL);
		pthread_mutex_unlock(&mutex1);
		return;
	} 
	
	struct free_block *nf = (struct free_block *) used_block;

	for (curElem = list_begin (&freelist); curElem != list_end (&freelist);
			 curElem = list_next (curElem)){
		
		f = list_entry (curElem, struct free_block, elem);
		preMerge = false;
	
		if ((size_t) used_block < (size_t) f){ //we know used block comes before beginning of curElem
			if (p != NULL){
				if ((void *) p + p->length == (void *) used_block){
					p->length += used_block->length;
					preMerge = true;
					nf = p;
				}
			}
			if (!preMerge){ //case where there is no previous element that has been merged 
				list_insert_ordered(&freelist, &nf->elem, &compare_elems, NULL);
			}
			
			if ((size_t) end_of_used_block == (size_t) f){ //used block is contiguous to free memory
				//must merge on right side of the used block
				nf->length += f->length;
				list_remove(&f->elem);
			}
			
			pthread_mutex_unlock(&mutex1);
			return;
		}
		prevElem = curElem;
		p = list_entry(prevElem, struct free_block, elem);
	}
		
	//reached end of list, need to add free memory to the end
	if ((void *) p + p->length == (void *) used_block){ 
		// prev free elem is contiguous to used_block on left side
		//of used_block
		p->length += used_block->length;
		nf = p;
	} else {
		//could have alternatively used list_insert_ordered, but it may be less efficient
		//especially when pushing to the end of the list, up to o(n)
		list_push_back(&freelist, &nf->elem);
	}
	pthread_mutex_unlock(&mutex1);
}

/* Return the number of elements in the free list. */ 
size_t mem_sizeof_free_list(void){
	
	return list_size(&freelist);
}

/* Dump the free list. */
void mem_dump_free_list(void){
	struct list_elem *curElem;
	for (curElem = list_begin (&freelist); curElem != list_end (&freelist);
         curElem = list_next (curElem)){
			printf("%p %zu\n", curElem, list_entry(curElem, struct free_block, elem)->length);
	}
}
