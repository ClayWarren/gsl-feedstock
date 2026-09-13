#include <stdio.h>
#include <stdarg.h>

static int print_message(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int main(void) {
    return print_message("Verified header-declared vprintf: %d\n", 42) > 0 ? 0 : 1;
}
