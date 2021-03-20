#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include "editor_structs.h"
#include "line.h"

void init_gap_buff(line *lne);

// Used to move the gap left in the array
// Assuming that position belong to given array
void move_gap_left(line *lne, int position);

// Used to move the gap right in the array
void move_gap_right(line *lne, int position);

/* Move mouse cursor to particular position
 * RETURN:- line pointer with adjusting gap buffer, 
 * at that point data can be added or can be removed
 * NOTE:- position should belong to same block.
 */
line* move_cursor(line *subline, int position) ;


#endif
