#include <stdio.h>
#include <stdlib.h>

#include <cguard/scope_guard.h>
#include <cguard/scope_guard_stdio.h>
#include <cguard/scope_guard_stdlib.h>

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

#if CGUARD_HAS_SCOPE_GUARD && CGUARD_SCOPE_GUARD_ENABLE

typedef struct {
    int value;
} test_box_t;

typedef struct {
    int locked;
} test_mutex_t;

static int box_free_count = 0;
static int fclose_count = 0;
static int unlock_count = 0;
static int free_count = 0;
static int order_log[4];
static int order_index = 0;
static int acquisition_count = 0;

static FILE *cguard_tmpfile(void) {
#if defined(_MSC_VER)
    FILE *file = NULL;
    if (tmpfile_s(&file) != 0) {
        return NULL;
    }
    return file;
#else
    return tmpfile();
#endif
}

static void test_box_release(test_box_t *box) {
    if (box != NULL) {
        ++box_free_count;
        free(box);
    }
}

static void counted_char_free(char *value) {
    if (value != NULL) {
        free(value);
        ++free_count;
    }
}

static void counted_file_close(FILE *file) {
    if (file != NULL) {
        fclose(file);
        ++fclose_count;
    }
}

static void test_mutex_unlock(test_mutex_t *mutex) {
    if (mutex != NULL && mutex->locked != 0) {
        mutex->locked = 0;
        ++unlock_count;
    }
}

SG_DEFINE_PTR_CLEANUP(test_box_cleanup, test_box_t *, test_box_release)
SG_DEFINE_PTR_CLEANUP(test_mutex_cleanup, test_mutex_t *, test_mutex_unlock)
SG_DEFINE_FREE_CLEANUP(test_free_cleanup, char *)
SG_DEFINE_FCLOSE_CLEANUP(test_fclose_cleanup)
SG_DEFINE_PTR_CLEANUP(test_counted_char_free_cleanup, char *, counted_char_free)
SG_DEFINE_PTR_CLEANUP(test_counted_file_close_cleanup, FILE *, counted_file_close)

static void cleanup_first(int **value) {
    (void)value;
    order_log[order_index++] = 1;
}

static void cleanup_second(int **value) {
    (void)value;
    order_log[order_index++] = 2;
}

static void cleanup_third(int **value) {
    (void)value;
    order_log[order_index++] = 3;
}

static int cleanup_on_return(void) {
    test_box_t *box = malloc(sizeof(*box));
    if (box == NULL) {
        return -1;
    }

    SG_DEFER(test_box_cleanup) test_box_t *guard = box;
    guard->value = 42;
    return guard->value;
}

static int cleanup_on_goto(void) {
    test_box_t *box = malloc(sizeof(*box));
    if (box == NULL) {
        return -1;
    }

    SG_DEFER(test_box_cleanup) test_box_t *guard = box;
    guard->value = 7;
    goto done;

done:
    return guard->value;
}

static int cleanup_on_break(void) {
    int count = 0;
    for (int i = 0; i < 3; ++i) {
        test_box_t *box = malloc(sizeof(*box));
        if (box == NULL) {
            return -1;
        }

        SG_DEFER(test_box_cleanup) test_box_t *guard = box;
        ++count;
        if (i == 1) {
            break;
        }
    }

    return count;
}

static int cleanup_on_continue(void) {
    int count = 0;
    for (int i = 0; i < 3; ++i) {
        test_box_t *box = malloc(sizeof(*box));
        if (box == NULL) {
            return -1;
        }

        SG_DEFER(test_box_cleanup) test_box_t *guard = box;
        ++count;
        if (i < 2) {
            continue;
        }
    }

    return count;
}

static int cleanup_on_fallthrough(void) {
    int hit = 0;

    switch (1) {
    case 0:
        break;
    case 1: {
        test_box_t *box = malloc(sizeof(*box));
        if (box == NULL) {
            return -1;
        }

        SG_DEFER(test_box_cleanup) test_box_t *guard = box;
        ++hit;
        (void)guard;
    }
        /* fall through */
    case 2:
        ++hit;
        break;
    default:
        break;
    }

    return hit;
}

