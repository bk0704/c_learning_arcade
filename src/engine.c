#include "common.h"
#include "engine.h"

#define MAX_OPTIONS 5

static bool g_abort_station = false;

static void run_task(const Task *t, StationResult *res);
static void read_input(char *buf, size_t size);
static bool match_answer(const Task *t, const char *input);
static void print_why(const char *why);
static bool equals_ignore_case(const char *a, const char *b);
static int option_count(const Task *t);

StationResult run_station(int station_id, const Task *tasks, int task_count) {
    StationResult result = {station_id, 0, 0, 0, 0};

    if (!tasks || task_count <= 0) {
        puts("No tasks configured for this station yet.");
        return result;
    }

    g_abort_station = false;

    for (int i = 0; i < task_count; ++i) {
        if (g_abort_station) {
            break;
        }

        printf("\n" C_BOLD "Task %d/%d" C_RESET "\n", i + 1, task_count);
        run_task(&tasks[i], &result);

        if (!g_abort_station) {
            result.total_tasks++;
        }
    }

    int max_points = result.total_tasks * 2;
    int total_correct = result.correct_first_try + result.correct_with_hint;

    printf("\nStation %02d Summary:\n", station_id);
    printf("Tasks: %d | Correct: %d | With Hint: %d | Points: %d/%d\n",
           result.total_tasks, total_correct, result.correct_with_hint,
           result.total_points, max_points);

    if (g_abort_station) {
        puts(C_DIM "Station exited early; progress saved." C_RESET);
    }

    return result;
}

static void run_task(const Task *t, StationResult *res) {
    if (!t || !res) {
        return;
    }

    bool hint_used = false;
    bool format_hint_shown = false;
    int attempts = 0;
    bool need_prompt = true;
    char input[MAX_INPUT];

    while (!g_abort_station) {
        if (need_prompt) {
            printf("%s\n", t->prompt ? t->prompt : "(no prompt)");
            if (t->type == TASK_QUIZ) {
                int count = option_count(t);
                for (int i = 0; i < count; ++i) {
                    if (t->options[i]) {
                        printf("  %d) %s\n", i + 1, t->options[i]);
                    }
                }
            }
            need_prompt = false;
        }

        printf("> ");
        fflush(stdout);
        read_input(input, sizeof(input));

        if (g_abort_station) {
            return;
        }

        if (input[0] == '\0') {
            puts("Please enter a response or type 'hint', 'skip', or 'exit'.");
            need_prompt = true;
            continue;
        }

        if (strcmp(input, "hint") == 0) {
            if (t->hint && t->hint[0] != '\0') {
                puts(t->hint);
                hint_used = true;
            } else {
                puts("No hint available for this task.");
            }
            need_prompt = true;
            continue;
        }

        if (strcmp(input, "why") == 0) {
            puts("Answer or skip first, then I'll explain why.");
            need_prompt = true;
            continue;
        }

        if (strcmp(input, "skip") == 0) {
            puts("Task skipped.");
            print_why(t->why);
            return;
        }

        if (strcmp(input, "exit") == 0) {
            g_abort_station = true;
            puts("Exiting station...");
            return;
        }

        bool valid_answer = true;
        bool correct = false;

        if (t->type == TASK_QUIZ) {
            int count = option_count(t);
            if (count == 0) {
                valid_answer = false;
            } else {
                bool numeric = true;
                for (size_t i = 0; input[i]; ++i) {
                    if (!isdigit((unsigned char)input[i])) {
                        numeric = false;
                        break;
                    }
                }

                if (numeric) {
                    long value = strtol(input, NULL, 10);
                    if (value < 1 || value > count) {
                        valid_answer = false;
                    } else {
                        correct = ((int)value - 1) == t->correct_index;
                    }
                } else {
                    int selected = -1;
                    for (int i = 0; i < count; ++i) {
                        if (t->options[i] && equals_ignore_case(t->options[i], input)) {
                            selected = i;
                            break;
                        }
                    }
                    if (selected == -1) {
                        valid_answer = false;
                    } else {
                        correct = selected == t->correct_index;
                    }
                }
            }
        } else {
            correct = match_answer(t, input);
            valid_answer = true;
        }

        if (!valid_answer) {
            puts("Please choose one of the listed options or enter a command.");
            need_prompt = true;
            continue;
        }

        attempts++;

        if (correct) {
            if (!hint_used && attempts == 1) {
                res->correct_first_try++;
                res->total_points += 2;
                puts(C_GREEN "Correct!" C_RESET);
            } else {
                res->correct_with_hint++;
                res->total_points += 1;
                puts(C_GREEN "Correct (partial credit)." C_RESET);
            }
            print_why(t->why);
            return;
        }

        puts("Not quite. Try again, or type 'hint', 'skip', or 'exit'.");

        if (t->type == TASK_ASK) {
            if (attempts >= 2 && !format_hint_shown) {
                if (t->answers[0]) {
                    printf("Expected answers include: ");
                    for (int i = 0; i < MAX_OPTIONS && t->answers[i]; ++i) {
                        if (i > 0) {
                            printf(", ");
                        }
                        printf("%s", t->answers[i]);
                    }
                    printf("\n");
                }
                hint_used = true;
                format_hint_shown = true;
            }
        }

        need_prompt = true;
    }
}

static void read_input(char *buf, size_t size) {
    if (!buf || size == 0) {
        return;
    }

    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        g_abort_station = true;
        puts("Input closed. Exiting station...");
        return;
    }

    size_t len = strcspn(buf, "\r\n");
    buf[len] = '\0';

    size_t start = 0;
    while (buf[start] && isspace((unsigned char)buf[start])) {
        start++;
    }
    size_t end = strlen(buf);
    while (end > start && isspace((unsigned char)buf[end - 1])) {
        end--;
    }

    size_t final_len = end - start;
    if (start > 0 && final_len > 0) {
        memmove(buf, buf + start, final_len);
    }
    buf[final_len] = '\0';

    lower_inplace(buf);
}

static bool match_answer(const Task *t, const char *input) {
    if (!t || !input || input[0] == '\0') {
        return false;
    }

    for (int i = 0; i < MAX_OPTIONS; ++i) {
        const char *ans = t->answers[i];
        if (!ans) {
            continue;
        }
        if (equals_ignore_case(ans, input)) {
            return true;
        }
    }

    return false;
}

static void print_why(const char *why) {
    if (why && why[0] != '\0') {
        puts(why);
    } else {
        puts("WHY: We'll add an explanation soon.");
    }
}

static bool equals_ignore_case(const char *a, const char *b) {
    if (!a || !b) {
        return false;
    }
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

static int option_count(const Task *t) {
    int count = 0;
    if (!t) {
        return 0;
    }
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        if (t->options[i]) {
            count++;
        }
    }
    return count;
}
