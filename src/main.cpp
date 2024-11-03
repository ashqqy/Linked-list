#include <stdio.h>

#include "DLList.h"

//-------------------------------------------------

int main ()
{
    FILE* dump_file = fopen ("dump.dot", "w");

    dllist_t dllist = {};
    DLListInit (&dllist);

    // InsertEnd   (&dllist, 10);
    // InsertEnd   (&dllist, 20);
    // InsertEnd   (&dllist, 30);
    // InsertEnd   (&dllist, 40);
    // InsertEnd   (&dllist, 50);
    // InsertEnd   (&dllist, 60);
    // InsertEnd   (&dllist, 70);
    // InsertEnd   (&dllist, 80);
    InsertAfter (&dllist, 0, 10);
    InsertAfter (&dllist, 1, 20);
    InsertAfter (&dllist, 2, 30);
    InsertAfter (&dllist, 3, 40);
    InsertAfter (&dllist, 4, 50);
    InsertAfter (&dllist, 5, 60);
    InsertAfter (&dllist, 2, 25);
    InsertAfter (&dllist, 6, 70);
    // InsertAfter (&dllist, 8, 90);
    // InsertAfter (&dllist, 9, 100);

    DLListDump (dump_file, &dllist);

    fclose(dump_file);
}

//-------------------------------------------------

// TODO функция поиска по логическому индексу физического
// TODO красивый дамп
// TODO InsertEnd на основе 
// TODO prev нулевого указывает на последний
// TODO next последнего на ноль
// TODO InsertBegin на основе 