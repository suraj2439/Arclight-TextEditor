#include <stdio.h>
#include <stdlib.h>

#define MAX_CHAR_IN_SUBLINE	5
#define MAX_CHAR 		-128

typedef struct line {
        char *curr_line;
	// gap buffer left and right pointers
	int gap_left, gap_right;
	// size of gap_buffer
	int gap_size;
        // if line is big store next remaining line
        struct line *rem_line;
} line;


/* struct for one line,
 * contain pointer of correspondig line */
typedef struct node_l { 
	line line;
} node_l;


typedef struct win {
	node_l *head;
	int tot_lines, head_indx;
} win;


// Used to move the gap left in the array
// Assuming that position should be less than size of array
void move_gap_left(line *lne, int position) {
	char *buffer = lne->curr_line;
	int *gap_left = &(lne->gap_left);
	int *gap_right = &(lne->gap_right);

	if(lne->gap_size == 0) {
		*gap_left = position;
		*gap_right = position - 1;
	}
	// Move the gap left character by character
	while (position < (*gap_left)) {
		(*gap_left)--;  
		(*gap_right)--;  
		buffer[(*gap_right) + 1] = buffer[*gap_left]; 
	}
	return;	
}  


// Used to move the gap right in the array
void move_gap_right(line *lne, int position) {
	// extract data from line structure to handle gap 
	char *buffer = lne->curr_line;
        int *gap_left = &(lne->gap_left);
        int *gap_right = &(lne->gap_right);

	// if gap size is zero
	if(lne->gap_size == 0) {
		if(position >= MAX_CHAR_IN_SUBLINE) {
			*gap_left = MAX_CHAR_IN_SUBLINE;
			*gap_right = MAX_CHAR_IN_SUBLINE - 1;
			return;
		}
		*gap_left = position;
		*gap_right = position - 1;
		return;
	}

	// Move the gap right character by character   
	while (position > (*gap_left)) {  
		(*gap_left)++;
                (*gap_right)++;
		// For testing
		// TODO modify this
		if(*gap_right >= MAX_CHAR_IN_SUBLINE) {
			(*gap_left)--;
			(*gap_right)--;
			//printf("already at last cannot move further right return from here\n");
			return;
		}

		buffer[(*gap_left) - 1] = buffer[*gap_right];
	}
	return;
}  


/* Move mouse cursor to particular position
 * RETURN:- line pointer with adjusting gap buffer, 
 * at that point data can be added or can be removed
 * NOTE:- position should belong to same block.
 */
line* move_cursor(line *subline, int position) {
	line *lne = subline;
	// take next subline till position doesn't belong to corresponding curr_line
	while((MAX_CHAR_IN_SUBLINE - lne->gap_size) < position) {
		// position not belong to current subline take next line
		// decrease position
		position -= (MAX_CHAR_IN_SUBLINE - lne->gap_size);
		// if current subline is last
		if(lne->rem_line == NULL)
			break;
		// take next subline
		lne = lne->rem_line;
	}
	
	// Now position belongs to current subline
	// move cursor(gap) at appropriate position
	if (position < lne->gap_left)
		move_gap_left(lne, position);
	else if(position > lne->gap_right) 
		move_gap_right(lne, position);
	return lne;
}


// Function to insert character at position
void insert_at_pos(line *subline, int position, char data) {
	// move the cursor at appropriate position
	line *lne = move_cursor(subline, position);

	// if current subline is full, 
	// malloc new subline between current and next subline
	if(lne->gap_size == 0) {
		line *nl = (line*)malloc(sizeof(line));
		nl->curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
		nl->gap_left = 0;
		nl->gap_right = MAX_CHAR_IN_SUBLINE - 1;
		nl->gap_size = MAX_CHAR_IN_SUBLINE;

		int j = 0;
		for(int i = lne->gap_left; i < MAX_CHAR_IN_SUBLINE; i++) {
			// copy data from current subline to newly mallocated array
			// to grow gap
			insert_at_pos(nl, j, lne->curr_line[i]);
			lne->gap_size++;
			lne->gap_right++;
			j++;
		}
		// adjust pointers
		nl->rem_line = lne->rem_line;
		lne->rem_line = nl;

		// if cursor is at the last
		if(lne->gap_left >= MAX_CHAR_IN_SUBLINE)
			// take next subline
			lne = lne->rem_line;
	}
	
	// Insert the character in the gap and  
	// move the gap
	lne->curr_line[lne->gap_left] = data;
	lne->gap_left++;
	lne->gap_size--;
	return;
}



