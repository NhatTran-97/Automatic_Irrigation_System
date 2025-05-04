#include "STM8S.h"
#include "lcd.h"
#include "ds1307.h"
#include "stdio.h"
#include "stm8s_tim4.h"

unsigned char bl_state;
unsigned char data_value;
//////////
void GetKeyPressed(void);
static uint8_t keycode, keynum;
void scankey(void);
#define delaykey        100
unsigned char keyvalues[16] = {   '1',   '4',    '7',  '*',       // Values for each key in the 4x4
                                  '2',   '5',    '8',  '0',
                                  '3',   '6',    '9',  '#',
                                  'A',   'B',    'C',  'D' };
void Delay_us(uint32_t time_delay);
void HAL_Delay(uint32_t time_delay);
void clock_setup(void);
void GPIO_setup(void);
void WWDG_Config();
///////////////////////////////////////////////////////////
#define valve1_off		GPIO_WriteLow(GPIOC,GPIO_PIN_1)
#define valve1_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_1)
#define valve2_off		GPIO_WriteLow(GPIOC,GPIO_PIN_2)
#define valve2_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_2)
#define valve3_off		GPIO_WriteLow(GPIOC,GPIO_PIN_3)
#define valve3_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_3)
#define valve4_off		GPIO_WriteLow(GPIOC,GPIO_PIN_4)
#define valve4_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_4)
#define valve5_off		GPIO_WriteLow(GPIOC,GPIO_PIN_5)
#define valve5_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_5)
#define valve6_off		GPIO_WriteLow(GPIOC,GPIO_PIN_6)
#define valve6_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_6)
#define valve7_off		GPIO_WriteLow(GPIOC,GPIO_PIN_7)
#define valve7_on		GPIO_WriteHigh(GPIOC,GPIO_PIN_7)
#define valve8_off		GPIO_WriteLow(GPIOG,GPIO_PIN_0)
#define valve8_on		GPIO_WriteHigh(GPIOG,GPIO_PIN_0)

#define motor_off		GPIO_WriteLow(GPIOE,GPIO_PIN_5)
#define motor_on		GPIO_WriteHigh(GPIOE,GPIO_PIN_5)

#define Row1_low    		GPIO_WriteLow(GPIOB,GPIO_PIN_4)
#define Row1_high		GPIO_WriteHigh(GPIOB,GPIO_PIN_4)

#define Row2_low    		GPIO_WriteLow(GPIOB,GPIO_PIN_5)
#define Row2_high		GPIO_WriteHigh(GPIOB,GPIO_PIN_5)

#define Row3_low    		GPIO_WriteLow(GPIOB,GPIO_PIN_6)
#define Row3_high		GPIO_WriteHigh(GPIOB,GPIO_PIN_6)

#define Row4_low    		GPIO_WriteLow(GPIOB,GPIO_PIN_7)
#define Row4_high		GPIO_WriteHigh(GPIOB,GPIO_PIN_7)

#define Column1    		GPIO_ReadInputPin(GPIOB, GPIO_PIN_0)
#define Column2    		GPIO_ReadInputPin(GPIOB, GPIO_PIN_1)
#define Column3    		GPIO_ReadInputPin(GPIOB, GPIO_PIN_2)
#define Column4    		GPIO_ReadInputPin(GPIOB, GPIO_PIN_3)
/////////////////////////////////////////////////////////////////
#define add_8bytes_minute 			8  // bat dau tu vi tri thu 8
#define add_2bytes_auto_M_save 	                16 // bat dau tu vi tri thu 16
#define add_8bytes_dayofweek	 	        18 // bat dau tu vi tri thu 8
#define add_2bytes_alarm			 26 // bat dau tu vi tri thu 26, gio va phut hen gio
#define add_3bytes_count_steps			 28 // gia tri 16 bit dem so giay da chay, step nho vi tri van da chay
#define add_8bytes_valve_state			 31

void UART2_Config(void);
void USART_putc(volatile unsigned char c);
void sendUartFn();


void display_alarm(void);
void setup_time(void);
void number_press(void);
void setup_next(void);
void edit_timenow(void);
void edit_timenow_next(void);
void setup_alarm(void);
void off_all_valve(void);
void setup_alarm_next(void);
void process_dow_data(void);
void Save(void);
uint8_t data[30],array_receive[30];
char *DoW[]  = {"C.Nhat", "Thu:2 ","Thu:3 ","Thu:4 ","Thu:5 ","Thu:6 ","Thu:7 "};
uint8_t date,month,dow,hour,minute,second, year;
uint8_t keycode,mode_time=0,edit_time=0,setup_alarm_time=0;
static uint8_t alarm[2],dayofweek[8];		// luu 2 bytes hen gio-phut,  dayofweek 8bytes - byte thu 8 la dem so thu da cai dat
static uint8_t  temp[8],ckeck_data=0;							
static uint8_t  run_state=0; // trang thai dang chay =1, ko chay =0
static uint8_t	valve_number =0; // 8 steps cho 8 van, 
uint8_t valve_state[8]; // trang thai trung gian de dieu khien ra van
uint8_t valve_minute[8]; // thoi gian hen cho moi van    															// luu data 8bytes
uint8_t auto_manu_save[2]; // [0] =1 la hen gio, [0] =0 la nhan nut, [1] = 1 la co nho, [1] = 0 la khong nho.  // luu data 2bytes
uint8_t count_steps_read[3]; // 3 byte doc gia tri giay da dem, va so van da chay
uint8_t num_setup[25], position_cursor=0, keyD_Next=0;
uint8_t pre_second,setup_mode=0;
static uint16_t count_time_second; // dem thoi gian hen gio chay moi van
uint8_t datareceive;
static uint8_t countbyte=0;
char enable_receive = 0,check=0;
uint16_t count_time_ms;
uint16_t value;
uint8_t time_second=60;
///////////////////////////////////////////


INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
{
     datareceive = UART2_ReceiveData8();
     switch(datareceive)
	{		
        case '`':	//start characteractic
                enable_receive = 1; 
                countbyte=0;
	break;
            case 'x':	//stop
               countbyte=0;
               enable_receive = 0;  
	break;				
	default:  
              if (enable_receive ==0) break;
              if (enable_receive ==1) 
              {
                  array_receive[countbyte] = datareceive;         
                  countbyte++;
                  if(countbyte>=26) 
                  { 
                    check =1; countbyte=0;
                  }
              }
         break;
           }   
    /* Disable the UART2 Receive interrupt */
    UART2_ClearFlag(UART2_FLAG_RXNE);   
}

void writeRam(uint8_t Address, uint8_t len, uint8_t* data)
{
	for(int i=0; i<len ; i++)
	{
		write_ds1307(Address+i, data[i]);
	}
}
void readRam(uint8_t Address, uint8_t len, uint8_t* data)
{ 
	for(int i=0; i<len ; i++)
	{
		data[i] = read_ds1307(Address+i);
	}
}


void main(void)
{	clock_setup();
        ITC_DeInit(); 
        UART2_Config();
	GPIO_setup();
        LCD_init();
        I2C_LCD_Clear();
        
	//DS1307_Init();
    WWDG_Config();
    IWDG_ReloadCounter();        
        
        
	readRam( add_8bytes_minute, 8, valve_minute);  		// read address 0x08, length byte is 8		 
	readRam( add_2bytes_auto_M_save, 2, auto_manu_save); // alarm or press button to active motor
	readRam( add_8bytes_dayofweek, 8, dayofweek);  				
	readRam( add_2bytes_alarm, 2, alarm);   					
	readRam( add_3bytes_count_steps, 3, count_steps_read);   				// 2 bytes
	readRam( add_8bytes_valve_state, 8, valve_state );   				// 8 bytes	
         
	if(auto_manu_save[1] ==1){ // co nho
			count_time_second = count_steps_read[0]*256+count_steps_read[1]; // 16 bit count
			valve_number = count_steps_read[2]; // vi tri so van da cahy truoc do
			}
	else { count_time_second=0; valve_number=0;}
	
	for(int i =0;i<7;i++)	
	{if((dayofweek [i]<1)|(dayofweek [i]>7)) dayofweek [i] =' ';} 
			num_setup[17]=dayofweek[0]; num_setup[18]=dayofweek[1]; num_setup[19]=dayofweek[2]; 
			num_setup[20]=dayofweek[3]; num_setup[21]=dayofweek[4]; num_setup[22]=dayofweek[5]; num_setup[23]=dayofweek[6];	
		process_dow_data();
                
        
    while(1)
    {    /* USER CODE BEGIN 3 */
        IWDG_ReloadCounter(); // reset watchdog              
                
                if(check==1) // co data moi truyen qua uart
                {
                 if(( array_receive[0]=='S')&( array_receive[1]=='y')&( array_receive[2]=='n')) // tra loi 28byte cho ESP
                 {
                 USART_putc('`');
                 array_receive[0]= hour;array_receive[1]= minute;  array_receive[2]= second;array_receive[3]= dow;
                 array_receive[4]= date;array_receive[5]= month;  array_receive[6]= year;array_receive[7]= alarm[0];array_receive[8]= alarm[1];
                 array_receive[9]= dayofweek[0];array_receive[10]= dayofweek[1];array_receive[11]= dayofweek[2];
                 array_receive[12]= dayofweek[3];array_receive[13]= dayofweek[4];array_receive[14]= dayofweek[5];array_receive[15]= dayofweek[6];
                 array_receive[16]= auto_manu_save[0];array_receive[17]= auto_manu_save[1];
                 array_receive[18]=valve_minute[0];array_receive[19]=valve_minute[1];array_receive[20]=valve_minute[2];
                 array_receive[21]=valve_minute[3];array_receive[22]=valve_minute[4];array_receive[23]=valve_minute[5];
                 array_receive[24]=valve_minute[6];array_receive[25]=valve_minute[7];
                 
                     for(int k=0; k<26; k++)
                        {
                          IWDG_ReloadCounter(); // reset watchdog  
                          UART2_SendData8(array_receive[k]);
                        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
                        }
    
                 USART_putc('x');
                 
                 }
                  
                  
                DS1307_SetHour(array_receive[0]);
		DS1307_SetMinute(array_receive[1]);
                DS1307_SetSecond(array_receive[2]);
                DS1307_SetDayOfWeek(array_receive[3]);
                DS1307_SetDate(array_receive[4]);
                DS1307_SetMonth(array_receive[5]);
                DS1307_SetYear(array_receive[6]);
        alarm[0]=array_receive[7];alarm[1]=array_receive[8]; // gio va phut bat dau mo motor
		writeRam( add_2bytes_alarm, 2, alarm);
        dayofweek[0]=array_receive[9];dayofweek[1]=array_receive[10];dayofweek[2]=array_receive[11];  // 7 byte tu chu nhat den thu 7
        dayofweek[3]=array_receive[12];dayofweek[4]=array_receive[13];dayofweek[5]=array_receive[14];dayofweek[6]=array_receive[15];
                writeRam( add_8bytes_dayofweek, 8, dayofweek);	// luu thu trong tuan va gia tri so ngay da cai dat
        auto_manu_save[0]=array_receive[16]; auto_manu_save[1]=array_receive[17];
                writeRam( add_2bytes_auto_M_save, 2, auto_manu_save);
        valve_minute[0]=array_receive[18];valve_minute[1]=array_receive[19]; valve_minute[2]=array_receive[20];        
        valve_minute[3]=array_receive[21];valve_minute[4]=array_receive[22]; valve_minute[5]=array_receive[23];
        valve_minute[6]=array_receive[24];valve_minute[7]=array_receive[25]; 
                writeRam( add_8bytes_minute, 8, valve_minute);   
                while(1) // doi watchdog
                check =0; // chi thuc thi 1 lan
                
                }
           ////////////////////////////////////////////////////////////////////////      
                if(bl_state==1)        // man hinh dang sang thi dem nut
                {
                    count_time_ms++;
                    if((setup_mode!=0)|| (mode_time!=0)|| (edit_time!=0))
                      value =10000;
                      else value = 2000;
                      
                    if (count_time_ms>=value)
                    {
                      count_time_ms = 0;
                      setup_mode =0; mode_time =0; edit_time =0; setup_alarm_time=0;  // reset các bien khi tat man hinh
                      I2C_LCD_Clear(); bl_state = 0;
                      LCD_send((display_off), CMD);
                    }
                }
      ////////////////////////////////////////////////////////////////////////////////////
                
                date = DS1307_GetDate();
		month = DS1307_GetMonth();
		year = DS1307_GetYear();
		dow = DS1307_GetDay();
		hour = DS1307_GetHours();
		minute = DS1307_GetMinutes();
		second = DS1307_GetSeconds();

		/////////////////////////// read count every second
			if((pre_second !=second)&(run_state==1))
			{
				pre_second = second;  // previous second
				count_time_second++;
				
				if((auto_manu_save[1]==1)&(run_state==1)) { // luu lai thong tin theo thoi gian 10s khi dang chay
					count_steps_read[0] = count_time_second/256;
					count_steps_read[1] = count_time_second %256;
					count_steps_read[2] = valve_number;
					writeRam(add_3bytes_count_steps, 3, count_steps_read);
					writeRam(add_8bytes_valve_state, 8, valve_state );} // luu trang thai 8 valve
			}
		
		if(auto_manu_save[0]==1) // alarm is chose
			{ 	if((dow==dayofweek[0])|(dow==dayofweek[1])|(dow==dayofweek[2])|(dow==dayofweek[3])|(dow==dayofweek[4])|(dow==dayofweek[5])|(dow==dayofweek[6])) // day of week is match
						{	
							if((hour==alarm[0])& (minute==alarm[1])) run_state=1; // chi mo khi co dien
							 
						}	
			}
		
		if(run_state==1) // run motor and active the valves
			{       motor_on;
                  
				if(valve_number==0)
				{
					if((valve_minute[0]!=0)& (valve_state[0]==0)){valve_state[0]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[0]*time_second))&(valve_state[0]==1)){count_time_second=0;valve_state[0]=0; valve_number =1;}
					if(valve_minute[0]==0){valve_number =1;count_time_second=0;} // ko su dung valve 1		
				}
				else if(valve_number==1)
				{
					if((valve_minute[1]!=0)& (valve_state[1]==0)){valve_state[1]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[1]*time_second))&(valve_state[1]==1)){count_time_second=0;valve_state[1]=0; valve_number =2;}
					if(valve_minute[1]==0){valve_number =2;count_time_second=0;} // ko su dung valve 1		
				}
				else if(valve_number==2)
				{
					if((valve_minute[2]!=0)& (valve_state[2]==0)){valve_state[2]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[2]*time_second))&(valve_state[2]==1)){count_time_second=0;valve_state[2]=0; valve_number =3;}
					if(valve_minute[2]==0){valve_number =3;count_time_second=0;} // ko su dung valve 1		
				}
				else if(valve_number==3)
				{
					if((valve_minute[3]!=0)& (valve_state[3]==0)){valve_state[3]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[3]*time_second))&(valve_state[3]==1)){count_time_second=0;valve_state[3]=0; valve_number =4;}
					if(valve_minute[3]==0){valve_number =4;count_time_second=0;} // ko su dung valve 1		
				}				
				else if(valve_number==4)
				{
					if((valve_minute[4]!=0)& (valve_state[4]==0)){valve_state[4]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[4]*time_second))&(valve_state[4]==1)){count_time_second=0;valve_state[4]=0; valve_number =5;}
					if(valve_minute[4]==0){valve_number =5;count_time_second=0;} // ko su dung valve 1		
				}
                                else if(valve_number==5)
				{
					if((valve_minute[5]!=0)& (valve_state[5]==0)){valve_state[5]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[5]*time_second))&(valve_state[5]==1)){count_time_second=0;valve_state[5]=0; valve_number =6;}
					if(valve_minute[5]==0){valve_number =6;count_time_second=0;} // ko su dung valve 1		
				}
                                else if(valve_number==6)
				{
					if((valve_minute[6]!=0)& (valve_state[6]==0)){valve_state[6]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[6]*time_second))&(valve_state[6]==1)){count_time_second=0;valve_state[6]=0; valve_number =7;}
					if(valve_minute[6]==0){valve_number =7;count_time_second=0;} // ko su dung valve 1		
				}
				else if(valve_number==7)
				{
					if((valve_minute[7]!=0)& (valve_state[7]==0)){valve_state[7]=1; count_time_second=0;}
					if((count_time_second >= (valve_minute[7]*time_second))&(valve_state[7]==1)){count_time_second=0;valve_state[7]=0; valve_number =0;run_state=0;}
					if(valve_minute[7]==0){
					if(count_time_second==3) {valve_number =0;	
					count_time_second=0;run_state=0;}} // ko su dung valve 1		
				}
					if(valve_state[0]==1) valve1_on; else {if(count_time_second==2) valve1_off;}
					if(valve_state[1]==1) valve2_on; else {if(count_time_second==2) valve2_off;}	
					if(valve_state[2]==1) valve3_on; else {if(count_time_second==2) valve3_off;}	
					if(valve_state[3]==1) valve4_on; else {if(count_time_second==2) valve4_off;}
					if(valve_state[4]==1) valve5_on; else {if(count_time_second==2) valve5_off;}	
					if(valve_state[5]==1) valve6_on; else {if(count_time_second==2) valve6_off;}	
					if(valve_state[6]==1) valve7_on; else {if(count_time_second==2) valve7_off;}
					if(valve_state[7]==1) valve8_on; else {valve8_off;}
			}
                 if(run_state!=1) motor_off;       
