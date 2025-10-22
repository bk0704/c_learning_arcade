#include "shell.h"
#include "stations.h"

/* ===== Shell-owned state ===== */
static GameState G;

/* forward decls for handlers */
static void cmd_help(const char *arg);
static void cmd_map(const char *arg);
static void cmd_play(const char *arg);
static void cmd_score(const char *arg);
static void cmd_quit(const char *arg);

typedef struct {
    const char *name;
    void (*fn)(const char *);
    const char *desc;
} Command;

/* Station registry (id order 02..15) */
static const Station REG[STATION_COUNT] = {
    {  2, "compilation",  "Compilation Runway",           station_compilation },
    {  3, "fundamentals", "Fundamentals Arena",           station_fundamentals },
    {  4, "functions",    "Functions Lab",                station_functions },
    {  5, "precision",    "Precision Casino",             station_precision },
    {  6, "imperative",   "Imperative Playground",        station_imperative },
    {  7, "types",        "Type System Bench",            station_types },
    {  8, "preprocessor", "Preprocessor Studio",          station_preprocessor },
    {  9, "pointers",     "Pointer Maze",                 station_pointers },
    { 10, "array1d",      "1D Array Workshop",            station_array1d },
    { 11, "arrays_ptrs",  "Arrays ↔ Pointers Tower",      station_arrays_ptrs },
    { 12, "memory",       "Memory-Mgmt Tycoon",           station_memory },
    { 13, "ptrptr",       "Pointer-to-Pointer Lab",       station_ptrptr },
    { 14, "funptr",       "Function-Pointer Arcade",      station_funptr },
    { 15, "strings",      "Chars & Strings Café",         station_strings }
};

/* Commands table */
static const Command CMDS[] = {
    { "help",  cmd_help,  "List commands and usage" },
    { "map",   cmd_map,   "Show stations 02..15 with progress" },
    { "play",  cmd_play,  "Start a station: play <02..15|keyword>" },
    { "score", cmd_score, "Show totals" },
    { "quit",  cmd_quit,  "Exit program" },
};
static const size_t CMDS_N = sizeof(CMDS)/sizeof(CMDS[0]);

/* ===== util ===== */
static int parse_station_arg(const char *arg) {
    if (!arg || !*arg) return -1;
    /* accept two-digit numeric or keyword */
    if (isdigit((unsigned char)arg[0])) {
        int v = atoi(arg);
        if (v >= 2 && v <= 15) return v;
        return -1;
    }
    /* keyword */
    for (int i = 0; i < STATION_COUNT; ++i) {
        if (strcmp(arg, REG[i].keyword) == 0) return REG[i].id;
    }
    return -1;
}

static const Station* find_station(int id) {
    for (int i = 0; i < STATION_COUNT; ++i) if (REG[i].id == id) return &REG[i];
    return NULL;
}

static void prompt(void) {
    printf(C_BOLD "c-arcade" C_RESET " (%d pts) > ", G.total_score);
    fflush(stdout);
}

/* ===== public ===== */
void shell_init(void) {
    memset(&G, 0, sizeof(G));
#if DEBUG
    fprintf(stdout, C_DIM "[DEBUG] Shell init complete\n" C_RESET);
#endif
}

void shell_teardown(void) {
#if DEBUG
    fprintf(stdout, C_DIM "[DEBUG] Shell teardown complete\n" C_RESET);
#endif
}

/* ===== handlers ===== */
static void cmd_help(const char *arg) {
    (void)arg;
    puts(C_CYAN "Commands:" C_RESET);
    for (size_t i = 0; i < CMDS_N; ++i) {
        printf("  %-6s %s\n", CMDS[i].name, CMDS[i].desc);
    }
    printf("\nBuild: DEBUG=%d  |  Stations: %d  |  play <02..15|keyword>\n",
           DEBUG, STATION_COUNT);
}

static void cmd_map(const char *arg) {
    (void)arg;
    puts(C_CYAN "Stations:" C_RESET);
    for (int i = 0; i < STATION_COUNT; ++i) {
        const char *mark = G.completed[i] ? MARK_OK : MARK_NO;
        const int id = REG[i].id;
        const char *kw = REG[i].keyword;
        const char *title = REG[i].title;
        printf("  [%02d] %-12s — %s  (%d pts, attempts %d)  %s\n",
       id, kw, mark, G.station_scores[i], G.attempted[i], title);

    }
    /* totals */
    int answered = 0;
    for (int i = 0; i < STATION_COUNT; ++i) answered += (G.attempted[i] > 0);
    printf("\nTotals: score=%d  answered=%d/%d\n",
           G.total_score, answered, STATION_COUNT);
}

static void cmd_play(const char *arg) {
    if (!arg || !*arg) {
        puts("usage: play <02..15|keyword>");
        return;
    }
    char tmp[MAX_INPUT];
    strncpy(tmp, arg, sizeof(tmp)-1); tmp[sizeof(tmp)-1] = 0;
    trim_eol(tmp); lower_inplace(tmp);

    int id = parse_station_arg(tmp);
    if (id < 0) {
        puts("invalid station. try: play 02  or  play pointers");
        return;
    }
    const Station *st = find_station(id);
    if (!st || !st->fn) { puts("station not found."); return; }

    int idx = -1; for (int i = 0; i < STATION_COUNT; ++i) if (REG[i].id == id) { idx = i; break; }

    printf(C_BOLD "[%02d] %s" C_RESET " — %s\n", st->id, st->keyword, st->title);
    G.attempted[idx] += 1;

    /* Phase 2 placeholder call */
    st->fn();

    /* You can simulate awarding a point on attempt in Phase 2 */
    G.station_scores[idx] += 0;
    if (!G.completed[idx]) G.completed[idx] = true; /* mark tried as completed for now */
}

static void cmd_score(const char *arg) {
    (void)arg;
    int sum = 0;
    for (int i = 0; i < STATION_COUNT; ++i) sum += G.station_scores[i];
    G.total_score = sum;
    printf("Score: %d pts  | stations attempted: ", G.total_score);
    int printed = 0;
    for (int i = 0; i < STATION_COUNT; ++i) if (G.attempted[i]) {
        printf("%s%02d", printed++ ? "," : "", REG[i].id);
    }
    if (!printed) printf("none");
    puts("");
}

static bool g_should_quit = false;
static void cmd_quit(const char *arg) {
    (void)arg;
    puts("Goodbye!");
    g_should_quit = true;
}

/* ===== REPL ===== */
void shell_loop(void) {
    char line[MAX_INPUT];

    puts(C_GREEN "Welcome to C Arcade — type 'help' to begin." C_RESET);
    while (!g_should_quit) {
        prompt();
        if (!fgets(line, sizeof(line), stdin)) {
            puts("\nEOF");
            break;
        }
        trim_eol(line);
        /* split into command + optional arg */
        char *cmd = line;
        while (*cmd && isspace((unsigned char)*cmd)) ++cmd;
        if (!*cmd) continue;

        char *sp = cmd;
        while (*sp && !isspace((unsigned char)*sp)) ++sp;
        char *arg = NULL;
        if (*sp) { *sp++ = '\0'; while (*sp && isspace((unsigned char)*sp)) ++sp; arg = sp; }

        lower_inplace(cmd);

        bool handled = false;
        for (size_t i = 0; i < CMDS_N; ++i) {
            if (strcmp(cmd, CMDS[i].name) == 0) {
                CMDS[i].fn(arg);
                handled = true;
                break;
            }
        }
        if (!handled) {
            puts("unknown command. try 'help'");
        }
    }
}
