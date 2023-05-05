#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

struct Node {
    struct Node *next;
    char *name;
} *null;

// attach node after head. Head ->
int insertNode(struct Node ** head, struct Node *n){
    n->next = (*head);
    (*head) = n;

}

// Check if there are previous, next nodes.
// The cool thing about this function is that it will remove the entered node out of whatever list it is in
// We don't need to know what list it is in. The Node structure itself is just reset to null.

int removeNode(struct Node ** head, struct Node *n){ // if the names are equal

    struct Node *temp;
    struct Node *null = NULL;

    // if the node is in the middle
    while((*head)->next != NULL)
        if (strcmp((*head)->next->name, n->name) == 0){
            printf("%s = %s\n",(*head)->next->name, n->name);
            temp = (*head)->next;
            (*head)->next = temp->next;
        }
        (*head) = (*head)->next;
    
}

int printList(struct Node ** head){

    printf("[");
    while((*head) = NULL){
        printf("%s",(*head)->name);
        if((*head)->next != NULL){
            printf(", ");
        }
        (*head) = (*head)->next;
        
    }
    printf("]\n");
}

int main(){
    struct Node n1, n2, n3, n4, n5;
    struct Node *list = malloc(sizeof(struct Node));
    list->next = NULL;


    n1.name = "Node 1";
    n2.name = "Node 2";
    n3.name = "Node 3";
    n4.name = "Node 4";
    n5.name = "Node 5";

    insertNode(&list, &n1);
    // insertNode(list, &n2);
    // insertNode(list, &n3);
    // insertNode(list, &n4);
    // insertNode(list, &n5);

    // printList(list);
    // removeNode(list, &n3);
    // printList(list);
    
   
    // printList(list);
}