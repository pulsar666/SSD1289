#include "colors.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "FONT.H"


/* SELECT BOARD */
#define LAND_TIGERV2
//#define OTHER

/*
 * GLCD registers
 */
#define R00 0x00
#define R01 0x01
#define R02 0x02
#define R03 0x03
#define R04 0x04
#define R05 0X05
#define R06 0x06
#define R07 0x07
#define R08 0x08
#define R09 0x09
#define R0A 0x0A
#define R0B 0x0B
#define R0C 0x0C
#define R0D 0x0D
#define R0E 0x0E
#define R0F 0x0F
#define R10 0x10
#define R11 0x11
#define R12 0x12
#define R22 0x22
#define R28 0x28
#define R30 0x30
#define R31 0x31
#define R32 0x32
#define R33 0x33
#define R34 0x34
#define R35 0x35
#define R36 0x36
#define R37 0x37
#define R38 0x38
#define R39 0x39
#define R3A 0x3A
#define R3B 0x3B
#define R44 0x44
#define R45 0x45
#define R46 0x46
#define R4E 0x4E
#define R4F 0x4F

/*
 * HARDWARE PINS AND OTHER DEFS
 */
#define TIMER0 0
#define CMD '0'
#define DATA '1'
#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
#define PORT4 4

#define LCD_DATA 0xFF /* PORT 2 */
#define LCD_DIR (1<<21) /* PORT 0 */
#define LCD_EN (1<<19)	/* PORT 0 */
#define LCD_LE (1<<20)	/* PORT 0 */
#define CS (1<<22) /* PORT 0 */
#define RS (1<<23) /* PORT 0 */
#define WR (1<<24) /* PORT 0 */
#define RD (1<<25) /* PORT 0 */

/*
 * Select board or define LCD_PORT connections as per your board(OTHER)
 */
#ifdef LAND_TIGERV2
	#define LCD_PORT PORT2
	#define LCD_CNTRL PORT0
#else
	#ifdef OTHER
		#define LCD_PORT
		#define LCD_CNTRL
	#endif
#endif

struct LCD {
	unsigned short curr_x, curr_y, foreground, background;
	char font_size;
}info;



char str[12];
char *num2str(int num);


/*
 * Draw a rectangular box through size (arg1,arg2) to (arg3,arg4)
 * arg5 - Colour of box
 * First co-ordinate set is of the bottom left corner of the box
 */
void LCD_DrawRectangle(unsigned short,unsigned short,unsigned short,unsigned short,unsigned int);

/*
 * Write data to GPIO
 */
void GPIO_WriteData(int port,unsigned short data);



/*
 * Write data to LCD
 * Arg1 - Data
 * Arg2 - CMD/DATA (depending on whether it's data to GRAM or an instruction)
 */
void write_LCD(unsigned short,char);

/*
 * read data from LCD
 * arg1 - register address(refer to SSD1289 datasheet)
 * arg2 - command or data register
 */
unsigned short read_LCD(unsigned char,unsigned char);

/*
 * Directly write to register
 * arg1 - register
 * arg2 - data
 */
void write_reg(unsigned char,unsigned short);

/*
 * Read data from LCD register
 * arg - register address
 */
unsigned short read_reg(unsigned char);

/*
 * Set cursor to a particular pixel on LCD
 * arg1 - x coordinate
 * arg2 - y coordinate
 */
void LCD_SetCursor(unsigned short,unsigned short);

/*
 * Clear LCD with specified colour
 * arg - colour
 */
void LCD_Clear(unsigned short colour);

/*
 * Set window of specified area
 * arg1,arg2 - x,y coordinates of start
 * arg3,arg4 - x,y coordinates of end
 */
void LCD_SetWindow(unsigned short, unsigned short, unsigned short , unsigned short);

/*
 * Draw a single dot at arg1,arg2 position
 */

void LCD_DrawDot(unsigned short ,unsigned short );

/*
 * Draw a straight line from (arg1,arg2) to (arg3,arg4)
 */

void LCD_DrawLine(unsigned short, unsigned short, unsigned short, unsigned short);

/*
 * Initialize LCD
 */
void LCD_init();

/*
 * Draw circle with center at (arg1,arg2) and radius arg3
 */
void LCD_DrawCircle(unsigned short, unsigned short, unsigned short);

/*
 * Display on
 */
void LCD_DisplayOn(void);

/*
 * Display off
 */
void LCD_DisplayOff(void);

/*
 * Write a single character on screen at current x,y position
 */
void LCD_PutChar(unsigned char);

/*
 * Write a string of colour arg3, size arg2
 * arg1 specifies string
 */
void LCD_PutString(char *,char,unsigned int);


