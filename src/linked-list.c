#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked-list.h"

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

static const int INSERT_BEFORE_HEAD = -1;

static void* ListRecalloc (void* memory, size_t new_capacity, size_t elem_size, size_t previous_capacity, const void* poison)
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

void ListInit (list_t* list, int capacity, int elem_size)
{
    assert (list != NULL);
    assert (capacity > 0);
    assert (elem_size > 0);

    list->data = calloc ((size_t) capacity, (size_t) elem_size);
    CUSTOM_ASSERT (list->data != NULL);

    list->next = (int*) calloc ((size_t) capacity, sizeof (int));
    CUSTOM_ASSERT (list->next != NULL);

    #ifdef DOUBLY_LINKED_LIST
    list->prev = (int*) calloc ((size_t) capacity, sizeof (int));
    CUSTOM_ASSERT (list->prev != NULL);
    #endif

    list->next[0] = 1;

    #ifdef DOUBLY_LINKED_LIST
    list->prev[0] = capacity - 1;
    #endif

    for (int i = 1; i < capacity; ++i)
    {
        list->next[i] = i + 1;

        #ifdef DOUBLY_LINKED_LIST
        list->prev[i] = i - 1;
        #endif
    }

    list->next[capacity - 1] = 0;

    list->elem_size = elem_size;
    list->size = 0;
    list->capacity = capacity;
    list->free = 0;
    list->head = -1;
    list->tail = -1;
}

void ListDestroy (list_t* list)
{
    assert (list != NULL);

    free (list->data); list->data = NULL;
    free (list->next); list->next = NULL;

    #ifdef DOUBLY_LINKED_LIST
    free (list->prev); list->prev = NULL;
    #endif

    memset (list, 0, sizeof (list_t));
}

//-------------------------------------------------

void ListInsertAfter (list_t* list, int target_index, void* insert_data)
{
    assert (list != NULL);
    assert (insert_data != NULL);
    assert (((target_index >= 0) && (target_index < list->capacity)) || (target_index == INSERT_BEFORE_HEAD));

    if (list->size == list->capacity - 1)
    {
        ListResize (list, list->capacity * 2);
    }

    int insert_data_index = list->free;
    int next_free = list->next[list->free];
    #ifdef DOUBLY_LINKED_LIST
    #else
    int last_free = ListFindPrevFree (list, list->free);
    #endif

    memcpy (GET_BY_INDEX (list->data, insert_data_index, list->elem_size), insert_data, (size_t) list->elem_size);

    if ((target_index == list->tail) || (list->tail == -1))
    {
        list->tail = insert_data_index;
    }

    if (list->head == -1)
    {
        list->head = insert_data_index;
    }

    // for ListInsertBegin
    //  Insert after tail and change head (works because the list is circular)
    if (target_index == INSERT_BEFORE_HEAD)
    {
        target_index = list->tail;
        list->head = insert_data_index;
    }

    // Restore ring buffer in the list of free elements (only double-linked list)
    #ifdef DOUBLY_LINKED_LIST
    list->prev[next_free] = list->prev[list->free];
    list->next[list->prev[list->free]] = next_free;
    #else 
    list->next[last_free] = next_free;
    #endif

    // Configure the connection in a new element
    list->next[insert_data_index] = list->next[target_index];
    list->next[target_index]      = insert_data_index;

    #ifdef DOUBLY_LINKED_LIST
    list->prev[list->next[insert_data_index]] = insert_data_index;
    list->prev[insert_data_index]             = target_index;
    #endif

    list->free = next_free;
    list->size += 1;
}

void ListInsertEnd (list_t* list, void* insert_data)
{
    assert (list != NULL);
    assert (insert_data != NULL);

    ListInsertAfter (list, list->tail, insert_data);
}

void ListInsertBegin (list_t* list, void* insert_data)
{
    assert (list != NULL);
    assert (insert_data != NULL);

    ListInsertAfter (list, INSERT_BEFORE_HEAD, insert_data);
}

