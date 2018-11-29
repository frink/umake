#include "target.h"
#include "arg_parse.h"
/* Add Target
 * n    Current node
 * t    Target to be placed into current node
 *
 * Adds target to current node
 *
 */
static void addTarget(Node* n, char* t);

/* Print String List
 * l    String list given 
 *
 * Prints all strings held in current list l
 *
 */
static void printStrList(strList l);

/* Node Append
 * n    Pointer to the head of the linked list
 * t    Target to be added
 *
 * Traverses linked list and sets the last node to the new node created
 * Also adds target to the new node
 *
 */
void nodeAppend(list* head, char* t) {

    Node* newNode = (Node*) malloc(sizeof(Node));
    Node* lastNode = *head;

    addTarget(newNode, t);

    newNode->next = EMPTY;

    if(*head == EMPTY)  
        *head = newNode;

    else {
        while(lastNode->next != EMPTY)
            lastNode = lastNode->next;
        lastNode->next = newNode;
    }
}


/* Returns last node
 * n    head of the linked list
 *
 * Iterates through the linked list and returns the last node that doesn't 
 * have a next.
 *
 * Used for adding rules to the most recent target created
 *
 */
Node* getLastNode(Node* n) {

    while(n->next != EMPTY)
        n = n->next;

    return n;

}

/* Node find
 * n    Head of the linked list
 * s    String to be compared
 *
 * Iterates through the linked list and returns the node with the matching
 * target. Used for recursively running dependencies
 *
 */
Node* nodeFind(Node* n, char* s) {
    
    Node* curr = n;

    while(curr != EMPTY) {

        if(strcmp(curr->target, s) == 0)
            return curr;
        
        curr = curr->next;

    }

    return EMPTY;
}

/* Add Target
 * n    Current node
 * t    Target to be placed into current node
 *
 * Adds target to current node
 *
 */
static void addTarget(Node* n, char* t) {

    int dependencies = 0;
    char** targsAndDep = arg_parse(t, &dependencies);


    int i = 1;
    char* dep;
    while(targsAndDep[i] != EMPTY) {
        
        if( strcmp(targsAndDep[i], ":") != 0) {
            dep = strdup(targsAndDep[i]);
            strListAppend(&n->dependencies, dep);
        }
        i++;
    }

    char* token = strdup(targsAndDep[0]);
    char* target;

    target = strtok(token, ":");

    n->target = strdup(target);
}

/* Free Node Data
 * n    Current node
 *
 * Iterates through the entire linked list and frees all of the attributes
 * and the node itself
 */
void freeNodeData(Node* n) {

    while(n != EMPTY) {
    
        Node* curr = n;
        free(n->target);

        while(n->rules != EMPTY) {
          free(n->rules->string);
          n->rules = n->rules->next;
        }

        while(n->dependencies != EMPTY) {
          free(n->dependencies->string);
          n->dependencies = n->dependencies->next;
        }
        
        n = n->next;
        free(curr);
    }
}

/* Print List
 * n    Current node
 *
 * Iterates through the entire linked list and prints all of the data from
 * each node.
 *
 * Used for debugging purposes
 *
 */
void printList(Node *n) {

    int i = 1;
    while (n != EMPTY) {
        printf("Node Number %d Data\n", i);
        printf("###################");
        printf("Target: %s\n", n->target);
        printf("Rules:\n");
        printStrList(n->rules);
        printf("Dependencies:\n");
        printStrList(n->dependencies);
        n = n->next;
        ++i;
        printf("\n");
    }
}

/* String List Append
 * l     list to be traversed
 * s     string attribute of new list will be set to s
 *
 * Appends a strList object to the end of the given list with string s
 *
 */
void strListAppend(strList* l,  char* s) {

    strNode* new = malloc(sizeof(strNode));
    new->next = EMPTY;
    new->string = strdup(s);

    while(*l != EMPTY)
        l = &((*l)->next);

    *l = new;
}

/* String List find
 * l     List to be traversed
 * s     String to find
 *
 * Iterates through a given list and returns the string node with the given
 * string as an attribute
 *
 * Used for debugging purposes
 *
 */
strNode* strListFind(strList l, char* s) {

    while(l != EMPTY && strcmp(l->string, s) != 0)
        l = l->next;

   return l;

}

/* Print String List
 * l    String list given 
 *
 * Prints all strings held in current list l
 *
 */
static void printStrList(strList l) {

    while(l != EMPTY) {

        printf("%s; ", l->string);
        l = l->next;
    }

    printf("\n");
}
