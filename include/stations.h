#pragma once
#include "common.h"

/* Station entry (registry row) */
typedef struct {
    int id;                 /* 2..15 */
    const char *keyword;    /* e.g., "compilation" */
    const char *title;      /* pretty name */
    void (*fn)(void);       /* launcher */
} Station;

/* 14 stations: 02..15 */
void station_compilation(void);     /* 02 */
void station_fundamentals(void);    /* 03 */
void station_functions(void);       /* 04 */
void station_precision(void);       /* 05 */
void station_imperative(void);      /* 06 */
void station_types(void);           /* 07 */
void station_preprocessor(void);    /* 08 */
void station_pointers(void);        /* 09 */
void station_array1d(void);         /* 10 */
void station_arrays_ptrs(void);     /* 11 */
void station_memory(void);          /* 12 */
void station_ptrptr(void);          /* 13 */
void station_funptr(void);          /* 14 */
void station_strings(void);         /* 15 */
