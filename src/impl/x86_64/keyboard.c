#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "print.h"

#define KBD_PORT 0x60
#define SCANCODE_ENTER 0x1C
#define SCANCODE_BACKSPACE 0x0E
#define SCANCODE_A 0x1E
#define KEY_RELEASED_MASK 0x80
#define BUFFER_SIZE 256
#define MAX_BUFFER_LENGTH 512
#define MAX_TOKENS 10  // Maximum number of tokens to return
#define MAX_TOKEN_SIZE 256  // Maximum size for each token

int atoi(const char *str) {
    int result = 0;
    int sign = 1;
    
    // Skip leading whitespace (if any)
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\v' || *str == '\f' || *str == '\r') {
        str++;
    }

    // Check for sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert the characters to integer
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}


// Function to split the string by whitespace
void split_whitespace(const char* str, char tokens[MAX_TOKENS][MAX_TOKEN_SIZE], size_t* num_tokens) {
    size_t i = 0, j = 0, token_index = 0;
    
    *num_tokens = 0;  // Initialize the number of tokens to 0

    while (str[i] != '\0' && token_index < MAX_TOKENS) {
        // Skip leading whitespace
        while (str[i] == ' ' || str[i] == '\t') {
            i++;
        }

        // Start of a new token
        j = 0;
        
        // Copy characters until whitespace or end of string
        while (str[i] != '\0' && str[i] != ' ' && str[i] != '\t' && j < MAX_TOKEN_SIZE - 1) {
            tokens[token_index][j++] = str[i++];
        }
        
        if (j > 0) {  // Only store tokens that have content
            tokens[token_index][j] = '\0';  // Null-terminate the current token
            token_index++;
        }

        // Skip over any remaining whitespace
        while (str[i] == ' ' || str[i] == '\t') {
            i++;
        }
    }

    *num_tokens = token_index;  // Set the number of tokens found
}

char input_buffer[BUFFER_SIZE];
size_t buffer_index = 0;

void delete_last_table_char() {
    if (buffer_index > 0) {
        buffer_index--;  // Move the index back by one to "remove" the last character
        input_buffer[buffer_index] = '\0';  // Null-terminate the string at the new end
    }
}

// Function to copy one string to another
void my_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Function to concatenate two strings
void my_strcat(char *dest, const char *src) {
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Function to calculate the length of a string
size_t my_strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return s - str;
}


int my_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

// Function to check if a buffer is equal to a string
bool strequal(const char buffer[BUFFER_SIZE], const char *str) {
    // Check if the length of the string is greater than the buffer
    if (my_strcmp(buffer, str) == 0) {
        return true; // The buffer and string are equal
    }
    return false; // The buffer and string are not equal
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}


void add_char_to_buffer(char c) {
    if (buffer_index < BUFFER_SIZE - 1) {  // Ensure there's room for the character and a null terminator
        input_buffer[buffer_index++] = c;
        input_buffer[buffer_index] = '\0'; // Null-terminate the string
    } else {
        // Handle buffer overflow if necessary
        print_str("Buffer is full!\n");
    }
}

// Function to reset the buffer
void reset_buffer() {
    memset(input_buffer, 0, BUFFER_SIZE);  // Clear the buffer
    buffer_index = 0;  // Reset the index
}

char tolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// State variables
static bool key_down[256] = {false};  // Track state for each key

char scancode_to_char(uint8_t scancode) {
    switch (scancode) {
        case 0x02: return '1'; 
        case 0x03: return '2'; 
        case 0x04: return '3'; 
        case 0x05: return '4'; 
        case 0x06: return '5'; 
        case 0x07: return '6'; 
        case 0x08: return '7'; 
        case 0x09: return '8'; 
        case 0x0A: return '9'; 
        case 0x0B: return '0'; 
        case 0x10: return 'Q'; 
        case 0x11: return 'W'; 
        case 0x12: return 'E'; 
        case 0x13: return 'R'; 
        case 0x14: return 'T'; 
        case 0x15: return 'Y'; 
        case 0x16: return 'U'; 
        case 0x17: return 'I'; 
        case 0x18: return 'O'; 
        case 0x19: return 'P'; 
        case 0x1E: return 'A'; 
        case 0x1F: return 'S'; 
        case 0x20: return 'D'; 
        case 0x21: return 'F'; 
        case 0x22: return 'G'; 
        case 0x23: return 'H'; 
        case 0x24: return 'J'; 
        case 0x25: return 'K'; 
        case 0x26: return 'L'; 
        case 0x2C: return 'Z'; 
        case 0x2D: return 'X'; 
        case 0x2E: return 'C'; 
        case 0x2F: return 'V'; 
        case 0x30: return 'B'; 
        case 0x31: return 'N'; 
        case 0x32: return 'M'; 
        case 0x39: return ' '; // Space key
        case 0x0F: return ' '; // Tab key
        default: return 0; // Return 0 for unknown scancodes
    }
}

