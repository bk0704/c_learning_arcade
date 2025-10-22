#include "shell.h"

int main(void) {
    shell_init();
    shell_loop();
    shell_teardown();
    return 0;
}
