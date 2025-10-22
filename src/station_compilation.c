#include "common.h"
#include "engine.h"

void station_compilation(void) {
    const Task tasks[] = {
        {
            TASK_QUIZ,
            "Which step removes comments and expands macros?",
            {"Lexical Analysis", "Preprocessing", "Optimization", NULL, NULL},
            1,
            {NULL},
            "It's before lexical analysis",
            "WHY: The preprocessor handles macros and comments before tokenization."
        },
        {
            TASK_ASK,
            "What structure represents nested syntax rules?",
            {NULL, NULL, NULL, NULL, NULL},
            -1,
            {"context free grammar", "cfg", NULL, NULL, NULL},
            "Think grammar types",
            "WHY: Context-Free Grammar defines valid language syntax for parsers."
        }
    };

    StationResult res = run_station(2, tasks, (int)(sizeof(tasks) / sizeof(tasks[0])));
    printf("Points earned: %d\n", res.total_points);
}
