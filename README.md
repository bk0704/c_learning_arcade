# C Arcade: Interactive C Learning REPL

Author: Bilal Killisly  
License: MIT  
Status: Phase 2 complete — scaffold + REPL shell

---

## Overview

C Arcade is a console REPL that turns the first 15 slides of an intro C
course into hands-on stations (02–15). Each station will become a small
lab with examples, short challenges, and a clear “WHY” line.

Topics: compilation, fundamentals, functions, precision, imperative
programming, type system, preprocessor, pointers, arrays, memory,
pointer-to-pointer, function pointers, and strings.

---

## Project structure

    c_arcade/
      include/
        common.h      # globals, macros, types
        shell.h       # REPL public API
        stations.h    # station registry & prototypes

      src/
        main.c
        shell.c
        station_compilation.c
        station_fundamentals.c
        station_functions.c
        station_precision.c
        station_imperative.c
        station_types.c
        station_preprocessor.c
        station_pointers.c
        station_array1d.c
        station_arrays_ptrs.c
        station_memory.c
        station_ptrptr.c
        station_funptr.c
        station_strings.c

      tests/
        golden_path.txt
        notes.md

      CMakeLists.txt
      README.md

---

## Build and run

CLion: open the folder, let CMake configure, run the `c_arcade` target.

Command line:

    cmake -S . -B build
    cmake --build build
    ./build/c_arcade

Optional compile options:

- Address/UB sanitizers: `-fsanitize=address,undefined -fno-omit-frame-pointer -O1`
- Crash-lab explainers: `-DBAD` (not enabled by default)
- ASCII progress marks: `-DUSE_ASCII_MARKS`

---

## Commands (Phase 2)

- `help`   : list commands and show DEBUG state
- `map`    : show stations 02–15 with progress
- `play <id|keyword>` : start a station (e.g., `play 02`, `play pointers`)
- `score`  : show total points and attempted stations
- `quit`   : exit the REPL

Stations currently print placeholders. Interactivity arrives in Phase 3.

---

## Learning coverage (02–15)

- 02 compilation    — preprocess, compile, assemble, link
- 03 fundamentals   — printf/scanf specifiers, int vs double division
- 04 functions      — prototypes, scope, pass-by-value
- 05 precision      — FP error, epsilon compare, overflow note
- 06 imperative     — control flow and mutable state
- 07 types          — casts, promotions, `size_t`, `%zu`
- 08 preprocessor   — macros, include guards, compile-time flags
- 09 pointers       — addresses, pointer arithmetic, `void*` casts
- 10 array1d        — arrays, decay to pointer, `a[i]` vs `*(a+i)`
- 11 arrays_ptrs    — 2D arrays, row-major, pointer math
- 12 memory         — stack vs heap, `malloc/calloc/realloc/free`
- 13 ptrptr         — pointer-to-pointer (re-pointing)
- 14 funptr         — function pointers, dispatch tables
- 15 strings        — `fgets`, `strcspn`, `strlen` vs `sizeof`, literals

---

## Current status

- [x] Scaffold with CMake
- [x] Public headers and include guards
- [x] REPL loop and command routing
- [x] Station placeholders (02–15)
- [x] Unicode marks with ASCII fallback option
- [ ] Phase 3: mini-lab engine (ask, quiz, WHY)
- [ ] Phases 4–6: interactive stations, memory tracker, sandboxes

---

## Contributing

Issues and PRs welcome. Keep station IDs stable; add new topics above 15.

---

## License (MIT)

Copyright (c) 2025 Bilal Killisly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
