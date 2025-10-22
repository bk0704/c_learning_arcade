#include "common.h"

typedef struct {
    const char *title;
    const char *narrative[8];
    const char *prompt;
    const char *explain;
} StoryScene;

typedef struct {
    bool completed;
    bool exit_requested;
} SceneOutcome;

static bool detect_test_mode(void);
static SceneOutcome run_scene(const StoryScene *scene, bool test_mode, bool has_more);
static void run_part1(bool test_mode);
static bool read_line(char *buf, size_t size);
static bool equals_ignore_case(const char *a, const char *b);

void station_compilation(void) {
    bool test_mode = detect_test_mode();
    run_part1(test_mode);
}

static bool detect_test_mode(void) {
    const char *env = getenv("TEST");
    if (env && env[0] != '\0' && env[0] != '0') {
        return true;
    }

    const char *flag_source = NULL;

#ifdef __GNUC__
    extern const char *station_compilation_args __attribute__((weak));
    if (station_compilation_args && station_compilation_args[0] != '\0') {
        flag_source = station_compilation_args;
    }
#endif

    if (!flag_source) {
        const char *candidates[] = {
            getenv("C_ARCADE_PLAY_ARGS"),
            getenv("PLAY_ARGS"),
            getenv("PLAY_FLAGS"),
            getenv("STATION_ARGS"),
            getenv("STATION_FLAGS"),
            getenv("STATION02_ARGS"),
            getenv("STATION02_FLAGS"),
            NULL
        };
        for (size_t i = 0; candidates[i]; ++i) {
            if (candidates[i][0] != '\0') {
                flag_source = candidates[i];
                break;
            }
        }
    }

    if (flag_source) {
        const char *p = flag_source;
        while (*p && isspace((unsigned char)*p)) {
            ++p;
        }
        if (*p) {
            const char *start = p;
            bool numeric_token = true;
            while (*p && !isspace((unsigned char)*p)) {
                if (!isdigit((unsigned char)*p)) {
                    numeric_token = false;
                }
                ++p;
            }
            if (numeric_token) {
                while (*p && isspace((unsigned char)*p)) {
                    ++p;
                }
            } else {
                p = start;
            }
        }
        if (strncmp(p, "--test", 6) == 0) {
            char next = p[6];
            if (next == '\0' || isspace((unsigned char)next)) {
                return true;
            }
        } else {
            const char *found = strstr(p, " --test");
            if (!found && strncmp(p, "--", 2) != 0) {
                found = strstr(p, "\t--test");
            }
            if (found) {
                const char *flag = found;
                while (*flag == ' ' || *flag == '\t') {
                    ++flag;
                }
                if (strncmp(flag, "--test", 6) == 0) {
                    char next = flag[6];
                    if (next == '\0' || isspace((unsigned char)next)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static bool read_line(char *buf, size_t size) {
    if (!buf || size == 0) {
        return false;
    }
    if (!fgets(buf, size, stdin)) {
        return false;
    }
    trim_eol(buf);
    return true;
}

static bool equals_ignore_case(const char *a, const char *b) {
    if (!a || !b) {
        return false;
    }
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;
        if (tolower(ca) != tolower(cb)) {
            return false;
        }
    }
    return *a == '\0' && *b == '\0';
}

static SceneOutcome run_scene(const StoryScene *scene, bool test_mode, bool has_more) {
    SceneOutcome outcome = { false, false };
    if (!scene) {
        return outcome;
    }

    printf("[SCENE] %s\n", scene->title);
    for (size_t i = 0; i < sizeof(scene->narrative) / sizeof(scene->narrative[0]); ++i) {
        if (!scene->narrative[i]) {
            break;
        }
        printf("[NARRATIVE] %s\n", scene->narrative[i]);
    }

    printf("[PROMPT] %s\n", scene->prompt);

    bool explanation_shown = false;
    char input[MAX_INPUT];

    while (!explanation_shown) {
        if (!test_mode) {
            printf("> ");
            fflush(stdout);
            if (!read_line(input, sizeof(input))) {
                outcome.exit_requested = true;
                return outcome;
            }
        } else {
            input[0] = '\0';
        }

        char lowered[MAX_INPUT];
        strncpy(lowered, input, sizeof(lowered) - 1);
        lowered[sizeof(lowered) - 1] = '\0';
        lower_inplace(lowered);

        if (equals_ignore_case(lowered, "exit")) {
            outcome.exit_requested = true;
            return outcome;
        }

        if (!explanation_shown) {
            if (equals_ignore_case(lowered, "skip")) {
                explanation_shown = true;
            } else if (equals_ignore_case(lowered, "why")) {
                puts("Give it a shot first, or type 'skip' to reveal the answer.");
                continue;
            } else {
                explanation_shown = true;
            }
        }
    }

    printf("[EXPLAIN] %s\n", scene->explain);
    outcome.completed = true;

    if (test_mode) {
        return outcome;
    }

    if (has_more) {
        puts("--- Press ENTER to continue ---");
    } else {
        puts("Press ENTER to wrap up, or type 'why' to revisit the explanation.");
    }

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!read_line(input, sizeof(input))) {
            outcome.exit_requested = true;
            return outcome;
        }

        char lowered[MAX_INPUT];
        strncpy(lowered, input, sizeof(lowered) - 1);
        lowered[sizeof(lowered) - 1] = '\0';
        lower_inplace(lowered);

        if (lowered[0] == '\0' || equals_ignore_case(lowered, "next")) {
            break;
        }
        if (equals_ignore_case(lowered, "why")) {
            printf("[EXPLAIN] %s\n", scene->explain);
            continue;
        }
        if (equals_ignore_case(lowered, "exit")) {
            outcome.exit_requested = true;
            return outcome;
        }

        if (has_more) {
            puts("Type 'why' to review, 'next' or ENTER to continue, or 'exit' to bail.");
        } else {
            puts("Type 'why' to review, 'next'/ENTER to finish, or 'exit' to bail.");
        }
    }

    return outcome;
}

static void run_part1(bool test_mode) {
    const StoryScene scenes[] = {
        {
            "You Wake Up in a Compiler’s Womb",
            {
                "Let’s assume for some fucked up reason you woke up as a newborn C program file.",
                "You’re lying naked in a directory called /src, next to some other code that hasn’t even been debugged yet.",
                "You’re not alive — not really. You’re just text. Syntax. Your soul is semicolons and braces.",
                "Then you hear it: a low industrial hum — the Compiler.",
                "The Compiler isn’t emotional. He’s an old, efficient factory worker. He doesn’t run you, he translates you into machine code before you ever execute.",
                NULL
            },
            "Before passing through, what does a compiler actually do to a source file like you?",
            "A compiler translates C source into machine code AHEAD OF TIME, producing a platform-specific executable (e.g., .exe/.out). Once done, it goes away; the CPU runs the machine code directly (native execution)."
        },
        {
            "The Interpreter Shows Up (The First Fucked Up Cousin)",
            {
                "The air glitches. A robed figure arrives: the Interpreter — eyes bloodshot, vibe chaotic.",
                "“I don’t wait,” he says. “I read you and run you NOW.”",
                "Python, Scheme, and Haskell float around, executing as they’re spoken.",
                NULL
            },
            "What’s the main difference between the Interpreter and the Compiler?",
            "An interpreter translates WHILE executing, statement by statement. No standalone executable is produced; the interpreter must remain present to run the program. This enables flexibility and rapid feedback, but costs speed each run."
        },
        {
            "The Hybrid (The Other, Even More Fucked Up Cousin)",
            {
                "Another cousin struts in wearing a jacket that says JVM: the Hybrid.",
                "“I do both,” he says. “First I compile to bytecode, then I interpret or JIT-compile hot paths to machine code at runtime.”",
                NULL
            },
            "Why do modern systems use this hybrid approach?",
            "Hybrid/JIT models combine PORTABILITY (bytecode runs wherever a VM exists) with SPEED (JIT compiles frequently executed code to optimized machine code). Used by Java (JVM), C# (.NET CLR), and modern JavaScript engines."
        },
        {
            "The Target Code Family Tree",
            {
                "You tour the archives: Machine code, Bytecode, Assembly, and Transpiled code (source-to-source).",
                "Machine code is raw binary and CPU-specific; bytecode is portable and VM-dependent; assembly is mnemonic and assembled later; transpilation turns one high-level language into another.",
                NULL
            },
            "Which of these forms runs directly on the CPU without any helper?",
            "Only MACHINE CODE executes natively. Bytecode needs a VM/interpreter; assembly needs an assembler; transpiled code still needs a compiler or interpreter for its target language."
        },
        {
            "The Trial of Advantages",
            {
                "Compiler King: speed, optimization, and stronger IP protection.",
                "Interpreter Sage: flexibility, interactive debugging, portability.",
                "Hybrid: starts portable, becomes fast on hot paths via JIT.",
                NULL
            },
            "Summarize when each approach shines.",
            "Compilation shines when performance and security are priorities (native machine code). Interpretation shines for rapid iteration, debugging, and portability. Hybrid/JIT shines when you want both portability and adaptive performance."
        },
        {
            "The Reflection",
            {
                "You realize they’re not enemies; they just differ on WHEN translation happens.",
                NULL
            },
            "In one sentence: what’s the real difference between compilation and interpretation?",
            "Timing and product: compilation happens BEFORE execution and outputs a standalone target program; interpretation happens DURING execution by translating as it runs. Hybrid compiles to bytecode then interprets/JITs at runtime."
        }
    };

    size_t completed_scenes = 0;
    size_t completed_prompts = 0;

    for (size_t i = 0; i < sizeof(scenes) / sizeof(scenes[0]); ++i) {
        bool has_more = (i + 1) < (sizeof(scenes) / sizeof(scenes[0]));
        SceneOutcome outcome = run_scene(&scenes[i], test_mode, has_more);
        if (outcome.completed) {
            completed_scenes++;
            completed_prompts++;
        }
        if (outcome.exit_requested) {
            break;
        }
    }

    printf("[SUMMARY] scenes=%zu prompts=%zu\n", completed_scenes, completed_prompts);
}
