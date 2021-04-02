#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
  
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
  
// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)
  
// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')
  
// trie node
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
  
    char color;
    // isEndOfWord is true if the node represents
    // end of a word
    bool isEndOfWord;
} TrieNode;
 

// Returns new trie node (initialized to NULLs)
TrieNode *getNode(void) {
    TrieNode *pNode = NULL;
  
    pNode = (TrieNode *)malloc(sizeof(TrieNode));
  
    if (pNode) {
        int i;
  
	pNode->color = 0;
        pNode->isEndOfWord = false;
  
        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }

    return pNode;
}
  
// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void insert(TrieNode *root, const char *key, char color) {
    int level;
    int length = strlen(key);
    int index;
  
    TrieNode *pCrawl = root;
  
    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
  
        pCrawl = pCrawl->children[index];
    }
  
    // mark last node as leaf
    pCrawl->isEndOfWord = true;
    pCrawl->color = color;
}
  
// Returns true if key presents in trie, else false
bool search(TrieNode *root, const char *key, char *color) {
    int level;
    int length = strlen(key);
    int index;
    struct TrieNode *pCrawl = root;
  
    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(key[level]);
  
        if (!pCrawl->children[index])
            return false;
  
        pCrawl = pCrawl->children[index];
    }
  
    *color = pCrawl->color;
    return (pCrawl != NULL && pCrawl->isEndOfWord);
}
  

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
    /*
    printf("%s --- %s\n", "these", output[search(root, "these")] );
    printf("%s --- %s\n", "their", output[search(root, "their")] );
    printf("%s --- %s\n", "thaw", output[search(root, "thaw")] );
*/		  
    return 0;
}
