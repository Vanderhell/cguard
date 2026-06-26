# cguard

Header-only C utilities for explicit result handling and optional scope-based cleanup.

## Public headers

- `#include <cguard/result.h>`
- `#include <cguard/scope_guard.h>`
- `#include <cguard/scope_guard_stdio.h>`
- `#include <cguard/scope_guard_stdlib.h>`
- `#include <cguard/cguard.h>`

## Targets

- `cguard::result`
- `cguard::scope_guard`
- `cguard::cguard`

## Result API

Use `result_t` for status-only functions and `RESULT_DEFINE(name, value_type)` for typed results.

```c
#include <cguard/result.h>

RESULT_DEFINE(parse_int_result_t, int);

static parse_int_result_t parse_int(const char *text) {
    if (text == NULL) {
        return parse_int_result_t_err(RESULT_ERR_NULL, "missing input");
    }
    return parse_int_result_t_ok(42);
}
```

Status is authoritative. `result_t` and typed results store `status`, optional borrowed `message`, and an optional `value`.

`result_from_errno_buffer()` copies the message into caller storage. `result_from_errno()` uses internal storage with defined lifetime.

## Scope guards

`SG_DEFER(cleanup_fn)` attaches a cleanup callback when `CGUARD_HAS_SCOPE_GUARD` is true.

Cleanup callbacks passed to `SG_DEFER` follow the compiler ABI and receive `T *` for a guarded `T`.
Type-safe releaser adapters such as `SG_DEFINE_PTR_CLEANUP(name, pointer_type, release_fn)` call the release function with the held value, so ordinary releasers like `free(void *)` and `fclose(FILE *)` work without casts.

Hosted helpers live in separate headers:

- `SG_DEFINE_FREE_CLEANUP(name, pointer_type)` from `scope_guard_stdlib.h`
- `SG_DEFINE_FCLOSE_CLEANUP(name)` from `scope_guard_stdio.h`

`SG_DISMISS(var)` transfers ownership by clearing the guarded variable.

Scope cleanup is lexical only. It does not run through `longjmp`, `abort`, `_Exit`, reset, hard fault, or power loss.

## Build

```sh
cmake -S . -B build -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

On Visual Studio generators, `ctest` needs `-C Debug`.

## Install and use

```cmake
find_package(cguard CONFIG REQUIRED)
target_link_libraries(app PRIVATE cguard::cguard)
```

For FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(cguard
    GIT_REPOSITORY https://github.com/Vanderhell/cguard.git
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(cguard)
target_link_libraries(app PRIVATE cguard)
```

## Verification

See `docs/VERIFICATION.md` for the exact commands that were run locally.