/////////////////////////////////////////////////                 	
		if((mode_time==0)&(edit_time==0)&(setup_mode==0))
				{
				LCD_SetCursor(0,0);
				sprintf((char*)data,"Gio:    %0.2d:%0.2d:%0.2d    ",hour ,minute ,second );
				I2C_LCD_Puts((char*)data);	
				LCD_SetCursor(1,0);	
				I2C_LCD_Puts((char*)DoW[dow-1]);
				LCD_SetCursor(1,7);
				sprintf((char*)data,"%0.2d/%0.2d/20%0.2d", date, month , year );
				I2C_LCD_Puts((char*)data);	
				if(auto_manu_save[0] ==1) //  hen gio
					{ 
					display_alarm();  // display on row 2,3
					}
				else {//LCD_SetCursor(2,0); I2C_LCD_Puts("                    ");
					LCD_SetCursor(3,0); I2C_LCD_Puts("  Che Do Nhan nut   ");}					
				}
				
		if(ckeck_data!=0)	{	edit_timenow();} // error messages	
///////////////////////////////////////		
		  GetKeyPressed();
                 // if (keycode !=0) { LCD_SetCursor(2,5); LCD_Write_Chr(keycode);}           
       if ((keycode !=0)&(bl_state == 1))       // khi man hinh dang sang thi nhan nut duoc
       {        count_time_ms = 0; // reset bien tat man hinh
		if((mode_time==0)&(edit_time==0)&(keycode=='A')&(auto_manu_save[0]==0)&(setup_mode==0)) // tat mo bang tay
                        {     // LCD_send((display_off), CMD);                          
                                        run_state++; // nhat toggle 
                                        if(run_state>=2){run_state=0; off_all_valve();}
                        }
/////////////////////////////////////////////////	                                  
		if((keycode=='*')&(edit_time==0))  // trang thai thuong va ko o che do chinh gio thi ok
			{		setup_time();
                        
			}
		if((keycode!=0)&(keycode>=0x30)&(keycode< 0x3A)) // press number 0-9 
			{		number_press();
			}
		if((mode_time>0)&(keycode=='D')) // next for setup time 1-8
			{		setup_next();
			}
		/////////////////////////////////////////////////// real time clock setup
		if((keycode=='C')& (mode_time==0)&(setup_mode==0)) // call, ko o che do cai dat gio mo van (hen gio)
			{		edit_timenow(); 
			}
		if((edit_time>0)&(keycode=='D')) // next for setup time 1-8
			{		edit_timenow_next();
			}
		//////////////////////////////////////////////
		if((edit_time==0)&(mode_time==0)&(keycode=='B')) // chon che do bang tay hay hen gio
			{		setup_alarm(); 
			}
		if((setup_mode>0)&(keycode=='D'))
			{		setup_alarm_next();
			}
       }
           if ((bl_state == 0)&(keycode !=0)) // man hinh dang tat thi sang len khi nhan nut
                { bl_state = 1;
                  LCD_send((display_on), CMD);
                  count_time_ms = 0;  // sang lai man hinh
                } 
	//	HAL_Delay (10);
  /* USER CODE END 3 */           
    } 
}

