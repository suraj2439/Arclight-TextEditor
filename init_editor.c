#include "init_editor.h"

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

		while((c = fgetc(fd_get)) != '\n') {
			if(c == '\t')
				for(int i = 0; i < TAB_SPACE; i++)
					insert_at_pos(lne, indx++, ' ');
			else 
                		insert_at_pos(lne, indx++, c);
		}
		
		(w->head)[i].line_size = indx;	// size of total line
	}

	return fd_get;	
}
