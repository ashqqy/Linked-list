#include <stdio.h>

#include "DLList.h"

//-------------------------------------------------

int main ()
{
    dllist_t dllist = {};
    InsertAfter (&dllist, 1);
    DLListDump (&dllist);
}

//-------------------------------------------------