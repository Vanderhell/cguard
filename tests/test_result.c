#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <cguard/result.h>

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

static int result_call_count = 0;

static result_t ok_once(void) {
    ++result_call_count;
    return result_success();
}

static result_t fail_once(void) {
    ++result_call_count;
    return result_failure(RESULT_ERR_IO, "io failed");
}

RESULT_DEFINE(int_result_t, int);
RESULT_DEFINE(pointer_result_t, char *);

typedef struct {
    int x;
    int y;
} point_t;

RESULT_DEFINE(point_result_t, point_t);

static int_result_t parse_count(const char *text) {
    if (text == NULL) {
        return int_result_t_err(RESULT_ERR_NULL, "missing text");
    }

    return int_result_t_ok((int)strlen(text));
}

static pointer_result_t duplicate_text(const char *text) {
    if (text == NULL) {
        return pointer_result_t_err(RESULT_ERR_NULL, "missing text");
    }

    return pointer_result_t_ok((char *)text);
}

static point_result_t make_point(int x, int y) {
    point_t point = { x, y };
    return point_result_t_ok(point);
}

static result_t propagate_chain(void) {
    result_call_count = 0;
    RESULT_TRY(ok_once());
    RESULT_TRY(fail_once());
    RESULT_TRY(ok_once());
    return result_success();
}

static result_t propagate_success(void) {
    result_call_count = 0;
    RESULT_TRY(ok_once());
    return result_success();
}

static result_t preserve_error_with_goto(void) {
    result_t err = result_success();

    RESULT_GOTO(err, fail_once(), cleanup);
    return result_success();

cleanup:
    return err;
}

TEST(test_constructor_invariants) {
    result_t ok = result_success();
    result_t err = result_failure(RESULT_ERR_IO, "broken");
    result_t normalized = result_failure(RESULT_OK, "not allowed");

    ASSERT(ok.status == RESULT_OK);
    ASSERT(ok.message == NULL);
    ASSERT(err.status == RESULT_ERR_IO);
    ASSERT(err.message != NULL);
    ASSERT(normalized.status != RESULT_OK);
}

TEST(test_named_scalar_result) {
    int_result_t value = parse_count("abcd");

    ASSERT(int_result_t_is_ok(value));
    ASSERT(value.status == RESULT_OK);
    ASSERT(value.value == 4);
}

TEST(test_named_pointer_result) {
    pointer_result_t value = duplicate_text("hello");

    ASSERT(pointer_result_t_is_ok(value));
    ASSERT(value.value != NULL);
    ASSERT(strcmp(value.value, "hello") == 0);
}

TEST(test_named_struct_result) {
    point_result_t value = make_point(3, 7);

    ASSERT(point_result_t_is_ok(value));
    ASSERT(value.value.x == 3);
    ASSERT(value.value.y == 7);
}

TEST(test_propagation_once_on_error) {
    result_t value = propagate_chain();

    ASSERT(result_call_count == 2);
    ASSERT(value.status == RESULT_ERR_IO);
    ASSERT(strcmp(value.message, "io failed") == 0);
}

TEST(test_propagation_once_on_success) {
    result_t value = propagate_success();

    ASSERT(result_call_count == 1);
    ASSERT(value.status == RESULT_OK);
}

TEST(test_goto_preserves_original_error) {
    result_t value = preserve_error_with_goto();

    ASSERT(value.status == RESULT_ERR_IO);
    ASSERT(strcmp(value.message, "io failed") == 0);
}

TEST(test_errno_mapping_and_buffer) {
    char buffer[64];
    result_t value = result_from_errno_buffer(ENOENT, buffer, sizeof(buffer));

    ASSERT(value.status == RESULT_ERR_NOT_FOUND);
    ASSERT(value.message == buffer);
    ASSERT(buffer[0] != '\0');
}

TEST(test_errno_default_storage) {
    result_t value = result_from_errno(EINVAL);

    ASSERT(value.status == RESULT_ERR_INVAL);
    ASSERT(value.message != NULL);
}

TEST(test_status_strings) {
    ASSERT(strcmp(result_status_string(RESULT_ERR_BUSY), "busy") == 0);
    ASSERT(strcmp(result_status_string((result_status_t)-12345), "unknown") == 0);
}

int main(void) {
    puts("=== test_result ===\n");

    RUN(test_constructor_invariants); puts("");
    RUN(test_named_scalar_result); puts("");
    RUN(test_named_pointer_result); puts("");
    RUN(test_named_struct_result); puts("");
    RUN(test_propagation_once_on_error); puts("");
    RUN(test_propagation_once_on_success); puts("");
    RUN(test_goto_preserves_original_error); puts("");
    RUN(test_errno_mapping_and_buffer); puts("");
    RUN(test_errno_default_storage); puts("");
    RUN(test_status_strings); puts("");

    printf("\n=== Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed != 0) {
        printf(", %d FAILED", tests_failed);
    }
    puts(" ===");
    return tests_failed == 0 ? 0 : 1;
}
