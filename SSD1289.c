#include "SSD1289.h"
/*
 * LCD functions
 */
char *num2str(int num) {
	int i = 0, p = 0;
	char temp[11];
	for (i = 0; i < 10; i++) {
		temp[i] = ' ';
		str[i] = ' ';
	}
	temp[10] = '\0';
	if (num == 0) {
		str[0] = '0';
		str[1] = '\0';
		return str;
	}
	if (num < 0) {
		temp[0] = '-';
		num *= -1;
	} else
		temp[0] = ' ';
	i = 0;
	while (num > 0) {
		temp[9 - i] = (char) ((num % 10) + 48);
		num /= 10;
		i++;
	}
	for (i = 0; temp[i] != '\0'; i++) {
		if (temp[i] == ' ')
			continue;
		else {
			str[p] = temp[i];
			p++;
		}
	}
	str[p] = '\0';
	return str;
}

void LCD_delay_us(int delay)
{
	uint16_t i,us;
	for(i = 0;i<delay;i++)
	{
		us = 12;
		while (us--);
	}
}

void LCD_delay_ms(int delay)
{
	int i,j;
	for(i=0;i<delay;i++) {
		for(j=0;j<1000;j++)
			LCD_delay_us(1);
	}
}


void GPIO_WriteData(int port,unsigned short data)
{
	GPIO_SetValue(port,data&0x00FF);
	GPIO_ClearValue(port,(~data)&0x00FF);
}

void LCD_DrawRectangle(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2,unsigned int colour)
{
	unsigned short i,j;
	short dist_x,dist_y, inc_x,inc_y;
	info.foreground = colour;

	dist_x = x2-x1;
	dist_y = y2-y1;
	if(dist_x < 0) {
		inc_x=-1;
		dist_x*=-1;
	}
	else
		inc_x=1;
	if(dist_y < 0) {
		inc_y=-1;
		dist_y*=-1;
	}
	else
		inc_y=1;

	for(i=0;;) {
		for(j=0;;) {
			LCD_DrawDot(x1+i,y1+j);
			j+=inc_y;
			if(j<0) {
				if((j * -1) == dist_y)
					break;
			}
			else
				if(j == dist_y)
					break;
		}
		i+=inc_x;
		if(i<0) {
			if((i * -1) == dist_x)
				break;
		}
		else
			if(i == dist_y)
				break;

	}
}

#ifdef LAND_TIGERV2

	void write_LCD(unsigned short data,char ch) /* ch = CMD(command), ch =DATA(data) */
	{
		GPIO_SetDir(LCD_PORT,LCD_DATA,1); /*Set P2.0 to P2.7 as output pins */
		GPIO_SetValue(LCD_CNTRL,LCD_DIR); /* Set Buffer Direction to WR */
		GPIO_ClearValue(LCD_CNTRL,LCD_EN); /* Enable buffer outputs */
		GPIO_SetValue(LCD_CNTRL,LCD_LE); /* Latch D00-D07 Transparent */
		GPIO_WriteData(LCD_PORT,data&0x00FF); /* Writing LSB */
		GPIO_ClearValue(LCD_CNTRL,LCD_LE); /* Latch D00-D07 */
		GPIO_WriteData(LCD_PORT,(data>>8)&0x00FF); /* Writing MSB */
		GPIO_ClearValue(LCD_CNTRL,CS); /* Select Device */
		switch(ch) {
			case CMD:
					GPIO_ClearValue(PORT0,RS); /* Select Control register */
					break;
			case DATA:
					GPIO_SetValue(PORT0,RS); /* Select Data register */
					break;
		}
		GPIO_ClearValue(LCD_CNTRL,WR); /* Start Writing */
		GPIO_SetValue(LCD_CNTRL,RD);
		GPIO_SetValue(LCD_CNTRL,WR); /*Stop writing */
		GPIO_SetValue(LCD_CNTRL,CS); /* de-select device */
	}

	unsigned short read_LCD(unsigned char reg,unsigned char ch)
	{
		uint32_t PORT_VAL;
		unsigned short MSB,LSB;
		unsigned short dat=0;

		switch(ch) {
		case CMD:
				write_LCD(reg,CMD);
				GPIO_ClearValue(LCD_CNTRL,RS); /* Select Control register */
				break;
		case DATA:
				GPIO_SetValue(LCD_CNTRL,RS); /* Select Data register */
				break;
		}
		GPIO_SetDir(LCD_PORT,LCD_DATA,0); /*Set P2.0 to P2.7 as input pins */
		GPIO_ClearValue(LCD_CNTRL,LCD_DIR); /* Set Buffer Direction to RD */
		GPIO_ClearValue(LCD_CNTRL,LCD_EN); /* Enable MSB buffer outputs */
		GPIO_ClearValue(LCD_CNTRL,CS); /* Select Device */

		GPIO_ClearValue(LCD_CNTRL,RD); /* Start reading data */
		GPIO_SetValue(LCD_CNTRL,WR);
		PORT_VAL = GPIO_ReadValue(LCD_PORT); /*Read PORT2 MSB value */
		MSB = (unsigned short)PORT_VAL&LCD_DATA;
		GPIO_SetValue(LCD_CNTRL,LCD_EN); /* Enable buffer outputs */
		PORT_VAL = GPIO_ReadValue(LCD_PORT); /*Read PORT2 LSB value */
		LSB = (unsigned short)PORT_VAL&LCD_DATA;
		GPIO_SetValue(LCD_CNTRL,RD); /* Stop reading */
		GPIO_SetValue(LCD_CNTRL,CS); /* de-select device */
		dat = (MSB<<8)|LSB;
		return dat;
	}

