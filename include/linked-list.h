#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>

//-------------------------------------------------

typedef struct
{
    void* data;
    int* next;
    #ifdef DOUBLY_LINKED_LIST
    int* prev;
    #endif
    int elem_size;
    int size;
    int capacity;
    int free;
    int head;
    int tail;
} list_t;

//-------------------------------------------------

void ListInit (list_t* list, int capacity, int elem_size);
void ListDestroy (list_t* list);

void ListInsertAfter (list_t* list, int target_index, void* insert_data);
void ListInsertBegin (list_t* list, void* insert_data);
void ListInsertEnd (list_t* list, void* insert_data);
void ListDeleteElem (list_t* list, int target_index);

void ListResize (list_t* list, int new_capacity);

int ListFindNext (list_t* list, int target_index);
int ListFindPrev (list_t* list, int target_index);
int ListFindPrevFree (list_t* list, int target_index);

int ListLogicalToPhysicalIndex (list_t* list, int target_index);

void ListDump (FILE* dump_file, const list_t* list);

//-------------------------------------------------

#endif // LINKED_LIST_H
