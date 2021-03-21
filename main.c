#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "editor_structs.h"
#include "gap_buffer.h"
#include "init_editor.h"
#include "editor_func.h"
#include "line.h"
#include "stack.c"

void print_loc(int x, int y) {
        move(20, 20);
        mvprintw(10, 30, "x: %d y: %d", x, y);
}

void print_debug(int x, int y) {
        mvprintw(15, 60, "k: %d b: %d", x, y);
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




int main() {
	win window_1;
	FILE *fd_store_prev, *fd_store_next, *fd_main;
	init_window(&window_1, 5);
	stack st;
	init(&st);

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
	int pos_changed = 0;

	print_page(window_1);
	print_loc(line_no, col_no);
	move(line_no, col_no);
	while(1) {
		ch = getch();
		switch(ch) {
			case 'q':
				endwin();
				return 0;
			case 'z':
				undo(&st, &window_1, &line_no, &col_no, fd_store_prev, fd_store_next, fd_main);
				break;

			case KEY_LEFT:
				pos_changed = 1;
				if(col_no)
					col_no--;
				break;

			case KEY_RIGHT:
				pos_changed = 1;
				if(col_no < (window_1.head)[head_index(window_1, line_no)].line_size)
					col_no++;
				break;

			case KEY_DOWN:
				pos_changed = 1;
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
				pos_changed = 1;
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

			case KEY_BACKSPACE: {
				if(line_no == 0 && col_no == 0)
					continue;

				char operation = DEL_CHAR;
				if(col_no == 0)
					operation = DEL_LINE;
				char data = del_from_pos(&window_1, &line_no, &col_no, fd_store_prev, fd_store_next, fd_main);
				store_info(&st, pos_changed, data, operation, line_no, col_no);
                                pos_changed = 0;
				break;
			}

			case '\n':
				insert_new_line_at_pos(&window_1, &line_no, &col_no, fd_store_prev, fd_store_next, fd_main);
				store_info(&st, pos_changed, ch, INSERT_NEW_LINE, line_no, col_no);
                                pos_changed = 0;
				break;
			
			default: {
				int h_indx = head_index(window_1, line_no);
				(window_1.head)[h_indx].line_size++;
				insert_at_pos(&((window_1.head)[h_indx].line), col_no++, ch);
				store_info(&st, pos_changed, ch, INSERT_CHAR, line_no, col_no);
				pos_changed = 0;
			}
		}
		print_page(window_1);
		print_loc(line_no, col_no);
		move(line_no, col_no);
	}

	return 0;
}
