/* COP 3502C Assignment 5
This program is written by: Kevin Rodriguez */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 25

typedef struct student_node {
  int fine;
  char name[MAX_SIZE];
  int height;
  struct student_node *left;
  struct student_node *right;
}student_node;

student_node *create_node(int val, char* name) {
  // Allocate space for the node.
  student_node* temp;
  temp = (student_node *)malloc(sizeof(student_node));
  temp->fine = val;
  temp->height = 0;
  strncpy(temp->name, name, MAX_SIZE);
  temp->left = NULL;
  temp->right = NULL;
  return temp; // Return a pointer to the created node.
}

int Height(student_node* node) {
    if (node == NULL)
        return -1;  // Return -1 for NULL to represent height for an empty subtree

    int left_height = Height(node->left);   // Height of the left subtree
    int right_height = Height(node->right); // Height of the right subtree

    return 1 + (left_height > right_height ? left_height : right_height);
}

student_node* insert(student_node *root, student_node *element, int h) {

  // Inserting into an empty tree.
  if (root == NULL){
         printf("%s %d %d\n", element->name, element->fine, h);
    return element;
  }
  else {
    // element should be inserted to the right.
    if (strcmp(element->name, root->name) > 0) {
      // There is a right subtree to insert the node.
      if (root->right != NULL) {
        root->right = insert(root->right, element, h + 1);
      // Place the node directly to the right of root.
      }else {
        root->right = element;
        element->height = h;
        printf("%s %d %d\n", element->name, element->fine, element->height);
      }
    }

    // element should be inserted to the left.
    else if (strcmp(element->name, root->name) < 0) {
      // There is a left subtree to insert the node.
      if (root->left != NULL){
        root->left = insert(root->left, element, h + 1);
      // Place the node directly to the left of root.
      }else {
        root->left = element;
        element->height = h;
        printf("%s %d %d\n", element->name, element->fine, element->height);
      }
    }
    // Return the root pointer of the updated tree.
    return root;
  }
}

int calc_below(student_node* root, char* key) {
  int count = 0;
  if (root == NULL) {
    return 0;
  }
  if (root != NULL) {
    if (strcmp(root->name, key) < 0 || strcmp(root->name, key) == 0) {
      count += root->fine;
    }
    count += calc_below(root->left, key);
    count += calc_below(root->right, key);
  }
  return count;
}

double fine_sum(student_node* root) {
  double fine_count = 0;
  if (root == NULL) {
    return 0;
  }
  if (root != NULL) {
    fine_count += root->fine;

    fine_count += fine_sum(root->left);
    fine_count += fine_sum(root->right);
  }
  return fine_count;
}

double node_count(student_node* root) {
  double count = 1;
  if (root == NULL) {
    return 0;
  }

  count += node_count(root->left);
  count += node_count(root->right);

  return count;
}

student_node* search(student_node* root, char* key) {
  if (root == NULL) {
    return root;
  } 
  if (strcmp(root->name, key) == 0) {
      return root;
  }
  if (strcmp(root->name, key) > 0) {
    return search(root->left, key);
  } else if (strcmp(root->name, key) < 0) {
    return search(root->right, key);
  }
  return NULL;
}

// Returns 1 if node is a leaf node, 0 otherwise.
int isLeaf(student_node *node) {

  return (node->left == NULL && node->right == NULL);
}

// Returns 1 iff node has a left child and no right child.
int hasOnlyLeftChild(student_node *node) {
  return (node->left!= NULL && node->right == NULL);
}

// Returns 1 iff node has a right child and no left child.
int hasOnlyRightChild(student_node *node) {
  return (node->left== NULL && node->right != NULL);
}


// Returns the parent of the node pointed to by node in the tree rooted at
// root. If the node is the root of the tree, or the node doesn't exist in
// the tree, null will be returned.
student_node* parent(student_node *root, student_node *node) {
  // Take care of NULL cases.
  if (root == NULL || root == node)
    return NULL;

  // The root is the direct parent of node.
  if (root->left == node || root->right == node)
    return root;

  // Look for node's parent in the left side of the tree.
  if (strcmp(node->name, root->name) < 0)
    return parent(root->left, node);

  // Look for node's parent in the right side of the tree.
  else if (strcmp(node->name, root->name) > 0)
    return parent(root->right, node);

  return NULL; // Catch any other extraneous cases.

}
// Returns a pointer to the node storing the maximum value in the tree
// with the root, root. Will not work if called with an empty tree.
student_node* maxVal(student_node *root) {

  // Root stores the maximal value.
  if (root->right == NULL)
    return root;

  // The right subtree of the root stores the maximal value.
  else
    return maxVal(root->right);
}

// Returns a pointer to a node that stores value in it in the subtree
// pointed to by current_ptr. NULL is returned if no such node is found.
student_node* findNode(student_node *current_ptr, char* key) {

  // Check if there are nodes in the tree.
  if (current_ptr != NULL) {

    // Found the value at the root.
    if (strcmp(current_ptr->name, key) == 0)
      return current_ptr;

    // Search to the left.
    if (strcmp(current_ptr->name, key) > 0)
      return findNode(current_ptr->left, key);

    // Or...search to the right.
    else
      return findNode(current_ptr->right, key);

  }
  else
    return NULL; // No node found.
}

