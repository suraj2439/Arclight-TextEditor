#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include "editor_structs.h"
#include "gap_buffer.h"
#include "editor_func.h"

/*to free corresponding sublines*/
void free_line(line* lne);


// extract line from ADT to tmp file
// suplimentary fun
void extract_line(node_l *tmp, FILE *fd_store);


// use to copy line from next line from file to give line in datastructure
// NOTE:- extract_from_next 1 if line to be extracted from tmp_next file
// else 0 if line to be extracted from main file
// line must be present in either of two files if not , condn should be handle by caller of this fun
// return number of characters loaded into line from file
int next_line_into_data_struct(line *lne, int extract_from_next, FILE *fd_store_next, FILE *fd_main);


// use to copy line from prev line from file to give line in datastructure
// line must be present in either of two files if not , condn should be handle by caller of this fun
// return number of characters loaded into line from file
int prev_line_into_data_struct(line *lne, FILE *fd_store_prev);


/* func to check next line is available or not
 * return 0 if not available
 * return 1 if available in next tmp_file
 * reutrn 2 if not available in next tmp_file but available in main file
 * */
int check_next_line_available(FILE *fd_store_next, FILE *fd_main);


/* func to check next line is available or not
 * return 0 if not available
 * return 1 if available
 */
int check_prev_line_available(FILE *fd_store_prev);


/*
 * used to load next line at last, after removing first line.
 * fd_store_prev : file descripter to store line which is just to be removed from top of ADT
 * fd_store_next : file descripter to store line which is just to be removed from bottom of ADT
 * fd_get   : file descripter to pointing in main file.
 * RETURN: 0 for success, 1 for failure
 */
int load_next_line(win *w, FILE *fd_store_prev, FILE *fd_store_next, FILE *fd_main);


int load_prev_line(win *w, FILE *fd_store_prev, FILE *fd_store_next);

// use to return head_index
int head_index(win w, int line_no);


#endif
