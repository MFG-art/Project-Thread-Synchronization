#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include "structures.h"

//  attach node after head
int insertNode(struct Node *head, struct Node *n){
        n->next = head->next;
        if (head->next != NULL){
            head->next->prev = n;
        }
        head->next = n;
        n->prev = head;
    
}

// Check if there are previous, next nodes.
// The cool thing about this function is that it will remove the entered node out of whatever list it is in
// We don't need to know what list it is in. The Node structure itself is just reset to null.
int removeNode(struct Node *n){

    if (n->prev != NULL){
        n->prev->next = n->next;
    } 
    if (n->next != NULL){
        n->next->prev = n->prev;
    }
    // clear node pointers. Note, we don't ever touch the data. Vehicles don't change
    n->next = NULL;
    n->prev = NULL;
}

int printList(struct Node *temp){
    if (temp->next != 0){
        temp = temp->next;
        printf("[");
        while(temp != NULL){
            printf("%s",temp->name);
            if (temp->next != NULL){
                printf(", ");
            }
            temp = temp->next;
        }
        printf("]");
    } else { // if the list is empty
        printf("[ ]");
    }
}

