/**
 * @file test_scope_guard.c
 * @brief Unit tests for scope_guard.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_guard.h"

static int _tests = 0, _passed = 0, _failed = 0;
#define TEST(name) static void name(void)
#define RUN(name)  do { printf("  %-44s", #name); name(); } while(0)
#define ASSERT(cond) do { \
    _tests++; \
    if (cond) { _passed++; printf("."); } \
    else { _failed++; printf("\n  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while(0)

static int free_called   = 0;
static int fclose_called = 0;
static int custom_called = 0;

static void tracked_free(void *p) {
    void **pp = (void **)p;
    if (pp && *pp) { free(*pp); *pp = NULL; free_called++; }
}
static void tracked_fclose(FILE **f) {
    if (f && *f) { fclose(*f); *f = NULL; fclose_called++; }
}

typedef struct { int done; } custom_res_t;
static void custom_cleanup(custom_res_t **r) {
    if (r && *r) { (*r)->done = 1; custom_called++; }
}

static int lifo_order[2];
static int lifo_idx = 0;
static void cleanup_a(int **p) { (void)p; lifo_order[lifo_idx++] = 1; }
static void cleanup_b(int **p) { (void)p; lifo_order[lifo_idx++] = 2; }

static void early_return_inner(void) {
    SG_DEFER(tracked_free) char *p = malloc(8);
    (void)p;
}

typedef struct { int locked; } test_mtx_t;
static int test_unlock_count = 0;
static void test_mtx_unlock(test_mtx_t *m) { m->locked = 0; test_unlock_count++; }
SG_DEFINE_MUTEX_CLEANUP(test_mtx_cleanup, test_mtx_t, test_mtx_unlock)

TEST(test_sg_free_called) {
    free_called = 0;
    { SG_DEFER(tracked_free) char *p = malloc(16); (void)p; }
    ASSERT(free_called == 1);
}

TEST(test_sg_free_null_safe) {
    free_called = 0;
    { SG_DEFER(tracked_free) char *p = NULL; (void)p; }
    ASSERT(free_called == 0);
}

TEST(test_sg_fclose_called) {
    fclose_called = 0;
    {
        SG_DEFER(tracked_fclose) FILE *f = fopen("/tmp/cguard_test.txt", "w");
        ASSERT(f != NULL);
    }
    ASSERT(fclose_called == 1);
}

TEST(test_sg_auto_free_macro) {
    { SG_AUTO_FREE char *buf = malloc(32); ASSERT(buf != NULL); strcpy(buf, "ok"); }
    ASSERT(1);
}

TEST(test_sg_custom_cleanup) {
    custom_called = 0;
    custom_res_t res = { .done = 0 };
    { SG_DEFER(custom_cleanup) custom_res_t *g = &res; ASSERT(res.done == 0); }
    ASSERT(custom_called == 1);
    ASSERT(res.done == 1);
}

TEST(test_sg_early_return) {
    free_called = 0;
    early_return_inner();
    ASSERT(free_called == 1);
}

TEST(test_sg_lifo_order) {
    lifo_idx = 0;
    {
        int a = 0, b = 0;
        SG_DEFER(cleanup_a) int *ga = &a;
        SG_DEFER(cleanup_b) int *gb = &b;
        (void)ga; (void)gb;
    }
    ASSERT(lifo_order[0] == 2);
    ASSERT(lifo_order[1] == 1);
}

TEST(test_sg_define_mutex_cleanup) {
    test_unlock_count = 0;
    test_mtx_t mtx = { .locked = 1 };
    { SG_DEFER(test_mtx_cleanup) test_mtx_t *g = &mtx; ASSERT(mtx.locked == 1); }
    ASSERT(test_unlock_count == 1);
    ASSERT(mtx.locked == 0);
}

int main(void) {
    puts("=== test_scope_guard ===\n");
    RUN(test_sg_free_called);        puts("");
    RUN(test_sg_free_null_safe);     puts("");
    RUN(test_sg_fclose_called);      puts("");
    RUN(test_sg_auto_free_macro);    puts("");
    RUN(test_sg_custom_cleanup);     puts("");
    RUN(test_sg_early_return);       puts("");
    RUN(test_sg_lifo_order);         puts("");
    RUN(test_sg_define_mutex_cleanup); puts("");
    printf("\n=== Results: %d/%d passed", _passed, _tests);
    if (_failed) printf(", %d FAILED", _failed);
    puts(" ===");
    return _failed ? 1 : 0;
}
