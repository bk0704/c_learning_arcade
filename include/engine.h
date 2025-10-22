#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    TASK_ASK,
    TASK_QUIZ
} TaskType;

typedef struct {
    TaskType type;
    const char *prompt;
    const char *options[5];
    int correct_index;
    const char *answers[5];
    const char *hint;
    const char *why;
} Task;

typedef struct {
    int station_id;
    int total_tasks;
    int correct_first_try;
    int correct_with_hint;
    int total_points;
} StationResult;

StationResult run_station(int station_id, const Task *tasks, int task_count);

#endif /* ENGINE_H */