//-------------------------------------------------

void ListResize (list_t* list, int new_capacity)
{
    assert (list != NULL);
    assert (new_capacity >= 0);

    int old_capacity = list->capacity;
    list->capacity = new_capacity;

    list->data = (int*) ListRecalloc (list->data, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);
    CUSTOM_ASSERT (list->data != NULL);

    list->next = (int*) ListRecalloc (list->next, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);
    CUSTOM_ASSERT (list->next != NULL);

    #ifdef DOUBLY_LINKED_LIST
    list->prev = (int*) ListRecalloc (list->prev, (size_t) new_capacity, (size_t) list->elem_size, (size_t) old_capacity, NULL);
    CUSTOM_ASSERT (list->prev != NULL);
    #endif

    for (int i = list->free; i < new_capacity; ++i)
    {
        list->next[i] = i + 1;
        #ifdef DOUBLY_LINKED_LIST
        list->prev[i] = i - 1;
        #endif
    }

    list->next[list->free] = old_capacity;
    #ifdef DOUBLY_LINKED_LIST
    list->prev[list->free] = new_capacity - 1;
    #endif

    for (int i = list->next[list->free] + 1; i < new_capacity; ++i)
    {
        list->next[i] = i + 1;
        #ifdef DOUBLY_LINKED_LIST
        list->prev[i] = i - 1;
        #endif
    }

    #ifdef DOUBLY_LINKED_LIST
    list->prev[list->next[list->free]] = list->free;
    #endif
    list->next[new_capacity - 1] = list->free;
}

//-------------------------------------------------

void ListDeleteElem (list_t* list, int target_index)
{
    assert (list != NULL);
    assert ((target_index >= 0) && (target_index < list->capacity));

    memset (GET_BY_INDEX (list->data, target_index, list->elem_size), 0, (size_t) list->elem_size);

    if (target_index == list->head)
    {
        list->head = list->next[list->head];
    }

    if (target_index == list->tail)
    {
        #ifdef DOUBLY_LINKED_LIST
        list->tail = list->prev[list->tail];
        #else
        list->tail = ListFindPrev (list, list->tail);
        #endif
    }

    int next_index = list->next[target_index];
    #ifdef DOUBLY_LINKED_LIST
    int prev_index = list->prev[target_index];
    #else
    int prev_index = ListFindPrev (list, target_index);
    CUSTOM_ASSERT ((prev_index != -1) && "Invalid target index for delete");
    #endif

    #ifdef DOUBLY_LINKED_LIST
    int last_free = list->prev[list->free];
    #else
    int last_free = ListFindPrevFree (list, list->free);
    #endif

    int old_free = list->free;
    list->free = target_index;

    // Update links in main list
    list->next[prev_index] = next_index;
    #ifdef DOUBLY_LINKED_LIST
    list->prev[next_index] = prev_index;
    #endif

    // Add element to the list of free elements
    list->next[target_index] = old_free;
    #ifdef DOUBLY_LINKED_LIST
    list->prev[target_index] = last_free;
    #endif

    // Update links in list of free elements
    list->next[last_free] = target_index;
    #ifdef DOUBLY_LINKED_LIST
    list->prev[old_free] = target_index;
    #endif

    list->size -= 1;

    if (list->size == 0)
    {
        list->head = -1;
        list->tail = -1;
    }
}

//-------------------------------------------------

int ListFindNext (list_t* list, int target_index)
{
    assert (list != NULL);
    assert ((target_index >= 0) && (target_index < list->capacity));

    return list->next[target_index];
}

int ListFindPrev (list_t* list, int target_index)
{
    assert (list != NULL);
    assert ((target_index >= 0) && (target_index < list->capacity));

    #ifdef DOUBLY_LINKED_LIST
    return list->prev[target_index];
    #else // Singly-linked list
    if (target_index == list->head) 
    {
        return list->tail;
    }
    
    int current = list->head;
    do 
    {
        if (list->next[current] == target_index) 
        {
            return current;
        }

    current = list->next[current];
    } while (current != list->head);

    return -1;
    #endif // DOUBLY_LINKED_LIST
}

