#include "print.h"
#include <stdint.h>

uint8_t inb(uint16_t port);  // Function to read a byte from a port
void keyboard_handler(); // Function to print a string to the screen
