#ifndef __DS1307_H
#define __DS1307_H 

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_i2c.h"
#include "stm8s_clk.h"

#define DS1307_REG_SECOND 	0x00
#define DS1307_REG_MINUTE 	0x01
#define DS1307_REG_HOUR  	0x02
#define DS1307_REG_DOW    	0x03
#define DS1307_REG_DATE   	0x04
#define DS1307_REG_MONTH  	0x05
#define DS1307_REG_YEAR   	0x06
#define DS1307_REG_CONTROL 	0x07

typedef struct {
   uint8_t seconds;   //Seconds,          00-59
   uint8_t minutes;   //Minutes,          00-59
   uint8_t hours;      //Hours,         00-23
   uint8_t day;      //Day in a week,    1-7
   uint8_t date;      //Day in a month   1-31
   uint8_t month;      //Month,         1-12
   uint8_t year;      //Year            00-99
} DS1307_Time_t;

void DS1307_Init(void);

unsigned char DS1307_GetSeconds(void);
unsigned char DS1307_GetMinutes(void);
unsigned char DS1307_GetHours(void);
unsigned char DS1307_GetDay(void);
unsigned char DS1307_GetDate(void);
unsigned char DS1307_GetMonth(void);
unsigned char DS1307_GetYear(void);

void DS1307_SetDayOfWeek(uint8_t dow);
void DS1307_SetDate(uint8_t date);
void DS1307_SetMonth(uint8_t month);
void DS1307_SetYear(uint8_t year);
void DS1307_SetHour(uint8_t hour_24mode);
void DS1307_SetMinute(uint8_t minute);
void DS1307_SetSecond(uint8_t second);

unsigned char read_ds1307(unsigned char regadd);
void write_ds1307(uint8_t regAddr, uint8_t val);
   
void DS1307_GetDateTime(DS1307_Time_t* time);

void DS1307_SetTime(DS1307_Time_t* time);

#endif /* __STM8L_GPIO_H */