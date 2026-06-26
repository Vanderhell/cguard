#include <stdio.h>

#include "multi_tu_shared.h"

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { printf("  %-40s", #name); name(); } while (0)
#define ASSERT(cond)                                                          \
    do {                                                                      \
        ++tests_run;                                                          \
        if (cond) {                                                           \
            ++tests_passed;                                                   \
            putchar('.');                                                     \
        } else {                                                              \
            ++tests_failed;                                                   \
            printf("\n  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                     \
    } while (0)

TEST(test_multi_translation_unit_result) {
    multi_tu_result_t value = multi_tu_double(21);

    ASSERT(multi_tu_result_t_is_ok(value));
    ASSERT(value.value == 42);
}

int main(void) {
    puts("=== test_multi_tu ===\n");
    RUN(test_multi_translation_unit_result); puts("");
    printf("\n=== Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed != 0) {
        printf(", %d FAILED", tests_failed);
    }
    puts(" ===");
    return tests_failed == 0 ? 0 : 1;
}