//////////////
void display_alarm(void)
{
					LCD_SetCursor(2,0);sprintf((char*)data,"Gio Mo Motor  %0.2d:%0.2d", alarm[0], alarm[1]);I2C_LCD_Puts((char*)data);
					LCD_SetCursor(3,0); I2C_LCD_Puts("M-Thu:");  //6
					if(dayofweek[7]==1){sprintf((char*)data,     "%0.1d             ", dayofweek[0]);I2C_LCD_Puts((char*)data);}
					else if(dayofweek[7]==2){sprintf((char*)data,"%0.1d,%0.1d           ", dayofweek[0],dayofweek[1]);I2C_LCD_Puts((char*)data);}
					else if(dayofweek[7]==3){sprintf((char*)data,"%0.1d,%0.1d,%0.1d         ", dayofweek[0],dayofweek[1],dayofweek[2]);I2C_LCD_Puts((char*)data);}
					else if(dayofweek[7]==4){sprintf((char*)data,"%0.1d,%0.1d,%0.1d,%0.1d       ", dayofweek[0],dayofweek[1],dayofweek[2],dayofweek[3]);I2C_LCD_Puts((char*)data);}
				  else if(dayofweek[7]==5){sprintf((char*)data,"%0.1d,%0.1d,%0.1d,%0.1d,%0.1d     ", dayofweek[0],dayofweek[1],dayofweek[2],dayofweek[3],dayofweek[4]);I2C_LCD_Puts((char*)data);}
				  else if(dayofweek[7]==6){sprintf((char*)data,"%0.1d,%0.1d,%0.1d,%0.1d,%0.1d,%0.1d   ", dayofweek[0],dayofweek[1],dayofweek[2],dayofweek[3],dayofweek[4],dayofweek[5]);I2C_LCD_Puts((char*)data);}
				  else if(dayofweek[7]==7){sprintf((char*)data,"%0.1d,%0.1d,%0.1d,%0.1d,%0.1d,%0.1d,%0.1d ", dayofweek[0],dayofweek[1],dayofweek[2],dayofweek[3],dayofweek[4],dayofweek[5],dayofweek[6]);I2C_LCD_Puts((char*)data);}
					else {I2C_LCD_Puts("              ");}
}
void setup_time(void)
{
	mode_time++;
	position_cursor=0; // reset cursors postition
	keyD_Next=0; ;     //reset row cursors

	if(mode_time>=2){mode_time = 0;Save();}		
	if(mode_time!=0)
		{ 
		for(int i=0; i<8; i++){
			num_setup[i*2]=valve_minute[i]/10; // minute tens
			num_setup[(i*2)+1]=valve_minute[i]%10; // minute units
		}	 

		I2C_LCD_Clear();
		LCD_SetCursor(0,0);
		I2C_LCD_Puts("Cai Dat Phut Mo Van ");
		LCD_SetCursor(1,0);
		sprintf((char*)data,"V1:%0.2d' V2:%0.2d' V3:%0.2d'", valve_minute[0], valve_minute[1], valve_minute[2]); I2C_LCD_Puts((char*)data);
		LCD_SetCursor(2,0);
		sprintf((char*)data,"V4:%0.2d' V5:%0.2d' V6:%0.2d'", valve_minute[3], valve_minute[4], valve_minute[5]); I2C_LCD_Puts((char*)data);
		LCD_SetCursor(3,0);
		sprintf((char*)data,"V7:%0.2d' V8:%0.2d'", valve_minute[6], valve_minute[7]); I2C_LCD_Puts((char*)data);
		LCD_SetCursor(1,3); LCD_send((display_on|cursor_on | blink_on), CMD); //I2C_LCD_WriteCmd(CURSOR_ON);	
		}

}
void number_press(void)
{
	if(mode_time!=0) // edit setup time
		{ 
			if(keyD_Next==0) // fisrt row
				{	if(position_cursor==0){LCD_SetCursor(1,3);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=2) {position_cursor=0; LCD_SetCursor(1,3);}
				}
			 if(keyD_Next==1) // second row
				{	if(position_cursor==2){LCD_SetCursor(1,10);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=4)	{position_cursor=2;LCD_SetCursor(1,10);}
				}	
			 if(keyD_Next==2) // second row
				{	if(position_cursor==4){LCD_SetCursor(1,17);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=6) {position_cursor=4; LCD_SetCursor(1,17);}
				}	
			 if(keyD_Next==3) // second row
				{	if(position_cursor==6){LCD_SetCursor(2,3);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=8) {position_cursor=6; LCD_SetCursor(2,3);}
				}
				if(keyD_Next==4) // second row
				{	if(position_cursor==8){LCD_SetCursor(2,10);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=10)	{position_cursor=8;LCD_SetCursor(2,10);}
				}	
			 if(keyD_Next==5) // second row
				{	if(position_cursor==10){LCD_SetCursor(2,17);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=12) {position_cursor=10; LCD_SetCursor(2,17);}
				}	
			if(keyD_Next==6) // fisrt row
				{	if(position_cursor==12){LCD_SetCursor(3,3);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=14) {position_cursor=12; LCD_SetCursor(3,3);}
				}
			 if(keyD_Next==7) // second row
				{	if(position_cursor==14){LCD_SetCursor(3,10);}
					num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
					position_cursor++;
					if(position_cursor>=16)	{position_cursor=14;LCD_SetCursor(3,10);}
				}					
				
		}
		///////////////////////////////////////////
	if(edit_time!=0)
		{		 if(keyD_Next==0) // hour
						{	if(position_cursor==0){LCD_SetCursor(0,8);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=2)	{position_cursor=0; LCD_SetCursor(0,8);}
						}	
					if(keyD_Next==1) // minute
						{	if(position_cursor==2){LCD_SetCursor(0,11);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=4)	{position_cursor=2; LCD_SetCursor(0,11);}
						}
					if(keyD_Next==2) // day
						{	if(position_cursor==4){LCD_SetCursor(1,4);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=5)	{position_cursor=4; LCD_SetCursor(1,4);}
						}
					if(keyD_Next==3) // date
						{	if(position_cursor==5){LCD_SetCursor(1,7);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=7)	{position_cursor=5; LCD_SetCursor(1,7);}
						}
					if(keyD_Next==4) // month
						{	if(position_cursor==7){LCD_SetCursor(1,10);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=9)	{position_cursor=7; LCD_SetCursor(1,10);}
						}
					if(keyD_Next==5) // year
						{	if(position_cursor==11){LCD_SetCursor(1,15);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=13)	{position_cursor=11; LCD_SetCursor(1,15);}
						}
				//////////////////////////////////
					if(keyD_Next==6) // alarm hour
						{	if(position_cursor==13){LCD_SetCursor(2,14);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=15)	{position_cursor=13; LCD_SetCursor(2,14);}
						}			
					if(keyD_Next==7) // alarm hour
						{	if(position_cursor==15){LCD_SetCursor(2,17);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);
							position_cursor++;
							if(position_cursor>=17)	{position_cursor=15; LCD_SetCursor(2,17);}
						}
					if(keyD_Next==8) // alarm hour
						{	if(position_cursor==17){LCD_SetCursor(3,6);}
							num_setup[position_cursor]=keycode-48; LCD_Write_Chr(keycode);LCD_Write_Chr(',');	
						  if(position_cursor==23) {LCD_SetCursor(3,19);LCD_Write_Chr(' ');} // delete last comma ,
							position_cursor++;
							if(position_cursor>=24)	{position_cursor=17; LCD_SetCursor(3,6);}
						}						
		}			
}
void edit_timenow(void) // setup real time
{
		  edit_time++;
			position_cursor=0; // reset cursors postition
			keyD_Next=0; ;     //reset row cursors
			ckeck_data=0;
			////////////////////////////////////
			if(dow==1){LCD_SetCursor(1,0);	I2C_LCD_Puts("Thu:1  ");}  // change C.Nhat to Thu:1
			///////////////////////////////////
			if(edit_time==1){ // de hien thoi gian alarm len cai dat
					display_alarm();} 
			//////////////////////////////////
			if(edit_time>=2){edit_time = 0; // continue checking errror, if good it will reset to 0
					hour  = num_setup[0]*10+ num_setup[1];
					minute = num_setup[2]*10+ num_setup[3];
					dow   = num_setup[4];
					date  = num_setup[5]*10+ num_setup[6];
					month = num_setup[7]*10+ num_setup[8]; // 9, 10 du ko su dung
					year  = num_setup[11]*10+num_setup[12];
					alarm[0] = num_setup[13]*10+ num_setup[14];
					alarm[1] = num_setup[15]*10+ num_setup[16];
				
					 process_dow_data();
				 ////////////////////////////////////////////////////////////////////////

					if(hour>23) ckeck_data=1;
					else if(minute>59) ckeck_data=2;
					else if((dow<1)|(dow>7)) ckeck_data=3;
					else if((date<1)|(date>31)) ckeck_data=4;
					else if((month<1)|(month>12)) ckeck_data=5;
				  else if(year>99) ckeck_data=6;
					else if(alarm[0]>23) ckeck_data=7;
					else if(alarm[1]>59)	ckeck_data=8;
					else if((dayofweek[0]<1)|((dayofweek[0]>7)&(dayofweek[0]!=' '))) ckeck_data=9;
					else if((dayofweek[1]<1)|((dayofweek[1]>7)&(dayofweek[1]!=' '))) ckeck_data=9;	 
					else if((dayofweek[2]<1)|((dayofweek[2]>7)&(dayofweek[2]!=' '))) ckeck_data=9;
					else if((dayofweek[3]<1)|((dayofweek[3]>7)&(dayofweek[3]!=' '))) ckeck_data=9;	
					else if((dayofweek[4]<1)|((dayofweek[4]>7)&(dayofweek[4]!=' '))) ckeck_data=9;
					else if((dayofweek[5]<1)|((dayofweek[5]>7)&(dayofweek[5]!=' '))) ckeck_data=9;
					else if((dayofweek[6]<1)|((dayofweek[6]>7)&(dayofweek[6]!=' '))) ckeck_data=9;
		
							else {DS1307_SetDate(date);
										DS1307_SetMonth(month);
										DS1307_SetYear(year);
										DS1307_SetDayOfWeek(dow);
										DS1307_SetHour(hour);
										DS1307_SetMinute(minute);
                                                                                DS1307_SetSecond(00);
										writeRam( add_8bytes_dayofweek, 8, dayofweek);	// luu thu trong tuan va gia tri so ngay da cai dat
										writeRam( add_2bytes_alarm, 2, alarm);
										LCD_send((display_on|cursor_off | blink_off), CMD); //I2C_LCD_WriteCmd(CURSOR_OFF); // off cursor to go back main screen
										I2C_LCD_Clear();
								}
							
				if(ckeck_data!=0){I2C_LCD_Clear(); LCD_SetCursor(0,0);I2C_LCD_Puts  ("Kiem tra ");LCD_send((display_on|cursor_off | blink_off), CMD);}
				if(ckeck_data==1) {I2C_LCD_Puts("Gio: 0-23h "); HAL_Delay (3000);}
				if(ckeck_data==2) {I2C_LCD_Puts("Phut: 0-60'"); HAL_Delay (3000);}
				if(ckeck_data==3) {I2C_LCD_Puts("Thu: 1-7   "); HAL_Delay (3000);}
				if(ckeck_data==4) {I2C_LCD_Puts("Ngay: 1-31 "); HAL_Delay (3000);}
				if(ckeck_data==5) {I2C_LCD_Puts("Thang: 1-12"); HAL_Delay (3000);}
				if(ckeck_data==6) {I2C_LCD_Puts("Nam: 00-99 "); HAL_Delay (3000);}
				if(ckeck_data==7) {I2C_LCD_Puts("H-Gio:0-23h"); HAL_Delay (3000);}
				if(ckeck_data==8) {I2C_LCD_Puts("H-Phut:0-60"); HAL_Delay (3000);}
				if(ckeck_data==9) {I2C_LCD_Puts("H-Thu: 1-7 "); HAL_Delay (3000);}
			}
			
	if(edit_time==1)
		{ 
					num_setup[0]=hour/10; 
					num_setup[1]=hour%10; 
					num_setup[2]=minute/10; 
					num_setup[3]=minute%10; 
					num_setup[4]=dow; 
					num_setup[5]=date/10; 
					num_setup[6]=date%10;
					num_setup[7]=month/10; 
					num_setup[8]=month%10;
				//	num_setup[9]=year/1000; 
				//	num_setup[10]=(year/100)%10;
					num_setup[11]=year/10;
					num_setup[12]=year%10;
					num_setup[13]=alarm[0]/10; 
					num_setup[14]=alarm[0]%10; 
					num_setup[15]=alarm[1]/10; 
					num_setup[16]=alarm[1]%10; 
					num_setup[17]=dayofweek[0]; num_setup[18]=dayofweek[1]; num_setup[19]=dayofweek[2]; 
					num_setup[20]=dayofweek[3]; num_setup[21]=dayofweek[4]; num_setup[22]=dayofweek[5]; num_setup[23]=dayofweek[6];
			
			LCD_SetCursor(0,8); LCD_send((display_on|cursor_on | blink_on), CMD);	
		}
}
void process_dow_data(void)
{
				////////////////////// chuong trinh sap xep tu nho den lon
					int i,j,k;
					for(i=17;i<24;i++)				
					for(j=i+1;j<24;j++)			
					if(num_setup[i]>num_setup[j])							
					{													
					int tg1	=num_setup[i];           
					num_setup[i]=num_setup[j];	        
					num_setup[j]=tg1;	              
					}
						
			for(int i =0;i<8;i++)	temp [i]=	num_setup[i+17]; // copy
					
			/////////////////////////////////////		chuong trinh xoa so bi trung trong mang	 
				 dayofweek[7] = 7; // so byte can ktra trung nhau thi xoa bot, dayofweek[7] la 1 bien tam dem so ngay hop le
				for (i = 0; i < dayofweek[7]; i++)
					{
						for(j = i + 1; j < dayofweek[7]; j++)
							{
								if(temp[i] == temp[j])
								{
									for(k = j; k < dayofweek[7]; k++)
									{
										temp[k] = temp[k + 1];
									}
									dayofweek[7]--;
									j--;
								}
							}
					}
					/////////////
					int m=0;
					while(temp[m]!=' '){dayofweek[m] = temp[m];m++;} // copy do dai mang den ky tu ' '
					 dayofweek[7] = m; // so byte co gia tri da cai dat
}
void edit_timenow_next(void)
{
			keyD_Next++;
			position_cursor=0;
			if(keyD_Next>=9) keyD_Next=0;
			if(keyD_Next==0) {LCD_SetCursor(0,8); position_cursor=0;}
			if(keyD_Next==1) {LCD_SetCursor(0,11); position_cursor=2;}
			if(keyD_Next==2) {LCD_SetCursor(1,4); position_cursor=4;}
			if(keyD_Next==3) {LCD_SetCursor(1,7); position_cursor=5;}
			if(keyD_Next==4) {LCD_SetCursor(1,10); position_cursor=7;} // 9,10 ko su dung
			if(keyD_Next==5) {LCD_SetCursor(1,15); position_cursor=11;}
			if(keyD_Next==6) {LCD_SetCursor(2,14); position_cursor=13;}
			if(keyD_Next==7) {LCD_SetCursor(2,17); position_cursor=15;}
			if(keyD_Next==8) { for(int i =17;i<25;i++)	num_setup [i]=	' '; for(int i =0;i<7;i++)	dayofweek[i]=	' '; LCD_SetCursor(3,0); I2C_LCD_Puts("M-Thu:              ");LCD_SetCursor(3,6); position_cursor=17;} // khi cai thu thi xoa truoc
}
void setup_next(void)
{ 		
			keyD_Next++;
			position_cursor=0;
			if(keyD_Next>=8) keyD_Next=0;
			if(keyD_Next==0) {LCD_SetCursor(1,3); position_cursor=0;}
			if(keyD_Next==1) {LCD_SetCursor(1,10); position_cursor=2;}
			if(keyD_Next==2) {LCD_SetCursor(1,17); position_cursor=4;}
			if(keyD_Next==3) {LCD_SetCursor(2,3); position_cursor=6;}
			if(keyD_Next==4) {LCD_SetCursor(2,10); position_cursor=8;}
			if(keyD_Next==5) {LCD_SetCursor(2,17); position_cursor=10;}
			if(keyD_Next==6) {LCD_SetCursor(3,3); position_cursor=12;}
			if(keyD_Next==7) {LCD_SetCursor(3,10); position_cursor=14;}

}
void Save(void)
{ 	

	for(int i=0; i<8; i++){
			valve_minute[i] = num_setup[i*2]*10 + num_setup[(i*2)+1]; // minute tens	
			}	
			
	if (valve_minute[0]>60)			{ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van1: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[1]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van2: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}
	else if (valve_minute[2]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van3: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[3]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van4: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[4]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van5: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[5]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van6: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[6]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van7: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}	
	else if (valve_minute[7]>60){ mode_time=1; position_cursor=0; keyD_Next=0; LCD_SetCursor(0,0); I2C_LCD_Puts("Kiem tra Van8: 0-60'");LCD_SetCursor(0,12);HAL_Delay (3000);}		
	 
	else { I2C_LCD_Clear();
		  LCD_SetCursor(0,5); I2C_LCD_Puts("Dang Luu..."); LCD_send((display_on|cursor_off | blink_off), CMD);				
			writeRam( add_8bytes_minute, 8, valve_minute);  // start address 0x08, length byte is 8
			HAL_Delay (2000);
		  I2C_LCD_Clear();
	 }
	
}
void setup_alarm(void)
{
	setup_mode++; // =0: alarm  ;;; =1 press button
	run_state=0; // stop
	off_all_valve(); 
	valve_number =0; // reset
			for(int i=0;i<8; i++) valve_state[i] = 0; // tat cac bien trung gian
	
	keyD_Next=0;     //reset row cursors
	if(setup_mode>=3){setup_mode = 0; writeRam( add_2bytes_auto_M_save, 2, auto_manu_save);} 
		I2C_LCD_Clear(); 
	if(setup_mode==1) { LCD_SetCursor(2,0); if(auto_manu_save[0]==1) {keyD_Next=0; I2C_LCD_Puts("Che do: Hen gio     ");} else {keyD_Next=1; I2C_LCD_Puts("Che do: Bang tay    ");}}
	if(setup_mode==2) { LCD_SetCursor(2,0); if(auto_manu_save[1]==1) {keyD_Next=0; I2C_LCD_Puts("Trang thai: Co nho  ");} else {keyD_Next=1; I2C_LCD_Puts("Trang thai:Khong nho");}}			
}
void off_all_valve(void)
{
motor_off; valve1_off;valve2_off;valve3_off;valve4_off;valve5_off;valve6_off;valve7_off;valve8_off;
}
void setup_alarm_next(void)
{
			keyD_Next++;
			if(keyD_Next>=2) keyD_Next=0;
			if((keyD_Next==0)&(setup_mode==1)) {auto_manu_save[0]=1; LCD_SetCursor(2,0);  I2C_LCD_Puts("Che do: Hen gio     ");}
			if((keyD_Next==1)&(setup_mode==1)) {auto_manu_save[0]=0; LCD_SetCursor(2,0);  I2C_LCD_Puts("Che do: Bang tay    ");}
			if((keyD_Next==0)&(setup_mode==2)) {auto_manu_save[1]=1; LCD_SetCursor(2,0); I2C_LCD_Puts ("Trang thai: Co nho  ");}
			if((keyD_Next==1)&(setup_mode==2)) {auto_manu_save[1]=0; LCD_SetCursor(2,0); I2C_LCD_Puts ("Trang thai:Khong nho");}
}

