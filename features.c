#include "features.h"

void init_colors() {        
        start_color();
        init_pair(ORANGE, ORANGE, COLOR_BLACK);
	init_pair(RED, RED, COLOR_BLACK);
        init_pair(GREEN, GREEN, COLOR_BLACK);
	init_pair(YELLOW, YELLOW, COLOR_BLACK);
	init_pair(WHITE, WHITE, COLOR_BLACK);
	init_pair(DBLUE, DBLUE, COLOR_BLACK);
	init_pair(LBLUE, LBLUE, COLOR_BLACK);
	init_pair(COMMENT, COMMENT, COLOR_BLACK);
        // 3(orange), 15(white), 9(red), 10(light_green), 11(yerllow), 14(light blue), 21(dark blue), 39(light blue)

}

TrieNode* init_keywords() {

        char grp1[][8] = {"int", "char", "double", "long", "auto",
                     "signed", "unsigned", "void", "float", "short"};
        char grp2[][8] = {"register", "extern", "static", "volatile", "const"};

        char grp3[][8] = {"typedef", "struct", "enum", "union", "scanf", "printf"};

        char grp4[][8] = {"continue", "break", "return", "sizeof", "include"};

        char grp5[][8] = {"for", "while", "do", "goto"};

        char grp6[][8] = {"if", "else", "switch", "case", "default"};

        TrieNode *root = getNode();

        // Construct trie
        int i;
        for (i = 0; i < ARRAY_SIZE(grp1); i++)
                insert(root, grp1[i], ORANGE);
        for (i = 0; i < ARRAY_SIZE(grp2); i++)
                insert(root, grp2[i], RED);
        for (i = 0; i < ARRAY_SIZE(grp3); i++)
                insert(root, grp3[i], GREEN);
        for (i = 0; i < ARRAY_SIZE(grp4); i++)
                insert(root, grp4[i], LBLUE);
        for (i = 0; i < ARRAY_SIZE(grp5); i++)
                insert(root, grp5[i], DBLUE);
        for (i = 0; i < ARRAY_SIZE(grp6); i++)
                insert(root, grp6[i], YELLOW);

        return root;
}


TrieNode_c* init_codebase(FILE *fd) {
	TrieNode_c *root = codebaseNode();
	char key[MAX_KEY];
	char ch;
	while(1) {
		if((ch = fgetc(fd)) == -1)
			return root;
		ungetc(ch, fd);

		fscanf(fd, "%s", key);
	
		int lower, upper;
		fscanf(fd, "%d", &lower);
		fscanf(fd, "%d", &upper);

		insert_in_codebase(root, key, lower, upper);
	}
}


char* accept_codebase_key(int x, int y) {
	move(CB_X, CB_Y);
	clrtoeol();
	refresh();
	char ch;
	char* arr = (char*)malloc(sizeof(char) * MAX_KEY);
	for(int i = 0; (ch = getch()) != '\n'; i++) {
		arr[i] = ch;
		mvprintw(CB_X, CB_Y + i, "%c", ch);
	}
	move(CB_X, CB_Y);
	clrtoeol();
	move(x, y);
	refresh();
	return arr;
}

void store_key(FILE *fd, char *key, int lower, int upper) {
	fprintf(fd, "%s\n", key);
	fprintf(fd, "%d\n", lower);
	fprintf(fd, "%d\n", upper);
	return;
}

void print_cbError() {
	move(CB_X, CB_Y);
	clrtoeol();
	refresh();
	mvprintw(CB_X, CB_Y, "%s", "Invalid Key, Press F1 to retry Or any other key to continue to editor...");
	refresh();
	return;
}

void print_cbAccept() {
        move(CB_X, CB_Y);
	clrtoeol();
        refresh();
        mvprintw(CB_X, CB_Y, "%s", "Now Select CODE and press 'CTRL+SHIFT+V' to store in CodeBase...");
        refresh();
        return;
}

void print_cbSuccess(char *key) {
	move(CB_X, CB_Y);
	clrtoeol();
        refresh();
        mvprintw(CB_X, CB_Y, "%s%s", "Your code is successfully added in codebase with key: ", key);
        refresh();
	getch();
	move(CB_X, CB_Y);
	clrtoeol();
        return;
}

void print_cbNotFound(char *key) {
	move(CB_X, CB_Y);
        clrtoeol();
        refresh();
        mvprintw(CB_X, CB_Y, "%s%s%s", "Oops! '", key, "' is not pressent in codebase...");
        refresh();
	getch();
        move(CB_X, CB_Y);
        clrtoeol();
        return;

}

void print_ReadOnly() {
        move(CB_X, CB_Y);
        clrtoeol();
        refresh();
        mvprintw(CB_X, CB_Y, "%s", "Read Only File, Cannot write on this file...");
        refresh();
        return;

}


int validate_codebase_key(char* key) {
	for(int i = 0; key[i] != '\0'; i++) {
		if((!isalnum(key[i])) && key[i] != '_')
			return 0;
	}
	return 1;
}

char** init_shortcut_keys() {
	char **shortcut_key = (char**)malloc(sizeof(char*) * TOT_SHORTCUT_KEYS);
        shortcut_key[0] = "printf(\"%\",);";
        shortcut_key[1] = "scanf(\"%\",);";
        shortcut_key[2] = "#include <>";
        shortcut_key[3] = "#define ";
        shortcut_key[4] = "int main() {\n    \n    return 0;\n}";
        shortcut_key[5] = "for(int i = ; )";
        shortcut_key[6] = "while()";
	shortcut_key[7] = "return ;";
	shortcut_key[8] = "typedef struct {\n    \n} ;";
	return shortcut_key;
}

int shortcut_key_indx(int *ch, int *move) {
        switch(*ch) {
               	case CTRL('p'):
                        *ch = SHORTCUT_KEY;
                        *move = 4;
                        return 0;
                case CTRL('a'):
                        *ch = SHORTCUT_KEY;
                        *move = 4;
                        return 1;
		case CTRL('e'):
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 2;
		case CTRL('d'):
                        *ch = SHORTCUT_KEY;
                        *move = 0;
                        return 3;
		case CTRL('n'):
                        *ch = SHORTCUT_KEY;
                        *move = 16;
                        return 4;
		case CTRL('f'):
                        *ch = SHORTCUT_KEY;
                        *move = 3;
                        return 5;
		case CTRL('w'):
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 6;
		case CTRL('r'):
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 7;
		case CTRL('t'):
                        *ch = SHORTCUT_KEY;
                        *move = 10;
                        return 8;
                default:
                        return -1;
        }
}


void check_bracket(int *ch, char *start, char *end) {
	int tmp = *ch;
	*ch = BRACKET;
	switch(tmp) {
		case '(':
			*start = '(';
			*end = ')';
			return;

		case '{':
			*start = '{';
			*end = '}';
			return;
			
		case '[':
			*start = '[';
			*end = ']';
			return;
		default:
			*ch = tmp;
			return;
	}
}