void keyboard_handler() {
    uint8_t scancode = inb(KBD_PORT);
    bool is_key_release = (scancode & KEY_RELEASED_MASK) != 0;
    
    // Extract the actual scancode without the release bit
    if (is_key_release) {
        scancode &= ~KEY_RELEASED_MASK;
        key_down[scancode] = false;  // Key is released
    } else {
        if (!key_down[scancode]) {
            key_down[scancode] = true;  // Key is pressed down

            if (scancode == SCANCODE_ENTER) {
                char tokens[MAX_TOKENS][MAX_TOKEN_SIZE];
                size_t num_tokens;

                 split_whitespace(input_buffer, tokens, &num_tokens);

                print_str("\n");
                if (strequal(input_buffer, "cmd")) {
                    print_str("You Are Aleardy In Cmd\n");
                } else if (strequal(input_buffer, "clear")) {
                    print_clear();
                } else if (strequal(input_buffer, "quit")) {
                     __asm__ volatile (
        "mov $0x5307, %ax\n"   // Set AX to 0x5307 (shutdown function)
        "int $0x15"            // Call BIOS interrupt 15h
    );

            } else if (strequal(tokens[0],"echo")) {
                if (strequal(tokens[1],"")) {
                    print_str("Argument 1 Not Found\n");
                } else {
                    print_str(tokens[1]);
                    print_str("\n");
                }

                 } else if (strequal(tokens[0],"color")) {
                if (strequal(tokens[1],"")) {
                    print_str("Argument 1 Not Found\n");
                } else {
                   int token_value = atoi(tokens[1]);  // Convert string to integer

switch (token_value) {
    case 1:
        print_set_color(PRINT_COLOR_BLUE, PRINT_COLOR_BLACK);
        break;
    case 2:
        print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
        break;
    case 3:
        print_set_color(PRINT_COLOR_CYAN, PRINT_COLOR_BLACK);
        break;
    case 4:
        print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
        break;
    case 5:
        print_set_color(PRINT_COLOR_MAGENTA, PRINT_COLOR_BLACK);
        break;
    case 6:
        print_set_color(PRINT_COLOR_BROWN, PRINT_COLOR_BLACK);
        break;
    case 7:
        print_set_color(PRINT_COLOR_LIGHT_GRAY, PRINT_COLOR_BLACK);
        break;
    case 8:
        print_set_color(PRINT_COLOR_DARK_GRAY, PRINT_COLOR_BLACK);
        break;
    case 9:
        print_set_color(PRINT_COLOR_LIGHT_BLUE, PRINT_COLOR_BLACK);
        break;
    case 10:
        print_set_color(PRINT_COLOR_LIGHT_GREEN, PRINT_COLOR_BLACK);
        break;
    case 11:
        print_set_color(PRINT_COLOR_LIGHT_CYAN, PRINT_COLOR_BLACK);
        break;
    case 12:
        print_set_color(PRINT_COLOR_LIGHT_RED, PRINT_COLOR_BLACK);
        break;
    case 13:
        print_set_color(PRINT_COLOR_PINK, PRINT_COLOR_BLACK);
        break;
    case 14:
        print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
        break;
    case 15:
        print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
        break;
    default:
        print_str("Invalid color\n");
        break;
}
                }


                } else if (strequal(input_buffer, ""))  {

                


                
    
                     

    
    // mov al, 0xFE     ; Command for power off (varies by system)
    // out 0x64, al     ; Send command to system
                } else {
                             

                     char buffer[MAX_BUFFER_LENGTH];

    // Create the concatenated string: "No Such A Command: " + input_buffer + "\n"
    const char *prefix = "No Such A Command: ";
    const char *suffix = "\n";

    // Ensure the buffer is large enough
    if (my_strlen(prefix) + my_strlen(input_buffer) + my_strlen(suffix) >= MAX_BUFFER_LENGTH) {
        // Handle buffer overflow situation
        return;
    }

    // Copy the prefix into the buffer
    my_strcpy(buffer, prefix);

    // Concatenate the input_buffer into the buffer
    my_strcat(buffer, input_buffer);

    // Concatenate the suffix into the buffer
    my_strcat(buffer, suffix);

    // Print the result
    print_str(buffer);
                }
                reset_buffer();
            } else if (scancode == SCANCODE_BACKSPACE) {
                clear_lastchar();
                delete_last_table_char();
            } else {
            

                // Map scancodes to characters
               
                char c = tolower(scancode_to_char(scancode));
             
                if (c != 0) {
                    print_char(c);  // Print the character corresponding to the scancode
                    add_char_to_buffer(c);
                }
            }
        }
    }
}
