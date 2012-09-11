/* offset.c - test offset macro */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

/* List element. */
struct list_elem 
  {
    struct list_elem *prev;     /* Previous list element. */
    struct list_elem *next;     /* Next list element. */
  };

struct mynode {
    int x;
    int y;
    struct list_elem elem;
};

/*
#define NULL ((void *) 0)
*/
#define offsetof1(TYPE, MEMBER) ((size_t) &((TYPE *) 0)->MEMBER)

#define list_entry(LIST_ELEM, STRUCT, MEMBER)           \
        ((STRUCT *) ((uint8_t *) &(LIST_ELEM)->next     \
                     - offsetof1 (STRUCT, MEMBER.next)))

#define list_entry2(LIST_ELEM, STRUCT, MEMBER)          \
        ((STRUCT *) ((uint8_t *) LIST_ELEM              \
                     - offsetof1 (STRUCT, MEMBER)))

int main(int argc, char **argv)
{
    struct mynode node;
    struct mynode *np, *p1, *p2;

    np = &node;
    
    node.x = 3;
    node.y = 11;
    
    p1 = list_entry(&np->elem, struct mynode, elem);
    p2 = list_entry2(&np->elem, struct mynode, elem);
    
    printf("list_entry : %08X\n", (void *) p1);
    printf("list_entry2: %08X\n", (void *) p2);
    
    return 0;
    
}