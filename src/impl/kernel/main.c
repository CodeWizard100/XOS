#include "print.h"
#include "keyboard.h"

void kernel_main() {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome To XOS!\n");




    while (1) {
        keyboard_handler();
    }
}
