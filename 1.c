#include <stdio.h>
#include <stdlib.h>
#define MAX_CHAR_IN_LINE	10
typedef struct line {
        char *curr_line;
/*if line is big store next remaining line*/
        struct line *rem_line;
} line;


/* struct for one line,
 * contain pointer of correspondig line */
typedef struct node_l { 
	struct node_l* next;
	struct node_l* prev;
	line line;
} node_l;

typedef struct win {
	node_l *head;
	int tot_lines;
	int row_ptr, col_ptr;
	struct win *next_buff, *prev_buff;
	int next_buff_load_rem, prev_buff_load_rem;
} win;


void insertBegin(node_l** start, line line) {
	node_l* new_node = (node_l*)malloc(sizeof(node_l));

	if(new_node) {
		new_node->next = new_node->prev = NULL;
		new_node->line = line;
	}
	else return;

	if(*start == NULL) {
		*start = new_node;
		(*start)->next = (*start)->prev = *start;
		return;
	}

	// Pointer points to last Node
	node_l *last = (*start)->prev;

	// setting up previous and next of new node
	new_node->next = *start;
	new_node->prev = last;

	// Update next and previous pointers of start
	// and last.
	last->next = (*start)->prev = new_node;
 
	// Update start pointer
	*start = new_node;
	return;
}


/*use to initialise window with nex and prev buff*/
void init_window(win *w, int tot_lines) {
	line empty_line;
	empty_line.curr_line = NULL;
	empty_line.rem_line = NULL;

	win next, prev;
	next.head = prev.head = NULL;

	/*initialise window structure*/
	w->head = NULL;
	w->tot_lines = tot_lines;
	w->next_buff = &next;
	w->prev_buff = &prev;

	w->next_buff_load_rem = w->prev_buff_load_rem = 0;

	for(int i=0; i<tot_lines; i++)
		insertBegin(&w->head, empty_line);
	for(int i=0; i<tot_lines; i++)
                insertBegin(&w->prev_buff->head, empty_line);
	for(int i=0; i<tot_lines; i++)
                insertBegin(&w->next_buff->head, empty_line);
	return;
}

/*
//should be use in main fun to initalise window with next and prev buff
void init_window(win *w, int tot_lines) {
	win_buff *window;
	window->head = w->head;
	window->tot_lines = tot_lines;
	window->load_rem = 0;

	//init main window
	init_one_win(window, tot_lines);
	//init prev window
	init_one_win(w->prev_buff, tot_lines);
	//init next window
	init_one_win(w->next_buff, tot_lines);
	return;
}
*/

line init_line() {
	line empty_line;
	empty_line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_LINE);
	empty_line.rem_line = NULL;
	return empty_line;
}

FILE* load_file(win *w, char *filename) {
	exit(1);
	FILE *fd;
	char c;
	char arr[MAX_CHAR_IN_LINE];
	line *empty_line;

	node_l *line_node;
	/*use to point node of current line*/
	line_node = w->head;
	line *lne = &(line_node->line);

	fd = fopen(filename, "r");
	for(int i = 0; i < w->tot_lines; i++) {
		int indx = 0;
		lne->curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_LINE);
		while(c = fgetc(fd)) {
			if(indx == MAX_CHAR_IN_LINE) {
				empty_line = (line*)malloc(sizeof(line));

				*empty_line = init_line();
				lne->rem_line = empty_line;
				lne = lne->rem_line;
				indx = 0;
			}
			
			lne->curr_line[indx++] = c;
			if(c == '\n') {
				lne->curr_line[indx] = '\n';
				break;
			}
		}
		line_node = line_node->next;
		lne = &(line_node->line);
	}

	return fd;	
}


void print(win w) {
	node_l *line_node;
        line_node = w.head;
        line *lne = &(line_node->line);

	for(int i = 0; i < w.tot_lines; i++) {
		char c = 1;
		int indx = 0;
                while(c != '\n') {
                        if(indx == MAX_CHAR_IN_LINE) {
				if(lne->rem_line == NULL)
					break;
                                lne = lne->rem_line;
                                indx = 0;
                        }
                        if(indx < MAX_CHAR_IN_LINE)
                                c = lne->curr_line[indx++];
			printf("%c", c);
                }
		line_node = line_node->next;
		lne = &(line_node->line);
        }
}


void free_rem_lines(line* lne) {
	line* tmp_1 = lne->rem_line;
	line* tmp_2;
	while(tmp_1) {
		if(tmp_1->rem_line) 
			tmp_2 = tmp_1->rem_line;
		else tmp_2 = NULL;
		free(tmp_1);
		tmp_1->rem_line = NULL;
		tmp_1 = tmp_2;
	}
	lne->rem_line = NULL;
	return;
}


/*
 * used to load next line at last, after removing first line.
 * fdd: file descripter to store line which is to be removed.
 */
void load_next_line(win *w, FILE *fdd) {
	node_l* tmp;
	tmp = w->head;
	w->head = w->head->next;

	line *data_line = &tmp->line;
	int indx = 0;
	char data = data_line->curr_line[indx++];
	while(data != '\n') {
		fputc(data, fdd);
		if(indx == MAX_CHAR_IN_LINE) {
			data_line = data_line->rem_line;
			indx = 0;
		}
		data = data_line->curr_line[indx++];
		if(indx == 0) {
			printf("%c ", data);
			exit(1);
		}
	}
	/*to store last \n at the end of line*/
	fputc('\n', fdd);

	free_rem_lines(&tmp->line);
	return;
}

int main() {
	win window_1;
	init_window(&window_1, 10);

	/*
	load_file(&window_1, "cpy2.c");
	print(window_1);
	FILE *fdd;
	fdd = fopen("hello.txt", "w+");
	load_next_line(&window_1, fdd);
	load_next_line(&window_1, fdd);
	*/
	return 0;
}
