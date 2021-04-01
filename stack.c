#include <stdio.h>
#include <stdlib.h>

#define INSERT_CHAR	1
#define DEL_CHAR	2
#define DEL_LINE	3
#define INSERT_NEW_LINE	4
<<<<<<< HEAD
#define LOAD_NEXT_LINE	5
#define LOAD_PREV_LINE	6
=======
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
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


void store_info(stack *st, int pos_changed, char data_c, char operation, int x, int y) {
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

	// if operation is del_char then data must be stored in 
	// stack to undo whenever required
	if(tmp.operation == DEL_CHAR) {
		int indx = tmp.freq % MAX_DATA_IN_ONE_NODE;
		if(indx) {
			// fill data
			((*st)->dta->arr)[indx++] = data_c;
			// increase freq count
			(*st)->freq += 1;
			// update final position
			((*st)->final_pos).x = x;
			((*st)->final_pos).y = y;
			return;
		}
		else {
			// malloc new node to store data as size is reached to threshold
			data *new = (data*)malloc(sizeof(data));
			new->next = NULL;
			// fill data
			(new->arr)[indx] = data_c;
			// adjust pointer(stack using linked list)
			new->next = (*st)->dta;
			(*st)->dta = new;
			// increase freq by one
			(*st)->freq += 1;
			// change final position
			((*st)->final_pos).x = x;
			((*st)->final_pos).y = y;
			return;
		}
	}
	// for other operations 
	else {
		// increase freq count
		(*st)->freq += 1;
		// update final position
		((*st)->final_pos).x = x;
		((*st)->final_pos).y = y;
	}
}


<<<<<<< HEAD
void undo(stack *st, win *w, int *line_no, int* win_line, int *position, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	if(isEmpty(*st))
		return;
	node tmp = peek(*st);
	int tmp_wl = *win_line;
	// goto the position where last operation was performed
	*win_line = (*st)->final_pos.x;
=======
void undo(stack *st, win *w, int *line_no, int *position, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	if(isEmpty(*st))
		return;
	node tmp = peek(*st);
	// goto the position where last operation was performed
	*line_no = (*st)->final_pos.x;
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
	*position = (*st)->final_pos.y;

	switch(tmp.operation) {
		case INSERT_CHAR:
			// undo
			while( (*st)->freq-- )
<<<<<<< HEAD
				del_from_pos(w, win_line, position, fd_store_prev, fd_store_next, fd_main);
			pop(st);
			*line_no += (*win_line - tmp_wl);
=======
				del_from_pos(w, line_no, position, fd_store_prev, fd_store_next, fd_main);
			pop(st);
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
			break; 

		case INSERT_NEW_LINE:
                        // undo
                        while( (*st)->freq-- )
<<<<<<< HEAD
                                del_from_pos(w, win_line, position, fd_store_prev, fd_store_next, fd_main);
                        pop(st);
			*line_no += (*win_line - tmp_wl);
=======
                                del_from_pos(w, line_no, position, fd_store_prev, fd_store_next, fd_main);
                        pop(st);
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
                        break;

		case DEL_CHAR: {
			while( (*st)->freq ) {
				if( (*st)->freq % MAX_DATA_IN_ONE_NODE == 0) {
                                        data *tmp = (*st)->dta;
                                        (*st)->dta = (*st)->dta->next;
                                        free(tmp);
                                }
				int indx =((*st)->freq-1) % MAX_DATA_IN_ONE_NODE;
				char data = ((*st)->dta->arr)[indx];

<<<<<<< HEAD
				int h_indx = head_index(*w, *win_line);
=======
				int h_indx = head_index(*w, *line_no);
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
				(w->head)[h_indx].line_size++;
				insert_at_pos(&((w->head)[h_indx].line), (*position)++, data);
				(*st)->freq--;
			}
			pop(st);
<<<<<<< HEAD
			*line_no += (*win_line - tmp_wl);
=======
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
			break;
		}

		case DEL_LINE:
                        // undo
                        while( (*st)->freq-- )
<<<<<<< HEAD
                                insert_new_line_at_pos(w, win_line, position, fd_store_prev, fd_store_next, fd_main);
                        pop(st);
			*line_no += (*win_line - tmp_wl);
                        break;

		case LOAD_NEXT_LINE:
			while( (*st)->freq-- ) {
                                load_prev_line(w, fd_store_prev, fd_store_next);
				(*line_no)--;
			}
                        pop(st);
                        break;

		case LOAD_PREV_LINE:
			while( (*st)->freq-- ) {
                                load_next_line(w, fd_store_prev, fd_store_next, fd_main);
                                (*line_no)++;
                        }
                        pop(st);
                        break;
=======
                                insert_new_line_at_pos(w, line_no, position, fd_store_prev, fd_store_next, fd_main);
                        pop(st);
                        break;
			
>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
	}
}

/*
int main() {
<<<<<<< HEAD
=======

>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
	stack s;
	init(&s);
	push(&s, 5);
	//push(&s, 9);
	pop(&s);
	printf("%d\n", pop(&s));
<<<<<<< HEAD
=======

>>>>>>> ac5280ee8b130e7d9f0090ce3ba06c5fac39cb60
	return 0;
}
*/
