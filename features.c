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
        // 3(orange), 15(white), 9(red), 10(light_green), 11(yerllow), 14(light blue), 21(dark blue), 39(light blue)

}

void init_keywords() {
	char keys[][8] = {"the", "a", "there", "answer", "any",
                     "by", "bye", "their"};
	
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

