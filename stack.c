#include <stdio.h>
#include <stdlib.h>

#define INSERT_CHAR	1
#define DEL_CHAR	2
#define DEL_LINE	3
#define MAX_DATA_IN_ONE_NODE	10

typedef struct data {
	char arr[MAX_DATA_IN_ONE_NODE];
	struct data *next;
} data;


typedef struct position {
	int x, y;
} position;

typedef struct node {
	data *dta;
	char operation, freq;
	position final_pos;
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


void push(stack *st, node *nn) {
	if(isEmpty(*st)) {
		*st = nn;
		return;
	}
	nn->next = *st;
	*st = nn;
	return;
}



node pop(stack *st) {
	node empty, res;
	node *tmp;
	// to indicate stack is empty
	empty.operation = -1;
	if(isEmpty(*st))
		return empty;
	res = *(*st);
	tmp = *st;
	*st = (*st)->next;
	free(tmp);
	return res;
}


node peek(stack st) {
	if(st)
		return *st;
	else {
		node empty;
		empty.operation = -1;
		return empty;
	}	
}

void free_stack(stack *st) {
	while(!isEmpty(*st))
		pop(st);
	return;
}


void store_info(stack *st, int pos_changed, char data_c, char operation, char freq, int x, int y) {
	node tmp = peek(*st);
	// if stack is empty or operation is different than prev operation or position is changed then
	// add info as new node in stack
	if(isEmpty(*st) || tmp.operation != operation || pos_changed) {
		// malloc new node to store info
		node *nn = (node*)malloc(sizeof(node));
		if(operation == DEL_CHAR) {
			nn->dta = (data*)malloc(sizeof(data));
			nn->freq = 0;
			(nn->dta->arr)[(nn->freq)++] = data_c;
			nn->operation = operation;
			(nn->final_pos).x = x;
			(nn->final_pos).y = y;
			push(st, nn);
			return;
		}
		else {
			nn->dta = NULL;
                        nn->freq = 1;
                        nn->operation = operation;
                        (nn->final_pos).x = x;
                        (nn->final_pos).y = y;
                        push(st, nn);
                        return;
		}
	}

	if(tmp.operation == DEL_CHAR) {
		int indx = tmp.freq % MAX_DATA_IN_ONE_NODE;
		if(indx) {
			((*st)->dta->arr)[indx++] = data_c;
			(*st)->freq += 1;
			((*st)->final_pos).x = x;
			((*st)->final_pos).y = y;
			return;
		}
		else {
			data *new = (data*)malloc(sizeof(data));
			new->next = NULL;
			(new->arr)[indx] = data_c;
			new->next = (*st)->dta;
			(*st)->dta = new;
			(*st)->freq += 1;
			((*st)->final_pos).x = x;
			((*st)->final_pos).y = y;
			return;
		}
	}
	else {
		if(! pos_changed) {
			(*st)->freq += 1;
			((*st)->final_pos).x = x;
			((*st)->final_pos).y = y;
		}
	}
}


void undo(stack *st, win *w, int *line_no, int *position, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	node tmp = peek(*st);
	switch(tmp.operation) {
		case INSERT_CHAR:
			del_from_pos(w, line_no, position, fd_store_prev, fd_store_next, fd_main);
	}
}

int main() {
	/*
	stack s;
	init(&s);
	push(&s, 5);
	//push(&s, 9);
	pop(&s);
	printf("%d\n", pop(&s));
*/
	return 0;
}


