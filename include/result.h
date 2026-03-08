/**
 * @file result.h
 * @brief Lightweight result/error type for C (inspired by Rust's Result<T,E>)
 *
 * Provides a unified, readable pattern for returning success/failure from
 * functions, eliminating the chaos of mixed -1, NULL, errno, and custom enums.
 *
 * @version 1.0.0
 * @license MIT
 */

#ifndef CGUARD_RESULT_H
#define CGUARD_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Error codes
 * ---------------------------------------------------------------------- */

/**
 * @brief Standard result status codes.
 *
 * You can extend this with your own codes starting at RESULT_ERR_USER_BASE.
 */
typedef enum {
    RESULT_OK              =  0,   /**< Success */
    RESULT_ERR_GENERIC     = -1,   /**< Unspecified error */
    RESULT_ERR_NULL        = -2,   /**< Unexpected NULL */
    RESULT_ERR_NOMEM       = -3,   /**< Allocation failure */
    RESULT_ERR_IO          = -4,   /**< I/O error */
    RESULT_ERR_INVAL       = -5,   /**< Invalid argument */
    RESULT_ERR_TIMEOUT     = -6,   /**< Operation timed out */
    RESULT_ERR_OVERFLOW    = -7,   /**< Buffer / integer overflow */
    RESULT_ERR_NOT_FOUND   = -8,   /**< Resource not found */
    RESULT_ERR_PERMISSION  = -9,   /**< Permission denied */
    RESULT_ERR_BUSY        = -10,  /**< Resource busy */
    RESULT_ERR_USER_BASE   = -1000 /**< Start of user-defined codes */
} result_status_t;

/* -------------------------------------------------------------------------
 * Core result type
 * ---------------------------------------------------------------------- */

/**
 * @brief Generic result type carrying a status code and optional message.
 *
 * For functions that return only success/failure (no value):
 * @code
 *   result_t write_config(const char *path);
 *
 *   result_t r = write_config("cfg.ini");
 *   if (!r.ok) {
 *       fprintf(stderr, "Error: %s\n", r.msg);
 *       return r;
 *   }
 * @endcode
 */
typedef struct {
    bool            ok;      /**< true on success, false on error */
    result_status_t status;  /**< Status code (RESULT_OK or RESULT_ERR_*) */
    const char     *msg;     /**< Human-readable error message (may be NULL) */
} result_t;

/* -------------------------------------------------------------------------
 * Typed result macros  – RESULT(type)
 * ---------------------------------------------------------------------- */

/**
 * @brief Declare an anonymous struct carrying a value + result metadata.
 *
 * Use for functions that return both a value and a status:
 * @code
 *   RESULT(int) parse_int(const char *s);
 *
 *   RESULT(int) r = parse_int("42");
 *   if (!r.ok) handle_error(r.status);
 *   printf("parsed: %d\n", r.value);
 * @endcode
 *
 * @note Each RESULT(T) creates an anonymous struct, so they are not
 *       interchangeable across translation units unless typedef'd explicitly.
 */
#define RESULT(T) struct { bool ok; result_status_t status; const char *msg; T value; }

/* -------------------------------------------------------------------------
 * Constructor helpers
 * ---------------------------------------------------------------------- */

/** Create a successful result_t */
#define RESULT_OK_VAL()  ((result_t){ .ok = true,  .status = RESULT_OK, .msg = NULL })

/** Create a failed result_t with a status code */
#define RESULT_ERR_VAL(s)       ((result_t){ .ok = false, .status = (s), .msg = NULL })

/** Create a failed result_t with a status code and message */
#define RESULT_ERR_MSG(s, m)    ((result_t){ .ok = false, .status = (s), .msg = (m)  })

/** Create a successful typed result */
#define RESULT_VAL_OK(val) \
    { .ok = true, .status = RESULT_OK, .msg = NULL, .value = (val) }

/** Create a failed typed result (value is zero-initialized) */
#define RESULT_VAL_ERR(s, m) \
    { .ok = false, .status = (s), .msg = (m), .value = {0} }

/* -------------------------------------------------------------------------
 * Propagation helpers
 * ---------------------------------------------------------------------- */

/**
 * @brief Return early if result is not ok.
 *
 * @code
 *   result_t r = some_fn();
 *   RESULT_TRY(r);   // returns r if !r.ok
 * @endcode
 */
#define RESULT_TRY(r) do { if (!(r).ok) return (r); } while(0)

/**
 * @brief Execute a call and immediately propagate on error.
 *
 * @code
 *   RESULT_TRY_CALL(write_header(f));
 * @endcode
 */
#define RESULT_TRY_CALL(expr) do { result_t _r = (expr); RESULT_TRY(_r); } while(0)

/**
 * @brief Assert result is ok; if not, jump to a label.
 *
 * @code
 *   RESULT_GOTO(open_file(path), cleanup);
 * @endcode
 */
#define RESULT_GOTO(expr, label) \
    do { result_t _r = (expr); if (!_r.ok) goto label; } while(0)

/* -------------------------------------------------------------------------
 * Utility functions
 * ---------------------------------------------------------------------- */

/**
 * @brief Convert a result_status_t to a human-readable string.
 */
static inline const char *result_status_str(result_status_t s) {
    switch (s) {
        case RESULT_OK:             return "OK";
        case RESULT_ERR_GENERIC:    return "Generic error";
        case RESULT_ERR_NULL:       return "Unexpected NULL";
        case RESULT_ERR_NOMEM:      return "Out of memory";
        case RESULT_ERR_IO:         return "I/O error";
        case RESULT_ERR_INVAL:      return "Invalid argument";
        case RESULT_ERR_TIMEOUT:    return "Timeout";
        case RESULT_ERR_OVERFLOW:   return "Overflow";
        case RESULT_ERR_NOT_FOUND:  return "Not found";
        case RESULT_ERR_PERMISSION: return "Permission denied";
        case RESULT_ERR_BUSY:       return "Resource busy";
        default:                    return "Unknown error";
    }
}

/**
 * @brief Convert an errno value to a result_t.
 *
 * Useful for wrapping POSIX calls:
 * @code
 *   FILE *f = fopen(path, "r");
 *   if (!f) return result_from_errno(errno);
 * @endcode
 */
#include <errno.h>
static inline result_t result_from_errno(int e) {
    switch (e) {
        case 0:       return RESULT_OK_VAL();
        case ENOMEM:  return RESULT_ERR_MSG(RESULT_ERR_NOMEM,      strerror(e));
        case EINVAL:  return RESULT_ERR_MSG(RESULT_ERR_INVAL,      strerror(e));
        case EIO:     return RESULT_ERR_MSG(RESULT_ERR_IO,         strerror(e));
        case ENOENT:  return RESULT_ERR_MSG(RESULT_ERR_NOT_FOUND,  strerror(e));
        case EACCES:
        case EPERM:   return RESULT_ERR_MSG(RESULT_ERR_PERMISSION, strerror(e));
        case EBUSY:   return RESULT_ERR_MSG(RESULT_ERR_BUSY,       strerror(e));
        default:      return RESULT_ERR_MSG(RESULT_ERR_GENERIC,    strerror(e));
    }
}

#ifdef __cplusplus
}
#endif

#endif /* CGUARD_RESULT_H */
