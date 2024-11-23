/* COP 3502C Programming Assignment 6
This program is written by: Kevin Rodriguez */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_CHARS 2000000

struct trie {
    int Node_Freq;
    int Sum_Prefix_Freq;
    int Max_Freq;
    int isWord;//0/1
    struct trie* next[26];
};

struct trie* init();
void insert(struct trie* tree, char word[], int k, int freq);
int search(struct trie* tree, char word[], int k) ;
void printAll(struct trie* tree, char cur[], int level) ;
struct trie* del(struct trie* root, char key[], int depth);
int isEmpty(struct trie* root);
char* query(struct trie* tree, char word[], int k);

void freeDictionary(struct trie* tree);

int main() {

    struct trie* myDictionary = init();

    // Read in number of words.
    int i, n;
    int commandNum;

    scanf("%d", &n);

    for (i=0; i< n; i++) {
        char word[MAX_CHARS];
        int freq;
        char prefix[MAX_CHARS];
        scanf("%d", &commandNum);
        if (commandNum == 1) {
            scanf("%s", word);
            scanf("%d", &freq);
            insert(myDictionary, word, 0, freq);
        }
        else if (commandNum == 2) {
            scanf("%s", prefix);
            char* result = query(myDictionary, prefix, 0); 
            if (result == NULL || result[0] == '\0') { 
                printf("unrecognized prefix\n");
            } else {
                printf("%s\n", result); 
            }
            free(result);
        }
    }
    freeDictionary(myDictionary);
    return 0;
}

struct trie* init() {

    // Create the struct, not a word.
    struct trie* myTree = malloc(sizeof(struct trie));
    myTree->Node_Freq = 0;
    myTree->Sum_Prefix_Freq = 0;
    myTree->Max_Freq = 0;
    myTree->isWord = 0;

    // Set each pointer to NULLL.
    int i;
    for (i=0; i<26; i++)
        myTree->next[i] = NULL;

    // Return a pointer to the new root.
    return myTree;
}

//checks if all the 26 childners are NULL or not.
int isEmpty(struct trie* root)
{
    for (int i = 0; i < 26; i++)
        if (root->next[i])
            return 0;
    return 1;
}


// Recursive function to delete a key from given Trie
struct trie* del(struct trie* root, char key[], int k)
{
    // If tree is empty
    if (!root)
        return NULL;

    // If last character of key is being processed
    if (k == strlen(key)) {

        // This node is no more end of word after
        // removal of given key
        if (root->isWord) //if it is a valid word
            root->isWord = 0; //mark it as not word anymore

        // If it is not prefix of any other word
        if (isEmpty(root)) {
            free (root);
            root = NULL;
        }

        return root;
    }

    // If not last character, recur for the child
    int index = key[k] - 'a';
    root->next[index] = del(root->next[index], key, k + 1);

    // If root does not have any child (its only child got
    // deleted), and it is not end of another word.
    if (isEmpty(root) && root->isWord == 0) {
        free (root);
        root = NULL;
    }

    return root;
}


void insert(struct trie* tree, char word[], int k, int freq) {
    // Update the sum prefix frequency for the current node
    tree->Sum_Prefix_Freq += freq;
    // Base case: End of the word
    if (k == strlen(word)) {
        tree->isWord = 1;
        tree->Node_Freq += freq;
        return;
    }

    // Calculate the next index
    int nextIndex = word[k] - 'a';

    // If the next node doesn't exist, create it
    if (tree->next[nextIndex] == NULL) {
        tree->next[nextIndex] = init();
    }

    // Recur to the next node
    insert(tree->next[nextIndex], word, k + 1, freq);

    // Update the maximum child frequency after the recursive call
    tree->Max_Freq = 0; // Reset to recalculate
    for (int i = 0; i < 26; i++) {
        if (tree->next[i] != NULL) {
            int childFreq = tree->next[i]->Sum_Prefix_Freq;
            if (childFreq > tree->Max_Freq) {
                tree->Max_Freq = childFreq;
            }
        }
    }
}

int search(struct trie* tree, char word[], int k) {

    if (k == strlen(word))
        return tree->isWord;

    // If the next place doesn't exist, word is not a word.
    int nextIndex = word[k] - 'a';
    if (tree->next[nextIndex] == NULL)
        return 0;

    return search(tree->next[nextIndex], word, k+1);
}

// Just frees all the memory pointed to by tree (directly and indirectly)
void freeDictionary(struct trie* tree) {

    int i;
    for (i=0; i<26; i++)
        if (tree->next[i] != NULL)
            freeDictionary(tree->next[i]);

    free(tree);
}

char* query(struct trie* tree, char prefix[], int k) {
    if (k == strlen(prefix)) {
        char* prediction = (char*) malloc(MAX_CHARS * sizeof(char)); // allocates memory for prediction string
        if (!prediction) {
            printf("Memory allocation failed\n");
            return NULL;
        }
        prediction[0] = '\0';
        int count = 0; //keeps track if more than one char that matches MAX_freq
        for (int i = 0; i < 26; i++) {
            if (tree->next[i] != NULL && tree->next[i]->Sum_Prefix_Freq == tree->Max_Freq) {
                count++;
                int len = strlen(prediction); //gets length of prediction string currently
                prediction[len] = (char)('a' + i); //adds char to prediction string
                prediction[len + 1] = '\0'; //adds null terminator to end of string
            }
        }
        if (count > 1) { //if more than one char that matches MAX_freq
            int len = strlen(prediction);
            for (int j = 0; j < len - 1; j++) {
                for (int k = 0; k < len - j - 1; k++) {
                    if (prediction[k] > prediction[k+1]) { //bubble sort to sort prediction string
                        char temp = prediction[k];
                        prediction[k] = prediction[k+1];
                        prediction[k+1] = temp;
                    }
                }
            }
        }
        return prediction;
    }

    int nextIndex = prefix[k] - 'a';
    if (tree->next[nextIndex] == NULL) { //if next char doesn't exist
        return NULL;
    } else {
        return query(tree->next[nextIndex], prefix, k + 1); //recurse to next char in given prefix.
    }
}

// Prints all words stored in the trie in alphabetical order.
void printAll(struct trie* tree, char cur[], int level) {

    // Stop!
    if (tree == NULL) return;

    // Print this node, if it's a word.
    if (tree->isWord) {
        cur[level] = '\0';
        printf("%s\n", cur);
        printf("%d\n", tree->Node_Freq);
    }

    // Recursively print all words in each subtree,
    // in alpha order.
    int i;
    for (i=0; i<26; i++) {
        if (tree->next[i]) {
            cur[level] = 'a' + i;
            printAll(tree->next[i], cur, level + 1);
        }
    }
}
