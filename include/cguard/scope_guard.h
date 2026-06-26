#ifndef CGUARD_SCOPE_GUARD_H
#define CGUARD_SCOPE_GUARD_H

#ifndef CGUARD_SCOPE_GUARD_ENABLE
#define CGUARD_SCOPE_GUARD_ENABLE 1
#endif

#ifndef CGUARD_SCOPE_GUARD_REQUIRE
#define CGUARD_SCOPE_GUARD_REQUIRE 0
#endif

#if defined(__has_attribute)
#if __has_attribute(cleanup)
#define CGUARD_HAS_SCOPE_GUARD 1
#endif
#endif

#if !defined(CGUARD_HAS_SCOPE_GUARD)
#if defined(__GNUC__) && !defined(_MSC_VER)
#define CGUARD_HAS_SCOPE_GUARD 1
#else
#define CGUARD_HAS_SCOPE_GUARD 0
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__) || defined(__GNUC__)
#define CGUARD_SCOPE_GUARD_UNUSED __attribute__((unused))
#else
#define CGUARD_SCOPE_GUARD_UNUSED
#endif

#if CGUARD_SCOPE_GUARD_REQUIRE && !CGUARD_HAS_SCOPE_GUARD
#error "scope guard support is required but unavailable"
#endif

#if CGUARD_SCOPE_GUARD_ENABLE && CGUARD_HAS_SCOPE_GUARD

#define SG_DEFER(cleanup_fn) __attribute__((cleanup(cleanup_fn)))

#define SG_DISMISS(var)                                                       \
    do {                                                                     \
        (var) = 0;                                                           \
    } while (0)

#define SG_DEFINE_PTR_CLEANUP(name, pointer_type, release_fn)                 \
    static inline CGUARD_SCOPE_GUARD_UNUSED void name(pointer_type *resource) { \
        if (resource != NULL && *resource != NULL) {                          \
            release_fn(*resource);                                            \
            *resource = NULL;                                                 \
        }                                                                     \
    }

#endif

#define CGUARD_VERSION_MAJOR 2
#define CGUARD_VERSION_MINOR 0
#define CGUARD_VERSION_PATCH 0
#define CGUARD_VERSION_STR "2.0.0"

#ifdef __cplusplus
}
#endif

#endif
