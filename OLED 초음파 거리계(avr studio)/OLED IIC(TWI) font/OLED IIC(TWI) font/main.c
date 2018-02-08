#define F_CPU 16000000UL
#define __PROG_TYPES_COMPAT__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#define add 0x78


const prog_uchar font [][20] = {
{0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0x38, 0x18, 0x88, 0xF8, 0xF0, 0x0F, 0x1F, 0x1F, 0x13, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},
{0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0XF8, 0X38, 0X18, 0x02, 0x12, 0x1B, 0x1D, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00},
{0x00, 0x00, 0xE0, 0xF0, 0xF0, 0x38, 0x88, 0xF8, 0xF8, 0x70, 0x10, 0x18, 0x0C, 0x1E, 0x1E, 0x1B, 0x19, 0x18, 0x08, 0x00},
{0x00, 0x00, 0x00, 0x70, 0x70, 0xB8, 0x88, 0xC8, 0x78 ,0x78, 0x0E, 0x1E, 0x16, 0x10, 0x18, 0x1C, 0x1F, 0x0F, 0x07, 0x00},
{0x00, 0x00, 0x00, 0xC0, 0xE0, 0x78, 0x38, 0x98, 0xC8, 0x40, 0x04, 0x06, 0x13, 0x1F, 0x1C, 0x1E, 0x0F, 0x05, 0x04, 0x00},
{0x00, 0x00, 0x00, 0xC0, 0xF0, 0xB8, 0x98, 0x98, 0x18, 0x08, 0x0C, 0x1C, 0x14, 0x10, 0x18, 0x1C, 0x1F, 0x0F, 0x07, 0x00},
{0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0x98, 0x88, 0x38, 0x38, 0x00, 0x0F, 0x1F, 0x1F, 0x13, 0x1D, 0x1F, 0x0F, 0x07, 0x00},
{0x00, 0x00, 0x00, 0x20, 0x38, 0x98, 0xD0, 0xF0, 0x30, 0x18, 0x00, 0x00, 0x38, 0x3E, 0x3F, 0x07, 0x01, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x70, 0xF0, 0xF8, 0xC8, 0xF8, 0x78, 0x30, 0x0E, 0x1F, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x0F, 0x07, 0x00},
{0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0xC8, 0xF8, 0xF8, 0xF0, 0x0C, 0x1C, 0x1C, 0x11, 0x19, 0x0D, 0x0F, 0x07, 0x03, 0x00}
};


void write(uint8_t com)
{
	TWCR=0xA4;	//Send START
	while (!(TWCR & (1<<TWINT)));

	TWDR=add;	//slave address + R/W
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));
	
	TWDR=0x00;	//command 모드
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));

	TWDR=com;	//data 출력
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));

	TWCR=0x94;	//Send STOP
}

void data_write(uint8_t data)
{
	TWCR=0xA4;	//Send START
	while (!(TWCR & (1<<TWINT)));

	TWDR=add;	//slave address + R/W
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));
	
	TWDR=0x40;	//data 모드
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));

	TWDR=data;	//data 출력
	TWCR=0x84;
	while (!(TWCR & (1<<TWINT)));

	TWCR=0x94;	//Send STOP
}


void draw_font(int x, int y, int ch)
{
	unsigned int i, j, k =0;
	unsigned int high, low;

	high = x / 16;
	low = x % 8;

	for(i=0; i<2; i++)
	{
		write(0xb0+i+y);
		write(0x00+low);
		write(0x18+high);
		write(0xA7);
		for(j=0; j < 10; j++)
		{
			data_write(pgm_read_byte(&font[ch][k++]));
		}
	}
}


void clear(void)
{
	unsigned int i, j;

	for(i=0; i<4; i++)
	{
		write(0xb0+i);
		write(0x00);
		write(0x10);
		for(j=0; j < 128; j++)
		{
			data_write(0x00);
		}
	}
}


void oled_init(void)
{
	write(0xAE);   //display off
	write(0x20); //Set Memory Addressing Mode
	write(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	write(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
	write(0xc8); //Set COM Output Scan Direction
	write(0x00);//---set low column address
	write(0x10);//---set high column address
	write(0x40);//--set start line address

	write(0x81);//--set contrast control register*
	write(0xFF);

	write(0xa1);//--set segment re-map 0 to 127
	write(0xa6);//--set normal display
	write(0xa8);//--set multiplex ratio(1 to 64)
	write(0x1F);//
	write(0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	write(0xd3);//-set display offset
	write(0x00);//-not offset
	write(0xd5);//--set display clock divide ratio/oscillator frequency
	write(0xf0);//--set divide ratio
	write(0xd9);//--set pre-charge period

	write(0x22); //
	write(0xda);//--set com pins hardware configuration
	write(0x02);
	write(0xdb);//--set vcomh

	write(0x20);//0x20,0.77xVcc
	write(0x8d);//--set DC-DC enable
	write(0x14);//
	write(0xaf);//--turn on oled panel
}

int main(void)
{
	DDRC = 0xFF;
	TWBR = 12;	//(CPU_Clock/SCL_Clock) - 16)/2 = TWBR*(4^TWPS)
	TWSR = 0x00;

	oled_init();
	_delay_ms(50);

	clear();

	while(1)
	{
		int k;

		for(k=0; k<10; k++)
		{
			draw_font(32,1,k);
			draw_font(48,1,k);
			draw_font(64,1,k);
			draw_font(80,1,k);
			_delay_ms(1000);
		}
	}
}