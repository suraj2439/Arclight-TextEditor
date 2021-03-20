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

