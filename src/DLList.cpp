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
    dllist->head = 0;
    dllist->tail = 0;
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
    dllist->prev[dllist->free]               = index_elem;
    dllist->prev[next_free_temporary]        = dllist->prev[0];

    // изменяем free
    dllist->free = next_free_temporary;

    // изменяем head и tail
    dllist->head = dllist->next[0];
    dllist->tail = dllist->prev[0];

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

    // изменяем head и tail
    dllist->head = dllist->next[0];
    dllist->tail = dllist->prev[0];

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

    #define FREE_COLOR  "\"lightgreen\""
    #define BUSY_COLOR  "\"coral\""
    #define TITLE_COLOR "\"lightblue\""

    fprintf (dump_file, "digraph G\n");
    fprintf (dump_file, "{\n");
    fprintf (dump_file, "splines=ortho;\n");
    fprintf (dump_file, "node[shape=\"Mrecord\", style=\"rounded, filled\"];\n\n");

    fprintf (dump_file, "free[label = \"free = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", dllist->free);
    fprintf (dump_file, "head[label = \"head = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", dllist->head);
    fprintf (dump_file, "tail[label = \"tail = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", dllist->tail);
    fprintf (dump_file, "0[label = \"{ <i>0|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor =" TITLE_COLOR "];\n", dllist->data[0], dllist->next[0], dllist->prev[0]);
    for (int i = 1; i < LIST_SIZE + 1; ++i)
        fprintf (dump_file, "%d[label = \"{ <i>%d|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor = " BUSY_COLOR "];\n", i, i, dllist->data[i], dllist->next[i], dllist->prev[i]);
    fprintf (dump_file, "\n");

    // выстраиваем ячейки в строчку
    fprintf (dump_file, "{ rank = same; ");
    for (int i =0 ; i < LIST_SIZE + 1; i++)
        fprintf (dump_file, "%d; ", i);
    fprintf (dump_file, "}\n");

    // соединяем невидимыми линиями
    for (int i = 0; i < LIST_SIZE; ++i)
        fprintf (dump_file, "%d->%d [weight = 5000, style=invis]\n", i, i + 1);
    fprintf (dump_file, "\n");

    // соединяем стрелками next
    for (int i = 0; i < LIST_SIZE + 1; ++i)
    {
        if (dllist->next[i] != 0)
            fprintf (dump_file, "%d->%d \n", i, dllist->next[i]);
    }
    fprintf (dump_file, "\n");

    // красим свободные ячейки в зеленый
    fprintf (dump_file, "free->%d;\n", dllist->free);
    int free_block = dllist->free;
    while (free_block != 0)
    {
        fprintf (dump_file, "%d[fillcolor = " FREE_COLOR "];\n", free_block);
        free_block = dllist->next[free_block];
    }

    // указываем на head и tail
    fprintf (dump_file, "head->%d;\n", dllist->head);
    fprintf (dump_file, "tail->%d;\n", dllist->tail);

    fprintf (dump_file, "}\n");

    #undef FREE_COLOR
    #undef BUSY_COLOR
}

//-------------------------------------------------