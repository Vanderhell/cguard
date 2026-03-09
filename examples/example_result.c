/**
 * @file example_result.c
 * @brief Practical examples of result.h usage
 *
 * Compile:
 *   gcc -std=c11 -I../include -o example_result example_result.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "result.h"

/* -------------------------------------------------------------------------
 * Example 1: Basic result_t return
 * ---------------------------------------------------------------------- */

result_t write_to_file(const char *path, const char *data) {
    FILE *f = fopen(path, "w");
    if (!f) return result_from_errno(errno);

    if (fputs(data, f) == EOF) {
        fclose(f);
        return RESULT_ERR_MSG(RESULT_ERR_IO, "fputs failed");
    }

    fclose(f);
    return RESULT_OK_VAL();
}

void example_basic(void) {
    puts("--- example_basic ---");

    result_t r = write_to_file("/tmp/cguard_result.txt", "hello result\n");
    if (!r.ok) {
        fprintf(stderr, "write failed [%d]: %s\n", r.status, r.msg);
        return;
    }
    puts("file written successfully");
}

/* -------------------------------------------------------------------------
 * Example 2: Typed result RESULT(T)
 * ---------------------------------------------------------------------- */

typedef RESULT(int) int_result_t;

int_result_t parse_positive_int(const char *s) {
    if (!s) return (int_result_t) RESULT_VAL_ERR(RESULT_ERR_NULL, "input is NULL");

    char *end;
    long val = strtol(s, &end, 10);

    if (end == s || *end != '\0')
        return (int_result_t) RESULT_VAL_ERR(RESULT_ERR_INVAL, "not a valid integer");

    if (val <= 0)
        return (int_result_t) RESULT_VAL_ERR(RESULT_ERR_INVAL, "must be positive");

    return (int_result_t) RESULT_VAL_OK((int)val);
}

void example_typed(void) {
    puts("--- example_typed ---");

    const char *inputs[] = { "42", "-5", "abc", NULL };

    for (int i = 0; i < 4; i++) {
        int_result_t r = parse_positive_int(inputs[i]);
        if (r.ok) {
            printf("  parse('%s') = %d\n", inputs[i], r.value);
        } else {
            printf("  parse('%s') failed: %s\n",
                   inputs[i] ? inputs[i] : "(null)", r.msg);
        }
    }
}

/* -------------------------------------------------------------------------
 * Example 3: RESULT_TRY – error propagation
 * ---------------------------------------------------------------------- */

result_t step_one(void) { return RESULT_OK_VAL(); }
result_t step_two(void) { return RESULT_ERR_MSG(RESULT_ERR_IO, "disk full"); }
result_t step_three(void) { return RESULT_OK_VAL(); }

result_t run_pipeline(void) {
    RESULT_TRY_CALL(step_one());    /* ok – continues */
    RESULT_TRY_CALL(step_two());    /* fails – returns error immediately */
    RESULT_TRY_CALL(step_three());  /* never reached */
    return RESULT_OK_VAL();
}

void example_propagation(void) {
    puts("--- example_propagation ---");

    result_t r = run_pipeline();
    if (!r.ok) {
        printf("pipeline failed at step two: [%s] %s\n",
               result_status_str(r.status), r.msg);
    }
}

/* -------------------------------------------------------------------------
 * Example 4: RESULT_GOTO – cleanup label pattern
 * ---------------------------------------------------------------------- */

result_t load_and_process(const char *path) {
    result_t ret = RESULT_OK_VAL();

    FILE *f = fopen(path, "r");
    if (!f) { ret = result_from_errno(errno); goto done; }

    char buf[64];
    if (!fgets(buf, sizeof(buf), f)) {
        ret = RESULT_ERR_MSG(RESULT_ERR_IO, "could not read line");
        goto done;
    }

    printf("  read line: %s", buf);

done:
    if (f) fclose(f);
    return ret;
}

void example_goto(void) {
    puts("--- example_goto ---");

    result_t r = load_and_process("/etc/hostname");
    if (!r.ok) fprintf(stderr, "  error: %s\n", r.msg);
    else puts("  loaded successfully");

    r = load_and_process("/nonexistent/path");
    if (!r.ok) printf("  expected error: [%s] %s\n",
                      result_status_str(r.status), r.msg);
}

/* -------------------------------------------------------------------------
 * Example 5: errno wrapping
 * ---------------------------------------------------------------------- */
void example_errno(void) {
    puts("--- example_errno ---");

    FILE *f = fopen("/root/forbidden", "r");
    if (!f) {
        result_t r = result_from_errno(errno);
        printf("  fopen failed: [%s] %s\n",
               result_status_str(r.status), r.msg);
    }
}

/* -------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */
int main(void) {
    puts("=== cguard result examples ===\n");

    example_basic();      puts("");
    example_typed();      puts("");
    example_propagation(); puts("");
    example_goto();       puts("");
    example_errno();

    puts("\n=== all done ===");
    return 0;
}
