#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAX_CHAR_IN_SUBLINE	5
#define MAX_CHAR 		-128
#define BACKSPACE		8
#define DELETE			127

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
	int line_size;
} node_l;


typedef struct win {
	node_l *head;
	int tot_lines, head_indx;
} win;


void print(win w);
void move_gap_left(line *lne, int position) ;
void move_gap_right(line *lne, int position);
line* move_cursor(line *subline, int position);
void init_gap_buff(line *lne);
void print_loc(int x, int y);
void insert_at_pos(line *subline, int position, char data);
void del_from_pos(win *w, int *line_no, int *position, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main);
void init_window(win *w, int tot_lines);
FILE* load_file(win *w, char *filename);
void print_page(win w);
void print(win w);
void free_line(line* lne);
void load_next_line(win *w, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main);
void load_prev_line(win *w, FILE *fd_store_prev, FILE *fd_store_next);
void extract_line(node_l *tmp, FILE *fd_store);
void insert_new_line_at_pos(win *w, int *line_no, int *position, FILE *fd_prev, FILE *fd_nxt, FILE *fd_main);
int head_index(win w, int line_no);
int prev_line_into_data_struct(line *lne, FILE *fd_store_prev);
int next_line_into_data_struct(line *lne, int extract_from_next, FILE *fd_store_next, FILE *fd_main);
int check_next_line_available(FILE *fd_store_next, FILE *fd_main);
int check_prev_line_available(FILE *fd_store_prev);


// Used to move the gap left in the array
// Assuming that position belong to given array
void move_gap_left(line *lne, int position) {
	char *buffer = lne->curr_line;
	int *gap_left = &(lne->gap_left);
	int *gap_right = &(lne->gap_right);

	if(lne->gap_size == 0) {
		*gap_left = position;	//TODO changed (don)
		*gap_right = position-1;
		return;
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
		*gap_right = position -1;
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
	else 
		move_gap_right(lne, position);
	return lne;
}



void init_gap_buff(line *lne) {
        // initially gap size = size of subline,
        // left will be at pos 0 and right at max-1
        lne->gap_left = 0;
        lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
        lne->gap_size = MAX_CHAR_IN_SUBLINE;
        return;
}



void print_loc(int x, int y) {
        move(20, 20);
        mvprintw(10, 30, "x: %d y: %d", x, y);
}

void print_debug(int x, int y) {
        mvprintw(15, 60, "k: %d b: %d", x, y);
}


// Function to insert character at position
// NOTE: line_size should be modified by caller of this function it is not modified in this fun
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



void del_from_pos(win *w, int *lne_no, int *col_no, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	int line_no = *lne_no, position = *col_no;
	// if position not 0th position
	if(position) {
		// decrease col_no(used in gui)
		(*col_no)--;

		int l_no = head_index(*w, line_no);

        	line *lne = &((w->head)[l_no].line);

		(w->head)[l_no].line_size--;		// decrease line size

		// Now position belongs to current subline
        	// move cursor(gap) at appropriate position
		lne = move_cursor(lne, position);
		// increase left boundary of gap buff to del char at given pos
		lne->gap_left--;		//TODO decide what to do if only one character to remove now subline is empty
		lne->gap_size++;
		return;
	}

	// delete character from position 0
	else {
		// if first line
		if(line_no == 0) {
			// check if line is present in filename_prev.tmp file
			if(! check_prev_line_available(fd_store_prev))
				return;

			line tmp = w->head->line;
			int tmp_line_size = w->head->line_size;

			line *new_line = (line*)malloc(sizeof(line));
			// init new line
			new_line->curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
			// init next subline pointer with NULL
                        new_line->rem_line = NULL;
                        // init gap buffer
                        init_gap_buff(new_line);
                       
			(w->head->line) = *new_line;
			*new_line = tmp;
			// init line size
			w->head->line_size = 0;

			char c;

			line *lne = &(w->head->line);
			// load previous line from file and update line_size
			w->head->line_size = prev_line_into_data_struct(lne, fd_store_prev);

			lne = &(w->head->line);
			while(lne->rem_line)
				lne = lne->rem_line;
			lne->rem_line = new_line;
			// adjust col_no(used in gui)
			*col_no = w->head->line_size;
			w->head->line_size += tmp_line_size;
			return;
		}
		// not first line
		else {
			char ch;
	        	int extract_from_next = 0, not_available = 0;
		        int check = check_next_line_available(fd_store_next, fd_main);
		        
			// next line not available
		        if(! check)
		                not_available = 1;
		        // available in next_tmp_file
		        if(check == 1)
                		extract_from_next = 1;

			// head_indx to current line(circular array)
			int l_no = head_index(*w, line_no);

			// malloc new line structure to join at end of previous line
			line *new_line = (line*)malloc(sizeof(line));
			*new_line = (w->head)[l_no].line;
		
			// add current line at the end of previous line	
			int prev = head_index(*w, line_no - 1);
			line *lne = &( (w->head[prev]).line );
                        while(lne->rem_line)
                                lne = lne->rem_line;
                        lne->rem_line = new_line;

			// update co-ordinates(used in gui)
			(*lne_no)--;
			*col_no = (w->head[prev]).line_size;
			(w->head[prev]).line_size += (w->head[l_no]).line_size;
			
			int i;
			for(i = line_no; i < w->tot_lines - 1; i++) {
				int curr = head_index(*w, i), next = head_index(*w, i+1);
				// shift lines to make space at end to load next line from file	
				w->head[curr].line = w->head[next].line;
				w->head[curr].line_size = w->head[next].line_size;
			}

			int last = head_index(*w, i);
			new_line = &(w->head[last].line);
			
			// init new line
                        new_line->curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
                        // init next subline pointer with NULL
                        new_line->rem_line = NULL;
                        // init gap buffer
                        init_gap_buff(new_line);

			// next line not available to load
			if(not_available) {
				new_line->curr_line[0] = MAX_CHAR;
				w->head[last].line_size = 0;
				return;
			}
		
			// load next line from file and update line size	
			w->head[last].line_size = next_line_into_data_struct(new_line, extract_from_next, fd_store_next, fd_main);
			return;
		}

	}
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
		(w->head)[i].line_size = 0;
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
		ungetc(c, fd_get);

		while((c = fgetc(fd_get)) != '\n')
                	insert_at_pos(lne, indx++, c);
		
		(w->head)[i].line_size = indx;	// size of total line
	}

	return fd_get;	
}


