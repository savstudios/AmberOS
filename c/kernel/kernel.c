#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong OS */
#if defined(__linux__)
#error "You are not using a cross compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 target. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
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

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

/* Calculate the length of a string */

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

/* Initialize the terminal

	Set some variables (terminal row, column and color);
	and set the VGA entry to an empty character.
*/

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/* Terminal Scroll Function */

/* Pseudocode:

	Our terminal buffer works like a giant table.
	[' ',' ',' ',' ',]
	To find the exact location, we multiply the VGA width by the row the terminal is at
	then we add the offset (the column the character is on).
	so for example, we will put 'a' in this table (seperated into multiple lines for
	extra clarity:
	['','','','','',
	'','','','a','']
	to calculate the index, we would need the row of the terminal (2) (and subtract
	one from the number), and the column, (4).

	All together, if our VGA_WIDTH is 5, the calculation would be:
	1 * 5 + 4 = 9

	Pseudocode:

	1. Gather the top-line from the buffer.
	2. Clear the top-line.
	3. Gather the 2nd line text from the buffer.
	4. Copy the text onto the top-line.
	5. Clear the 2nd line.
	6. Continue.
*/

static inline uint16_t terminal_returnchar(size_t x, size_t y){

	return terminal_buffer[y * VGA_WIDTH + x];

}

void terminal_scroll(){

	for(int y = 0; y < VGA_HEIGHT; y++){
		for(int x = 0; x < VGA_WIDTH; x++){
			size_t index = y * VGA_WIDTH + x;
			/* check if entry is row 0 (top row) */
			if(y == 0){
				/* clear the row */
				terminal_buffer[index] = vga_entry(' ', terminal_color);
			} else {
				/* gather all the characters from the line */
				char char_at_index = terminal_returnchar(x, y);
				/* move the text into the top line */
				terminal_buffer[index - 80] = vga_entry(char_at_index, terminal_color);
				/* clear the row */
				terminal_buffer[index] = vga_entry(' ', terminal_color);
			}
		}
	}

}

void terminal_putchar(char c)
{

	/* Check if the character is a newline character (\n) */

	if (c == '\n') {
		/* If it is, we will just reset the column and increase the terminal row */
		++terminal_row;
		terminal_column = 0;
	}
	/* If not, we continue like always */
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}

}

void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

void kernel_main(void)
{
	/* Initialize terminal */
	terminal_initialize();

	terminal_writestring("Hello, Kernel World!\nIt's a nice day.");
}
