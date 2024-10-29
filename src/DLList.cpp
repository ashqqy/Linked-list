#include <stdio.h>
#include <assert.h>

#include "DLList.h"

//-------------------------------------------------

void DLListInit (dllist_t* dllist)
{
    assert (dllist != NULL);

    for (int i = 0; i < LIST_SIZE; ++i)
        dllist->dllist_data[i] = -1;
}

//-------------------------------------------------

dllist_error_t InsertAfter (dllist_t* dllist, list_elem_t insert_elem)
{
    assert (dllist != NULL);

    if (dllist->head < LIST_SIZE - 1)   
    {
        dllist->dllist_data[dllist->head] = insert_elem;
    }
    else if (dllist->tail != 0)
    {
        dllist->head = 0;
        dllist->dllist_data[dllist->head] = insert_elem;
    }
    else
    {
        return LIST_OVERFLOW;
    }
    
    return OK;
}

//-------------------------------------------------

int DLListNext (dllist_t* dllist, int elem_index)
{
    assert (dllist != NULL);

    return (elem_index + 1) % LIST_SIZE;
}

//-------------------------------------------------

void DLListDump (dllist_t* dllist)
{
    assert (dllist != NULL);

    printf ("head = [%d]", dllist->head);
    printf ("tail = [%d]", dllist->tail);

    for (int i = 0; i < LIST_SIZE; ++i)
        printf ("data[%d] = [%d];\n", i, dllist->dllist_data[i]);
}

//-------------------------------------------------