student_node* delete(student_node* root, char* key) {
    student_node *delnode, *new_del_node, *save_node;
    student_node *par;
    int save_fine, save_height;
    char save_name[MAX_SIZE];

    // Find the node to delete
    delnode = findNode(root, key); 
    par = parent(root, delnode);

    // Case 1: Node to delete is a leaf node (no children)
    if (isLeaf(delnode)) {
        if (par == NULL) { // Deleting the root node, which is a leaf
            free(root);
            return NULL;
        }

        if (strcmp(key, par->name) < 0) {
            free(par->left);
            par->left = NULL;
        }

        else {
            free(par->right);
            par->right = NULL;
        }
        return root;
    }

    // Case 2: Node to delete has only a left child
    if (hasOnlyLeftChild(delnode)) {
        if (par == NULL) { // Deleting the root node
        save_node = delnode->left;
        free(delnode);
        preorderHeight(save_node);
        return save_node;  // New root is the left child
        }

        // Adjust parent's pointer
        if (strcmp(key, par->name) < 0) {
            save_node = par->left;
            par->left = par->left->left;
            free(save_node);
            preorderHeight(par->left);
        }
        else {
            save_node = par->right;
            par->right = par->right->left;
            free(save_node);
            preorderHeight(par->right);
        }


        return root;
    }

    // Case 2.1: Node to delete has only a right child
    if (hasOnlyRightChild(delnode)) {
        if (par == NULL) {
            save_node = delnode->right;
            free(delnode);
            preorderHeight(save_node);
            return save_node;  // New root is the right child
        }

        if (strcmp(key, par->name) < 0) {
            save_node = par->left;
            par->left = par->left->right;
            free(save_node);
            preorderHeight(par->left);
        }
        else {
            save_node = par->right;
            par->right = par->right->right;
            free(save_node);
            preorderHeight(par->right);
        }
        return root;
    }

    // Case 3: Node to delete has two children
    new_del_node = maxVal(delnode->left); // Find max node in left subtree

    strcpy(save_name,new_del_node->name);
    save_fine = new_del_node->fine;
    save_height = delnode->height;

    delete(root, save_name);

    strcpy(delnode->name, save_name);
    delnode->fine = save_fine;
    delnode->height = save_height;

    return root;
}

void preorderHeight(student_node* root) {
    if (root != NULL) {
        root->height -= 1;
        preorderHeight(root->left);
        preorderHeight(root->right);
    }
}

void preorderPrint(student_node* root) {
    if (root != NULL) {
        printf("%s %d %d\n", root->name, root->fine, root->height);
        preorderHeight(root->left);
        preorderHeight(root->right);
    }
}

student_node* deduct(student_node* root, char* key, int val) {
  student_node* temp;
  temp = root;
  if (root == NULL) {
    return;
  }
  temp = search(root, key);
  if (temp == NULL) {
      return;
  }
  temp->fine -= val;
  if (temp->fine <= 0) {
    temp->fine = 0;
    root = delete(root, temp->name);
    printf("%s removed\n", key);
    return root;
  } else {
    printf("%s %d %d\n", temp->name, temp->fine, temp->height);
    return root;
  }
}

void free_tree(student_node* root) {
    if (root == NULL) {
        return;
    }

    // Recursively free the left and right subtrees
    free_tree(root->left);
    free_tree(root->right);

    // Free the current node
    free(root);
}


int main() {
  int n, fine, h = 0, count = 0;
  char command[MAX_SIZE];
  char name[MAX_SIZE];
  student_node *my_root=NULL, *temp_node;
  scanf("%d", &n);
  for (int i=0; i<n; i++) {
    scanf("%s", command);
    if (strcmp(command, "add") == 0) {
      if (my_root == NULL) {
        scanf("%s%d", name, &fine);
        my_root = create_node(fine, name);
        insert(NULL, my_root, h);
      }
      else {
        scanf("%s%d", name, &fine);
        if (search(my_root, name) == NULL) {
        temp_node = create_node(fine, name);
        insert(my_root, temp_node, h + 1);
        } else {
            temp_node = search(my_root, name);
            temp_node->fine += fine;
            printf("%s %d %d\n", temp_node->name, temp_node->fine, temp_node->height);
        }
      }
    } else if (strcmp(command, "height_balance") == 0) {
        int lh = 0, rh = 0;
        lh = Height(my_root->left);
        rh = Height(my_root->right);
        if (lh == rh) {
            printf("left height = %d right height = %d balanced\n", lh, rh);
        } else {
            printf("left height = %d right height = %d not balanced\n", lh, rh);
        }
    } else if (strcmp(command, "calc_below") == 0) {
        scanf("%s", name);
        int count = calc_below(my_root, name);
        printf("%d\n", count);
    } else if (strcmp(command, "average") == 0) {
        double average = fine_sum(my_root) / node_count(my_root);
        printf("%.2f\n", average);
    } else if (strcmp(command, "deduct") == 0) {
        scanf("%s%d", name, &fine);
        if (findNode(my_root, name) == NULL) {
            printf("%s not found\n", name);
        } else {
        my_root = deduct(my_root, name, fine);
        }
    } else if (strcmp(command, "search") == 0) {
        scanf("%s", name);
        temp_node = search(my_root, name);
        if (temp_node != NULL) {
            printf("%s %d %d\n", temp_node->name, temp_node->fine, temp_node->height);
        } else {
            printf("%s not found\n", name);
        }
    }
  }

  free_tree(my_root);

  return 0;
}
