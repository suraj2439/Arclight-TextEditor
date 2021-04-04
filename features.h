#ifndef FEATURES_H
#define FEATURES_H

#include <stdlib.h>
#include <ncurses.h>
#include "editor_structs.h"
#include "trie.h"

#define ORANGE		3
#define RED		9
#define GREEN		47
#define YELLOW		11
#define WHITE		15
#define DBLUE		21
#define LBLUE		39
#define COMMENT		44

#define TOT_SHORTCUT_KEYS	9

char** init_shortcut_keys();

int shortcut_key_indx(int *ch, int *move);

void init_colors();

TrieNode* init_keywords();

void check_bracket(int *ch, char *start, char *end);

#endif