static int cleanup_nested_lifo(void) {
    order_index = 0;
    {
        int a = 0;
        int b = 0;
        int c = 0;
        SG_DEFER(cleanup_first) int *ga = &a;
        SG_DEFER(cleanup_second) int *gb = &b;
        SG_DEFER(cleanup_third) int *gc = &c;
        (void)ga;
        (void)gb;
        (void)gc;
    }

    return order_log[0] == 3 && order_log[1] == 2 && order_log[2] == 1;
}

TEST(test_cleanup_on_return) {
    ASSERT(cleanup_on_return() == 42);
}

TEST(test_cleanup_on_goto) {
    ASSERT(cleanup_on_goto() == 7);
}

TEST(test_cleanup_on_break) {
    ASSERT(cleanup_on_break() == 2);
}

TEST(test_cleanup_on_continue) {
    ASSERT(cleanup_on_continue() == 3);
}

TEST(test_cleanup_on_fallthrough) {
    ASSERT(cleanup_on_fallthrough() == 2);
}

TEST(test_nested_lifo_cleanup) {
    ASSERT(cleanup_nested_lifo() == 1);
}

TEST(test_null_resource_safe) {
    free_count = 0;
    {
        SG_DEFER(test_counted_char_free_cleanup) char *value = NULL;
        (void)value;
    }

    ASSERT(free_count == 0);
}

TEST(test_typed_free_adapter) {
    {
        SG_DEFER(test_counted_char_free_cleanup) char *value = malloc(8);
        ASSERT(value != NULL);
    }

    ASSERT(free_count == 1);
}

TEST(test_stdio_adapter) {
    {
        SG_DEFER(test_counted_file_close_cleanup) FILE *file = cguard_tmpfile();
        ASSERT(file != NULL);
    }

    ASSERT(fclose_count == 1);
}

TEST(test_dismiss_transfer) {
    free_count = 0;
    {
        SG_DEFER(test_counted_char_free_cleanup) char *value = malloc(8);
        ASSERT(value != NULL);
        char *owned = value;
        SG_DISMISS(value);
        free(owned);
    }

    ASSERT(free_count == 0);
}

TEST(test_manual_close_then_dismiss) {
    fclose_count = 0;
    {
        SG_DEFER(test_counted_file_close_cleanup) FILE *file = cguard_tmpfile();
        ASSERT(file != NULL);
        if (file != NULL) {
            ++fclose_count;
            fclose(file);
            SG_DISMISS(file);
        }
    }

    ASSERT(fclose_count == 1);
}

TEST(test_failed_acquisition_not_armed) {
    acquisition_count = 0;
    {
        SG_DEFER(test_free_cleanup) char *value = NULL;
        if (value == NULL) {
            ++acquisition_count;
        }
    }

    ASSERT(acquisition_count == 1);
}

TEST(test_mutex_cleanup_after_lock) {
    unlock_count = 0;
    test_mutex_t mutex = { 1 };
    {
        SG_DEFER(test_mutex_cleanup) test_mutex_t *guard = &mutex;
        ASSERT(guard->locked == 1);
    }

    ASSERT(unlock_count == 1);
    ASSERT(mutex.locked == 0);
}

#else

TEST(test_scope_guard_unavailable) {
    int available = CGUARD_HAS_SCOPE_GUARD;
    ASSERT(available == 0);
}

#endif

int main(void) {
    puts("=== test_scope_guard ===\n");

#if CGUARD_HAS_SCOPE_GUARD && CGUARD_SCOPE_GUARD_ENABLE
    RUN(test_cleanup_on_return); puts("");
    RUN(test_cleanup_on_goto); puts("");
    RUN(test_cleanup_on_break); puts("");
    RUN(test_cleanup_on_continue); puts("");
    RUN(test_cleanup_on_fallthrough); puts("");
    RUN(test_nested_lifo_cleanup); puts("");
    RUN(test_null_resource_safe); puts("");
    RUN(test_typed_free_adapter); puts("");
    RUN(test_stdio_adapter); puts("");
    RUN(test_dismiss_transfer); puts("");
    RUN(test_manual_close_then_dismiss); puts("");
    RUN(test_failed_acquisition_not_armed); puts("");
    RUN(test_mutex_cleanup_after_lock); puts("");
#else
    RUN(test_scope_guard_unavailable); puts("");
#endif

    printf("\n=== Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed != 0) {
        printf(", %d FAILED", tests_failed);
    }
    puts(" ===");
    return tests_failed == 0 ? 0 : 1;
}
