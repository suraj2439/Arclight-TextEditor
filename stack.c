#include <stdio.h>
#include <stdlib.h>


typedef struct node {
	int x, y;
	int operation;
	struct node *next;
} node;

typedef node* stack;

void init(stack *st) {
	*st = NULL;
	return;
}

int isEmpty(stack st) {
	return (st == NULL) ? 1:0;
}

void push(stack *st, int data) {
	node *nn;
	nn = (node*)malloc(sizeof(node));
	nn->data = data;
	nn->next = NULL;

	if(isEmpty(*st)) {
		*st = nn;
		return;
	}
	nn->next = *st;
	*st = nn;
	return;
}

int pop(stack *st) {
	if(isEmpty(*st))
		return 0;
	int p = (*st)->data;
	node *tmp = *st;
	*st = (*st)->next;
	free(tmp);
	return p;
}


void free_stack(stack *st) {
	while(!isEmpty(*st)) {
		pop(st);
	}
	return;
}

int main() {
	stack s;
	init(&s);
	push(&s, 5);
	push(&s, 9);
	pop(&s);
	printf("%d\n", pop(&s));

	return 0;
}


