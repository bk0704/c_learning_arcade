#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#define MAX_INPUT      256
#define STATION_COUNT  14     /* 02..15 inclusive */

typedef enum { OK = 0, ERR = 1 } Status;

/* Game state tracked by the shell (no heap in Phase 2) */
typedef struct {
    int total_score;
    int station_scores[STATION_COUNT];
    bool completed[STATION_COUNT];
    int attempted[STATION_COUNT];
} GameState;

/* tiny helpers */
static inline void trim_eol(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) { s[--n] = '\0'; }
}

static inline void lower_inplace(char *s) {
    if (!s) return;
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

/* color (you can disable if you prefer) */
#define C_RESET  "\x1b[0m"
#define C_DIM    "\x1b[2m"
#define C_GREEN  "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_CYAN   "\x1b[36m"
#define C_BOLD   "\x1b[1m"

/* Marks: Unicode by default; flip to ASCII if needed */
#ifndef USE_ASCII_MARKS
#  define MARK_OK  "✓"
#  define MARK_NO  "✗"
#else
#  define MARK_OK  "OK"
#  define MARK_NO  "--"
#endif
