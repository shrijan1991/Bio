#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// Assignment 3: Defines the entry point for the console application.


typedef enum { false, true } bool;

///A C struct representing a node
struct Node {
	struct Node *parent;
    bool isLeaf;
    int interval[20];
    int intervalSize;
    struct Node *right;
    struct Node *left;
    int leafLabel;
    // Find some other way to do this better
    int treeNum;
};

/// function to initialize a node **Equivalent to constuctor in oop**
/// Summary - create  a new node
/// Params 1. _parent - Parent node *NULL for Root*
/// Returns - an initialized node
struct Node* newNode(struct Node* _parent, int treeNum)
{
	// Allocate memory for new node
	struct Node* node = (struct Node*) malloc(sizeof(struct Node));
	// Set it as not a leaf
    node->isLeaf = false;
    // Initialize all relevant variables to 0
    node->intervalSize = 0;
    node-> treeNum = treeNum;
    node->left = NULL;
    node->right = NULL;
    node->parent = _parent;
	return(node);
}

struct Node *goodEdge[10];
int goodEdgeHash[10];
int relabel[10];
int hashtable[10][10];
int hashtablesize[10];
int orderCounter = 1;
int goodEdgeCount = 0;
char treeVal[10][60];
int treeValEnd[10];
int MAXDEPTH1 = -1;

/// Recursive function to pre order traverse the tree
/// Summary - Does multiple things based on the tree we are working on
/// T1. adds to hashtable
/// T2. computes goodedge
/// Params 1. cur -> current node
/// Returns - nothing
void preOrderTraverse(struct Node* cur) {
    int i,j, max, min, size;
    // For internal nodes
    if (cur != NULL && cur->isLeaf == false) {
        //For T1: Add to hashtable from internal node
        if (cur->treeNum == 1) {
            if (cur->parent == NULL) {  // If the current node is root
                j = cur->interval[cur->intervalSize - 1];
                hashtablesize[j] = cur->intervalSize;
                for (i = 0; i< cur->intervalSize; i++) {
                    hashtable[j][i] = cur->interval[i];

                }
            } else if(cur->parent->left == cur) { // If the current node is left node
                j = cur->interval[cur->intervalSize - 1];
                hashtablesize[j] = cur->intervalSize;
                for (i = 0; i< cur->intervalSize; i++) {
                    hashtable[j][i] = cur->interval[i];

                }
            } else {                // If the current node is right node
                j = cur->interval[0];
                hashtablesize[j] = cur->intervalSize;
                for (i = 0; i< cur->intervalSize; i++) {
                    hashtable[0][i] = cur->interval[i];
                }
            }
        } else {            // For T2: calculate min, max and size of the interval, and check if the node is
            // Arbitrary large number for min
            min = 100000;
            max =  0;
            size = cur->intervalSize;
            for (i = 0; i< cur->intervalSize; i++) { // get min and max
                min = cur->interval[i] < min ? cur->interval[i] : min;
                max = cur->interval[i] > max ? cur->interval[i] : max;
            }
            if(size == max - min + 1) {
                if ((min == hashtable[min][0] && max== hashtable[min][hashtablesize[min] - 1]) || (min == hashtable[max][0] && max== hashtable[max][hashtablesize[max] - 1])) { // check with min and max
                    goodEdge[goodEdgeCount] = cur;
                    goodEdgeHash[goodEdgeCount] = (min == hashtable[min][0] && max== hashtable[min][hashtablesize[min] - 1]) ? min: max;
                    goodEdgeCount++;
                }
            }
        }
        preOrderTraverse(cur->left);
        preOrderTraverse(cur->right);
    }
}

