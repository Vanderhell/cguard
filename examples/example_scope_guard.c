/**
 * @file example_scope_guard.c
 * @brief Practical examples of scope_guard.h usage
 *
 * Compile:
 *   gcc -std=c11 -I../include -o example_scope_guard example_scope_guard.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_guard.h"

/* -------------------------------------------------------------------------
 * Example 1: Auto-free heap memory
 * ---------------------------------------------------------------------- */
void example_auto_free(void) {
    puts("--- example_auto_free ---");

    SG_AUTO_FREE char *buf = malloc(64);
    if (!buf) { puts("alloc failed"); return; }

    snprintf(buf, 64, "Hello, scope guard!");
    puts(buf);

    /* buf is automatically freed here – no manual free() needed */
}

/* -------------------------------------------------------------------------
 * Example 2: Auto-close file
 * ---------------------------------------------------------------------- */
void example_auto_fclose(void) {
    puts("--- example_auto_fclose ---");

    SG_AUTO_FCLOSE FILE *f = fopen("/tmp/cguard_test.txt", "w");
    if (!f) { perror("fopen"); return; }

    fprintf(f, "written by cguard example\n");

    /* f is automatically fclose()'d here */
    puts("file written and closed automatically");
}

/* -------------------------------------------------------------------------
 * Example 3: Multiple resources, early return safe
 * ---------------------------------------------------------------------- */
int example_multi_resource(const char *src_path, const char *dst_path) {
    puts("--- example_multi_resource ---");

    SG_AUTO_FCLOSE FILE *src = fopen(src_path, "r");
    if (!src) { perror("open src"); return -1; }  /* dst not open yet – fine */

    SG_AUTO_FCLOSE FILE *dst = fopen(dst_path, "w");
    if (!dst) { perror("open dst"); return -1; }  /* src closed automatically */

    SG_AUTO_FREE char *line = malloc(256);
    if (!line) { return -1; }                      /* both files closed automatically */

    while (fgets(line, 256, src)) {
        fputs(line, dst);
    }

    puts("copy done, all resources released automatically");
    return 0;
}

/* -------------------------------------------------------------------------
 * Example 4: Custom cleanup – SPI / peripheral pattern
 * ---------------------------------------------------------------------- */

/* Simulated SPI driver */
typedef struct { int bus; int active; } spi_t;

static void spi_begin(spi_t *s) { s->active = 1; printf("SPI bus %d: BEGIN\n", s->bus); }
static void spi_end_fn(spi_t *s) { if (s->active) { s->active = 0; printf("SPI bus %d: END\n", s->bus); } }

/* Cleanup wrapper for SG_DEFER */
static void spi_cleanup(spi_t **s) { spi_end_fn(*s); }

void example_spi(void) {
    puts("--- example_spi ---");

    spi_t spi = { .bus = 1 };
    spi_begin(&spi);

    SG_DEFER(spi_cleanup) spi_t *guard = &spi;

    printf("doing SPI transaction on bus %d\n", spi.bus);
    /* spi_end_fn() called automatically at end of scope */
}

/* -------------------------------------------------------------------------
 * Example 5: Mutex pattern
 * ---------------------------------------------------------------------- */

/* Fake mutex for demo (no pthreads dependency) */
typedef struct { int locked; const char *name; } fake_mutex_t;
static void fake_mutex_lock(fake_mutex_t *m)   { m->locked = 1; printf("LOCK   %s\n", m->name); }
static void fake_mutex_unlock(fake_mutex_t *m) { m->locked = 0; printf("UNLOCK %s\n", m->name); }

/* Generate a cleanup function for fake_mutex_t */
SG_DEFINE_MUTEX_CLEANUP(fake_mutex_cleanup, fake_mutex_t, fake_mutex_unlock)

void example_mutex(void) {
    puts("--- example_mutex ---");

    fake_mutex_t mtx = { .name = "my_mutex" };
    fake_mutex_lock(&mtx);

    SG_DEFER(fake_mutex_cleanup) fake_mutex_t *guard = &mtx;

    puts("critical section");

    if (1) {
        puts("early return path – mutex still unlocked correctly");
        return; /* unlock happens automatically */
    }

    puts("normal path");
    /* unlock happens automatically */
}

/* -------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */
int main(void) {
    puts("=== cguard scope_guard examples ===\n");

    example_auto_free();
    puts("");
    example_auto_fclose();
    puts("");
    example_multi_resource("/etc/hostname", "/tmp/cguard_copy.txt");
    puts("");
    example_spi();
    puts("");
    example_mutex();

    puts("\n=== all done ===");
    return 0;
}
