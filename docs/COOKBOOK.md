# Cookbook

## Named typed result

```c
#include <cguard/result.h>
RESULT_DEFINE(read_count_result_t, int);
```

## Propagation

```c
result_t step(void) {
    RESULT_TRY(open_step());
    return result_success();
}
```

## Goto cleanup

```c
result_t ret = result_success();
RESULT_GOTO(ret, read_step(), cleanup);
```

## Typed free cleanup

```c
#include <cguard/scope_guard_stdlib.h>
SG_DEFINE_FREE_CLEANUP(cleanup_name, char *)
```

If you need a custom releaser, define it to take the held value:

```c
static void release_buffer(char *value) {
    free(value);
}

SG_DEFINE_PTR_CLEANUP(cleanup_name, char *, release_buffer)
```

## Manual fclose then dismiss

```c
fclose(file);
SG_DISMISS(file);
```