/// Recursive function to postordertraverse and relabel the tree
/// Summary - Does multiple things based on the tree we are working on
/// T1. relabels the tree in increasing order
/// T2. relabels the tree based on first tree
/// Params 1. cur -> current node
/// Returns - nothing
void relabelLeaves(struct Node *cur ) {
    // Intervals is the accumulation of interval count
    int i,intervals[10];
    int intervalCount = 0;
    if(cur != NULL && cur->isLeaf == false) { // For internal nodes
        relabelLeaves(cur->left);
        struct Node *left = cur->left;
        if(left->isLeaf) {
            intervals[intervalCount] = left->leafLabel;
            intervalCount++;
        } else {
            for (i=0; i<left->intervalSize; i++){
                intervals[intervalCount] = left->interval[i];
                intervalCount++;
            }
        }
        relabelLeaves(cur->right);

        struct Node *right = cur->right;
        if(right != NULL) {
            if(right->isLeaf) {
                intervals[intervalCount] = right->leafLabel;
                intervalCount++;
            } else {
                for (i=0; i<right->intervalSize; i++){
                    intervals[intervalCount] = right->interval[i];
                    intervalCount++;
                }
            }
            if(cur->treeNum == 2) {
                cur->intervalSize = 0;
            }
            // Set interval in internal nodes
            for(i=0;i<intervalCount; i++) {
                cur->interval[cur->intervalSize] = intervals[i];
                cur->intervalSize += 1;
            }
        }


    }
    if (cur != NULL && cur->isLeaf == true) { // for leaf relabel

        if(cur->treeNum == 1) {
            relabel[cur->leafLabel] = orderCounter;
            cur->leafLabel = orderCounter;
            orderCounter++;
        } else {
            //printf("%d ", cur->leafLabel);
            cur->leafLabel = relabel[cur->leafLabel];
            //printf("%d ", cur->leafLabel);
        }
    }

}

/// Function to print tree - preorder
void buildTreeString(struct Node *cur, int depth, int spaces, bool removeGoodEdge) {
    int lvlspc = (int) pow(2, MAXDEPTH1  - depth - 1) * 4; // The number of spaces between nodes at that depth
    int i, j;
    bool flag = false;
    if(cur == NULL) {
        return;
    }
    // Insert internal node
    if(cur->isLeaf == false) {
        treeVal[depth][spaces] = 'N';
    }
    // If left node is leaf insert to appropriate position else go to the internal node
    if(cur->left->isLeaf == true) {
        treeVal[MAXDEPTH1][spaces - lvlspc] = cur->left->leafLabel + '0';
        for(i = spaces - lvlspc+ 1; i<spaces; i++) {
            treeVal[depth][i] = '-';
        }
        for(i=depth; i<MAXDEPTH1; i++) {
            treeVal[i][spaces - lvlspc] = '|';
        }
    } else {
        buildTreeString(cur->left, depth + 1, spaces/2, removeGoodEdge);
        if(removeGoodEdge == true) {
            for(i=0; i < goodEdgeCount; i++) {
                if (cur->left->treeNum == 1) {
                    flag = true;
                    for(j = 0; j < cur->left->intervalSize; j++) {
                        if (hashtable[goodEdgeHash[i]][j] != cur->left->interval[j]) {
                            flag = false;
                            break;
                        }
                    }
                } else {
                    if(goodEdge[i] == cur->left) {
                        flag = true;
                        break;
                    }
                }

            }
        }
        if (flag == false) {
            for(i = spaces/2; i<spaces; i++) {
                treeVal[depth][i] = '-';
                }
            treeVal[depth][spaces/2] = '|';
        }


    }
    // If right node is leaf insert to appropriate position else go to the internal node
    if(cur->right->isLeaf == true) {
        treeVal[MAXDEPTH1][spaces + lvlspc] = cur->right->leafLabel + '0';
        for(i = spaces + 1; i<spaces+lvlspc; i++) {
            treeVal[depth][i] = '-';
        }
        for(i=depth; i<MAXDEPTH1; i++) {
            treeVal[i][spaces + lvlspc] = '|';
        }
    } else {
        buildTreeString(cur->right, depth + 1, spaces + lvlspc/2, removeGoodEdge);
        if(removeGoodEdge == true) {
            for(i=0; i < goodEdgeCount; i++) {
                if (cur->right->treeNum == 1) {
                    flag = true;
                    for(j = 0; j < cur-> right->intervalSize; j++) {
                        if (hashtable[goodEdgeHash[i]][j] != cur->right->interval[j]) {
                            flag = false;
                            break;
                        }
                    }
                } else {
                    if(goodEdge[i] == cur->right) {
                        flag = true;
                        break;
                    }
                }

            }
            if (flag == false) {
                for(i = spaces+1; i<spaces + lvlspc/2; i++) {
                    treeVal[depth][i] = '-';
                }
                treeVal[depth][spaces + lvlspc/2] = '|';
            }

        }

    }


}

/// Function get maxdepth of tree by inorder traversal
void inOrderTraverse(struct Node *cur, int depth) {
    if (cur == NULL) {
        return;
    }
    if(depth > MAXDEPTH1) {
        MAXDEPTH1 = depth;
    }
    inOrderTraverse(cur->left, depth + 1);
    inOrderTraverse(cur->right, depth + 1);
}