// TODO delete line if del position is 0
void del_from_pos(win *w, int line_no, int position) {
	int end = w->tot_lines - w->head_indx - 1;

	line *lne;
	if(line_no <= end)
		lne = &((w->head)[line_no].line);
	else
		lne = &((w->head)[line_no - w->tot_lines].line);

	//line *lne = (line_no <= end) ? &(w->head[line_no].line) : &(w->head[line_no - w->tot_lines]);
	line *prev_lne = NULL;
	
	// take next subline till position doesn't belong to corresponding curr_line
        while((MAX_CHAR_IN_SUBLINE - lne->gap_size) < position) {
                // position not belong to current subline take next line
                // if current subline is last
                if(lne->rem_line == NULL)
                        break;
                // take next subline
		prev_lne = lne;
                lne = lne->rem_line;
		// decrease position
                position -= (MAX_CHAR_IN_SUBLINE - lne->gap_size);
        }

	// Now position belongs to current subline
        // move cursor(gap) at appropriate position
        if (position < lne->gap_left)
                move_gap_left(lne, position);
        else if(position > lne->gap_right)
                move_gap_right(lne, position);

	// if left gap boundary is at 0, take previous subline
	// if prev subline not exist, then del not possible return
	if(lne->gap_left == 0) {
		if(prev_lne)
			lne = prev_lne;
		else
			return;
	}
	// increase left boundary of gap buff to del char at given pos
	lne->gap_left--;
	lne->gap_size++;
	return;
}


void init_gap_buff(line *lne) {
	// initially gap size = size of subline,
	// left will be at pos 0 and right at max-1
	lne->gap_left = 0;
	lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
	lne->gap_size = MAX_CHAR_IN_SUBLINE;
	return;
}


/*use to initialise window*/
void init_window(win *w, int tot_lines) {
	w->head = (node_l*)malloc(sizeof(node_l) * tot_lines);
	w->tot_lines = tot_lines;
	w->head_indx = 0;

	for(int i = 0; i < tot_lines; i++) {
		// init subline with array of constant size
		(w->head)[i].line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
		// init next subline pointer with NULL
		(w->head)[i].line.rem_line = NULL;
		init_gap_buff(&(w->head[i]).line);
	}
	return;
}


/*use to load text from file into ADT*/
FILE* load_file(win *w, char *filename) {
	FILE *fd_get = fopen(filename, "r");

	char c;
	line *lne, *empty_line;	
	for(int i = 0; i < w->tot_lines; i++) {
		lne = &((w->head)[i].line);
		int indx = 0;
		
		c = fgetc(fd_get);
        	// next line not present
        	if(c == -1) {
                	lne->curr_line[0] = MAX_CHAR;
                	return fd_get;
        	}
        	insert_at_pos(lne, indx++, c);
        	
		while((c = fgetc(fd_get)) != '\n')
                	insert_at_pos(lne, indx++, c);
	}

	return fd_get;	
}


/*use to print contents of ADT - for testing*/
void print(win w) {
	line *lne;
	int h_indx;
	for(int i = 0; i < w.tot_lines; i++) {
		h_indx = i;
		// circular array
		if(h_indx + w.head_indx >= w.tot_lines)
			h_indx = h_indx - w.tot_lines;

		lne = &((w.head)[h_indx].line);
		char c = 1;
		int indx = 0;

		if(lne->curr_line[0] == MAX_CHAR)
                        return;

                while(1) {
                        if(lne->gap_size != 0 && indx == lne->gap_left)
				indx = lne->gap_right + 1;
			if(indx == MAX_CHAR_IN_SUBLINE) {
				if(lne->rem_line == NULL) {
					printf("\n");
					break;
				}
                                lne = lne->rem_line;
                                indx = 0;
                        }
			
			c = lne->curr_line[indx++];
			printf("%c", c);
                }
        }
	return;
}


/*to free corresponding sublines*/
void free_line(line* lne) {
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
	free(lne->curr_line);
	lne->curr_line = NULL;
        return;
}



// extract line from ADT to tmp file
// suplimentary fun
void extract_line(node_l *tmp, FILE *fd_store) {
        line *data_line = &tmp->line;
	int indx = 0;

	if(data_line->gap_size != 0 && indx == data_line->gap_left)
		indx = data_line->gap_right + 1;
	char data = data_line->curr_line[indx++];

	while(1) {
		// store data in tmp_prev file
		fputc(data, fd_store);

		// skip gap 
		if(data_line->gap_size != 0 && indx == data_line->gap_left)
			indx = data_line->gap_right + 1;
		// if indx is at end of subline
		if(indx == MAX_CHAR_IN_SUBLINE) {
			// next subline not present append '\n' at end of line
			if(data_line->rem_line == NULL) {
				fputc('\n', fd_store);
				break;
			}
			// take next subline
			data_line = data_line->rem_line;
			indx = 0;
		}
		data = data_line->curr_line[indx++];
	}
	return;
}


