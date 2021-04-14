#ifndef FEATURES_H
#define FEATURES_H

#include <stdlib.h>
#include <ctype.h>
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
#define CB_X		34	
#define CB_Y		50
#define TOT_SHORTCUT_KEYS	9

char** init_shortcut_keys();

int shortcut_key_indx(int *ch, int *move);

void init_colors();

TrieNode* init_keywords();

TrieNode_c* init_codebase(FILE *fd);

char *accept_codebase_key(int x, int y);

void store_key(FILE *fd, char *key, int lower, int upper);

int validate_codebase_key(char* key);

void print_cbError();

void print_cbAccept();

void print_cbSuccess(char* key);

void print_cbNotFound(char *key);

void check_bracket(int *ch, char *start, char *end);

#endif
