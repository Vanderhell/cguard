# cguard


[![CI](https://github.com/Vanderhell/cguard/actions/workflows/ci.yml/badge.svg)](https://github.com/Vanderhell/cguard/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C11](https://img.shields.io/badge/C-11-blue.svg)]()
[![Header only](https://img.shields.io/badge/header--only-yes-brightgreen.svg)]()

> Lightweight C utilities: automatic resource cleanup and unified error handling.

**Header-only. No dependencies. C11. GCC/Clang.**

---

## What's inside

| Header | What it does |
|---|---|
| `scope_guard.h` | RAII-style cleanup — free, fclose, unlock, etc. run automatically |
| `result.h` | Unified success/failure return values — no more `-1 vs NULL vs errno` |
| `cguard.h` | Include both with one line |

---

## Quick start

```c
#include "cguard.h"

// --- scope_guard: no manual free() or fclose() needed ---
int process(const char *path) {
    SG_AUTO_FREE  char *buf = malloc(256);
    SG_AUTO_FCLOSE FILE *f  = fopen(path, "r");
    if (!f) return -1;           // buf AND f cleaned up automatically

    fgets(buf, 256, f);
    printf("%s\n", buf);
    return 0;                    // same here
}

// --- result: clean error propagation ---
result_t write_config(const char *path, const char *data) {
    FILE *f = fopen(path, "w");
    if (!f) return result_from_errno(errno);
    fputs(data, f);
    fclose(f);
    return RESULT_OK_VAL();
}

int main(void) {
    result_t r = write_config("/tmp/cfg.txt", "key=value\n");
    if (!r.ok) {
        fprintf(stderr, "error [%s]: %s\n",
                result_status_str(r.status), r.msg);
        return 1;
    }
    return 0;
}
```

---

## Installation

cguard is header-only. Copy the headers into your project, or install system-wide.

### Option A – copy headers (simplest)

```sh
cp include/scope_guard.h include/result.h your_project/include/
```

### Option B – CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(cguard
    GIT_REPOSITORY https://github.com/Vanderhell/cguard.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(cguard)
target_link_libraries(your_target PRIVATE cguard)
```

### Option C – CMake install

```sh
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
cmake --install build
```

Then in your CMakeLists.txt:
```cmake
find_package(cguard REQUIRED)
target_link_libraries(your_target PRIVATE cguard::cguard)
```

---

## scope_guard.h

### Core macro

```c
SG_DEFER(cleanup_fn) type var = ...;
```

Calls `cleanup_fn(&var)` when `var` goes out of scope — regardless of how
the scope is exited (return, goto, early exit).

Cleanup order is **LIFO**: last declared → first cleaned.

### Built-in helpers

```c
SG_AUTO_FREE   char  *buf = malloc(256);    // → free()
SG_AUTO_FCLOSE FILE  *f   = fopen(...);     // → fclose()
```

### Custom cleanup

```c
void my_spi_cleanup(spi_t **s) { spi_end(*s); }

spi_begin(&spi);
SG_DEFER(my_spi_cleanup) spi_t *guard = &spi;
// spi_end() called automatically
```

### Mutex / lock pattern

```c
// Generate a cleanup function for your mutex type:
SG_DEFINE_MUTEX_CLEANUP(pthread_cleanup, pthread_mutex_t, pthread_mutex_unlock)

pthread_mutex_lock(&mtx);
SG_DEFER(pthread_cleanup) pthread_mutex_t *guard = &mtx;
// pthread_mutex_unlock() called automatically
```

### Compiler requirements

`scope_guard.h` requires `__attribute__((cleanup))`, available in:

| Compiler | Supported |
|---|---|
| GCC 3.4+ | ✅ |
| Clang 3.0+ | ✅ |
| MSVC | ❌ |
| ICC | ✅ (recent) |

---

## result.h

### Basic result_t

```c
result_t r = do_something();
if (!r.ok) {
    fprintf(stderr, "[%s] %s\n", result_status_str(r.status), r.msg);
    return r;   // propagate
}
```

### Typed result – RESULT(T)

```c
RESULT(int) parse_int(const char *s) {
    if (!s) return (RESULT(int)) RESULT_VAL_ERR(RESULT_ERR_NULL, "null");
    return (RESULT(int)) RESULT_VAL_OK(atoi(s));
}

RESULT(int) r = parse_int("42");
if (r.ok) printf("got: %d\n", r.value);
```

### Error propagation

```c
// Return early on first error:
RESULT_TRY(r);

// Call function and propagate its error immediately:
RESULT_TRY_CALL(write_header(f));

// Jump to cleanup label on error:
RESULT_GOTO(open_db(path), cleanup);
```

### Wrapping errno

```c
FILE *f = fopen(path, "r");
if (!f) return result_from_errno(errno);
```

### Status codes

| Code | Meaning |
|---|---|
| `RESULT_OK` | Success |
| `RESULT_ERR_GENERIC` | Unspecified error |
| `RESULT_ERR_NULL` | Unexpected NULL |
| `RESULT_ERR_NOMEM` | Allocation failure |
| `RESULT_ERR_IO` | I/O error |
| `RESULT_ERR_INVAL` | Invalid argument |
| `RESULT_ERR_TIMEOUT` | Timeout |
| `RESULT_ERR_OVERFLOW` | Overflow |
| `RESULT_ERR_NOT_FOUND` | Not found |
| `RESULT_ERR_PERMISSION` | Permission denied |
| `RESULT_ERR_BUSY` | Resource busy |
| `RESULT_ERR_USER_BASE` | Start of your own codes |

---

## Building examples and tests

```sh
# With make:
make run-tests

# With CMake:
cmake -B build -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build
```

---

## Design decisions

**Why header-only?**
Zero integration friction. Drop in and go.

**Why GCC/Clang only for scope_guard?**
`__attribute__((cleanup))` is not available on MSVC. This is clearly documented
rather than silently broken. Embedded and Linux targets — the primary audience —
are almost exclusively GCC/Clang.

**Why structs for RESULT(T)?**
The alternative (out-parameters) is noisier. For performance-critical hot paths
on embedded, avoid `RESULT(T)` on large types or measure first.

**Why not C99?**
`stdbool.h` and designated initialisers are available in C99 too; the library
uses C11 only for `_Static_assert` in a future version. C99 compatibility is
on the roadmap.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

MIT — see [LICENSE](LICENSE).
