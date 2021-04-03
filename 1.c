#include <stdio.h>
#include <stdlib.h>

// Structure defined for each node
struct node {
	int value;
	struct node* left;
	struct node* right;
};

//function which initializes a new node
struct node* createNode(int data) {
    int nodeSize = sizeof(struct node);
    struct node* newNode = malloc(nodeSize);
    newNode->value = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}


//inserts a node in BST
struct node* insertNode(struct node* current, int data) {
    // if the current reaches NULL, data is to be inserted here
    if (current == NULL)
        return createNode(data);
    if (current->value < data)
        current->right = insertNode(current->right, data); 
    else if (current->value > data)
        current->left  = insertNode(current->left, data); 
 
    return current;
}

void inOrderTraversal(struct node* current){  //recursive code to print in-order
    if(current == NULL)  // reaches end of any leaf and can’t go any deeper in tree
       return;
    
    inOrderTraversal(current->left);
    printf("%d \n", current->value);
    inOrderTraversal(current->right);
}


int main(){
    struct node *root;
    root  = NULL;
    root = insertNode(root, 8);
    insertNode(root, 3);
    insertNode(root, 1);
    insertNode(root, 6);
    insertNode(root, 7);
    insertNode(root, 10);
    insertNode(root, 14);
    insertNode(root, 4);

    inOrderTraversal(root);
    return 0;
}
