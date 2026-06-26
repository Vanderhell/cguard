#ifndef CGUARD_RESULT_H
#define CGUARD_RESULT_H

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define CGUARD_THREAD_LOCAL __declspec(thread)
#else
#define CGUARD_THREAD_LOCAL _Thread_local
#endif

#if defined(__clang__) || defined(__GNUC__)
#define CGUARD_RESULT_UNUSED __attribute__((unused))
#else
#define CGUARD_RESULT_UNUSED
#endif

typedef enum result_status_t {
    RESULT_OK = 0,
    RESULT_ERR_GENERIC = -1,
    RESULT_ERR_NULL = -2,
    RESULT_ERR_NOMEM = -3,
    RESULT_ERR_IO = -4,
    RESULT_ERR_INVAL = -5,
    RESULT_ERR_TIMEOUT = -6,
    RESULT_ERR_OVERFLOW = -7,
    RESULT_ERR_NOT_FOUND = -8,
    RESULT_ERR_PERMISSION = -9,
    RESULT_ERR_BUSY = -10,
    RESULT_ERR_USER_BASE = -1000
} result_status_t;

typedef struct result_t {
    result_status_t status;
    const char *message;
} result_t;

static inline result_t result_success(void) {
    return (result_t){ RESULT_OK, NULL };
}

static inline result_status_t result_status_error(result_status_t status) {
    return status == RESULT_OK ? RESULT_ERR_GENERIC : status;
}

static inline result_t result_failure(result_status_t status, const char *message) {
    return (result_t){ result_status_error(status), message };
}

static inline bool result_is_ok(result_t value) {
    return value.status == RESULT_OK;
}

#define RESULT_IS_OK(value) (result_is_ok((value)))

#define RESULT_TRY(expr)                                                      \
    do {                                                                      \
        result_t _cguard_result = (expr);                                     \
        if (!RESULT_IS_OK(_cguard_result)) {                                  \
            return _cguard_result;                                            \
        }                                                                     \
    } while (0)

#define RESULT_TRY_TYPE(type, expr)                                           \
    do {                                                                      \
        type _cguard_result = (expr);                                         \
        if (_cguard_result.status != RESULT_OK) {                             \
            return _cguard_result;                                            \
        }                                                                     \
    } while (0)

#define RESULT_GOTO(dst, expr, label)                                         \
    do {                                                                      \
        (dst) = (expr);                                                       \
        if (!RESULT_IS_OK(dst)) {                                             \
            goto label;                                                       \
        }                                                                     \
    } while (0)

#define RESULT_DEFINE(name, value_type)                                       \
    typedef struct name {                                                     \
        result_status_t status;                                               \
        const char *message;                                                  \
        value_type value;                                                     \
    } name;                                                                   \
    static inline CGUARD_RESULT_UNUSED name name##_ok(value_type value) {     \
        name result_value = (name){ 0 };                                      \
        result_value.status = RESULT_OK;                                      \
        result_value.value = value;                                           \
        return result_value;                                                  \
    }                                                                         \
    static inline CGUARD_RESULT_UNUSED name name##_err(result_status_t status,\
                                                       const char *message) { \
        name result_value = (name){ 0 };                                      \
        result_value.status = result_status_error(status);                    \
        result_value.message = message;                                       \
        return result_value;                                                  \
    }                                                                         \
    static inline CGUARD_RESULT_UNUSED bool name##_is_ok(name value) {        \
        return value.status == RESULT_OK;                                     \
    }                                                                         \
    typedef int name##_cguard_result_define_anchor

static inline const char *result_status_string(result_status_t status) {
    switch (status) {
    case RESULT_OK:
        return "ok";
    case RESULT_ERR_GENERIC:
        return "generic error";
    case RESULT_ERR_NULL:
        return "null pointer";
    case RESULT_ERR_NOMEM:
        return "out of memory";
    case RESULT_ERR_IO:
        return "I/O error";
    case RESULT_ERR_INVAL:
        return "invalid argument";
    case RESULT_ERR_TIMEOUT:
        return "timeout";
    case RESULT_ERR_OVERFLOW:
        return "overflow";
    case RESULT_ERR_NOT_FOUND:
        return "not found";
    case RESULT_ERR_PERMISSION:
        return "permission denied";
    case RESULT_ERR_BUSY:
        return "busy";
    case RESULT_ERR_USER_BASE:
        return "user-defined";
    default:
        return "unknown";
    }
}

static inline result_status_t result_status_from_errno(int errnum) {
    switch (errnum) {
    case 0:
        return RESULT_OK;
    case ENOMEM:
        return RESULT_ERR_NOMEM;
    case EINVAL:
        return RESULT_ERR_INVAL;
    case EIO:
        return RESULT_ERR_IO;
    case ENOENT:
        return RESULT_ERR_NOT_FOUND;
    case EACCES:
#ifdef EPERM
    case EPERM:
#endif
        return RESULT_ERR_PERMISSION;
    case EBUSY:
        return RESULT_ERR_BUSY;
    default:
        return RESULT_ERR_GENERIC;
    }
}

static inline const char *result_errno_message_into(char *buffer, size_t size,
                                                    int errnum) {
    if (buffer == NULL || size == 0) {
        return NULL;
    }

#if defined(_MSC_VER)
    if (strerror_s(buffer, size, errnum) != 0) {
        (void)snprintf(buffer, size, "errno %d", errnum);
    }
#else
    const char *source = strerror(errnum);
    if (source == NULL) {
        source = "unknown errno";
    }

    (void)snprintf(buffer, size, "%s", source);
#endif
    buffer[size - 1] = '\0';
    return buffer;
}

static inline result_t result_from_errno_buffer(int errnum, char *buffer,
                                                size_t size) {
    result_status_t status = result_status_from_errno(errnum);
    if (status == RESULT_OK) {
        return result_success();
    }

    return result_failure(status, result_errno_message_into(buffer, size, errnum));
}

static inline result_t result_from_errno(int errnum) {
    static CGUARD_THREAD_LOCAL char message_buffer[128];
    return result_from_errno_buffer(errnum, message_buffer, sizeof(message_buffer));
}

#ifdef __cplusplus
}
#endif

#endif
