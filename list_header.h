#pragma once

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <string.h>
//#include <iostream>

const int MAX_LIST_SIZE = 10000;

typedef int64_t int_t;
typedef float type_t;

enum ListErrors {
    LIST_OK               = 0,
    LIST_CAPACITY_ERROR   = 1,
    LIST_SIZE_ERROR       = 2,
    LIST_TAIL_ERROR       = 3,
    LIST_FREE_ERROR       = 4,
    LIST_HEAD_ERROR       = 5,
    LIST_SIZE_OVERFLOW    = 6,
    LIST_DATA_NULL        = 7,
    LIST_PREV_NULL        = 8,
    LIST_NEXT_NULL        = 9,
    LIST_POISON_ERROR     = 10,
    LIST_NEXT_PREV_ERROR  = 11,
    LIST_PREV_NEXT_ERROR  = 12,
    ZERO_ELEM_ERROR       = 13,
};

static const char* ListErrorNames[] = {
    "OK",
    "CAPACITY IS INVALIDE",
    "SIZE IS INVALIDE",
    "TAIL INDEX IS INVALIDE",
    "FREE INDEX IS INVALIDE",
    "HEAD INDEX IS INVALIDE",
    "SIZE IS BIGGER THAN CAPACITY",
    "ARRAY DATA POINTER IS INVALIDE",
    "ARRAY PREV POINTER IS INVALIDE",
    "ARRAY NEXT POINTER IS INVALIDE",
    "POISONED POSITION IS USED",
    "NEXT ELEMENT OF PREV TO CURRENT ELEMENT IS NOT EQUAL TO CURRENT ELEMENT",
    "PREV ELEMENT OF NEXT TO CURRENT ELEMENT IS NOT EQUAL TO CURRENT ELEMENT",
    "ZERO ELEMENT WITCH IS CONTROL ELEMENT IS INCORRECT"
};

enum ListExitCodes {
    SUCCESS           = 0,
    CONSTRUCT_FAILURE = 1,
    PUSH_ERROR = 2,
    POP_ERROR = 3,
    GET_ERROR = 4,
};

typedef struct List {
    int_t head;
    int_t tail;
    int_t free;
    int_t size;
    int_t capacity;

    type_t* data;
    int_t* next;
    int_t* prev;

    bool is_sort;
} List;


#define LIST_DUMP(list, error_no) {                        \
    FILE* fp = fopen("List_(" #list ")_Dump.txt", "w");    \
    assert(fp != NULL);                                    \
    ListTextDump(list, stdout, error_no);                  \
    fclose(fp);                                            \
}

#define ASSERT_LIST(list) {                                \
    ListErrors error_no = ListOk(list);                    \
    if (error_no != LIST_OK) {                             \
        LIST_DUMP(list, error_no);                         \
        assert(!"OK");                                     \
    }                                                      \
}

type_t* MakeData(int_t data_size);

int_t* MakeNext(int_t next_size);

int_t* MakePrev(int_t prev_size);

ListExitCodes ListConstruct (List* list, int_t construct_size);

void ListTextDump(List* list, FILE* fp, ListErrors error_no);

ListErrors ListOk(List* list);

ListExitCodes ListDestruct(List* list);
