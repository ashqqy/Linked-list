#include <stdlib.h>
#include <stdio.h>

#include "LinkedList.h"

static const char* DUMP_COMMAND = "dot ./dump/dump.dot -Tpng -o ./dump/dump.png"; // linux

//-------------------------------------------------

int main ()
{
    FILE* dump_file = fopen ("./dump/dump.dot", "w");

    list_t list = {};
    ListInit (&list);

    InsertEnd (&list, 10);
    InsertEnd (&list, 20);
    InsertEnd (&list, 30);
    InsertEnd (&list, 40);
    InsertEnd (&list, 50);
    InsertEnd (&list, 60);
    InsertAfter (&list, 2, 25);
    // DeleteElem (&list, 2);

    // int next_3 = listNext (&list, 3);
    // printf ("next_3 = %d\n", next_3);

    // int real_ind_2 = FindRealIndex (&list, 2);
    // printf ("real_ind_2 = %d\n", real_ind_2);

    ListDump (dump_file, &list);

    fclose(dump_file);

    system (DUMP_COMMAND);
}

//-------------------------------------------------