void GetKeyPressed(void)
{   keycode = 0;   
    Row1_low;  keynum=0; 
    scankey(); 
    Row1_high; Row2_low; keynum = 4;
    scankey(); 
    Row2_high; Row3_low; keynum = 8;
    scankey(); 
    Row3_high; Row4_low; keynum = 12;
    scankey(); 
    Row4_high;
}
void scankey(void)
{
        if(!Column1)
            { Delay_us(delaykey);
                if(!Column1)
                { Delay_us(delaykey);  keycode = keyvalues[0+keynum];                  
                  while(!Column1){Delay_us(delaykey);}; 
                }
                else{Delay_us(delaykey);}    
            }
            else{Delay_us(delaykey);}
        if(!Column2)
            { Delay_us(delaykey);
                if(!Column2)
                { Delay_us(delaykey);  keycode = keyvalues[1+keynum];                  
                  while(!Column2){Delay_us(delaykey);}; 
                }
                else{Delay_us(delaykey);}    
            }
            else{Delay_us(delaykey);}
        if(!Column3)
            { Delay_us(delaykey);
                if(!Column3)
                { Delay_us(delaykey);  keycode = keyvalues[2+keynum];                  
                  while(!Column3){Delay_us(delaykey);}; 
                }
                else{Delay_us(delaykey);}    
            }
            else{Delay_us(delaykey);}        
        if(!Column4)
            { Delay_us(delaykey);
                if(!Column4)
                { Delay_us(delaykey);  keycode = keyvalues[3+keynum];                  
                  while(!Column4){Delay_us(delaykey);}; 
                }
                else{Delay_us(delaykey);}    
            }
            else{Delay_us(delaykey);}    
}
void clock_setup(void)
{
	CLK_DeInit();
	
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
	
	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, 
	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
        CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

void UART2_Config(void)
        {
        UART2_DeInit();
        UART2_Init((uint32_t)9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO,
                    UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
        UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
        ITC_SetSoftwarePriority(21, 1);
        enableInterrupts();  
        }
void USART_putc(volatile unsigned char c)
	{
          UART2_SendData8(c);
          while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
	}
void sendUartFn()
{
  char k;


}
void GPIO_setup(void)
{
	GPIO_DeInit(GPIOG); // reset trang thai cho tat ca port D
        GPIO_Init(GPIOG,GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_WriteLow(GPIOG,GPIO_PIN_0);
        
      //  GPIO_DeInit(GPIOA); // reset trang thai cho tat ca port D
      //  GPIO_Init(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);
      //  GPIO_Init(GPIOA, GPIO_PIN_1|GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
            
        
        GPIO_DeInit(I2C_PORT);
        GPIO_DeInit(GPIOE); // reset trang thai cho tat ca port D
        GPIO_Init(GPIOE,GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
     //   GPIO_WriteLow(GPIOE,GPIO_PIN_5);
  
      // GPIO_DeInit(GPIOD); // reset trang thai cho tat ca port D
      // GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_SLOW);
        
        GPIO_DeInit(GPIOB); // reset trang thai cho tat ca port D
        GPIO_Init(GPIOB,GPIO_PIN_0, GPIO_MODE_IN_PU_NO_IT);
        GPIO_Init(GPIOB,GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);        
        GPIO_Init(GPIOB,GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
        GPIO_Init(GPIOB,GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
        
        GPIO_Init(GPIOB,GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(GPIOB,GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(GPIOB,GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(GPIOB,GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);
        
        
        GPIO_DeInit(GPIOC); // reset trang thai cho tat ca port D
        GPIO_Init(GPIOC,GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);
        GPIO_Init(GPIOC,GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);
        
        
}


static void WWDG_Config(void) 
{
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: LSI/128 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  IWDG_SetReload(100);
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void Delay_us(uint32_t time_delay)
{
while(time_delay--)
{
IWDG_ReloadCounter();
}
}


void HAL_Delay(uint32_t time_delay)
{
while(time_delay--)
{
  Delay_us(200);
  
}
}
#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{ 
  while (1)
  {
  }
}
#endif