/*use to print contents of ADT - for testing*/
void print_page(win w) {
	line *lne;
	int h_indx, line_no;
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

		// to clear previously written line from screen
		move(i, 0);
                clrtoeol();
		int col = 0;
                while(1) {
                        if(lne->gap_size != 0 && indx == lne->gap_left)
				indx = lne->gap_right + 1;
			if(indx == MAX_CHAR_IN_SUBLINE) {
				if(lne->rem_line == NULL) {
					// new line
					break;
				}
                                lne = lne->rem_line;
                                indx = 0;
				continue;
                        }
			
			c = lne->curr_line[indx++];
			mvaddch(i, col++, c);
                }
        }
	return;
}


void print(win w) {
        line *lne;
        int h_indx;
	int a = 0;
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
				continue;
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

	// if line is empty means blank line, insert '\n' and return 	//TODO don changed
	if(data_line->gap_size == MAX_CHAR_IN_SUBLINE && data_line->rem_line == NULL) {
		fputc('\n', fd_store);
		return;
	}

	if(data_line->gap_size != 0 && indx == data_line->gap_left)
		indx = data_line->gap_right + 1;
	
	char data;
	while(1) {
		//indx++;
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
			if(data_line->gap_left == indx)		// if next next lines gap is at start(0th pos)	
				continue;
		}

		if(data_line->gap_size == MAX_CHAR_IN_SUBLINE) { 	// TODO must be handle in del_from_pos fun
			indx = MAX_CHAR_IN_SUBLINE -1;
			continue;
		}
		data = data_line->curr_line[indx++];
                //printf("%c %d %d %d %d\n", data, data_line->gap_left, data_line->gap_right, data_line->gap_size, indx);
                // store data in tmp_prev file
                fputc(data, fd_store);
	}
	return;
}


