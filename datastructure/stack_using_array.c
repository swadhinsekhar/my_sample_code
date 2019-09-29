/*
* STACK Operation : Array Representation
* LIFO - Last In - Fast Out
*
*  ADT Stack (Abstraction Datatype Stack)
*  Data:-
*    1. space for storing elements
*    2. top pointers
*  Operations:-
*    1. push(x)
*    2. pop()
*    3. peek(index)   //Looking at a value at a given position from the top
*    4. stack_top()
*    5. is_empty()
*    6. is_full()
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __stack
{
    int     size;   //Max sizeof an array
    int     top;    //track of the top
    int     *s;     //array
} stack;

void print_stack(stack *st, char *mark_ops)
{
    int     i;

    if(st && st->s) {
        printf("%s: ", mark_ops);
        for(i=0; i<st->size; i++) {
            printf("%d ", st->s[i]);
        }
        printf("\n");
    }
}

/*
* val = top - pos + 1
*/
int peek(stack *st, int pos)
{
    int     x = -1;

    if((st->top - pos +1) < 0) {
        printf("Unable to peek at pos : %d\n", pos);
    } else {
        x = st->s[st->top - pos + 1];
    }

    return x;
}

void peek_loop(stack *st, int count)
{
    int i, ret = -1;

    for(i=0; i<count; i++) {
        if((ret = peek(st, i)) < 0);
        else
            printf("peek at pos : %d val : %d\n", i, ret);
    }
}

int pop(stack *st)
{
    int     x = -1;

    if(st->top == -1) {             //is_empty()
        printf("stack is empty\n");
    } else {
        x = st->s[st->top];         //stack_top()
        printf("deleting at index : %d - val : %d\n", st->top, x);
        st->s[st->top] = 0;
        st->top--;
    }

    return x;
}

int push(stack *st, int val)
{
    if(st->top == st->size-1) {     //is_full()
        printf("Stack Overflow\n");
        return -1;
    } else {
        st->top++;
        st->s[st->top] = val;
        printf("pushed at pos : %d val : %d\n", st->top, val);
    }

    return 0;
}

int stack_create(stack *st)
{
    st->size = 5;
    st->top = -1;
    st->s = (int *) malloc(sizeof(st->size));
    if(NULL == st->s) {
        printf("Out of memory\n");
        return -1;
    }
    memset(st->s, 0, sizeof(st->s));

    return 0;
}

int main()
{
    stack   __st, *st;

    st = &__st;

    if(stack_create(st) < 0) {
        printf("Unable to create stack\n");
        return -1;
    }

    push(st, 10);
    push(st, 20);
    push(st, 30);
    push(st, 40);
    push(st, 50);
    push(st, 61);
    push(st, 71);
    print_stack(st, "push");

    peek_loop(st, 6);

    pop(st);
    pop(st);
    pop(st);
    pop(st);
    print_stack(st, "pop");

    peek_loop(st, 6);

    pop(st);
    pop(st);
    print_stack(st, "pop");

    peek_loop(st, 6);
}

