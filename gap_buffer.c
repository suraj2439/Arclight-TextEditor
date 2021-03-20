#include "gap_buffer.h"

void init_gap_buff(line *lne) {
        // initially gap size = size of subline,
        // left will be at pos 0 and right at max-1
        lne->gap_left = 0;
        lne->gap_right = MAX_CHAR_IN_SUBLINE - 1;
        lne->gap_size = MAX_CHAR_IN_SUBLINE;
        return;
}


// Used to move the gap left in the array
// Assuming that position belong to given array
void move_gap_left(line *lne, int position) {
        char *buffer = lne->curr_line;
        int *gap_left = &(lne->gap_left);
        int *gap_right = &(lne->gap_right);

        if(lne->gap_size == 0) {
                *gap_left = position;   //TODO changed (don)
                *gap_right = position-1;
                return;
        }
        // Move the gap left character by character
        while (position < (*gap_left)) {
                (*gap_left)--;
                (*gap_right)--;
                buffer[(*gap_right) + 1] = buffer[*gap_left];
        }
        return;
}

// Used to move the gap right in the array
void move_gap_right(line *lne, int position) {
        // extract data from line structure to handle gap 
        char *buffer = lne->curr_line;
        int *gap_left = &(lne->gap_left);
        int *gap_right = &(lne->gap_right);

        // if gap size is zero
        if(lne->gap_size == 0) {
                if(position >= MAX_CHAR_IN_SUBLINE) {
                        *gap_left = MAX_CHAR_IN_SUBLINE;
                        *gap_right = MAX_CHAR_IN_SUBLINE - 1;
                        return;
                }
                *gap_left = position;
                *gap_right = position -1;
                return;
        }

        // Move the gap right character by character   
        while (position > (*gap_left)) {
                (*gap_left)++;
                (*gap_right)++;
                // For testing
                // TODO modify this
                if(*gap_right >= MAX_CHAR_IN_SUBLINE) {
                        (*gap_left)--;
                        (*gap_right)--;
                        //printf("already at last cannot move further right return from here\n");
                        return;
                }

                buffer[(*gap_left) - 1] = buffer[*gap_right];
        }
        return;
}


/* Move mouse cursor to particular position
 * RETURN:- line pointer with adjusting gap buffer, 
 * at that point data can be added or can be removed
 * NOTE:- position should belong to same block.
 */
line* move_cursor(line *subline, int position) {
	line *lne = subline;
	// take next subline till position doesn't belong to corresponding curr_line
	while((MAX_CHAR_IN_SUBLINE - lne->gap_size) < position) {
		// position not belong to current subline take next line
		// decrease position
		position -= (MAX_CHAR_IN_SUBLINE - lne->gap_size);
		// if current subline is last
		if(lne->rem_line == NULL)
			break;
		// take next subline
		lne = lne->rem_line;
	}
	
	// Now position belongs to current subline
	// move cursor(gap) at appropriate position
	if (position < lne->gap_left)
		move_gap_left(lne, position);
	else 
		move_gap_right(lne, position);
	return lne;
}
