#include "editor_func.h"

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


// Use to delete one character from given position
// RETURN: deleted character
char del_from_pos(win *w, int *lne_no, int *col_no, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	int line_no = *lne_no, position = *col_no;
	char data;
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
		data = lne->curr_line[lne->gap_left - 1];
		lne->gap_left--;		//TODO decide what to do if only one character to remove now subline is empty
		lne->gap_size++;
		return data;
	}

	// delete character from position 0
	else {
		// if first line
		if(line_no == 0) {
			// check if line is present in filename_prev.tmp file
			if(! check_prev_line_available(fd_store_prev))
				return '\n';

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
			return '\n';
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
				return '\n';
			}

			// load next line from file and update line size
			w->head[last].line_size = next_line_into_data_struct(new_line, extract_from_next, fd_store_next, fd_main);
			return '\n';
		}

	}
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


void print_loc(int x, int y) {
	move(LOC_X, LOC_Y);
	clrtoeol();
        mvprintw(LOC_X, LOC_Y, "x: %d y: %d", x, y);
}

void print_debug(int x, int y) {
        mvprintw(1, 90, "k: %d b: %d", x, y);
}


/*use to print contents of ADT - for testing*/
void print_page(win w, TrieNode *keyword) {
	line *lne;
	int h_indx;
	char word_arr[100];
	for(int i = 0; i < w.tot_lines; i++) {
		// for keyword coloring
		int windx = 0;
		char comment = 0;
		char color = WHITE;

		h_indx = head_index(w, i);

		lne = &((w.head)[h_indx].line);
		char c = 1;
		int indx = 0;

		if(lne->curr_line[0] == MAX_CHAR) {
			for(; i < w.tot_lines; i++) {
				h_indx = head_index(w, i);
				lne = &((w.head)[h_indx].line);
				mvaddch(i, 0, TILDE);
			}
			refresh();
                        return;
		}

		// to clear previously written line from screen
		move(i, 0);
                clrtoeol();
		int col = 0;
	
		// 3(orange), 15(white), 9(red), 10(light_green), 11(yerllow), 21(dark blue), 39(light_blue)

		int brk = 0;
                while(1) {
                        if(lne->gap_size != 0 && indx == lne->gap_left)
				indx = lne->gap_right + 1;
			if(indx == MAX_CHAR_IN_SUBLINE) {
				if(lne->rem_line == NULL) {
					brk = 1;
					goto LABEL;
					// new line
				}
                                lne = lne->rem_line;
                                indx = 0;
				continue;
                        }
			
			c = lne->curr_line[indx++];
			if(c == '/' && comment == 0)
				comment = '/';
			else if(c == '/' && comment == '/') {
				comment = 1;
				attron(COLOR_PAIR(COMMENT));
                                mvaddch(i, col++, '/');
                                attroff(COLOR_PAIR(COMMENT));
			}			
	
		LABEL:
			if(comment == 1) {
				if(brk)
					break;
				attron(COLOR_PAIR(COMMENT));
                                mvaddch(i, col++, c);
                                attroff(COLOR_PAIR(COMMENT));
			}
			else if(c == '#' || c == '<' || c == ' ' || c == '(' || c == ';' || brk) {
				word_arr[windx++] = '\0';
				if(! search(keyword, word_arr, &color))
					color = WHITE;

				attron(COLOR_PAIR(color));
				for(int k = 0; word_arr[k] != '\0'; k++)
					mvaddch(i, col++, word_arr[k]);
				attroff(COLOR_PAIR(color));
				
				if(brk)
					break;
				mvaddch(i, col++, c);
				windx = 0;
			}
			else
				word_arr[windx++] = c;
                }
        }
	refresh();
	return;
}


void print_line(win w, int line_no, TrieNode *keyword) {
	line *lne;
        int h_indx;
        char word_arr[100];

	// for keyword coloring
	int windx = 0;
	char comment = 0;
	char color = WHITE;

	h_indx = head_index(w, line_no);

	lne = &((w.head)[h_indx].line);
	char c = 1;
	int indx = 0;

	if(lne->curr_line[0] == MAX_CHAR) {
		mvaddch(line_no, 0, TILDE);
		refresh();
		return;
	}

	// to clear previously written line from screen
	move(line_no, 0);
	clrtoeol();
	int col = 0;

	int brk = 0;
	while(1) {
		if(lne->gap_size != 0 && indx == lne->gap_left)
			indx = lne->gap_right + 1;
		if(indx == MAX_CHAR_IN_SUBLINE) {
			if(lne->rem_line == NULL) {
				brk = 1;
				goto LABEL;
			}
			lne = lne->rem_line;
			indx = 0;
			continue;
		}

		c = lne->curr_line[indx++];
		if(c == '/' && comment == 0)
			comment = '/';
		else if(c == '/' && comment == '/') {
			comment = 1;
			attron(COLOR_PAIR(COMMENT));
			mvaddch(line_no, col++, '/');
			attroff(COLOR_PAIR(COMMENT));
		}

	    LABEL:
		if(comment == 1) {
			if(brk)
				break;
			attron(COLOR_PAIR(COMMENT));
			mvaddch(line_no, col++, c);
			attroff(COLOR_PAIR(COMMENT));
		}        		
		else if(c == '#' || c == '<' || c == ' ' || c == '(' || c == ';' || brk) {
			word_arr[windx++] = '\0';
			if(! search(keyword, word_arr, &color))
				color = WHITE;

			attron(COLOR_PAIR(color));
			for(int k = 0; word_arr[k] != '\0'; k++)
				mvaddch(line_no, col++, word_arr[k]);
			attroff(COLOR_PAIR(color));

			if(brk)
				break;
			mvaddch(line_no, col++, c);
			windx = 0;
		}
		else
			word_arr[windx++] = c;
	}
	refresh();
	return;
}


// for print output on terminal, for debugging
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


