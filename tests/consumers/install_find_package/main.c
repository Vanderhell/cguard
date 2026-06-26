#include <cguard/cguard.h>

int main(void) {
    result_t value = result_success();
    return result_is_ok(value) ? 0 : 1;
}
