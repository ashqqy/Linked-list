#include <stdio.h>
#include <assert.h>

#include "DLList.h"

//-------------------------------------------------

void DLListInit (dllist_t* dllist)
{
    assert (dllist != NULL);

    dllist->data[0] = LIST_END;
    dllist->next[0] = LIST_END;
    dllist->prev[0] = LIST_END;

    for (int i = 1; i < LIST_SIZE + 1; ++i)
    {
        dllist->data[i] = -1;
        dllist->next[i] = i + 1;
        dllist->prev[i] = i - 1;
    }

    dllist->next[LIST_SIZE] = 0;

    dllist->free = 1;
}

//-------------------------------------------------

dllist_error_t InsertAfter (dllist_t* dllist, int index_elem, list_elem_t insert_elem)
{
    assert (dllist != NULL);

    // кладём элемент в дату
    dllist->data[dllist->free] = insert_elem;

    // запоминаем следующий свободный элемент
    int next_free_temporary = dllist->next[dllist->free];

    // корректируем массив next
    dllist->next[dllist->free] = dllist->next[index_elem];
    dllist->next[index_elem]   = dllist->free;

    // корректируем массив prev
    dllist->prev[dllist->next[dllist->free]] = dllist->free; // если вставляем в конец, то изменяется prev[0], 
                                                             // иначе - изменяется prev[next[free]]]
    dllist->prev[dllist->free] = index_elem;
    dllist->prev[next_free_temporary] = dllist->prev[0];

    // изменяем free
    dllist->free = next_free_temporary;

    return OK;
}

dllist_error_t InsertEnd (dllist_t* dllist, list_elem_t insert_elem)
{
    assert (dllist != NULL);

    return InsertAfter(dllist, dllist->prev[0], insert_elem);
}

dllist_error_t InsertBegin (dllist_t* dllist, list_elem_t insert_elem)
{
    assert (dllist != NULL);

    return InsertAfter(dllist, 0, insert_elem);
}

//-------------------------------------------------


dllist_error_t DeleteElem (dllist_t* dllist, int index_elem)
{
    assert (dllist != NULL);

    // удаляем элемент
    dllist->data[index_elem] = -1;

    // запомним следующий и предыдущий за удаляемым элемент
    int next_index_elem = dllist->next[index_elem];
    int prev_index_elem = dllist->prev[index_elem];

    // корректируем массив next
    dllist->next[prev_index_elem] = dllist->next[index_elem];
    dllist->next[index_elem] = dllist->free;

    // корректируем массив prev
    dllist->prev[next_index_elem] = dllist->prev[index_elem];
    dllist->prev[index_elem] = dllist->prev[0];

    // обновляем свободный элемент
    dllist->free = index_elem;
    dllist->prev[dllist->next[dllist->free]] = index_elem;

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
        fprintf (dump_file, "%d[label = \"%d|data = %d|<n>next = %d|<p>prev = %d\"];\n", i, i, dllist->data[i], dllist->next[i], dllist->prev[i]);
    for (int i = 0; i < LIST_SIZE + 1; ++i)
        {
        if (dllist->next[i] != 0)
            fprintf (dump_file, "%d:<n>->%d:<n>\n", i, dllist->next[i]);
        }

    fprintf (dump_file, "title:<f>->%d:<n>\n", dllist->free);

    fprintf (dump_file, "}\n");
}

//-------------------------------------------------