// use to copy line from next line from file to give line in datastructure
// NOTE:- extract_from_next 1 if line to be extracted from tmp_next file 
// else 0 if line to be extracted from main file
// line must be present in either of two files if not , condn should be handle by caller of this fun
// return number of characters loaded into line from file
int next_line_into_data_struct(line *lne, int extract_from_next, FILE *fd_store_next, FILE *fd_main) {
	char c;
	if(extract_from_next) {
		int count = 0;
                // if temp file in which next data is stored, has data extract from there,
                for(int i=0; ; i++) {
                        if(fseek(fd_store_next, -2, SEEK_CUR) == -1) {
                                fseek(fd_store_next, 0, SEEK_SET);      // goto start position
                                return count;
                        }
                        c = fgetc(fd_store_next);
                        if(c != '\n') {
                                // insert at position 0 because data is extracted in reverse passion
                                insert_at_pos(lne, 0, c);
                                count++;
                        }
                        else return count;
                }
        }

        // Now if line is not present in filename_nxt.tmp file
        // then extract from main file
        // next line not present
        else {
                int indx = 0;
                // insert till end of line
                while((c = fgetc(fd_main)) != '\n') {
                        insert_at_pos(lne, indx++, c);
                }
                return indx;
        }

}


// use to copy line from prev line from file to give line in datastructure
// line must be present in either of two files if not , condn should be handle by caller of this fun
// return number of characters loaded into line from file
int prev_line_into_data_struct(line *lne, FILE *fd_store_prev) {
	int count = 0;
	char c;
        // if temp file in which previous data is stored, has data extract from there,
        for(int i=0; ; i++) {
                if(fseek(fd_store_prev, -2, SEEK_CUR) == -1) {
                        fseek(fd_store_prev, 0, SEEK_SET);      // goto start position
                        return count;
                }

                c = fgetc(fd_store_prev);
                if(c != '\n') {
                        insert_at_pos(lne, 0, c);
                        count++;
                }
                else return count;
        }
}


/* func to check next line is available or not
 * return 0 if not available
 * return 1 if available in next tmp_file
 * reutrn 2 if not available in next tmp_file but available in main file
 * */
int check_next_line_available(FILE *fd_store_next, FILE *fd_main) {
	char ch;
        if(fseek(fd_store_next, -1, SEEK_CUR) != -1) {
                fseek(fd_store_next, 1, SEEK_CUR);
		return 1;
        }
        else if((ch = fgetc(fd_main)) == -1) {
                // next line not available
                return 0;
        }
        ungetc(ch, fd_main);    // restoring previous condition
	return 2;
}


/* func to check next line is available or not
 * return 0 if not available
 * return 1 if available
 */
int check_prev_line_available(FILE *fd_store_prev) {
	// check if line is present in filename_prev.tmp file
        if(fseek(fd_store_prev, -1, SEEK_CUR) != -1) {    // present
                fseek(fd_store_prev, 1, SEEK_CUR);
		return 1;
	}
        // else return as line not present to load previous line
        return 0;
}


/*
 * used to load next line at last, after removing first line.
 * fd_store_prev : file descripter to store line which is just to be removed from top of ADT
 * fd_store_next : file descripter to store line which is just to be removed from bottom of ADT
 * fd_get   : file descripter to pointing in main file.
 */
