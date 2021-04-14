#ifndef EDITOR_STRUCTS_H
#define EDITOR_STRUCTS_H

#define SUCCESS			0
#define FAILURE			1
#define TOT_LINES_IN_WINDOW	33
#define MAX_CHAR_IN_SUBLINE	5
#define MAX_CHAR 		-128
#define BACKSPACE		8
#define DELETE			127
#define TAB_SPACE		4
#define SHORTCUT_KEY		-5
#define BRACKET			-6
#define CTRL(x)           ((x) & 0x1f)

typedef struct line {
        char *curr_line;
	// gap buffer left and right pointers
	int gap_left, gap_right;
	// size of gap_buffer
	int gap_size;
        // if line is big store next remaining line
        struct line *rem_line;
} line;


/* struct for one line,
 * contain pointer of correspondig line */
typedef struct node_l { 
	line line;
	int line_size;
} node_l;


typedef struct win {
	node_l *head;
	int tot_lines, head_indx;
} win;

#endif
