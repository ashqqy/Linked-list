#ifndef DLLIST
#define DLLIST

//-------------------------------------------------

typedef int list_elem_t;

const int LIST_SIZE = 10;
const int LIST_END  = 0;

struct dllist_t
{
    list_elem_t data[LIST_SIZE + 1]; // +1 для нуля - конца последовательности
    int next[LIST_SIZE + 1];
    int prev[LIST_SIZE + 1];
    int free;
    int head;
    int tail;
};

enum dllist_error_t
{
    OK = 0,
    LIST_OVERFLOW = 100
};

//-------------------------------------------------

void DLListInit (dllist_t* dllist);

dllist_error_t InsertAfter (dllist_t* dllist, int index_elem, list_elem_t insert_elem);
dllist_error_t InsertBegin (dllist_t* dllist, list_elem_t insert_elem);
dllist_error_t InsertEnd (dllist_t* dllist, list_elem_t insert_elem);
dllist_error_t DeleteElem (dllist_t* dllist, int index_elem);

int DLListNext (dllist_t* dllist, int elem_index);

void DLListDump (FILE* dump_file, dllist_t* dllist);

//-------------------------------------------------

#endif // DLLIST