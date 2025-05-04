#include "lcd.h"
extern void HAL_Delay(uint32_t time_delay);
extern void Delay_us(uint32_t time_delay);
void LCD_init(void)
{                       
	PCF8574_init();
	HAL_Delay(10);
	
	bl_state = BL_ON;
    data_value = 0x04;
    PCF8574_write(data_value);
    
    HAL_Delay(10);

	LCD_send(0x33, CMD);
	LCD_send(0x32, CMD);

    LCD_send((_4_pin_interface | _2_row_display | _5x7_dots), CMD);         
    LCD_send((display_on | cursor_off | blink_off), CMD);     
    LCD_send((clear_display), CMD);         
    LCD_send((cursor_direction_inc | display_no_shift), CMD);        
}   

void LCD_toggle_EN(void)
{
    data_value |= 0x04;
    PCF8574_write(data_value);
    Delay_us(dly);
    data_value &= 0xF9;
    PCF8574_write(data_value);
    Delay_us(dly);
}
   
void LCD_send(unsigned char value, unsigned char mode)
{
    switch(mode)
    {
       case CMD:
       {
            data_value &= 0xF4;
            break;
       }
       case DAT:
       {
           data_value |= 0x01;
           break;
       }
    }
    
    switch(bl_state)
    {
       case BL_ON:
       {
          data_value |= 0x08;
          break;
       }
       case BL_OFF:
       {
          data_value &= 0xF7;
          break;
       }
    }
    
    PCF8574_write(data_value);
    LCD_4bit_send(value);
    Delay_us(dly);
}
    

void LCD_4bit_send(unsigned char lcd_data)       
{
    unsigned char temp = 0x00;

    temp = (lcd_data & 0xF0);
    data_value &= 0x0F;
    data_value |= temp;
    PCF8574_write(data_value);
    LCD_toggle_EN();

    temp = (lcd_data & 0x0F);
    temp <<= 0x04;
    data_value &= 0x0F;
    data_value |= temp;
    PCF8574_write(data_value);
    LCD_toggle_EN();
}  


void I2C_LCD_Puts(char *lcd_string)
{
    do
    {
        LCD_Write_Chr(*lcd_string++);
    }while(*lcd_string != '\0') ;
}


void LCD_Write_Chr(char char_data)
{
    if((char_data >= 0x20) && (char_data <= 0x7F))
	{
	    LCD_send(char_data, DAT);
	}
}


void I2C_LCD_Clear(void)
{
    LCD_send(clear_display, CMD);
    LCD_send(goto_home, CMD);
}


void LCD_SetCursor(unsigned char line_x, unsigned char chr_x)
{                                                   
      uint8_t pos_Addr;
	if(line_x == 0) 
	{
		pos_Addr = 0;
	}
	else if (line_x ==1)
	{
		pos_Addr = 0x40;
	}
	else if (line_x ==2)
	{
		pos_Addr = 0x14;
	}
	else 
	{
		pos_Addr = 0x54;
	}
	pos_Addr += chr_x;    
        LCD_send((0x80|pos_Addr), CMD);
}