/*
 * used to load next line at last, after removing first line.
 * fd_store_prev : file descripter to store line which is just to be removed from top of ADT
 * fd_store_next : file descripter to store line which is just to be removed from bottom of ADT
 * fd_get   : file descripter to pointing in main file.
 * */
void load_next_line(win *w, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {	
	/*extract first line of window from ADT to tmp file*/
        if(w->head->line.curr_line[0] != MAX_CHAR)
                extract_line(w->head, fd_store_prev);
	else return;

	// free mallocated sublines corresponding to data_line in ADT
	free_line(&w->head->line);

	// init new line at head
	w->head->line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
	w->head->line.rem_line = NULL;
	// init gap buffer of new line
	init_gap_buff(&w->head->line);

	// load next line from file
	line *lne = &(w->head)->line;
	char c;

	// if temp file in which next data is stored, has data extract from there, 
	for(int i=0; ; i++) {
                if(fseek(fd_store_next, -2, SEEK_CUR) == -1) {
                        if(fseek(fd_store_next, -1, SEEK_CUR) == -1)
                                break;
			return;
                }
                c = fgetc(fd_store_next);
                if(c != '\n')
			insert_at_pos(lne, 0, c);
		else return;
        }

	// Now if line is not present in filename_nxt.tmp file 
	// then extract from main file
	c = fgetc(fd_main);
	// next line not present
	if(c == -1) {
		lne->curr_line[0] = MAX_CHAR;
		return;
	}
	int indx = 0;
	insert_at_pos(lne, indx++, c);
	while((c = fgetc(fd_main)) != '\n')
		insert_at_pos(lne, indx++, c);

	w->head_indx = (w->head_indx + 1) % w->tot_lines;
	if(w->head_indx == 0)
		w->head -= (w->tot_lines-1);
	else w->head++;

	return;
}


void load_prev_line(win *w, FILE *fd_store_prev, FILE *fd_store_next) {
	// check if line is present in filename_prev.tmp file
	if(fseek(fd_store_prev, -1, SEEK_CUR) != -1)	// present
		fseek(fd_store_prev, 1, SEEK_CUR);
	// else return as line not present to load previous line
	else return;

	// point head pointer to previous line
	if(w->head_indx == 0) {
		w->head_indx = w->tot_lines - 1;
		w->head += (w->tot_lines - 1);
	}
	else {
		w->head_indx--;
		w->head--;
	}
	
	// extract line(prev) into filename_next.tmp
        if(w->head->line.curr_line[0] != MAX_CHAR)
                extract_line(w->head, fd_store_next);
        // free mallocated sublines corresponding to data_line in ADT
        free_line(&w->head->line);

	// init new line at head
        w->head->line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
        w->head->line.rem_line = NULL;
        // init gap buffer of new line
        init_gap_buff(&w->head->line);

        // load prev line from file
        line *lne = &(w->head)->line;
        char c;

        // if temp file in which previous data is stored, has data extract from there,
        for(int i=0; ; i++) {
                if(fseek(fd_store_prev, -2, SEEK_CUR) == -1) {
                        if(fseek(fd_store_prev, -1, SEEK_CUR) == -1)	// file is empty
                                break;
			else return;
                }
                c = fgetc(fd_store_prev);
                if(c != '\n')
                        insert_at_pos(lne, 0, c);
                else return;
        }
}


// NOTE:- first line is 0th line
void insert_new_line_at_pos(win *w, int line_no, int position, FILE *fd_prev, FILE *fd_nxt, FILE *fd_main) {
	// not last line
	if(line_no != w->tot_lines - 1) {
		// extract last line into filename_nxt.tmp file
		if(w->head_indx) {
			extract_line(&(w->head[-1]), fd_nxt);
			free_line(&(w->head[-1]).line);
		}
		else {
			extract_line(&(w->head[w->tot_lines - 1]), fd_nxt);
			free_line(&(w->head[w->tot_lines - 1]).line);
		}
		
		int curr, prev, end;
		end = w->tot_lines - w->head_indx - 1;
		prev = (w->head_indx) ? -1 : end - w->tot_lines - 1;
		curr = (prev - 1 < (-1*w->head_indx)) ? end : prev - 1;
		
		// shift line pointer by one unit to create space for blank line
		for(int i = 0; i < ( (w->tot_lines-1) - (line_no+1) ); i++) {
			(w->head)[prev].line = (w->head)[curr].line;
			prev--;
			prev = (prev < (-1*w->head_indx)) ? end : prev;
			curr = (prev - 1 < (-1*w->head_indx)) ? end : prev - 1;
		}
		
		// init new line
		(w->head)[prev].line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
		// init next subline pointer with NULL
                (w->head)[prev].line.rem_line = NULL;
		// init gap buffer
		init_gap_buff(&(w->head[prev]).line);

		// line in which new line to be inserted 
		line *lne = &(w->head[line_no]).line;
        	// take next subline till position doesn't belong to corresponding curr_line
	        while((MAX_CHAR_IN_SUBLINE - lne->gap_size) < position) {
       	        	// position not belong to current subline take next line
	                // if current subline is last
                	if(lne->rem_line == NULL)
                        	break;
                	// take next subline
                	lne = lne->rem_line;
                	// decrease position
                	position -= (MAX_CHAR_IN_SUBLINE - lne->gap_size);
		}

		lne = move_cursor(lne, position);
		position = lne->gap_right + 1;

		// insert data in next blank line 
		int indx = 0;
		while(position < MAX_CHAR_IN_SUBLINE) {
			if(position == lne->gap_left)
				position = lne->gap_right + 1;

			insert_at_pos(&((w->head)[prev].line), indx++, lne->curr_line[position++]);
		}
		(w->head)[prev].line.rem_line = lne->rem_line;
		
		// if next subline is present then point rem_line pointer to that subline 
		// so that copy operation will be saved
		lne->gap_size = MAX_CHAR_IN_SUBLINE - lne->gap_right;	// change gap_size
		// gap_right = end to indicate all data after position is deleted
		lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
		lne->rem_line = NULL;
	}

	else {
		extract_line(w->head, fd_prev);
		free_line( &(w->head->line) );

		int curr = -1, next, end;
                end = w->tot_lines - w->head_indx - 1;

                // shift line pointer by one unit to create space for blank line
                for(int i = 0; i < (w->tot_lines-1); i++) {
			curr++;
			curr = (curr <= end) ? curr : (-1*w->head_indx);
                        next = (curr + 1 <= end) ? curr+1 : (-1*w->head_indx);
                        (w->head)[curr].line = (w->head)[next].line;
                }

		// init new line
                (w->head)[next].line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
                // init next subline pointer with NULL
                (w->head)[next].line.rem_line = NULL;
                // init gap buffer
                init_gap_buff(&(w->head[next]).line);
		
		// line in which new line to be inserted
                line *lne = &(w->head[curr]).line;

                // take next subline till position doesn't belong to corresponding curr_line
                while((MAX_CHAR_IN_SUBLINE - lne->gap_size) < position) {
                        // position not belong to current subline take next line
                        // if current subline is last
                        if(lne->rem_line == NULL)
                                break;
                        // take next subline
                        lne = lne->rem_line;
                        // decrease position
                        position -= (MAX_CHAR_IN_SUBLINE - lne->gap_size);
                }

		lne = move_cursor(lne, position);
                position = lne->gap_right + 1;

                // insert data in next blank line
                int indx = 0;
                while(position < MAX_CHAR_IN_SUBLINE) {
                        if(position == lne->gap_left)
                                position = lne->gap_right + 1;

                        insert_at_pos( &((w->head)[next].line), indx++, lne->curr_line[position++] );
                }
                (w->head)[next].line.rem_line = lne->rem_line;

                // if next subline is present then point rem_line pointer to that subline
                // so that copy operation will be saved
                lne->gap_size = MAX_CHAR_IN_SUBLINE - lne->gap_right;   // change gap_size
                // gap_right = end to indicate all data after position is deleted
                lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
                lne->rem_line = NULL;
	}
	return;
}


int main() {
	win window_1;
	FILE *fd_store_prev, *fd_store_next, *fd_main;
	init_window(&window_1, 5);

	fd_main = load_file(&window_1,"1.c");
	/*
	printf("\n");
	insert_at_pos(&(window_1.head->line), 2, 'z');
	print(window_1);

	printf("\n");
        insert_at_pos(&(window_1.head->line), 2, 'k');
        print(window_1);
	
	printf("\n");
        insert_at_pos(&(window_1.head->line), 5, 'z');
	*/

	print(window_1);
	printf("fd\n");
	del_from_pos(&window_1,0, 1);
	printf("\n");
	print(window_1);

	/*
	fd_store_prev = fopen(".hi_pr.tmp", "w+");
	fd_store_next = fopen(".hi_nxt.tmp", "w+");

	print(window_1);
	printf("\n");

	for(int i = 0; i< 6; i++)
		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);

	print(window_1);
	printf("\n");
	insert_new_line_at_pos(&window_1, 4, 8, fd_store_prev, fd_store_next, fd_main) ;

	//load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);

	print(window_1);
	printf("\n");*/
	/*
	for(int i = 0; i< 10; i++)
		load_prev_line(&window_1, fd_store_prev, fd_store_next);
	print(window_1);
	*/
	return 0;
}
