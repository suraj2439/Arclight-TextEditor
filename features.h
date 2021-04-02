#ifndef FEATURES_H
#define FEATURES_H

#include <stdlib.h>
#include <ncurses.h>
#include "editor_structs.h"

#define ORANGE		3
#define RED		9
#define GREEN		10
#define YELLOW		11
#define WHITE		15
#define DBLUE		21
#define LBLUE		39

char** init_shortcut_keys();

int shortcut_key_indx(int *ch, int *move);

void init_colors();

#endif
