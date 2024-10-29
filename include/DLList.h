#ifndef DLLIST
#define DLLIST

//-------------------------------------------------

typedef int list_elem_t;

const int LIST_SIZE = 10;

struct dllist_t
{
    list_elem_t dllist_data[LIST_SIZE];
    int head;
    int tail;
};

enum dllist_error_t
{
    OK,
    LIST_OVERFLOW
};

//-------------------------------------------------

void DLListInit (dllist_t* dllist);
int DLListNext (dllist_t* dllist, int elem_index);
dllist_error_t InsertAfter (dllist_t* dllist, list_elem_t insert_elem);
void DLListDump (dllist_t* dllist);

//-------------------------------------------------

#endif // DLLIST