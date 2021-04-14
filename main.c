#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/stat.h>
#include "editor_structs.h"
#include "gap_buffer.h"
#include "init_editor.h"
#include "editor_func.h"
#include "line.h"
#include "stack.c"
#include "features.h"

void print_loc(int x, int y) {
        mvprintw(0, 90, "x: %d y: %d", x, y);
}

void print_debug(int x, int y) {
        mvprintw(1, 90, "k: %d b: %d", x, y);
}


/*use to print contents of ADT - for testing*/
void print_page(win w, TrieNode *keyword) {
	line *lne;
	int h_indx, line_no;
	char word_arr[100];
	for(int i = 0; i < w.tot_lines; i++) {
		// for keyword coloring
		int windx = 0;
		char comment = 0;
		char color = WHITE;

		h_indx = head_index(w, i);
		/*
		h_indx = i;
		// circular array
		if(h_indx + w.head_indx >= w.tot_lines)
			h_indx = h_indx - w.tot_lines;*/

		lne = &((w.head)[h_indx].line);
		char c = 1;
		int indx = 0;

		if(lne->curr_line[0] == MAX_CHAR)
                        return;

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
			else if(c == ' ' || c == '(' || c == ';' || brk) {
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
	return;
}

void print_line();


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



int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Please Enter filename.\n");
		return 0;
	}

	FILE *fd_store_prev, *fd_store_next, *fd_main;
	win window_1;
        init_window(&window_1, TOT_LINES_IN_WINDOW);
        stack st;
        init(&st);

	int new_file = 0;

	struct stat perm;
	if(stat(argv[1], &perm) == -1)
		new_file = 1;

	if(S_ISDIR(perm.st_mode)) {
		printf("Cannot Open Directory.\n");
		return 0;
	}
	/*
	if( perm.st_mode & S_IWUSR )
	        printf("write ");
	//return 0;
	*/

	if(new_file)
		fd_main = fopen(argv[1], "w+");
	else
		fd_main = load_file(&window_1, argv[1]);
		
	char prev_file[50] = ".";
	char next_file[50] = ".";
	strcat(strcat(prev_file, argv[1]), "_prev.tmp");
	strcat(strcat(next_file, argv[1]), "_next.tmp");

        fd_store_prev = fopen(prev_file, "w+");
        fd_store_next = fopen(next_file, "w+");	


	FILE *fd_cb_key, *fd_cb_data;
        fd_cb_key = fopen(".codebase.key", "r+");
        fd_cb_data = fopen(".codebase.data","r+");
        int codebase_mode = 0;
        TrieNode_c *codebase = init_codebase(fd_cb_key);

/*	
	for(int i = 0; shortcut_key[4][i] != 0; i++)
		printf("%c", shortcut_key[4][i]);
	return 0;
*/	
	/*
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

	init_colors();
	TrieNode *keyword = init_keywords();
	char **shortcut_key = init_shortcut_keys();

	int ch;
	int win_line = 0, win_col = 0, line_no = 0;
	int pos_changed = 0;

	print_page(window_1, keyword);
	print_loc(line_no, win_col);
	move(line_no, win_col);

	int cnt = 0;
	while(1) {
		ch = getch();
		cnt++;

		int move_left = 0;
		char start_bracket, end_bracket;
		int sk_index = shortcut_key_indx(&ch, &move_left);
		//print_debug(ch, 5);
		check_bracket(&ch, &start_bracket, &end_bracket);
		//printf(ch, 5);
		
		switch(ch) {
			case 'q':
	                	fclose(fd_main);
	        	        fclose(fd_store_prev);
        	        	fclose(fd_store_next);
				fclose(fd_cb_key);
		                fclose(fd_cb_data);

				if(new_file)
        		        	remove(argv[1]);
	                	remove(prev_file);
                		remove(next_file);

				endwin();
				return 0;

			// quit the program
			case CTRL('y'):
				goto SAVE;

			// undo function
			case 'z':
				undo(&st, &window_1, &line_no, &win_line, &win_col, fd_store_prev, fd_store_next, fd_main);
				break;

			// left arrow key
			case KEY_LEFT:
				pos_changed = 1;
	                        if(win_col)
        	                        win_col--;
                  	      	break;

			// right arrow key
			case KEY_RIGHT:
				pos_changed = 1;
	                        if(win_col < (window_1.head)[head_index(window_1, win_line)].line_size)
        	                        win_col++;
                	        break;

			// down arrow key
			case KEY_DOWN:
				pos_changed = 1;
                        	if(win_line < window_1.tot_lines - 1) {
                                	win_line++;
                                	line_no++;

                                	int h_indx = head_index(window_1, win_line);
                               		if(win_col > (window_1.head)[h_indx].line_size)
                       	        	        win_col = (window_1.head)[h_indx].line_size;
                	        }
				// if down arrow at bottom of window, load next line
        	                else {
					// load next line
	                                int check = load_next_line(&window_1, fd_store_prev, fd_store_next, fd_main);
					// if next line successfully loaded
                                	if(check == SUCCESS) {
                        	                line_no++;

                	                	int h_indx = head_index(window_1, win_line);
						// adjust column number
        	                        	if(win_col > (window_1.head)[h_indx].line_size)
	                                        	win_col = (window_1.head)[h_indx].line_size;
						
						// store info in stack for undo function
                                		store_info(&st, 0, ch, LOAD_NEXT_LINE, win_line, win_col);
					}
                        	}
                        	break;

			// up arrow key
			case KEY_UP:
				pos_changed = 1;
				// if curr line not first
                        	if(win_line > 0) {
                	                win_line--;
        	                        line_no--;
	
        	                        int h_indx = head_index(window_1, win_line);
	                                if(win_col > (window_1.head)[h_indx].line_size)
                                	        win_col = (window_1.head)[h_indx].line_size;

                        	}
				// if up arrow pressed at top of window, load prev line
                        	else {
                                	int check = load_prev_line(&window_1, fd_store_prev, fd_store_next);
					// if prev line successfully loaded
                                	if(check == SUCCESS) {
                                        	line_no--;

                                		int h_indx = head_index(window_1, win_line);
                                		if(win_col > (window_1.head)[h_indx].line_size)
                                	 	       win_col = (window_1.head)[h_indx].line_size;

						store_info(&st, 0, ch, LOAD_PREV_LINE, win_line, win_col);
					}
                        	}
                        	break;

			case KEY_BACKSPACE: {
        	                // at start of file, do nothing
	                        if(line_no == 0 && win_col == 0)
                        	        continue;

                	        char operation = DEL_CHAR;
        	                if(win_col == 0) {
	                                operation = DEL_LINE;
                                	line_no--;
                        	}
                        	char data = del_from_pos(&window_1, &win_line, &win_col, fd_store_prev, fd_store_next, fd_main);
                        	store_info(&st, pos_changed, data, operation, win_line, win_col);
                	        pos_changed = 0;
	                        break;
        	        }

			// enter key
			case '\n':
	                        line_no++;
                        	insert_new_line_at_pos(&window_1, &win_line, &win_col, fd_store_prev, fd_store_next, fd_main);
        	                store_info(&st, pos_changed, ch, INSERT_NEW_LINE, win_line, win_col);
                	        pos_changed = 0;
	                        break;
			
			case '\t': {
				int h_indx = head_index(window_1, win_line);
                                for(int i = 0; i < TAB_SPACE; i++) {
                                        (window_1.head)[h_indx].line_size++;
                                        insert_at_pos(&((window_1.head)[h_indx].line), win_col++, ' ');
                                        store_info(&st, pos_changed, ch, INSERT_CHAR, win_line, win_col);
                                        if(pos_changed)
                                                pos_changed = 0;
                                }
				break;
			}

			// insert bracket with pair
			case BRACKET: { 
				int h_indx = head_index(window_1, win_line);
                                (window_1.head)[h_indx].line_size += 2;
                                insert_at_pos(&((window_1.head)[h_indx].line), win_col++, start_bracket);
				insert_at_pos(&((window_1.head)[h_indx].line), win_col++, end_bracket);
                                store_info(&st, pos_changed, ch, INSERT_CHAR, win_line, win_col);
				win_col--;
                                pos_changed = 0;
				break;
			}
		
			// insert ; at 	      
			case CTRL(';'): {
				int h_indx = head_index(window_1, win_line);
				insert_at_pos(&((window_1.head)[h_indx].line), (window_1.head)[h_indx].line_size++, ';');
				break;
			}

			case CTRL('h'):
				pos_changed = 1;
				win_col = 0;
				break;
			case CTRL('l'):
				pos_changed = 1;
                                win_col = (window_1.head)[head_index(window_1, win_line)].line_size;
                                break;
			case CTRL('o'): {
				pos_changed = 1;
				line_no -= win_line;
                                win_line = 0;

                                int h_indx = head_index(window_1, win_line);
                                if(win_col > (window_1.head)[h_indx].line_size)
                                        win_col = (window_1.head)[h_indx].line_size;
                                break;
			}
                        case CTRL('k'): {
				pos_changed = 1;
				line_no += window_1.tot_lines-1 - win_line;
				win_line = window_1.tot_lines-1;

				int h_indx = head_index(window_1, win_line);
				if(win_col > (window_1.head)[h_indx].line_size)
					win_col = (window_1.head)[h_indx].line_size;
                                break;
			}	

			case SHORTCUT_KEY: {
				for(int i = 0; shortcut_key[sk_index][i] != '\0'; i++) {
					int h_indx = head_index(window_1, win_line);

					if(shortcut_key[sk_index][i] == '\n') {
						line_no++;
		                                insert_new_line_at_pos(&window_1, &win_line, 
								&win_col, fd_store_prev, fd_store_next, fd_main);
                		                store_info(&st, pos_changed, ch, INSERT_NEW_LINE, win_line, win_col);
					}
					else {
						(window_1.head)[h_indx].line_size++;
	                                	insert_at_pos(&((window_1.head)[h_indx].line), win_col++, shortcut_key[sk_index][i]);
        	                        	store_info(&st, pos_changed, ch, INSERT_CHAR, win_line, win_col);
					}
					if(pos_changed)
						pos_changed = 0;
				}
				for(int i = 0; i < move_left; i++) {
                                        win_col--;
					if(win_col < 0) {
						line_no--;
						win_line--;
						win_col = (window_1.head[head_index(window_1, win_line)]).line_size;
					}
				}
				break;
			}

		CODEBASE_KEY_MODE:
			case KEY_F(1): {
				// accept input key from user
				char *key = accept_codebase_key(win_line, win_col);
				// validate key
				if(! validate_codebase_key(key)) {
					// if invalid print msg on screen
					print_cbError();
					ch = getch();
					// if retry option selected, goto start of this block
					if(ch == KEY_F(1)) {
						move(CB_X, CB_Y);
						clrtoeol();
						goto CODEBASE_KEY_MODE;
					}
					// user dont want to retry, back to editor
					else {
						move(CB_X, CB_Y);
                                                clrtoeol();
						move(win_line, win_col);
						break;
					}
				}
				// key is valid, accept data to store in codebase
				print_cbAccept();
				
				// goto end to store data
				fseek(fd_cb_data, 0, SEEK_END);
				// store lower(start) offset of data
                                long int lower = ftell(fd_cb_data);
				// fill data
				while((ch = getch()) != KEY_F(2)) {
					fputc(ch, fd_cb_data);
				}
				// store upper(end) offset of data
				long int upper = ftell(fd_cb_data);
				// insert key in trie
				insert_in_codebase(codebase, key, (int)lower, (int)upper);
				// insert key in codebase file
				store_key(fd_cb_key, key, (int)lower, (int)upper);

				print_cbSuccess(key);

				break;
			}

			case KEY_F(3): {
				// accept key to search in trie
				char *key = accept_codebase_key(win_line, win_col);
				int lower, upper;
				// search input key
			  	if(search_in_codebase(codebase, key, &lower, &upper)) {
					// goto start of data
					fseek(fd_cb_data, lower, SEEK_SET);
					while(ftell(fd_cb_data) != upper) {
						ch = fgetc(fd_cb_data);
						
						if(ch != '\n') {
							int h_indx = head_index(window_1, win_line);
	                	        	        (window_1.head)[h_indx].line_size++;
        	                	        	insert_at_pos(&((window_1.head)[h_indx].line), win_col++, ch);
	                	                	store_info(&st, pos_changed, ch, INSERT_CHAR, win_line, win_col);
        	         	       		        pos_changed = 0;
						}
						else {
							line_no++;
			                                insert_new_line_at_pos(&window_1, &win_line, &win_col, 
									fd_store_prev, fd_store_next, fd_main);
                        			        store_info(&st, pos_changed, ch, INSERT_NEW_LINE, win_line, win_col);
 			                               pos_changed = 0;
						}
					}
				}
				else print_cbNotFound(key);
			}
				break;

			default:{
                        	int h_indx = head_index(window_1, win_line);
                        	(window_1.head)[h_indx].line_size++;
                        	insert_at_pos(&((window_1.head)[h_indx].line), win_col++, ch);
                        	store_info(&st, pos_changed, ch, INSERT_CHAR, win_line, win_col);
                        	pos_changed = 0;
			}
		}

		print_page(window_1, keyword);
		print_loc(line_no, win_col);
		move(win_line, win_col);
		refresh();
	}

	SAVE:
		endwin();

		// retrive contents from fd_store_prev file
		long tmp;
		// new tempory file to generate file from tmp files and 
		// buffer at time of save
		FILE *fd_new = fopen("suraj.c", "w+");
		// save offset prev file
		tmp = ftell(fd_store_prev);

		fseek(fd_store_prev, SEEK_SET, 0);

		// from start to tmp offset
		for(int i = 0; i != tmp; i++)
			fputc(fgetc(fd_store_prev), fd_new);


		// retrive contents from window buffer
		line *lne;
		for(int i = 0; i < window_1.tot_lines; i++) {
			int h_indx = head_index(window_1, i);
			lne = &((window_1.head)[h_indx].line);
                	char c = 1;
	                int indx = 0;

			// next line not available
        	        if(lne->curr_line[0] == MAX_CHAR)
                	        break;

			while(1) {
                        	if(lne->gap_size != 0 && indx == lne->gap_left)
                                	indx = lne->gap_right + 1;
	                        if(indx == MAX_CHAR_IN_SUBLINE) {
        	                        if(lne->rem_line == NULL) {
                        	                fputc('\n', fd_new);
                	                        break;
                                	}
	                                lne = lne->rem_line;
        	                        indx = 0;
                	                continue;
                        	}
	                        c = lne->curr_line[indx++];
        	                fputc(c, fd_new);
                	}
		}	

	
		long tmp_offset;
		char c;
		while(1) {
                       	if(fseek(fd_store_next, -2, SEEK_CUR) == -1) {
				if(fseek(fd_store_next, -1, SEEK_CUR) == -1)
					break;
				while((c = fgetc(fd_store_next)) != '\n')
                                        fputc(c, fd_new);
                                fputc('\n', fd_new);
	                        break;
			}
			c = fgetc(fd_store_next);
			if(c == '\n') {
				tmp_offset = ftell(fd_store_next) - 1;
				while((c = fgetc(fd_store_next)) != '\n')
					fputc(c, fd_new);
				fputc('\n', fd_new);

				fseek(fd_store_next, tmp_offset, SEEK_SET);
				//printf("%ld %c\n", tmp_offset, fgetc(fd_store_next));
			}
		}


                while((c = fgetc(fd_main)) != -1)
                        fputc(c, fd_new);


		stat(argv[1], &perm);
		chmod("suraj.c", perm.st_mode);

		fclose(fd_new);
		fclose(fd_main);
                fclose(fd_store_prev);
                fclose(fd_store_next);
		fclose(fd_cb_key);
		fclose(fd_cb_data);

		remove(argv[1]);
		remove(prev_file);
		remove(next_file);

		rename("suraj.c", argv[1]);

	return 0;
}
