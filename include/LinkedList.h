#ifndef LINKED_LIST_H
#define LINKED_LIST_H

//-------------------------------------------------

typedef int list_elem_t;

const int LIST_SIZE = 10;
const int LIST_END  = 0;

typedef struct
{
    list_elem_t data[LIST_SIZE + 1]; // +1 для нуля - конца последовательности
    int next[LIST_SIZE + 1];
    int prev[LIST_SIZE + 1];
    int free;
    int head;
    int tail;
} list_t;

//-------------------------------------------------

void ListInit (list_t* list);

void InsertAfter (list_t* list, int index_elem, list_elem_t insert_elem);
void InsertBegin (list_t* list, list_elem_t insert_elem);
void InsertEnd (list_t* list, list_elem_t insert_elem);
void DeleteElem (list_t* list, int index_elem);

int ListNext (list_t* list, int elem_index);
int FindRealIndex (list_t* list, int elem_index);

void ListDump (FILE* dump_file, list_t* list);

//-------------------------------------------------

#endif // LINKED_LIST_H