#endif

void write_reg(unsigned char reg,unsigned short dat)
{
	write_LCD((unsigned short)reg,CMD);
	write_LCD(dat,DATA);
}

unsigned short read_reg(unsigned char reg)
{
	return read_LCD((unsigned short)reg,CMD);
}

void LCD_SetCursor(unsigned short hor,unsigned short ver)
{
	if(hor > 0x00F0) {
		hor = 0;
		ver = ver+1;
	}
	if(ver > 0x0140) {
		ver = ver%0x0140;
	}
	write_reg(R4E, hor);
	write_reg(R4F, ver);
	info.curr_y = ver;
	info.curr_x = hor;
}

void LCD_Clear(unsigned short colour)
{
    unsigned long i;

    i = 0x00F0 * 0X0140;
    write_reg(R44, 0 | (239 << 8));
    write_reg(R45, 0);
    write_reg(R46, 319);
    write_reg(R4E, 0);
    write_reg(R4F, 0);

    /*
     *	Write to GRAM
     */
    write_LCD(R22,CMD);
    while(i--)
    {
        write_LCD(colour,DATA);
    }
    info.background = colour;
    info.foreground = colour;
}

void LCD_SetWindow(unsigned short xBeg, unsigned short yBeg, unsigned short xEnd, unsigned short yEnd)
{
	write_reg(R44, xBeg | ((xEnd) << 8));
	write_reg(R45, yBeg);
    write_reg(R46, yEnd);
}

/*
 * This function draws a dot of colour - 'colour'
 */
void LCD_DrawDot(unsigned short x,unsigned short y)
{
	LCD_SetCursor(x,y);
	write_reg(R22,info.foreground);
}

void LCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    unsigned short i;
    short sIncX, sIncY, sDeltaX, sDeltaY;
    unsigned short usXerr = 0, usYerr = 0, usRow, usCol, usDistance;

    sDeltaX = x2 - x1;
    sDeltaY = y2 - y1;
    usRow = x1;
    usCol = y1;
    if(sDeltaX > 0)
    {
        sIncX = 1;
    }
    else if(sDeltaX == 0)
    {
        sIncX = 0;
    }
    else
    {
        sIncX = -1;
        sDeltaX = -sDeltaX;
    }

    if(sDeltaY > 0)
    {
        sIncY = 1;
    }
    else if(sDeltaY == 0)
    {
        sIncY = 0;
    }
    else
    {
        sIncY = -1;
        sDeltaY = -sDeltaY;
    }

    if(sDeltaX > sDeltaY)
    {
        usDistance = sDeltaX;
    }
    else
    {
        usDistance = sDeltaY;
    }

    for(i = 0; i <= usDistance + 1; i++ )
    {
        LCD_DrawDot(usRow, usCol);
        usXerr += sDeltaX;
        usYerr += sDeltaY ;
        if(usXerr > usDistance)
        {
            usXerr -= usDistance;
            usRow += sIncX;
        }
        if(usYerr > usDistance)
        {
            usYerr -= usDistance;
            usCol += sIncY;
        }
    }
}

