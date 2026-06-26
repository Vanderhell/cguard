#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cguard/scope_guard.h>
#include <cguard/scope_guard_stdio.h>
#include <cguard/scope_guard_stdlib.h>

#if CGUARD_HAS_SCOPE_GUARD && CGUARD_SCOPE_GUARD_ENABLE

SG_DEFINE_FREE_CLEANUP(example_free_cleanup, char *)
SG_DEFINE_FCLOSE_CLEANUP(example_fclose_cleanup)

static FILE *cguard_fopen_read(const char *path) {
#if defined(_MSC_VER)
    FILE *file = NULL;
    if (fopen_s(&file, path, "r") != 0) {
        return NULL;
    }
    return file;
#else
    return fopen(path, "r");
#endif
}

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

static int copy_file(const char *source_path, const char *destination_path) {
    SG_DEFER(example_fclose_cleanup) FILE *source = cguard_fopen_read(source_path);
    if (source == NULL) {
        return -1;
    }

    SG_DEFER(example_fclose_cleanup) FILE *destination = cguard_fopen_write(destination_path);
    if (destination == NULL) {
        return -1;
    }

    SG_DEFER(example_free_cleanup) char *buffer = malloc(256);
    if (buffer == NULL) {
        return -1;
    }

    while (fgets(buffer, 256, source) != NULL) {
        if (fputs(buffer, destination) == EOF) {
            return -1;
        }
    }

    if (fflush(destination) != 0) {
        return -1;
    }

    if (fclose(destination) != 0) {
        SG_DISMISS(destination);
        return -1;
    }

    SG_DISMISS(destination);
    return 0;
}

int main(void) {
    char *value = malloc(16);
    if (value != NULL) {
        snprintf(value, 16, "%s", "cleanup");
    }

    {
        SG_DEFER(example_free_cleanup) char *guard = value;
        (void)guard;
    }

    return copy_file("/etc/hostname", "/tmp/cguard_copy.txt");
}

#else

int main(void) {
    puts("scope guards are unavailable in this build");
    return 0;
}

#endif
