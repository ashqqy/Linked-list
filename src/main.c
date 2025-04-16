#include <stdlib.h>
#include <stdio.h>

#include "LinkedList.h"

static const char* DUMP_COMMAND = "dot ./dump/dump.dot -Tpng -o ./dump/dump.png"; // linux

//-------------------------------------------------

// Example from Readme
int main ()
{
    system ("mkdir dump");
    FILE* dump_file = fopen ("./dump/dump.dot", "w");

    list_t list = {};
    ListInit (&list, 10, sizeof (int));

    int insert_elem = 10;
    ListInsertEnd (&list, &insert_elem);
    insert_elem -= 2;
    ListInsertBegin (&list, &insert_elem);
    insert_elem += 1;
    ListInsertAfter (&list, ListLogicalToPhysicalIndex (&list, 0), &insert_elem);

    ListDump (dump_file, &list);
    fclose (dump_file);
    system (DUMP_COMMAND);

    ListDestroy (&list);
}