void load_next_line(win *w, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {	
	char ch;
	int extract_from_next = 0;
	int check = check_next_line_available(fd_store_next, fd_main);
	// next line not available
	if(! check)
		return;
	// available in next_tmp_file
	if(check == 1)
		extract_from_next = 1;

	/*extract first line of window from ADT to tmp file*/
        if(w->head->line.curr_line[0] != MAX_CHAR)
                extract_line(w->head, fd_store_prev);
	else return;

	// free mallocated sublines corresponding to data_line in ADT
	free_line(&w->head->line);	//TODO: try to do without free

	// init new line at head
	w->head->line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
	w->head->line.rem_line = NULL;
	// init gap buffer of new line
	init_gap_buff(&w->head->line);
	// init line size
	w->head->line_size = 0;		//TODO don changed

	// load next line from file
	line *lne = &(w->head)->line;
	char c;

	// load next line and change line size accordingly
	w->head->line_size = next_line_into_data_struct(lne, extract_from_next, fd_store_next, fd_main);

	w->head_indx = (w->head_indx + 1) % w->tot_lines;
	
	if(w->head_indx == 0)	// TODO head_indx is sufficient to show circular array
		w->head -= (w->tot_lines-1);
	else w->head++;

	return;
}


void load_prev_line(win *w, FILE *fd_store_prev, FILE *fd_store_next) {
	// check previous line is available or not
	if(! check_prev_line_available(fd_store_prev))
		return;

	// point head pointer to previous line
	if(w->head_indx == 0) {
		w->head_indx = w->tot_lines - 1;
		w->head += (w->tot_lines - 1);
	}
	else {
		w->head_indx--;
		w->head--;
	}
	
	// extract last line into filename_next.tmp
        if(w->head->line.curr_line[0] != MAX_CHAR)
                extract_line(w->head, fd_store_next);
        // free mallocated sublines corresponding to data_line in ADT
        free_line(&w->head->line);	// TODO: try to do without free

	// init new line at head
        w->head->line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
        w->head->line.rem_line = NULL;
        // init gap buffer of new line
        init_gap_buff(&w->head->line);
	// init line size
	w->head->line_size = 0;

        // load prev line from file
        line *lne = &(w->head)->line;
	w->head->line_size = prev_line_into_data_struct(lne, fd_store_prev);
	return;
}


// NOTE:- first line is 0th line
void insert_new_line_at_pos(win *w, int *lne_no, int *col_no, FILE *fd_prev, FILE *fd_nxt, FILE *fd_main) {
	int line_no = *lne_no, position = *col_no;
	// not last line
	if(line_no != w->tot_lines - 1) {
		int last = head_index(*w, w->tot_lines-1);
		
		extract_line(&(w->head[last]), fd_nxt);
		free_line(&(w->head[last]).line);

		for(int i = w->tot_lines-1; i > line_no; i--) {
			int curr = head_index(*w, i-1);
	                int next = head_index(*w, i);

			(w->head[next]).line = (w->head[curr]).line;
			(w->head[next]).line_size = (w->head[curr]).line_size;
		}

		int empty = head_index(*w, line_no + 1);
		// init new line
		(w->head)[empty].line_size = 0;
                (w->head)[empty].line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
                // init next subline pointer with NULL
                (w->head)[empty].line.rem_line = NULL;
                // init gap buffer
                init_gap_buff(&(w->head[empty]).line);
	
		int current = head_index(*w, line_no);	
		line *lne = move_cursor(&(w->head)[current].line, position);
		position = lne->gap_right + 1;

		// insert data in next blank line 
                int indx = 0;
                while(position < MAX_CHAR_IN_SUBLINE) {
                        if(position == lne->gap_left)
                                position = lne->gap_right + 1;

                        insert_at_pos(&((w->head)[empty].line), indx++, lne->curr_line[position++]);
                }
	
		// update line size of both lines	
		(w->head[empty]).line_size = (w->head[current]).line_size - *col_no;
		(w->head[current]).line_size = *col_no;

		// if next subline is present then point rem_line pointer to that subline
                // so that copy operation will be saved
                (w->head)[empty].line.rem_line = lne->rem_line;
		lne->gap_size += indx;   // change gap_size
                // gap_right = end to indicate all data after position is deleted
                lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
                lne->rem_line = NULL;

		// update co ordinates(used in gui)
                (*lne_no)++;
                *col_no = 0;
	}
	// last line
	else {
		extract_line(w->head, fd_prev);
		free_line( &(w->head->line) );

                // shift line pointer by one unit to create space for blank line
                for(int i = 0; i < (w->tot_lines-1); i++) {
			int curr = head_index(*w, i);
                        int next = head_index(*w, i+1);
                        
			(w->head)[curr].line = (w->head)[next].line;
			(w->head)[curr].line_size = (w->head)[next].line_size;
                }

		int current = head_index(*w, line_no);
		int prev = head_index(*w, line_no-1);
		// init new line
                (w->head)[current].line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
                // init next subline pointer with NULL
                (w->head)[current].line.rem_line = NULL;
                // init gap buffer
                init_gap_buff(&(w->head[current]).line);
		
		// line in which new line to be inserted
                line *lne = &(w->head[prev]).line;

		// move cursor to appropriate position with adjusted gap buffer at position
		lne = move_cursor(lne, position);
                position = lne->gap_right + 1;

                // insert data in next blank line
                int indx = 0;
                while(position < MAX_CHAR_IN_SUBLINE) {
                        if(position == lne->gap_left)
                                position = lne->gap_right + 1;

                        insert_at_pos( &((w->head)[current].line), indx++, lne->curr_line[position++] );
                }

		// update line size of both lines
		(w->head[prev]).line_size = *col_no;
                (w->head[current]).line_size = (w->head[current]).line_size - *col_no;

                (w->head)[current].line.rem_line = lne->rem_line;

                // if next subline is present then point rem_line pointer to that subline
                // so that copy operation will be saved
                lne->gap_size += indx;   // change gap_size
                // gap_right = end to indicate all data after position is deleted
                lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
                lne->rem_line = NULL;

		// update co ordinates(used in gui)
                *col_no = 0;
	}
	return;
}

// use to return head_index
int head_index(win w, int line_no) {	//TODO: use in print fun
	 int h_indx = line_no;
	 // circular array
	 if(h_indx + w.head_indx >= w.tot_lines)
		 h_indx = h_indx - w.tot_lines;
	 return h_indx;
}


int main() {
	win window_1;
	FILE *fd_store_prev, *fd_store_next, *fd_main;
	init_window(&window_1, 5);

	fd_main = load_file(&window_1,"1.c");
	fd_store_prev = fopen(".hi_pr.tmp", "w+");
        fd_store_next = fopen(".hi_nxt.tmp", "w+");

	/*
	print(window_1);
	printf("\n");
	int a = 0, b = 0;
	for(int i = 0; i < 1; i++) {
		//load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
                //print(window_1);
                //printf("abc %d\n",window_1.head_indx);
                //printf("\n");

		a = 4, b = 0;
		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
		print(window_1);
	        printf("\n");

		del_from_pos(&window_1, &a, &b, fd_store_prev, fd_store_next, fd_main);
		//load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
		//load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
		//load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
		print(window_1);
		printf("\n");
		exit(1);
		
		a= 0, b = 100;
		del_from_pos(&window_1, &a, &b, fd_store_prev, fd_store_next, fd_main);
		print(window_1);
                printf("\n");

		a= 0, b = 100;
                del_from_pos(&window_1, &a, &b, fd_store_prev, fd_store_next, fd_main);
                print(window_1);
                printf("\n");

		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
		print(window_1);
		//printf("abc %d\n",window_1.head_indx);
		printf("\n");
		exit(1);

		load_prev_line(&window_1, fd_store_prev, fd_store_next);
                print(window_1);
		printf("abc %d\n",window_1.head_indx);
		printf("\n");

		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
                print(window_1);
		printf("abc %d\n",window_1.head_indx);
                printf("\n");

		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
                print(window_1);
                printf("abc %d\n",window_1.head_indx);
                printf("\n");

		load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
                print(window_1);
                printf("abc %d\n",window_1.head_indx);
                printf("\n");
	}
	exit(1);
*/
	// curses interface 
        initscr();
        noecho();
        keypad(stdscr, true);

	int ch;
	int line_no = 0, col_no = 0;

	print_page(window_1);
	print_loc(line_no, col_no);
	move(line_no, col_no);
	while(1) {
		ch = getch();
		switch(ch) {
			case 'q':
				endwin();
				return 0;

			case KEY_LEFT:
				if(col_no)
					col_no--;
				break;

			case KEY_RIGHT:
				if(col_no < (window_1.head)[head_index(window_1, line_no)].line_size)
					col_no++;
				break;

			case KEY_DOWN:
				if(line_no < window_1.tot_lines - 1) {
					line_no++;
					
					int h_indx = head_index(window_1, line_no);
					if(col_no > (window_1.head)[h_indx].line_size)
						col_no = (window_1.head)[h_indx].line_size;
				}
				else {
					load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
					
					int h_indx = head_index(window_1, line_no);
					if(col_no > (window_1.head)[h_indx].line_size)
						col_no = (window_1.head)[h_indx].line_size;
				}
				break;

			case KEY_UP:
				if(line_no > 0) {
					line_no--;
					
					int h_indx = head_index(window_1, line_no);
                                        if(col_no > (window_1.head)[h_indx].line_size)
                                                col_no = (window_1.head)[h_indx].line_size;

				}
				else {
					load_prev_line(&window_1, fd_store_prev, fd_store_next);

					int h_indx = head_index(window_1, line_no);
					if(col_no > (window_1.head)[h_indx].line_size)
                                                col_no = (window_1.head)[h_indx].line_size;
				}
				break;

			case KEY_BACKSPACE:
				del_from_pos(&window_1, &line_no, &col_no, fd_store_prev, fd_store_next, fd_main);
				break;

			case '\n':
				insert_new_line_at_pos(&window_1, &line_no, &col_no, fd_store_prev, fd_store_next, fd_main);
				break;
			
			default:{
				int h_indx = head_index(window_1, line_no);
				(window_1.head)[h_indx].line_size++;
				insert_at_pos(&((window_1.head)[h_indx].line), col_no++, ch);
				}
		}
		print_page(window_1);
		print_loc(line_no, col_no);
		move(line_no, col_no);
	}

	return 0;
}
