#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "You must use a ix86-elf compiler"
#endif

enum vga_color
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// Generate unique number for Back and Foreground color of VGA text.                // This is done by shifting the background value integer     // Light blue on light red would for example go like this    
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)         // (one of the integers in the vga_color enum) and then      // 0001 | 1001 << 4
{                                                                                   // use the bitwise OR operator on it to create a unique      // 0001 | 10010000
    return fg | bg << 4;                                                            // value for the combination of the colors.                  // 10001001
}                                                                                   

// Combines the color attribute with the character ASCII value.                     // Let's say we want to combine 'A' (1000001) with the color calculated above.
static inline uint16_t vga_entry(unsigned char uc, uint8_t color)                   // 01000001 | 10001001 << 8
{                                                                                   // 01000001 | 1000100100000000
    return (uint16_t) uc | (uint16_t) color << 8;                                   // 1000100101000001
}

// Counts length of a string by looping through it until it reaches a \0
size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;;u;

// Initialises the terminal
void terminal_initialize(void)
{
    terminal_row = 0;   // First, set the position of the cursor.
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);   // Pass in the colour using the function that makes the special bit that is 2 combined colors.
    terminal_buffer = (uint16_t*) 0xb8000;  // Set VGA mode to text mode.
    for (size_t y = 0; y < VGA_HEIGHT; y++) // Draw the screen vertically
    {
        for (size_t x = 0; x < VGA_WIDTH; x++) // Now horizontally
        {
            const size_t index = y * VGA_WIDTH + x; // idk tbh
            terminal_buffer[index] = vga_entry(' ', terminal_color); // Set the cell colour and text which is an empty space for now until you use the terminal_write() function in the main function.
        }
    }
}

// Puts the character with the colours at the location specified in the function arguments.
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

// Puts character in cell then move the cursor
void terminal_putchar(char c)
{
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size) // Writes string to the terminal the amount of times as the given size in the arguments
{
    for (size_t i = 0; i < size; i++) // Loops through string
        terminal_putchar(data[i]); // Puts the character at current byte of string
}

void terminal_writestring(const char* data) // Makes it easier for you by counting the length automatically by using the strlen() function.
{
    terminal_write(data, strlen(data));
}

void kernel_main(void) // play around
{
    terminal_initialize();
    terminal_writestring("Hello, World!\n");
}
