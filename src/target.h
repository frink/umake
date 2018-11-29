#if !defined(__TARGET_H__)
#define __TARGET_H__

    typedef
    struct strNodeStruct{
        struct strNodeStruct* next;
        char* string;
    }strNode;

    typedef strNode* strList;

    typedef
    struct tarNodeStruct{
        struct tarNodeStruct* next;
        int nodeNum;
        char* target;
        strNode* dependencies;
        strNode* rules;
    }Node;

    typedef Node* list;
    #define EMPTY NULL

   /* Node Append
    * n    Pointer to the head of the linked list
    * t    Target to be added
    *
    * Creates a new node with the given target
    *
    */
    void nodeAppend(Node** n, char* t);

   /* Returns last node
    * n    head of the linked list
    *
    * Iterates through the linked list and returns the last node that doesn't 
    * have a next.
    *
    * Used for adding rules to the most recent target created
    *
    */
    Node* getLastNode(Node* n);

   /* Node find
    * n    Head of the linked list
    * s    String to be compared
    *
    * Iterates through the linked list and returns the node with the matching
    * target. Used for recursively running dependencies
    *
    *
    */
    Node* nodeFind(Node* n, char* string);

   /* Free Node Data
    * n    Current node
    *
    * Iterates through the entire linked list and frees all of the attributes
    * and the node itself
    */
    void freeNodeData(Node* n);

   /* Print List
    * n    Current node
    *
    * Iterates through the entire linked list and prints all of the data from
    * each node.
    *
    * Used for debugging purposes
    *
    */
    void printList(Node* n);

   /* String List Append
    * l     list to be traversed
    * s     string attribute of new list will be set to s
    *
    * Appends a strList object to the end of the given list with string s
    *
    */
    void strListAppend(strList* l,  char* s);

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
    strNode* strListFind(strList l, char* s);

#endif
