#ifndef EDITOR_FUNC_H
#define EDITOR_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include "editor_structs.h"
#include "line.h"
#include "gap_buffer.h"
#include "trie.h"
#include "features.h"

#define LOC_X           35
#define LOC_Y           120


// Function to insert character at position
// NOTE: line_size should be modified by caller of this function it is not modified in this fun
void insert_at_pos(line *subline, int position, char data);

char del_from_pos(win *w, int *lne_no, int *col_no, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main);

// NOTE:- first line is 0th line
void insert_new_line_at_pos(win *w, int *lne_no, int *col_no, FILE *fd_prev, FILE *fd_nxt, FILE *fd_main);

void print_loc(int x, int y);

void print_debug(int x, int y);

/*use to print contents of ADT - for testing*/
void print_page(win w, TrieNode *keyword);

// use to print given line on screen
void print_line(win w, int line_no, TrieNode *keyword);

// for print output on terminal, for debugging
void print(win w);

#endif
