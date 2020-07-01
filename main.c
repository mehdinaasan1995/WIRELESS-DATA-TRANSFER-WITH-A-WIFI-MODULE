______________________________________-main.c________________________________
_____________________________________________________________
#include <msp430.h>
#include "esp8266.h"
#include "lcd.h"

char temprature2[2]={0xEE,0x01};
char air2[2]={0xEE,0x02};
char humidity2[2]={0xEE,0x03};
char wind2[2]={0xEE,0x04};
char cloud2[2]={0xEE,0x05};
char weather2[2]={0xEE,0x06};
char time2[2]={0xEE,0x07};
char date2[2]={0xEE,0x08};
char uptime2[2]={0xEE,0x09};
char system2[2]={0xEE,0x0A};
char connected2[2]={0xEE,0x0B};


char * Temprature;
char * Airpressure;
char * Humidity;
char * Wind_Speed;
char * Cloudiness;
char * Weather_Status;
char * Current_Time;
char * Current_Date;
char * Uptime;
char * System_Temprature;
char * Number_of_Connected;









void main(void) {





    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P1DIR |= BIT5;
    P1OUT |= BIT5;

    lcd_init();


    init_serial();
    init_esp();
    mode1();

    while(1)
    {



        Temprature = string_ret(temprature2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Temprature");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Temprature);
        __delay_cycles(1000000);
        lcd_cmd(0x01);
        __delay_cycles(3000);


        Airpressure = string_ret(air2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Air Pressure");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Airpressure);
        __delay_cycles(1000000);
        lcd_cmd(0x01);
        __delay_cycles(3000);

        Humidity = string_ret(humidity2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Humidity (%)");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Humidity);
        __delay_cycles(1000000);
        lcd_cmd(0x01);


        __delay_cycles(3000);
        Wind_Speed = string_ret(wind2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Wind Speed");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Wind_Speed);
        __delay_cycles(1000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);
        Cloudiness = string_ret(cloud2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Cloudiness");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Cloudiness);
        __delay_cycles(1000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);
        Weather_Status = string_ret(weather2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Weather Status");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Weather_Status);
        __delay_cycles(1000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);
        Current_Time = string_ret(time2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Current Time");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Current_Time);
        __delay_cycles(1000000);
        lcd_cmd(0x01);
        __delay_cycles(3000);

        Current_Date = string_ret(date2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Current Date ");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Current_Date);
        __delay_cycles(1000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);
        Uptime = string_ret(uptime2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Uptime(server)");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Uptime);
        __delay_cycles(3000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);
        System_Temprature = string_ret(system2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("System Temprature");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(System_Temprature);
        __delay_cycles(1000000);
        lcd_cmd(0x01);

        __delay_cycles(3000);

        Number_of_Connected = string_ret(connected2);
        __delay_cycles(3000);
        lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
        __delay_cycles(3000);
        display_line("Number_of_Connected");
        __delay_cycles(3000);
        lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
        __delay_cycles(3000);
        display_line(Number_of_Connected);
        __delay_cycles(1000000);
        lcd_cmd(0x01);
        __delay_cycles(3000);

    }
}

#pragma vector=NMI_VECTOR
__interrupt void nmi_ (void)
{
    while(IFG1 & OFIFG)  // wait for OSCFault to clear
    {
        IFG1 &= ~OFIFG;
        __delay_cycles(100);
    }
    IE1 |= OFIE;                              // Enable Osc Fault
}
