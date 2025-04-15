#include <stdio.h>
#include <assert.h>

#include "LinkedList.h"

//-------------------------------------------------

void ListInit (list_t* list)
{
    assert (list != NULL);

    list->data[0] = LIST_END;
    list->next[0] = LIST_END;
    list->prev[0] = LIST_END;

    for (int i = 1; i < LIST_SIZE + 1; ++i)
    {
        list->data[i] = -1;
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }

    list->next[LIST_SIZE] = 0;

    list->free = 1;
    list->head = 0;
    list->tail = 0;
}

//-------------------------------------------------

void InsertAfter (list_t* list, int index_elem, list_elem_t insert_elem)
{
    assert (list != NULL);

    // кладём элемент в дату
    list->data[list->free] = insert_elem;

    // запоминаем следующий свободный элемент
    int next_free_temporary = list->next[list->free];

    // корректируем массив next
    list->next[list->free] = list->next[index_elem];
    list->next[index_elem]   = list->free;

    // корректируем массив prev
    list->prev[list->next[list->free]] = list->free; // если вставляем в конец, то изменяется prev[0], 
                                                             // иначе - изменяется prev[next[free]]]
    list->prev[list->free]             = index_elem;
    list->prev[next_free_temporary]    = list->prev[0];

    // изменяем free
    list->free = next_free_temporary;

    // изменяем head и tail
    list->head = list->next[0];
    list->tail = list->prev[0];
}

void InsertEnd (list_t* list, list_elem_t insert_elem)
{
    assert (list != NULL);

    InsertAfter (list, list->prev[0], insert_elem);
}

void InsertBegin (list_t* list, list_elem_t insert_elem)
{
    assert (list != NULL);

    InsertAfter (list, 0, insert_elem);
}

//-------------------------------------------------

void DeleteElem (list_t* list, int index_elem)
{
    assert (list != NULL);

    // удаляем элемент
    list->data[index_elem] = -1;

    // запомним следующий и предыдущий за удаляемым элемент
    int next_index_elem = list->next[index_elem];
    int prev_index_elem = list->prev[index_elem];

    // корректируем массив next
    list->next[prev_index_elem] = list->next[index_elem];
    list->next[index_elem] = list->free;

    // корректируем массив prev
    list->prev[next_index_elem] = list->prev[index_elem];
    list->prev[index_elem] = list->prev[0];

    // обновляем свободный элемент
    list->free = index_elem;
    list->prev[list->next[list->free]] = index_elem;

    // изменяем head и tail
    list->head = list->next[0];
    list->tail = list->prev[0];
}

//-------------------------------------------------

int ListNext (list_t* list, int elem_index)
{
    assert (list != NULL);

    return list->next[elem_index];
}

//-------------------------------------------------

int FindRealIndex (list_t* list, int elem_index)
{
    assert (list != NULL);

    int real_index = 0;
    for (int i = 0; i < elem_index; ++i)
        real_index = list->next[real_index];

    return real_index;
}

//-------------------------------------------------

void ListDump (FILE* dump_file, list_t* list)
{
    assert (dump_file != NULL);
    assert (list != NULL);

    #define FREE_COLOR  "\"lightgreen\""
    #define BUSY_COLOR  "\"coral\""
    #define TITLE_COLOR "\"lightblue\""

    fprintf (dump_file, "digraph G\n");
    fprintf (dump_file, "{\n");
    fprintf (dump_file, "splines=ortho;\n");
    fprintf (dump_file, "nodesep=0.5;\n"); // расстояние между ячейками
    fprintf (dump_file, "node[shape=\"record\", style=\"rounded, filled\"];\n\n");

    fprintf (dump_file, "free[label = \"free = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->free);
    fprintf (dump_file, "head[label = \"head = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->head);
    fprintf (dump_file, "tail[label = \"tail = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->tail);
    fprintf (dump_file, "0[label = \"{ <i>0|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor =" TITLE_COLOR "];\n", list->data[0], list->next[0], list->prev[0]);
    for (int i = 1; i < LIST_SIZE + 1; ++i)
        fprintf (dump_file, "%d[label = \"{ <i>%d|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor = " BUSY_COLOR "];\n", i, i, list->data[i], list->next[i], list->prev[i]);
    fprintf (dump_file, "\n");

    // выстраиваем ячейки в строчку
    fprintf (dump_file, "{ rank = same; ");
    for (int i =0 ; i < LIST_SIZE + 1; i++)
        fprintf (dump_file, "%d; ", i);
    fprintf (dump_file, "}\n");

    // соединяем невидимыми линиями
    for (int i = 0; i < LIST_SIZE; ++i)
        fprintf (dump_file, "%d->%d [weight = 5000, style=invis]; \n", i, i + 1);
    fprintf (dump_file, "\n");

    // соединяем стрелками next
    for (int i = 0; i < LIST_SIZE + 1; ++i)
    {
        if (list->next[i] != 0)
            fprintf (dump_file, "%d->%d [weight = 0, color = blueviolet];\n", i, list->next[i]);
    }
    fprintf (dump_file, "\n");

    // соединяем стрелками prev
    for (int i = LIST_SIZE; i >= 0; --i)
    {
        if (list->next[i] != 0)
            fprintf (dump_file, "%d->%d [weight = 0, color = deeppink];\n", list->next[i], i);
    }
    fprintf (dump_file, "\n");

    // красим свободные ячейки в зеленый
    fprintf (dump_file, "free->%d;\n", list->free);
    int free_block = list->free;
    while (free_block != 0)
    {
        fprintf (dump_file, "%d[fillcolor = " FREE_COLOR "];\n", free_block);
        free_block = list->next[free_block];
    }

    // указываем на head и tail
    fprintf (dump_file, "head->%d;\n", list->head);
    fprintf (dump_file, "tail->%d;\n", list->tail);

    fprintf (dump_file, "}\n");

    #undef FREE_COLOR
    #undef BUSY_COLOR
}

//-------------------------------------------------