void LCD_init()
{
	unsigned short ID;
	GPIO_SetDir(LCD_CNTRL,CS|RS|WR|RD|LCD_DIR|LCD_EN|LCD_LE,1);	/* Set CS,RS,WR,RD pins as output for uC */

	ID = read_reg(0x0000); //Read device ID -- 0x8989 for SSD1289
	if(ID == 0x8989)
	{
			//------------- Start Initial Sequence -------------//
		    write_reg(R00, 0x0001); // Start internal OSC.
		    write_reg(R01, 0x3B3F); // Driver output control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
		    write_reg(R02, 0x0600); // set 1 line inversion
		    //------------- Power control setup ----------------//
		    write_reg(R0C, 0x0007); // Adjust VCIX2 output voltage
		    write_reg(R0D, 0x0006); // Set amplitude magnification of VLCD63
		    write_reg(R0E, 0x3200); // Set alternating amplitude of VCOM
		    write_reg(0x1E, 0x00BB); // Set VcomH voltage
		    write_reg(0x03, 0x6A64); // Step-up factor/cycle setting
		    //------------- RAM position control ---------------//
		    write_reg(R0F, 0x0000); // Gate scan position start at G0.
		    write_reg(R44, 0xEF00); // Horizontal RAM address position
		    write_reg(R45, 0x0000); // Vertical RAM address start position
		    write_reg(R46, 0x013F); // Vertical RAM address end position
		    // ----------- Adjust the Gamma Curve ----------//
		    write_reg(R30, 0x0000);
		    write_reg(R31, 0x0706);
		    write_reg(R32, 0x0206);
		    write_reg(R33, 0x0300);
		    write_reg(R34, 0x0002);
		    write_reg(R35, 0x0000);
		    write_reg(R36, 0x0707);
		    write_reg(R37, 0x0200);
		    write_reg(R3A, 0x0908);
		    write_reg(R3B, 0x0F0D);
		    //----------------- Set VCOM OTP ---------------//
		    write_reg(R28, 0x0006);
		    //---------------- Turn On display -------------//
		    write_reg(R10, 0x0000); // Sleep mode off.
		    LCD_delay_ms(50); // Wait 50mS
		    write_reg(R11, 0x6070); // Entry mode setup. 262K type B,16-bit data bus only
		    write_reg(R07, 0x0033); // Display ON

		    LCD_delay_ms(100);

		    LCD_Clear(BLACK);
		    info.foreground = WHITE;

	}
}

void LCD_DrawCircle(unsigned short x, unsigned short y, unsigned short r)
{
    unsigned short a, b;
    short di;

    a = 0;
    b = r;
    di = 3 - (r << 1);
    while(a <= b)
    {
        LCD_DrawDot(x + b, y - a);
        LCD_DrawDot(x - a, y + b);
        LCD_DrawDot(x - b, y - a);
        LCD_DrawDot(x - a, y - b);
        LCD_DrawDot(x + b, y + a);
        LCD_DrawDot(x + a, y - b);
        LCD_DrawDot(x + a, y + b);
        LCD_DrawDot(x - b, y + a);
        a++;
        if(di < 0)
        	di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        LCD_DrawDot(x + a, y + b);
    }
}

void LCD_PutChar(unsigned char ucChar)
{

    unsigned char ucTemp, ucRow, t;

    LCD_SetWindow(info.curr_x, info.curr_y, info.curr_x + info.font_size/2 - 1, info.curr_y + info.font_size - 1);
    LCD_SetCursor(info.curr_x, info.curr_y);

    write_LCD(R22,CMD);

    for(ucRow = 0; ucRow < info.font_size; ucRow++)
    {
        if(info.font_size== 12)
        	ucTemp = ASCII_1206[ucChar][ucRow];
        else
        	ucTemp = ASCII_1608[ucChar][ucRow];
        for(t = 0; t < info.font_size / 2; t++)
        {
             if(ucTemp & 0x01)
                 write_LCD(info.foreground,DATA);
             else
            	 write_LCD(info.background,DATA);
           	 ucTemp >>= 1;
        }
    }
}

void LCD_DisplayOn(void)
{
	write_reg(R07,0x0033);
}

void LCD_DisplayOff(void)
{
	write_reg(R07,0x0000);
}

void LCD_PutString(char *str,char f_size,unsigned int colour)
{
	int i=0;
	info.foreground = colour;
	if(f_size == 16)
		info.font_size = f_size;
	else
		info.font_size = 12;
    while(str[i]!='\0') {
    	if(str[i] == '\n') {
    		info.curr_x = 0;
			info.curr_y+= info.font_size + 1;
			i++;
			continue;
    	}
		if(str[i] == '\t') {
			info.curr_x+=4*(info.font_size/2);
			if((info.curr_x + (info.font_size/2)) > 0x00F0) {
				info.curr_x-= 0x00F0;
				info.curr_y+= info.font_size + 1;
			}
			i++;
			continue;
		}
       	LCD_PutChar((char)(str[i]-32));
    	info.curr_x+= info.font_size/2;
    	/* Fold to next line */
    	if((info.curr_x+(info.font_size/2)) > 0x00F0) {
    		info.curr_x = 0;
    		info.curr_y+= info.font_size + 1;
    	}
    	i++;
    }
}

