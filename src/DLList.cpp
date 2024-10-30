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

    if (dllist->head < LIST_SIZE)   
    {
        dllist->dllist_data[dllist->head] = insert_elem;
        dllist->head += 1;
    }
    else if (dllist->tail != 0 && (dllist->head % LIST_SIZE < dllist->tail))
    {  
        dllist->dllist_data[dllist->head % LIST_SIZE] = insert_elem;
        dllist->head += 1;
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

    printf ("head = [%d]\n", dllist->head);
    printf ("tail = [%d]\n", dllist->tail);

    for (int i = 0; i < LIST_SIZE; ++i)
        printf ("data[%d] = [%d];\n", i, dllist->dllist_data[i]);
}

//-------------------------------------------------
