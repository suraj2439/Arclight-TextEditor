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

        char grp3[][8] = {"typedef", "struct", "enum", "union"};

        char grp4[][8] = {"continue", "break", "return", "sizeof"};

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


char** init_shortcut_keys() {
	char **shortcut_key = (char**)malloc(sizeof(char*) * 7);
        shortcut_key[0] = "printf(\"%\");";
        shortcut_key[1] = "scanf(\"%\");";
        shortcut_key[2] = "#include <>";
        shortcut_key[3] = "#define ";
        shortcut_key[4] = "int main() {\n    \n    return 0;\n}";
        shortcut_key[5] = "for(int i = 0; )";
        shortcut_key[6] = "while()";
	return shortcut_key;
}

int shortcut_key_indx(int *ch, int *move) {
        switch(*ch) {
                case 'p':
                        *ch = SHORTCUT_KEY;
                        *move = 3;
                        return 0;
                case 's':
                        *ch = SHORTCUT_KEY;
                        *move = 3;
                        return 1;
                case 'i':
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 2;
                case 'd':
                        *ch = SHORTCUT_KEY;
                        *move = 0;
                        return 3;
                case 'm':
                        *ch = SHORTCUT_KEY;
                        *move = 16;
                        return 4;
                case 'f':
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 5;
                case 'w':
                        *ch = SHORTCUT_KEY;
                        *move = 1;
                        return 6;
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
