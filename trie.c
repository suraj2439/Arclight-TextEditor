#include "trie.h"


TrieNode *getNode(void) {
    TrieNode *pNode = NULL;
  
    pNode = (TrieNode *)malloc(sizeof(TrieNode));
  
    if (pNode) {
        int i;
  
	pNode->color = 0;
        pNode->isEndOfWord = 0;

	//adding extra character '_'
        for (i = 0; i < ALPHABET_SIZE + 1; i++)
            pNode->children[i] = NULL;
    }

    return pNode;
}
  

int char_to_index(char c) {
	if(c == '_')
		return ALPHABET_SIZE;
	else return ((int)c - (int)'a'); 
}

void insert(TrieNode *root, const char *key, char color) {
    int level;
    int length = strlen(key);
    int index;
  
    TrieNode *pCrawl = root;
  
    for (level = 0; level < length; level++) {
        index = char_to_index(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
  
        pCrawl = pCrawl->children[index];
    }
  
    // mark last node as leaf
    pCrawl->isEndOfWord = 1;
    pCrawl->color = color;
}
  
// Returns true if key presents in trie, else false
int search(TrieNode *root, const char *key, char *color) {
    int level;
    int length = strlen(key);
    int index;
    struct TrieNode *pCrawl = root;
  
    for (level = 0; level < length; level++) {
	if( (key[level] < 'a' || key[level] > 'z') && key[level] != '_')
		return 0;
        index = char_to_index(key[level]);
  
        if (!pCrawl->children[index])
            return 0;
  
        pCrawl = pCrawl->children[index];
    }
  
    *color = pCrawl->color;
    return (pCrawl != NULL && pCrawl->isEndOfWord);
}






TrieNode_c *codebaseNode(void) {
    TrieNode_c *pNode = NULL;

    pNode = (TrieNode_c *)malloc(sizeof(TrieNode_c));

    if (pNode) {
        int i;

        pNode->isEndOfWord = 0;
	pNode->lower = pNode->upper = 0;

        //adding extra character '_'
        for (i = 0; i < ALPHABET_SIZE + 1; i++)
            pNode->children[i] = NULL;
    }

    return pNode;
}


void insert_in_codebase(TrieNode_c *root, const char *key, int lower, int upper) {
    int level;
    int length = strlen(key);
    int index;

    TrieNode_c *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = char_to_index(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = codebaseNode();

        pCrawl = pCrawl->children[index];
    }

    // mark last node as leaf
    pCrawl->isEndOfWord = 1;
    pCrawl->lower = lower;
    pCrawl->upper = upper;
}


// Returns true if key presents in trie, else false
int search_in_codebase(TrieNode_c *root, const char *key, int *lower, int *upper) {
    int level;
    int length = strlen(key);
    int index;
    struct TrieNode_c *pCrawl = root;

    for (level = 0; level < length; level++) {
        if( (key[level] < 'a' || key[level] > 'z') && key[level] != '_')
                return 0;
        index = char_to_index(key[level]);

        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }

    *lower = pCrawl->lower;
    *upper = pCrawl->upper;
    //printf("search\n%d\n%d\n", *lower, *upper);
    return (pCrawl != NULL && pCrawl->isEndOfWord);
}


/*
int main() {
	char grp1[][8] = {"int", "char", "double", "long", "auto",
                     "signed", "unsigned", "void", "float", "short"};
        char grp2[][8] = {"register", "extern", "static", "volatile", "const"};

        char grp3[][8] = {"typedef", "struct", "enum", "union"};

        char grp4[][8] = {"continue", "break", "return", "sizeof"};

        char grp5[][8] = {"for", "while", "do", "goto"};

        char grp6[][8] = {"if", "else", "switch", "case", "default"};

        TrieNode_c *root = codebaseNode();

        // Construct trie
        int i;
        for (i = 0; i < ARRAY_SIZE(grp1); i++)
                insert_in_codebase(root, grp1[i], "suraj yerkal");
	
        for (i = 0; i < ARRAY_SIZE(grp2); i++)
                insert(root, grp2[i], 1);
        for (i = 0; i < ARRAY_SIZE(grp3); i++)
                insert(root, grp3[i], 5);
        for (i = 0; i < ARRAY_SIZE(grp4); i++)
                insert(root, grp4[i], 25);
        for (i = 0; i < ARRAY_SIZE(grp5); i++)
                insert(root, grp5[i], 9);
        for (i = 0; i < ARRAY_SIZE(grp6); i++)
                insert(root, grp6[i], 7);

	insert_in_codebase(root, "inta", "hello");
	
	char *tp;
	search_in_codebase(root, "inta", &tp);
	for(int i = 0; tp[i] != '\0'; i++)
		printf("%c", tp[i]);

	char tmp;
	printf("%d \n", search(root, "sizeof(", &tmp));
	printf("%d\n", tmp);

	return 0;
}*/

/*
// Driver
int main() {
    // Input keys (use only 'a' through 'z' and lower case)
    char keys[][8] = {"the", "a", "there", "answer", "any",
                     "by", "bye", "their"};
  
    char output[][32] = {"Not present in trie", "Present in trie"};
  
  
    TrieNode *root = getNode();
  
    // Construct trie
    int i;
    for (i = 0; i < ARRAY_SIZE(keys); i++)
        insert(root, keys[i], 0);
    insert(root, "BLACK", 50);
	      	
    char tmp;
    // Search for different keys
    printf("%s --- %s\n", "the", output[search(root, "th", &tmp)] );
    printf("%d\n", tmp);
    
    printf("%s --- %s\n", "these", output[search(root, "these")] );
    printf("%s --- %s\n", "their", output[search(root, "their")] );
    printf("%s --- %s\n", "thaw", output[search(root, "thaw")] );	  
    return 0;
}*/
