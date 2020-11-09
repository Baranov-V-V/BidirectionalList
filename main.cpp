//#include "TXLib.h"
#include "list_header.h"

type_t* MakeData(type_t* data, int_t data_size) {
    assert(data_size >= 0);

    for(int_t i = 0; i < data_size + 1; ++i) {
        data[i] = NAN;
    }

    return data;
}

int_t* MakeNext(int_t* next, int_t next_size) {
    assert(next_size >= 0);

    next[0] = 0;
    for(int_t i = 1; i < next_size; ++i) {
        next[i] = i + 1;
    }
    next[next_size] = 0;

    return next;
};

int_t* MakePrev(int_t* prev, int_t prev_size) {
    assert(prev_size >= 0);

    prev[0] = 0;
    for(int_t i = 1; i < prev_size + 1; ++i) {
        prev[i] = -1;
    }

    return prev;
};

ListExitCodes ListConstruct(List* list, int_t construct_size) {
    assert(list != NULL);
    assert(construct_size >= 0);

    list->data = (type_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->data != NULL);
    list->next = (int_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->next != NULL);
    list->prev = (int_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->prev != NULL);

    MakeData(list->data, construct_size);
    MakeNext(list->next, construct_size);
    MakePrev(list->prev, construct_size);

    list->capacity = construct_size;
    list->size = 0;
    list->head = 0;
    list->tail = 0;
    list->free = 1;
    list->is_sort = false;

    ASSERT_LIST(list);

    return SUCCESS;
}

