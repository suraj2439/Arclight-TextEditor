#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE 	(26)
#define MAX_KEY		100

// Converts key current character into index
// use only 'a' through 'z' and lower case
//#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// trie node
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];

    char color;
    // isEndOfWord is true if the node represents
    // end of a word
    char isEndOfWord;
} TrieNode;

// Returns new trie node (initialized to NULLs)
TrieNode *getNode(void);

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void insert(TrieNode *root, const char *key, char color);

// Returns true if key presents in trie, else false
int search(TrieNode *root, const char *key, char *color);


// trie node
typedef struct TrieNode_c {
    struct TrieNode_c *children[ALPHABET_SIZE];

    int lower, upper;
    // isEndOfWord is true if the node represents
    // end of a word
    char isEndOfWord;
} TrieNode_c;

// Returns new trie node (initialized to NULLs)
TrieNode_c *codebaseNode(void);

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void insert_in_codebase(TrieNode_c *root, const char *key, int lower, int upper);

// Returns true if key presents in trie, else false
int search_in_codebase(TrieNode_c *root, const char *key, int *lower, int *upper);

#endif
