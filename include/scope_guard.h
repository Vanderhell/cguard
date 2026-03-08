/**
 * @file scope_guard.h
 * @brief Automatic resource cleanup via scope-bound guards (RAII for C)
 *
 * Provides deterministic cleanup of resources when a variable goes out of
 * scope, regardless of how the scope is exited (return, goto, early exit).
 *
 * Requires GCC or Clang with __attribute__((cleanup)) support.
 * Not supported on MSVC.
 *
 * @version 1.0.0
 * @license MIT
 */

#ifndef CGUARD_SCOPE_GUARD_H
#define CGUARD_SCOPE_GUARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Compiler support check
 * ---------------------------------------------------------------------- */

#if !defined(__GNUC__) && !defined(__clang__)
#  error "scope_guard.h requires GCC or Clang (__attribute__((cleanup)) support)"
#endif

/* -------------------------------------------------------------------------
 * Core macro
 * ---------------------------------------------------------------------- */

/**
 * @brief Attach a cleanup function to a variable.
 *
 * The cleanup function is called automatically when the variable goes out
 * of scope. It receives a pointer to the variable as its single argument.
 *
 * @param func  Cleanup function with signature: void func(T *var)
 *
 * Example:
 * @code
 *   void my_free(void **p) { free(*p); }
 *   SG_DEFER(my_free) char *buf = malloc(256);
 * @endcode
 */
#define SG_DEFER(func) __attribute__((cleanup(func)))

/* -------------------------------------------------------------------------
 * Built-in cleanup functions
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Cleanup function for heap-allocated memory (free).
 * Use with: SG_DEFER(sg_free) void *ptr = malloc(...);
 */
static inline void sg_free(void *p) {
    void **pp = (void **)p;
    if (pp && *pp) {
        free(*pp);
        *pp = NULL;
    }
}

/**
 * @brief Cleanup function for FILE* (fclose).
 * Use with: SG_DEFER(sg_fclose) FILE *f = fopen(...);
 */
static inline void sg_fclose(FILE **f) {
    if (f && *f) {
        fclose(*f);
        *f = NULL;
    }
}

/* -------------------------------------------------------------------------
 * Convenience macros for common patterns
 * ---------------------------------------------------------------------- */

/** Auto-free heap pointer */
#define SG_AUTO_FREE    SG_DEFER(sg_free)

/** Auto-close FILE* */
#define SG_AUTO_FCLOSE  SG_DEFER(sg_fclose)

/* -------------------------------------------------------------------------
 * Generic / user-defined deferred calls
 * ---------------------------------------------------------------------- */

/**
 * @brief Declare a custom cleanup handler inline using a lambda-like block.
 *
 * This macro creates a local cleanup variable that will call your code
 * when it goes out of scope. Useful for one-off cleanup without writing
 * a named function.
 *
 * @note Uses GCC nested functions (GCC only, not Clang).
 *       For portable one-off cleanup, write a small static inline function.
 *
 * Example (GCC only):
 * @code
 *   sg_mutex_lock(&mtx);
 *   SG_ON_EXIT { sg_mutex_unlock(&mtx); };
 * @endcode
 */
#ifdef __GNUC__
#  ifndef __clang__
#    define _SG_CONCAT_(a, b)    a##b
#    define _SG_CONCAT(a, b)     _SG_CONCAT_(a, b)
#    define _SG_UNIQUE(base)     _SG_CONCAT(base, __LINE__)

#    define SG_ON_EXIT \
         auto void _SG_UNIQUE(_sg_cleanup_fn_)(int *); \
         int _SG_UNIQUE(_sg_guard_) SG_DEFER(_SG_UNIQUE(_sg_cleanup_fn_)) = 0; \
         void _SG_UNIQUE(_sg_cleanup_fn_)(int *_sg_unused_)
#  endif
#endif

/* -------------------------------------------------------------------------
 * Mutex helpers (generic – supply your own lock/unlock types)
 * ---------------------------------------------------------------------- */

/**
 * @brief Macro to define a typed mutex cleanup function.
 *
 * Usage:
 * @code
 *   SG_DEFINE_MUTEX_CLEANUP(pthread_cleanup, pthread_mutex_t, pthread_mutex_unlock)
 *
 *   pthread_mutex_lock(&mtx);
 *   SG_DEFER(pthread_cleanup) pthread_mutex_t *guard = &mtx;
 * @endcode
 */
#define SG_DEFINE_MUTEX_CLEANUP(name, mutex_type, unlock_fn)   \
    static inline void name(mutex_type **m) {                  \
        if (m && *m) { unlock_fn(*m); }                        \
    }

/* -------------------------------------------------------------------------
 * Version
 * ---------------------------------------------------------------------- */

#define CGUARD_VERSION_MAJOR 1
#define CGUARD_VERSION_MINOR 0
#define CGUARD_VERSION_PATCH 0
#define CGUARD_VERSION_STR   "1.0.0"

#ifdef __cplusplus
}
#endif

#endif /* CGUARD_SCOPE_GUARD_H */
