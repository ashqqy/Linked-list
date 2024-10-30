#include <stdio.h>

#include "DLList.h"

//-------------------------------------------------

int main ()
{
    dllist_t dllist = {};
    DLListInit (&dllist);
    dllist.tail = 4;
    dllist.head = 4;
    
    dllist_error_t err;
    for (int i = 0; i < LIST_SIZE; ++i)
    {
        err = InsertAfter (&dllist, i);
    }
    int next = DLListNext (&dllist, 10);
    printf ("next = %d\n", next);
    printf ("err = %d\n", err);
    DLListDump (&dllist);
}

//-------------------------------------------------