/// Function to print tree
void printTree(struct Node *T, bool removeGoodEdge) {
    int i, j;
    inOrderTraverse(T->left, 0); // Inorder traversal to get max depth
    int k = pow(2, MAXDEPTH1) * 4; // 2 to the power depth of the tree no of nodes * spaces at max depth

    for(i =0; i<= MAXDEPTH1; i++) {
        for(j =0; j< k; j++) {
            treeVal[i][j] = ' '; // Add spaces
        }
    }
    for(j =0; j< k/2; j++) {
        printf(" "); // Add spaces halfway
    }
    printf("%d\n", T->leafLabel); // Add rooted leaf
    for(j =0; j< k/2; j++) {
        printf(" ");
    }
    printf("|\n");   // Add pipe symbol
    buildTreeString(T->left,0,k/2, removeGoodEdge);
    for (i = 0; i<=MAXDEPTH1; i++) {
        printf("%s\n", treeVal[i]);
    }


}

/// Days algorithm
int days(struct Node *T1, struct Node *T2){

    printf("\n----------------------Before relabeling-----------------\n");
    printf("\nTree T1 is, \n");
    printTree(T1, false);
    printf("\nTree T2 is,\n");
    printTree(T2, false);
    /// First for loop in Days algorithm
    relabelLeaves(T1);
    relabelLeaves(T2);
    printf("\n------------------------After relabeling----------------\n");
    printf("\nTree T1 is, \n");
    printTree(T1, false);
    printf("\nTree T2 is,\n");
    printTree(T2, false);

    /// Second for loop in Days algorithm
    preOrderTraverse(T1);
    preOrderTraverse(T2);
    printf("\n------------------After removal of edge----------------\n");
    printf("\nTree T1 is, \n");
    printTree(T1, true);
    printf("\nTree T2 is,\n");
    printTree(T2, true);
    return 0;
}

int main()
{
    int i, n, leafLabel, leafCounter = 0;
    struct Node *cur,*Trees[2];
    printf("Enter the number of taxa: ");
    scanf("%d", &n);
    /// Building the tree with user input
    printf("The tree has %d internal nodes.\n", n - 2);
    for(i = 0; i < 2; i++) {
        printf("\n------------------------------------------------------\n");
        printf("Please enter the rooted phylogenic tree T%d.\nThe highest value label has been set as root with its first internal node as its child.\n", i+1);
        Trees[i] = newNode(NULL, i+1);
        cur = Trees[i];
        cur->isLeaf = false;
        cur->leafLabel = n;
        cur->left = newNode(cur, i+1);
        cur = cur->left;
        while (leafCounter < n-1) {
            /// Set value for left node
            printf("Enter the value for LEFT node(positive number for leaf node, 0 for internal node)? ");
            scanf("%d", &leafLabel);
            cur->left = newNode(cur, i+1);
            if(leafLabel >= 1) {
                leafCounter++;
                cur->left->isLeaf = true;
                cur->left->leafLabel = leafLabel;
            }
            // set value for right node
            printf("Enter the value for RIGHT node(positive number for leaf node, 0 for internal node)? ");
            scanf("%d", &leafLabel);
            cur->right = newNode(cur, i+1);
            if(leafLabel >= 1) {
                leafCounter++;
                cur->right->isLeaf = true;
                cur->right->leafLabel = leafLabel;
            }
            if(cur->left->isLeaf == false) {
                cur = cur->left;
                printf("Moving to internal node on left.\n");
            } else if (cur->right->isLeaf == false) {
                cur = cur->right;
                printf("Moving to internal node on right.\n");
            }
        }
        leafCounter = 0;
    }

    days(Trees[0], Trees[1]);

    /// Robinson-Fould's distance
    /// 2n-3 Edges in an unrooted binary tree, n -1 leaves and goodedge count = No. of bad edges in T2 wrt to T1 (Are symmetrical)
    printf("\n------------------------------------------------------\n");


    printf("Total no of edges: %d\nLeaf nodes: %d\nGood edges excluding rooted leaf: %d\n", 2*n -3, n, goodEdgeCount - 1);
    printf("The Robinson-Folds distance for two trees provided is, %d\n\n", (2*n -3 - (n - 1) - goodEdgeCount));

	return 0;
}

