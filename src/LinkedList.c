#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

//-------------------------------------------------

#define CUSTOM_ASSERT(expression) do                                                                             \
{                                                                                                                \
    if (!(expression))                                                                                           \
    {                                                                                                            \
        fprintf (stderr, "%s: %s:%d: Assertion `%s' failed.\n", __FUNCTION__, __FILE__, __LINE__, #expression);  \
        fprintf (stderr, "Program aborting\n");                                                                  \
        exit(EXIT_FAILURE);                                                                                      \
    }                                                                                                            \
}                                                                                                                \
while (0)

#define GET_BY_INDEX(array, index, elem_size) ((char*) (array) + (index) * (elem_size))

//-------------------------------------------------

void ListInit (list_t* list, int capacity, int elem_size)
{
    assert (list != NULL);

    list->data = calloc ((size_t) capacity, (size_t) elem_size);
    CUSTOM_ASSERT (list->data != NULL);
    list->next = (int*) calloc ((size_t) capacity, sizeof (int));
    CUSTOM_ASSERT (list->next != NULL);
    list->prev = (int*) calloc ((size_t) capacity, sizeof (int));
    CUSTOM_ASSERT (list->prev != NULL);

    list->next[0] = 1;
    list->prev[0] = capacity - 1;

    for (int i = 1; i < capacity; ++i)
    {
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }

    list->next[capacity - 1] = 0;

    list->elem_size = elem_size;
    list->size = 0;
    list->capacity = capacity;
    list->free = 0;
    list->head = 0;
    list->tail = 0;
}

void ListDestroy (list_t* list)
{
    assert (list != NULL);

    free (list->data); list->data = NULL;
    free (list->next); list->next = NULL;
    free (list->prev); list->prev = NULL;

    memset (list, 0, sizeof (list_t));
}

//-------------------------------------------------

void ListInsertAfter (list_t* list, int target_index, void* insert_data)
{
    assert (list != NULL);

    if (list->size == list->capacity - 1)
    {
        ListResize (list, list->capacity * 2);
    }

    memcpy (GET_BY_INDEX (list->data, list->free, list->elem_size), insert_data, (size_t) list->elem_size);

    int insert_data_index = list->free;
    int next_free = list->next[list->free];

    if (target_index == list->tail)
        list->tail = insert_data_index;

    // Restore ring buffer in the list of free elements
    list->prev[next_free] = list->prev[list->free];
    list->next[list->prev[list->free]] = next_free;

    // Configure the connection in a new element
    list->next[insert_data_index] = list->next[target_index];
    list->next[target_index]      = insert_data_index;

    list->prev[list->next[insert_data_index]] = insert_data_index;
    list->prev[insert_data_index]             = target_index;

    list->free = next_free;
    list->size += 1;
}

void ListInsertEnd (list_t* list, void* insert_data)
{
    assert (list != NULL);

    ListInsertAfter (list, list->tail, insert_data);
}

// TODO
void ListInsertBegin (list_t* list, void* insert_data)
{
    assert (list != NULL);

    ListInsertAfter (list, -1, insert_data);
}

//-------------------------------------------------

void ListResize (list_t* list, int new_capacity)
{
    assert (list != NULL);

    int old_capacity = list->capacity;
    list->capacity = new_capacity;

    list->data = (int*) CustomRecalloc (list->data, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);
    list->next = (int*) CustomRecalloc (list->next, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);
    list->prev = (int*) CustomRecalloc (list->prev, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);

    for (int i = list->free; i < new_capacity; ++i)
    {
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }

    list->next[list->free] = old_capacity;
    list->prev[list->free] = new_capacity - 1;

    for (int i = list->next[list->free] + 1; i < new_capacity; ++i)
    {
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }

    list->prev[list->next[list->free]] = list->free;
    list->next[new_capacity - 1] = list->free;
}

void* CustomRecalloc (void* memory, size_t new_capacity, size_t elem_size, size_t previous_capacity, const void* poison)
{   
    if (memory == NULL)
        return NULL;
        
    // If reduce memory, then reset to zero deleted elems
    if (previous_capacity > new_capacity)
        memset ((char*) memory + new_capacity * elem_size, 0, elem_size * (previous_capacity - new_capacity));

    void* save_memory = memory;
    memory = realloc (memory, new_capacity * elem_size);
    if (memory == NULL)
    {
        free (save_memory); save_memory = NULL;
        return NULL;
    }

    // If increase memory, then new elems fill with poison value
    if (previous_capacity < new_capacity)
    {
        if (poison != NULL)
        {
            for (size_t i = 0; i < new_capacity - previous_capacity; i++)
                memcpy(((char*) memory + (previous_capacity + i) * elem_size), poison, elem_size);
        }
        else
        {
            memset ((char*) memory + previous_capacity * elem_size, 0, (new_capacity - previous_capacity) * elem_size);
        }
    }

    return memory;
}

//-------------------------------------------------

void ListDeleteElem (list_t* list, int target_index)
{
    assert (list != NULL);

    memset (GET_BY_INDEX (list->data, target_index, list->elem_size), 0, (size_t) list->elem_size);

    if (target_index == list->head)
        list->head = list->next[list->head];
    if (target_index == list->tail)
        list->tail = list->prev[list->tail];

    int next_index = list->next[target_index];
    int prev_index = list->prev[target_index];

    // Update links in main list
    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;

    // Add element to the list of free elements
    int old_free = list->free;
    list->free = target_index;
    list->next[target_index] = old_free;
    list->prev[target_index] = list->prev[old_free];

    // Update links in list of free elements
    list->next[list->prev[old_free]] = target_index;
    list->prev[old_free] = target_index;

    list->size -= 1;
}

//-------------------------------------------------

int ListNext (list_t* list, int target_index)
{
    assert (list != NULL);

    return list->next[target_index];
}

//-------------------------------------------------

int ListFindRealIndex (list_t* list, int target_index)
{
    assert (list != NULL);

    int real_index = 0;
    for (int i = 0; i < target_index; ++i)
        real_index = list->next[real_index];

    return real_index;
}

//-------------------------------------------------

void ListDump (FILE* dump_file, const list_t* list)
{
    assert (dump_file != NULL);
    assert (list != NULL);

    #define FREE_COLOR  "\"lightgreen\""
    #define BUSY_COLOR  "\"coral\""
    #define TITLE_COLOR "\"lightblue\""

    fprintf (dump_file, "digraph G\n");
    fprintf (dump_file, "{\n");
    fprintf (dump_file, "splines=ortho;\n");
    fprintf (dump_file, "nodesep=0.5;\n");
    fprintf (dump_file, "node[shape=\"record\", style=\"rounded, filled\"];\n\n");

    fprintf (dump_file, "free[label = \"free = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->free);
    fprintf (dump_file, "head[label = \"head = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->head);
    fprintf (dump_file, "tail[label = \"tail = %d\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "]\n", list->tail);

    fprintf (dump_file, "title[label = \"{ capacity = %d | size = %d | elem_size = %d }\", style=\"rounded,filled\", fillcolor = " TITLE_COLOR "];\n", \
        list->capacity, list->size, list->elem_size);

    for (int i = 0; i < list->capacity; ++i)
        fprintf (dump_file, "%d[label = \"{ <i>%d|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor = " BUSY_COLOR "];\n", \
            i, i, *(int*) GET_BY_INDEX (list->data, i, list->elem_size), list->next[i], list->prev[i]);

    fprintf (dump_file, "\n");

    // выстраиваем ячейки в строчку
    fprintf (dump_file, "{ rank = same; ");
    for (int i = 0 ; i < list->capacity; ++i)
        fprintf (dump_file, "%d; ", i);

    fprintf (dump_file, "}\n");

    // соединяем невидимыми линиями
    for (int i = 0; i < list->capacity - 1; ++i)
        fprintf (dump_file, "%d->%d [weight = 5000, style=invis]; \n", i, i + 1);

    fprintf (dump_file, "\n");

    // соединяем стрелками next
    for (int i = 0; i < list->capacity; ++i)
        if ((list->next[i] != list->head) && (list->next[i] != list->free)) // do not draw non-informative arrows
            fprintf (dump_file, "%d->%d [weight = 0, color = blueviolet];\n", i, list->next[i]);

    fprintf (dump_file, "\n");

    // соединяем стрелками prev
    for (int i = list->capacity - 1; i >= 0; --i)
        if ((list->prev[i] != list->tail) && (i != list->free)) // do not draw non-informative arrows
            fprintf (dump_file, "%d->%d [weight = 0, color = deeppink];\n", i, list->prev[i]);

    fprintf (dump_file, "\n");

    // красим свободные ячейки в зеленый
    fprintf (dump_file, "free->%d;\n", list->free);
    int free_block = list->free;
    for (int i = list->size; i < list->capacity; ++i)
    {
        fprintf(dump_file, "%d[fillcolor = " FREE_COLOR "];\n", free_block);
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