int ListFindPrevFree (list_t* list, int target_index)
{
    assert (list != NULL);
    assert ((target_index >= 0) && (target_index < list->capacity));

    #ifdef DOUBLY_LINKED_LIST
    return list->prev[target_index];
    #else // Singly-linked list
    int current = list->free;
    do
    {
        if (list->next[current] == target_index) 
        {
            return current;
        }
        current = list->next[current];
    } while (current != list->free);

    return -1;
    #endif // DOUBLY_LINKED_LIST
}


//-------------------------------------------------

int ListLogicalToPhysicalIndex (list_t* list, int target_index) 
{
    assert(list != NULL);
    assert ((target_index >= 0) && (target_index < list->size));
    
    int real_index = list->head;
    int current_pos = 0;
    
    while (current_pos < target_index)
    {
        real_index = list->next[real_index];
        current_pos++;
    }

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

    // Create nodes for all elements
    for (int i = 0; i < list->capacity; ++i)
    {
        #ifdef DOUBLY_LINKED_LIST
        fprintf (dump_file, "%d[label = \"{ <i>%d|<d>data = %d|<n>next = %d|<p>prev = %d }\", fillcolor = " BUSY_COLOR "];\n", \
            i, i, *(int*) GET_BY_INDEX (list->data, i, list->elem_size), list->next[i], list->prev[i]);
        #else
        fprintf (dump_file, "%d[label = \"{ <i>%d|<d>data = %d|<n>next = %d }\", fillcolor = " BUSY_COLOR "];\n", \
            i, i, *(int*) GET_BY_INDEX (list->data, i, list->elem_size), list->next[i]);
        #endif
    }

    fprintf (dump_file, "\n");

    // Align all nodes horizontally
    fprintf (dump_file, "{ rank = same; ");
    for (int i = 0 ; i < list->capacity; ++i)
        fprintf (dump_file, "%d; ", i);

    fprintf (dump_file, "}\n");

    // Connect nodes with invisible edges for proper alignment
    for (int i = 0; i < list->capacity - 1; ++i)
        fprintf (dump_file, "%d->%d [weight = 5000, style=invis]; \n", i, i + 1);

    fprintf (dump_file, "\n");

    // Draw next pointers (purple arrows)
    for (int i = 0; i < list->capacity; ++i)
        if ((list->next[i] != list->head) && (list->next[i] != list->free)) // do not draw non-informative arrows
            fprintf (dump_file, "%d->%d [weight = 0, color = blueviolet];\n", i, list->next[i]);

    fprintf (dump_file, "\n");

    #ifdef DOUBLY_LINKED_LIST
    // Draw prev pointers (pink arrows)
    for (int i = list->capacity - 1; i >= 0; --i)
        if ((list->prev[i] != list->tail) && (i != list->free)) // do not draw non-informative arrows
            fprintf (dump_file, "%d->%d [weight = 0, color = deeppink];\n", i, list->prev[i]);
    #endif

    fprintf (dump_file, "\n");

    // Color free nodes green
    fprintf (dump_file, "free->%d;\n", list->free);
    int free_block = list->free;
    for (int i = list->size; i < list->capacity; ++i)
    {
        fprintf(dump_file, "%d[fillcolor = " FREE_COLOR "];\n", free_block);
        free_block = list->next[free_block];
    }

    // Point head and tail to their elements
    if (list->head != -1)
        fprintf (dump_file, "head->%d;\n", list->head);
    if (list->tail != -1)
        fprintf (dump_file, "tail->%d;\n", list->tail);

    fprintf (dump_file, "}\n");

    #undef FREE_COLOR
    #undef BUSY_COLOR
}

//-------------------------------------------------
