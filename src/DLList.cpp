#include <stdio.h>
#include <assert.h>

#include "DLList.h"

//-------------------------------------------------

void DLListInit (dllist_t* dllist)
{
    assert (dllist != NULL);

    dllist->dllist_data[0] = LIST_END;
    dllist->next[0]        = LIST_END;
    dllist->prev[0]        = LIST_END;

    for (int i = 1; i < LIST_SIZE + 1; ++i)
    {
        dllist->dllist_data[i] = -1;
        dllist->next[i]        = i + 1;
        dllist->prev[i]        = i - 1;
    }

    dllist->next[LIST_SIZE] = 0;

    dllist->free          = 1;
}

//-------------------------------------------------

dllist_error_t InsertEnd (dllist_t* dllist, list_elem_t insert_elem)
{
    assert (dllist != NULL);

//    return InsertAfter(dllist, dllist->prev[dllist->free], insert_elem);

    // кладём элемент в дату
    dllist->dllist_data[dllist->free] = insert_elem;
    // запоминаем следующий свободный элемент
    int next_free_temporary = dllist->next[dllist->free];
    // корректируем массив next
    dllist->next[dllist->prev[dllist->free]] = dllist->free;
    dllist->next[dllist->free] = LIST_END;
    // корректировать массив prev не требуется
    // изменяем free
    dllist->free = next_free_temporary;
    
    return OK;
}

//-------------------------------------------------

dllist_error_t InsertAfter (dllist_t* dllist, int index_elem, list_elem_t insert_elem)
{
    assert (dllist != NULL);

    printf ("\n\n========== index_elem = %d ==========\n", index_elem);

    // кладём элемент в дату
    printf ("---data---\n");
    printf ("data[free = %d] = %d === insert_elem = %d \n", dllist->free, dllist->dllist_data[dllist->free], insert_elem);
    dllist->dllist_data[dllist->free] = insert_elem;

    // запоминаем следующий свободный элемент
    printf ("---free_next---\n");
    printf ("next_free === next[free = %d] = %d\n", dllist->free, dllist->next[dllist->free]);
    int next_free_temporary = dllist->next[dllist->free];

    // корректируем массив next
    printf ("---next---\n");
    printf ("next[free = %d] = %d === next[index_elem = %d] = %d \n", dllist->free, dllist->next[dllist->free], index_elem, dllist->next[index_elem]);
    dllist->next[dllist->free] = dllist->next[index_elem];

    printf ("next[index_elem = %d] = %d === free = %d \n", index_elem, dllist->next[index_elem], dllist->free);
    dllist->next[index_elem]   = dllist->free;

    // корректируем массив prev
    printf ("---prev---\n");
    printf ("prev[next[free = %d] = %d] = %d === free = %d\n", dllist->free, dllist->next[dllist->free], dllist->prev[dllist->next[dllist->free]], dllist->free);
    // если вставляем в конец, то изменяется prev[0], иначе - изменяется prev[next[free]]]
    dllist->prev[dllist->next[dllist->free]] = dllist->free;
    printf ("prev[free = %d] = %d === index_elem = %d \n", dllist->free, dllist->prev[dllist->free], index_elem);
    dllist->prev[dllist->free] = index_elem;
    printf ("prev[free_next = %d] = %d === prev[free = %d] = %d \n", next_free_temporary, dllist->prev[next_free_temporary], dllist->free, dllist->prev[dllist->free]);
    dllist->prev[next_free_temporary] = dllist->prev[0];

    // изменяем free
    printf ("free = %d === free_next = %d", dllist->free, next_free_temporary);
    dllist->free = next_free_temporary;

    return OK;
}

//-------------------------------------------------

dllist_error_t DeleteElem (dllist_t* dllist, int index_elem)
{
    assert (dllist != NULL);

    // удаляем элемент
    dllist->dllist_data[index_elem] = -1;

    // корректируем массив next
    dllist->next[dllist->prev[index_elem]] = dllist->next[index_elem];
    dllist->next[index_elem] = dllist->free;

    // корректируем массив prev
    dllist->prev[dllist->next[index_elem]] = dllist->prev[index_elem];
    dllist->prev[index_elem] = -1; // FIXME

    // обновляем свободный элемент
    dllist->free = index_elem;

    return OK;
}

//-------------------------------------------------

int DLListNext (dllist_t* dllist, int elem_index)
{
    assert (dllist != NULL);

    return dllist->next[elem_index];
}

//-------------------------------------------------

void DLListDump (FILE* dump_file, dllist_t* dllist)
{
    assert (dump_file != NULL);
    assert (dllist != NULL);

    fprintf (dump_file, "digraph G\n");
    fprintf (dump_file, "{\n");
    fprintf (dump_file, "rankdir=LR;\n");
    fprintf (dump_file, "node[shape=\"record\", style=\"rounded\"];\n");
    fprintf (dump_file, "title[label = \"<f>free = %d\"];\n", dllist->free);

    for (int i = 0; i < LIST_SIZE + 1; ++i)
        fprintf (dump_file, "%d[label = \"%d|data = %d|<n>next = %d|<p>prev = %d\"];\n", i, i, dllist->dllist_data[i], dllist->next[i], dllist->prev[i]);
    for (int i = 0; i < LIST_SIZE + 1; ++i)
        {
        if (dllist->next[i] != 0)
            fprintf (dump_file, "%d:<n>->%d:<n>\n", i, dllist->next[i]);
        }

    fprintf (dump_file, "title:<f>->%d:<n>\n", dllist->free);

    fprintf (dump_file, "}\n");
}

//-------------------------------------------------