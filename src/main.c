#include <stdlib.h>
#include <stdio.h>

#include "LinkedList.h"

static const char* DUMP_COMMAND = "dot ./dump/dump.dot -Tpng -o ./dump/dump.png"; // linux

//-------------------------------------------------

int main ()
{
    FILE* dump_file = fopen ("./dump/dump.dot", "w");

    list_t list = {};
    ListInit (&list, 10, sizeof (int));

    int insert_elem = 10;
    ListInsertEnd (&list, &insert_elem);
    insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);

    ListDeleteElem (&list, 0);
    insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);
    insert_elem += 1;
    ListInsertAfter (&list, 1, &insert_elem);
    insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListInsertEnd (&list, &insert_elem);insert_elem += 1;
    ListDeleteElem (&list, 6);


    // int next_3 = listNext (&list, 3);
    // printf ("next_3 = %d\n", next_3);

    // int real_ind_2 = ListFindRealIndex (&list, 2);
    // printf ("real_ind_2 = %d\n", real_ind_2);

    ListDump (dump_file, &list);

    fclose(dump_file);

    system (DUMP_COMMAND);

    ListDestroy (&list);
}

//-------------------------------------------------