void ListTextDump(List* list, FILE* fp, ListErrors error_no) {
    assert(list != NULL);
    assert(fp != NULL);

    fprintf(fp, "Error code : %d (%s)\n\n", error_no, ListErrorNames[error_no]);
    fprintf(fp, "adress of List: [%p]\n", list);

    fprintf(fp, "{\n    capacity = %ld\n", list->capacity);
    fprintf(fp, "    size = %lld\n", list->size);
    fprintf(fp, "    head = %lld\n", list->head);
    fprintf(fp, "    tail = %lld\n", list->tail);
    fprintf(fp, "    free = %lld\n\n", list->free);

    fprintf(fp, "    data adress: [%p]\n", list->data);
    fprintf(fp, "    next adress: [%p]\n", list->next);
    fprintf(fp, "    prev adress: [%p]\n\n", list->prev);

    fprintf(fp, "    {\n");
    fprintf(fp, "             Data                        Next            Prev\n");
    for (int_t i = 0; i < list->capacity + 1; i++) {            //надо понять где poison а где нет
        if (i != 0 && list->prev[i] != -1) {
            fseek(fp, 7, SEEK_CUR);
            int symbols_count = 0;

            fprintf(fp, "*[%lld] %n", i, &symbols_count);

            fseek(fp, 10 - symbols_count, SEEK_CUR);
            fprintf(fp, " = %f (Valid!) %n", list->data[i], &symbols_count);

            fseek(fp, 24 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld %n", list->next[i], &symbols_count);

            fseek(fp, 15 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld\n", list->prev[i]);
        }
        else {
            fseek(fp, 8, SEEK_CUR);
            int symbols_count = 0;

            fprintf(fp, "[%lld]%n", i, &symbols_count);

            fseek(fp, 10 - symbols_count, SEEK_CUR);
            fprintf(fp, "= %f (Poison!)%n", list->data[i], &symbols_count);

            fseek(fp, 24 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld%n", list->next[i], &symbols_count);

            fseek(fp, 15 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld\n", list->prev[i]);
        }
    }
    fprintf(fp, "   }\n");

    fprintf(fp, "}\n");
}

void ListGraphDump(List* list, const char* file_name) {
    assert(list != NULL);
    assert(file_name != NULL);

    char* command_dot = (char*) calloc(100, sizeof(char));
    char* graph_file = (char*) calloc(strlen(file_name) + 10, sizeof(char));

    strcpy(graph_file, file_name);
    strcat(graph_file, ".dot");
    FILE* fp = fopen(graph_file, "w");
    assert(fp != NULL);

    strcat(command_dot, "dot -Tpng ");   //graph_test1.dot -o Demo3.png"
    strcat(command_dot, graph_file);
    strcat(command_dot, " -o ");
    strcat(command_dot, file_name);
    strcat(command_dot, "_graph.png");
    //printf("%s\n%s", graph_file, command_dot);

/*digraph structs {
    rankdir = LR

    node [shape=record];
    list_node1 [label="<index> index\n 0 |{<value> value\n 0.45 |<next> next\n 0 |<prev> prev\n 0}"];
    list_node2 [label="<index> index\n 1 |<value> value\n 45 |<next> next\n 2 |<prev> prev\n 0"];
} */

    fprintf(fp, "digraph structs {\n    rankdir = LR\n    node [shape=record];\n");
    for (int_t i = 0; i < list->capacity + 1; ++i) {
        fprintf(fp, "    list_node%lld [label =\"<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"];\n",
                i, i, list->data[i], list->next[i], list->prev[i]);
    }


    //list_node2:next -> list_node3:index;
    //list_node3:prev -> list_node2:index;

    for(int_t i = 1; i < list->capacity + 1; ++i) {
        fprintf(fp, "    list_node%lld:next -> list_node%lld:index\n", i, list->next[i]);
        if (list->prev[i] != -1) {
            fprintf(fp, "    list_node%lld:prev -> list_node%lld:index\n", i, list->prev[i]);
        }
    }

    fprintf(fp, "}");

    //const char* command_name = "dot -Tpng graph_test1.dot -o Demo5.png";
    //printf("%d", system(command_name));

    fclose(fp);

    //printf("command_dot: %s\n", command_dot);
    printf("graphing done with code: %d", system(command_dot));
}

ListErrors ListOk(List* list) {
    assert(list != NULL);

    if (list->size < 0) {
        return LIST_SIZE_ERROR;
    }
    if (list->capacity < 0) {
        return LIST_CAPACITY_ERROR;
    }
    if (list->size > list->capacity) {
        return LIST_SIZE_OVERFLOW;
    }
    if (list->head < 0 || list->head > list->capacity) {
        return LIST_HEAD_ERROR;
    }
    if (list->tail < 0 || list->tail > list->capacity) {
        return LIST_TAIL_ERROR;
    }
    if (list->free < 0 || list->free > list->capacity) {
        return LIST_FREE_ERROR;
    }

    if (list->data == NULL) {
        return LIST_DATA_NULL;
    }
    if (list->prev == NULL) {
        return LIST_PREV_NULL;
    }
    if (list->next == NULL) {
        return LIST_NEXT_NULL;
    }

    if (isfinite(list->data[0]) != 0 || list->prev[0] != 0 || list->next[0] != 0) {
        return ZERO_ELEM_ERROR;
    }

    for (int i = 1; i < list->capacity + 1; ++i) {
        if (isfinite(list->data[i]) != 0 && list->prev[i] == -1) {
            return LIST_POISON_ERROR;
        }
    }

    /*for (int i = 2; i < list->capacity; ++i) {
        if (list->prev[i] != -1 && list->prev[list->next[i]] != -1 && list->next[list->prev[i]] != -1) {
            if (list->data[list->prev[list->next[i]]] != list->data[i]) {
                return LIST_PREV_NEXT_ERROR;
            }
            if (list->data[list->next[list->prev[i]]] != list->data[i]) {
                return LIST_NEXT_PREV_ERROR;
            }
        }
    } */

    //if (list->data[list->prev[list->next[1]]] != list->data[1] && list->prev[1] != -1) {
      //  return LIST_PREV_NEXT_ERROR;
    //}
    /*if (list->data[list->next[list->prev[list->capacity - 1]]] != list->data[list->capacity - 1]) {  //ошибка
        return LIST_NEXT_PREV_ERROR;
    }   */
    return LIST_OK;
}

ListExitCodes ListDestruct(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    list->size = -1;
    list->capacity = -1;
    list->head = -1;
    list->tail = -1;
    list->free = -1;

    free(list->data);
         list->data = NULL;
    free(list->next);
         list->next = NULL;
    free(list->prev);
         list->prev = NULL;

    return SUCCESS;
}

ListExitCodes ListClear(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    list->size = 0;
    list->head = 0;
    list->tail = 0;
    list->free = 1;

    MakeData(list->data, list->capacity);
    MakePrev(list->prev, list->capacity);
    MakeNext(list->next, list->capacity);

    ASSERT_LIST(list);
    return SUCCESS;
}

int_t FindCurrentIndex(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (index >= list->size) {
        return -1;  //invalid index;
    }

    int_t current_index = list->head;
    for (int_t i = 0; i < index; ++i) {
        current_index = list->next[current_index];
    }

    ASSERT_LIST(list);
    return current_index;
}

ListExitCodes ListPushBack(List* list, type_t value) {
    assert(list != NULL);
    assert(isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    list->data[list->free] = value;

    if (list->size == 0) {
        list->head = 1;
    }
    else {
        list->next[list->tail] = list->free;
    }

    list->prev[list->free] = list->tail;

    list->tail = list->free;
    list->free = list->next[list->free];

    list->next[list->tail] = 0;

    ++list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPushFront(List* list, type_t value) {
    assert(list != NULL);
    assert(isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    list->data[list->free] = value;

    if (list->size == 0) {
        list->tail = 1;              //обработать вставку первого элемента
        list->head = 1;

        list->prev[list->head] = 0;
        list->next[list->head] = 0;
        list->free = 2;
    }
    else {
        int_t current_index = list->free;
        list->free = list->next[list->free];

        list->prev[current_index] = 0;
        list->prev[list->head] = current_index;

        list->next[current_index] = list->head;

        if (list->size == 1) {
            list->next[list->head] = 0;
        }

        list->head = current_index;
    }

    ++list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPushBefore(List* list, int_t index, type_t value) {
    assert(list != NULL);
    assert(isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    int_t index_to_push = FindCurrentIndex(list, index);

    if (index_to_push == -1) {
        printf("invalid index to push before\n");
        return PUSH_ERROR;
    }

    if (index_to_push == list->head) {
        ListPushFront(list, value);
        return SUCCESS;
    }

    list->data[list->free] = value;    //insert and free update
    int_t current_index = list->free;
    list->free = list->next[list->free];

    list->next[current_index] = index_to_push;                 //
    list->next[list->prev[index_to_push]] = current_index;

    list->prev[current_index] = list->prev[index_to_push];
    list->prev[index_to_push] = current_index;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPushAfter(List* list, int_t index, type_t value) {
    assert(list != NULL);
    assert(isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    int_t index_to_push = FindCurrentIndex(list, index);

    printf("index_to_push: %lld", index_to_push);

    if (index_to_push == -1) {
        printf("invalid index to push after\n");
        return PUSH_ERROR;
    }

    if (index_to_push == list->tail) {
        ListPushBack(list, value);
        return SUCCESS;
    }

    list->data[list->free] = value;
    int_t current_index = list->free;

    list->free = list->next[list->free];

    list->next[current_index] = list->next[index_to_push];
    list->next[index_to_push] = current_index;

    list->prev[list->next[current_index]] = current_index;
    list->prev[current_index] = index_to_push;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPopBack(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    list->data[list->tail] = NAN;
    if (list->size == 1) {
        list->next[list->tail] = 2;
        list->prev[list->tail] = -1;
        list->head = 0;
        list->tail = 0;
        list->free = 1;
    }
    else {
        list->next[list->tail] = list->free;
        list->free = list->tail;

        list->tail = list->prev[list->tail];
        list->prev[list->next[list->tail]] = -1;
    }

    --list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPopFront(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    list->data[list->head] = NAN;

    if (list->size == 1) {
        list->next[list->head] = 2;
        list->prev[list->head] = -1;
        list->head = 0;
        list->tail = 0;
        list->free = 1;
    }
    else {
        int_t next_tmp = list->next[list->head];
        list->next[list->head] = list->free;
        list->free = list->head;

        list->prev[list->head] = -1;
        list->head = next_tmp;
    }

    --list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPopBefore(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;
    }

    int_t index_to_pop = FindCurrentIndex(list, index);

    if (index_to_pop == -1) {
        printf("invalid index to pop before\n");
        return POP_ERROR;
    }

    if (index_to_pop == list->next[list->head] || index_to_pop == list->head) {
        ListPopFront(list);
        ASSERT_LIST(list);
        return SUCCESS;
    }

    list->next[list->prev[list->prev[index_to_pop]]] = index_to_pop;
    int_t prev_tmp = list->prev[list->prev[index_to_pop]];

    list->data[list->prev[index_to_pop]] = NAN;
    list->prev[list->prev[index_to_pop]] = -1;
    list->next[list->prev[index_to_pop]] = list->free;
    list->free = list->prev[index_to_pop];

    list->prev[index_to_pop] = prev_tmp;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPopAfter(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    int_t index_to_pop = FindCurrentIndex(list, index);

    if (index_to_pop == -1) {
        printf("invalid index to pop before\n");
        return POP_ERROR;
    }

    if (index_to_pop == list->prev[list->tail] || index_to_pop == list->tail) {
        ListPopBack(list);
        ASSERT_LIST(list);
        return SUCCESS;
    }

    list->prev[list->next[list->next[index_to_pop]]] = index_to_pop;
    int_t next_tmp = list->next[list->next[index_to_pop]];

    list->data[list->next[index_to_pop]] = NAN;
    list->prev[list->next[index_to_pop]] = -1;
    list->next[list->next[index_to_pop]] = list->free;
    list->free = list->next[index_to_pop];

    list->next[index_to_pop] = next_tmp;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPop(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    int_t index_to_pop = FindCurrentIndex(list, index);

    if (index_to_pop == -1) {
        printf("invalid index to pop before\n");
        return POP_ERROR;
    }

    if (index_to_pop == list->head) {
        ListPopFront(list);
        return SUCCESS;
    }

    if (index_to_pop == list->tail) {
        ListPopBack(list);
        return SUCCESS;
    }

    list->next[list->prev[index_to_pop]] = list->next[index_to_pop];
    list->prev[list->next[index_to_pop]] = list->prev[index_to_pop];

    list->data[index_to_pop] = NAN;
    list->prev[index_to_pop] = -1;
    list->next[index_to_pop] = list->free;
    list->free = index_to_pop;

    return SUCCESS;
}

type_t ListGetElem(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    int_t index_to_get = FindCurrentIndex(list, index);

    if (index_to_get == -1) {
        printf("invalid index to get elem\n");
        return GET_ERROR;
    }

    ASSERT_LIST(list);
    return list->data[index_to_get];
}

int_t ListGetIndex(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    int_t index_to_get = FindCurrentIndex(list, index);

    if (index_to_get == -1) {
        printf("invalid index to get\n");
        return GET_ERROR;
    }

    ASSERT_LIST(list);
    return index_to_get;
}

int NodesComp(List* list, int_t lhs, int_t rhs) {
    if (list->prev[lhs] == - 1) {
        return 1;
    }
    if (list->prev[rhs] == -1) {
        return -1;
    }

    return (int)(list->data[lhs] - list->data[rhs]);
}

void SwapNodes(List* list, int_t lhs, int_t rhs) {
    type_t data_tmp = list->data[lhs];
    list->data[lhs] = list->data[rhs];
    list->data[rhs] = data_tmp;

    int_t next_tmp = list->next[lhs];
    list->next[lhs] = list->next[rhs];
    list->next[rhs] = next_tmp;

    int_t prev_tmp = list->prev[lhs];
    list->prev[lhs] = list->prev[rhs];
    list->prev[rhs] = prev_tmp;
}

void QuickListSort(List* list, int_t left_pos, int_t right_pos, int (*comp)(List* list, int_t lhs, int_t rhs)) {
    int_t last = 0;

    if (left_pos >= right_pos) {
        return;
    }

    if (right_pos - left_pos == 1) {
        if ( (*comp)(list, left_pos, right_pos) < 0 ) {
            SwapNodes(list, left_pos, right_pos);
        }
    }
    SwapNodes(list, left_pos, (left_pos + right_pos) / 2);
    last = left_pos;

    for (int_t i = left_pos + 1; i <= right_pos; i++) {
        if ( (*comp)(list, i, left_pos) < 0 ) {
            SwapNodes(list, ++last, i);
        }
    }

    SwapNodes(list, left_pos, last);

    QuickListSort(list, left_pos, last - 1, comp);
    QuickListSort(list, last + 1, right_pos, comp);
}

ListExitCodes ListSort(List* list) {
    QuickListSort(list, 1, list->capacity, NodesComp);
    list->is_sort = true;
}


int main() {
    List list = {};
    ListConstruct(&list, 10);


    ListPushBack(&list, 10);
    ListPushBack(&list, 20);
    //ListPopFront(&list);
    //ListPopFront(&list);
    //ListPopBack(&list);
    ListPushFront(&list, 5);
    ListPushFront(&list, 7);
    //ListPop(&list, 2);
    //ListPopBefore(&list, 3);
    //ListPopAfter(&list, 1);
    //ListPushFront(&list, 8);
    //ListPushBack(&list, 30);
    ListPushBack(&list, 40);
    //ListPushFront(&list, 40);

    //ListPushAfter(&list, 1, 12);
    //ListPushBefore(&list, 2, 21);
    //printf("index: %d\n", FindCurrentIndex(&list, 5));
      //  ListPushFront(&list, 40);
    //ListPushFront(&list, 6);
    ListSort(&list);

    LIST_DUMP(&list, LIST_OK);
    //ListGraphDump(&list, "ListDump");
    return 0;
}
