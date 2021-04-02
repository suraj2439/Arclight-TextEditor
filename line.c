#include "line.h"


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
			if(c == '\t')
				for(int i = 0; i < TAB_SPACE; i++)
					insert_at_pos(lne, indx++, ' ');
			else insert_at_pos(lne, indx++, c);
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
 * RETURN: 0 for success, 1 for failure(if next line not available)
 */
int load_next_line(win *w, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main) {
	char ch;
	int extract_from_next = 0;
	int check = check_next_line_available(fd_store_next, fd_main);
	// next line not available
	if(! check)
		return FAILURE;
	// available in next_tmp_file
	if(check == 1)
		extract_from_next = 1;

	/*extract first line of window from ADT to tmp file*/
        if(w->head->line.curr_line[0] != MAX_CHAR)
                extract_line(w->head, fd_store_prev);
	else return FAILURE;	//TODO maybe this case will not arise

	// free mallocated sublines corresponding to data_line in ADT
	free_line(&w->head->line);	//TODO: try to do without free

	// init new line at head
	w->head->line.curr_line = (char*)malloc(sizeof(char) * MAX_CHAR_IN_SUBLINE);
	w->head->line.rem_line = NULL;
	// init gap buffer of new line
	init_gap_buff(&w->head->line);
	// init line size
	w->head->line_size = 0;

	// load next line from file
	line *lne = &(w->head)->line;
	char c;

	// load next line and change line size accordingly
	w->head->line_size = next_line_into_data_struct(lne, extract_from_next, fd_store_next, fd_main);

	w->head_indx = (w->head_indx + 1) % w->tot_lines;

	if(w->head_indx == 0)
		w->head -= (w->tot_lines-1);
	else w->head++;

	return SUCCESS;
}


int load_prev_line(win *w, FILE *fd_store_prev, FILE *fd_store_next) {
	// check previous line is available or not
	if(! check_prev_line_available(fd_store_prev))
		return FAILURE;

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
	return SUCCESS;
}


// use to return head_index
int head_index(win w, int line_no) {	//TODO: use in print fun
	 int h_indx = line_no;
	 // circular array
	 if(h_indx + w.head_indx >= w.tot_lines)
		 h_indx = h_indx - w.tot_lines;
	 return h_indx;
}
