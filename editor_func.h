#ifndef EDITOR_FUNC_H
#define EDITOR_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include "editor_structs.h"
#include "line.h"
#include "gap_buffer.h"

// Function to insert character at position
// NOTE: line_size should be modified by caller of this function it is not modified in this fun
void insert_at_pos(line *subline, int position, char data);

char del_from_pos(win *w, int *lne_no, int *col_no, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main);

// NOTE:- first line is 0th line
void insert_new_line_at_pos(win *w, int *lne_no, int *col_no, FILE *fd_prev, FILE *fd_nxt, FILE *fd_main);


#endif
