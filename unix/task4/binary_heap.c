#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef char* element_t;

struct binary_heap {
    unsigned int size;
    unsigned int capacity;
    unsigned int base_capacity;
    int (*comparator) (element_t, element_t);
    element_t *elements;
};

void heap_push(struct binary_heap *heap, element_t element) {
    unsigned int i, new_capacity;
    element_t *new_buf, *old_buf;

    if (heap->capacity == heap->size) {
        new_capacity = heap->capacity * 2;
        printf("Realloc on heap push(), old capacity is %d, new capacity is %d\n", heap->capacity, new_capacity);

        new_buf = malloc(new_capacity * sizeof(element_t));
        if (!new_buf) {
            return;
        }

        memcpy(new_buf, heap->elements, heap->size * sizeof(element_t));
        heap->capacity = new_capacity;
        old_buf = heap->elements;
        heap->elements = new_buf;      
        free(old_buf);      
    }

    if (heap->size == 0) {
    	heap->size++;
    	heap->elements[0] = element;
    } else {  	
  		for(i = ++heap->size - 1; i > 0 && (*(heap->comparator))(heap->elements[i / 2], element); i /= 2) {
       		heap->elements[i] = heap->elements[i / 2];
   		}
   		heap->elements[i] = element;
   	}
}
 
element_t* heap_pop(struct binary_heap *heap) {
	element_t *top, *old_buf, *new_buf;
    unsigned int index, swap, other, new_capacity;

    if ((heap->size <= (heap->capacity / 4)) && (heap->size > heap->base_capacity)) {
    	new_capacity = heap->capacity / 2;
    	printf("Realloc on heap pop(), old capacity is %d, new capacity is %d\n", heap->capacity, new_capacity);
        heap->capacity = new_capacity;
        
        new_buf = malloc(heap->capacity * sizeof(element_t));
        if (!new_buf) {
            return;
        }

        memcpy(new_buf, heap->elements, heap->size * sizeof(element_t));
        
        old_buf = heap->elements;
        heap->elements = new_buf;
        
        free(old_buf);
    }

    if (heap->size == 0) {
     	return NULL;
    }

    top = malloc(sizeof(element_t*));
    *top = heap->elements[0];
    heap->size--;
       
    element_t last = heap->elements[heap->size];

    for(index = 0; 1; index = swap) {
        swap = index * 2 + 1;

        if (swap >= heap->size) {
            break;
        }

        other = swap + 1;
        if ((other < heap->size) && ((*(heap->comparator))(heap->elements[swap], heap->elements[other]))) {
            swap = other;
        }

        int res = (*(heap->comparator))(last, heap->elements[swap]);
        if (!res) { 
            break;
        } 

        heap->elements[index] = heap->elements[swap];
    }

    heap->elements[index] = last;

    return top;
}

element_t* heap_top(struct binary_heap *heap) {
	element_t *top = NULL;

	if (heap->size > 0) {
		top = &(heap->elements[0]);
	}

    return top;
}

int cmp(element_t first, element_t second) {
	return strcmp(first, second) > 0;
}

int main(void) {

	element_t *elem = NULL;



	struct binary_heap heap = {
		.size = 0,
		.capacity = 3,
		.base_capacity = 3,
		.comparator = &cmp,
		.elements = malloc(strings_count * sizeof(element_t))
	};

	/**heap_push(&heap, "123");*/
	heap_push(&heap, "1");	
	heap_push(&heap, "12");
	heap_push(&heap, "as");
	heap_push(&heap, "1as2");
	heap_push(&heap, "zdfsdasd");
	heap_push(&heap, "44");
	heap_push(&heap, "asddd");
	heap_push(&heap, "hghh");
	heap_push(&heap, "yt");
	heap_push(&heap, "0");
	heap_push(&heap, "555");
	
	heap_push(&heap, "2");
	heap_push(&heap, "1");
	heap_push(&heap, "11");
	heap_push(&heap, "12");
	heap_push(&heap, "113");
	heap_push(&heap, "1234");

	int i;
	for (i = 0; i < heap.size; i++) {
		printf("elem %s\n", heap.elements[i]);
	}

	while ((elem = heap_pop(&heap)) != NULL) {
		printf("top %s\n", *elem);
	}


	printf("%d\n", strcmp("212", "2"));
	return 0;
}