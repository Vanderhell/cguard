#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cguard/result.h>

RESULT_DEFINE(example_int_result_t, int);

static FILE *cguard_fopen_write(const char *path) {
#if defined(_MSC_VER)
    FILE *file = NULL;
    if (fopen_s(&file, path, "w") != 0) {
        return NULL;
    }
    return file;
#else
    return fopen(path, "w");
#endif
}

static example_int_result_t parse_positive_int(const char *text) {
    if (text == NULL) {
        return example_int_result_t_err(RESULT_ERR_NULL, "input is null");
    }

    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || value <= 0) {
        return example_int_result_t_err(RESULT_ERR_INVAL, "expected a positive integer");
    }

    return example_int_result_t_ok((int)value);
}

static result_t write_message(const char *path, const char *message) {
    FILE *file = cguard_fopen_write(path);
    if (file == NULL) {
        return result_failure(RESULT_ERR_IO, "open failed");
    }

    if (fputs(message, file) == EOF) {
        fclose(file);
        return result_failure(RESULT_ERR_IO, "write failed");
    }

    if (fclose(file) != 0) {
        return result_failure(RESULT_ERR_IO, "close failed");
    }

    return result_success();
}

int main(void) {
    example_int_result_t parsed = parse_positive_int("42");
    if (example_int_result_t_is_ok(parsed)) {
        printf("parsed value: %d\n", parsed.value);
    }

    result_t written = write_message("/tmp/cguard_result.txt", "hello from cguard\n");
    if (!result_is_ok(written)) {
        fprintf(stderr, "write failed: %s (%s)\n", result_status_string(written.status), written.message);
        return 1;
    }

    return 0;
}
