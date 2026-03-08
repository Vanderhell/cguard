/**
 * @file test_result.c
 * @brief Unit tests for result.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "result.h"

static int _tests = 0, _passed = 0, _failed = 0;
#define TEST(name) static void name(void)
#define RUN(name)  do { printf("  %-46s", #name); name(); } while(0)
#define ASSERT(cond) do { \
    _tests++; \
    if (cond) { _passed++; printf("."); } \
    else { _failed++; printf("\n  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while(0)

/* --- helpers --- */
static result_t fn_ok(void)  { return RESULT_OK_VAL(); }
static result_t fn_err(void) { return RESULT_ERR_VAL(RESULT_ERR_IO); }
static int chain_count = 0;

static result_t call_chain(void) {
    chain_count = 0;
    result_t r;
    r = fn_ok();  chain_count++; RESULT_TRY(r);
    r = fn_err(); chain_count++; RESULT_TRY(r);
    r = fn_ok();  chain_count++;             /* should not run */
    return RESULT_OK_VAL();
}

typedef RESULT(int) int_result_t;

static int_result_t parse_int(const char *s) {
    if (!s) return (int_result_t){ .ok=false, .status=RESULT_ERR_NULL, .msg="null", .value=0 };
    return (int_result_t) RESULT_VAL_OK(atoi(s));
}

/* --- tests --- */

TEST(test_ok_val) {
    result_t r = RESULT_OK_VAL();
    ASSERT(r.ok == true);
    ASSERT(r.status == RESULT_OK);
    ASSERT(r.msg == NULL);
}

TEST(test_err_val) {
    result_t r = RESULT_ERR_VAL(RESULT_ERR_IO);
    ASSERT(r.ok == false);
    ASSERT(r.status == RESULT_ERR_IO);
}

TEST(test_err_msg) {
    result_t r = RESULT_ERR_MSG(RESULT_ERR_INVAL, "bad arg");
    ASSERT(r.ok == false);
    ASSERT(r.status == RESULT_ERR_INVAL);
    ASSERT(r.msg != NULL && strcmp(r.msg, "bad arg") == 0);
}

TEST(test_typed_ok) {
    int_result_t r = parse_int("99");
    ASSERT(r.ok == true);
    ASSERT(r.value == 99);
}

TEST(test_typed_null) {
    int_result_t r = parse_int(NULL);
    ASSERT(r.ok == false);
    ASSERT(r.status == RESULT_ERR_NULL);
}

TEST(test_try_stops_on_error) {
    result_t r = call_chain();
    ASSERT(r.ok == false);
    ASSERT(r.status == RESULT_ERR_IO);
    ASSERT(chain_count == 2);  /* third never reached */
}

static result_t all_ok_chain(void) {
    RESULT_TRY_CALL(fn_ok());
    RESULT_TRY_CALL(fn_ok());
    return RESULT_OK_VAL();
}

TEST(test_try_call_ok) {
    result_t r = all_ok_chain();
    ASSERT(r.ok == true);
}

TEST(test_status_str_ok) {
    ASSERT(strcmp(result_status_str(RESULT_OK), "OK") == 0);
}

TEST(test_status_str_nomem) {
    ASSERT(strcmp(result_status_str(RESULT_ERR_NOMEM), "Out of memory") == 0);
}

TEST(test_status_str_unknown) {
    ASSERT(strcmp(result_status_str((result_status_t)-999), "Unknown error") == 0);
}

TEST(test_from_errno_ok) {
    result_t r = result_from_errno(0);
    ASSERT(r.ok == true);
}

TEST(test_from_errno_nomem) {
    result_t r = result_from_errno(ENOMEM);
    ASSERT(r.ok == false && r.status == RESULT_ERR_NOMEM);
    ASSERT(r.msg != NULL);
}

TEST(test_from_errno_enoent) {
    result_t r = result_from_errno(ENOENT);
    ASSERT(r.ok == false && r.status == RESULT_ERR_NOT_FOUND);
}

TEST(test_from_errno_eacces) {
    result_t r = result_from_errno(EACCES);
    ASSERT(r.ok == false && r.status == RESULT_ERR_PERMISSION);
}

int main(void) {
    puts("=== test_result ===\n");
    RUN(test_ok_val);              puts("");
    RUN(test_err_val);             puts("");
    RUN(test_err_msg);             puts("");
    RUN(test_typed_ok);            puts("");
    RUN(test_typed_null);          puts("");
    RUN(test_try_stops_on_error);  puts("");
    RUN(test_try_call_ok);         puts("");
    RUN(test_status_str_ok);       puts("");
    RUN(test_status_str_nomem);    puts("");
    RUN(test_status_str_unknown);  puts("");
    RUN(test_from_errno_ok);       puts("");
    RUN(test_from_errno_nomem);    puts("");
    RUN(test_from_errno_enoent);   puts("");
    RUN(test_from_errno_eacces);   puts("");
    printf("\n=== Results: %d/%d passed", _passed, _tests);
    if (_failed) printf(", %d FAILED", _failed);
    puts(" ===");
    return _failed ? 1 : 0;
}
