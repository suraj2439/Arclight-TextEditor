#ifndef INIT_EDITOR_H
#define INIT_EDITOR_H

#include "stdio.h"
#include "stdlib.h"
#include "editor_func.h"
#include "editor_structs.h"
#include "gap_buffer.h"

/*use to initialise window*/
void init_window(win *w, int tot_lines);

/*use to load text from file into ADT*/
FILE* load_file(win *w, char *filename);

#endif
