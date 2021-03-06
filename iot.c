_________________ES8266.C__________________________________________
____________________________________________________________________


#include"esp8266.h"
#include "lcd.h"

unsigned int crc=0xFFFF;
int m=0,k=0;
unsigned int crctest;
char string2[30];
int length;
unsigned int crc_val;
unsigned int req_val;
unsigned int ret_val;
char pure_string[30];
char string_clean[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

char* x;
int val=0;
unsigned int i=0;
char string1[250]={0};
unsigned char rx_flag=0;
char hi[2]="hi";


//_______________________________________________________________
void init_serial()
{
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS_1;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;
}
//________________________________________________________________
void serial_putsc(unsigned char value)
{
    while (!(IFG2&UCA0TXIFG));                 // USCI_A0 TX buffer ready?
    UCA0TXBUF = value;                     // TX -> RXed character
}
//________________________________________________________________
void serial_puts(char* String)
{
    while(*String != '\n')
    {
        serial_putsc(*String);
        String++;
    }
    serial_putsc(*String);
}
//________________________________________________________________
void clear_buffer(void)
{
    for(i=199;i>0;i--)
    {
        string1[i] = 0;
    }
    string1[0] = 0;
    i=0;
    rx_flag=0;
}
//_______________________________________________________________
unsigned int wifiCommand_ack(char* sCmd,int waitTm,char* sTerm)
{
//    char lp = 0;
//    while(lp < waitTm)
//    {
        serial_puts(sCmd);
        _BIS_SR(LPM0_bits + GIE);      //  yoluyor yoladiktan sonra siliyor 
        while(waitTm--)
            _delay_cycles(1000);
        if(strstr(string1,sTerm) != 0)
        {
            for(;i>0;i--)
                string1[i] = 0;
            string1[0] = 0;
            i=0;
            rx_flag=0;
            return 1;
        }
//        _delay_cycles(1000000);
//        lp++;
//    }
    for(;i>0;i--)
        string1[i] = 0;
    string1[0] = 0;
    i=0;
    rx_flag=0;
    return 0;
}
//_________________________________________________________________
void wifiCommand_val(char* sCmd,int waitTm)
{
    serial_puts(sCmd);
    _BIS_SR(LPM0_bits + GIE);
    while(waitTm--)
        _delay_cycles(1000);
    //strcpy(string1,ret);
}
//_________________________________________________________________
void init_esp()
{
    P1DIR|=BIT0;
    P1OUT&=~BIT0;
    _delay_cycles(10000);
    P1OUT|=BIT0;
    _delay_cycles(3000000);
    wifiCommand_ack("AT\r\n",1000,"OK");
    wifiCommand_ack("AT+CIPCLOSE\r\n",100,"OK");
    val = wifiCommand_ack("AT+RST\r\n",200,"ready");
    wifiCommand_ack("AT+CWMODE=3\r\n",10,"OK");
    wifiCommand_ack("AT+CIFSR\r\n",10,"OK");
//    wifiCommand_val("AT+GMR\r\n",50);
//    clear_buffer();
}
//__________________________________________________________________
void mode1()
{
    //wifiCommand_ack("AT+CIPMUX=1\r\n",10,"OK");
    //wifiCommand_ack("AT+CIPSERVER=1,9998\r\n",10,"OK");
    wifiCommand_ack("AT+CIPMUX=0\r\n",10,"OK");
    wifiCommand_ack("AT+CWJAP=\"ELE417\",\"EmbeddeD\"\r\n",10000,"OK");
    //wifiCommand_ack("AT+CWJAP=\"Redmi\",\"karaismail\"\r\n",10000,"OK");
    wifiCommand_ack("AT+CIPSTART=\"TCP\",\"10.50.106.10\",10000\r\n",10000,"OK");
    wifiCommand_ack("AT+CIPSEND=2\r\n",100,"OK");
    wifiCommand_val("hi",200);
}
//____________________________________________________________________
unsigned int str_request(char* requ_type)
{
    wifiCommand_ack("AT+CIPSEND=2\r\n",2000,"OK");
    wifiCommand_val(requ_type, 2000);
    __delay_cycles(500000);
    for(k=0;k<119;k++){
        if((string1[k]=='I') && (string1[k+1]=='P') && (string1[k+2]=='D')){
             if(string1[k+5]==':'){
                 length=string1[k+7]-2;
                 for(m=0;m<length;m++,k++){
                     string2[m]=string1[k+8];

                 }
                 crc_val=crc16();
                 return crc_val;

             }
             else if(string1[k+6]==':'){
                  length=string1[k+8]-2;
                  for(m=0;m<length;m++,k++){
                      string2[m]=string1[k+9];

                  }
                 crc_val=crc16();
                  return crc_val;

             }
             return 0;
        }
    }
    return 0;
}
//_____________________________________________________________________
unsigned int crc16()
{

    crc = 0xFFFF;
    for(k=0;k<length-2;k++){
    crc ^= string2[k];

    for(m=0;m<8;m++){
        crctest=crc;
        crctest &= 1;
        crc = crc>>1;
        if(crctest==1){
            crc ^= 0xA001;
        }
    }

    }
    crctest = string2[length-2];
    crctest = crctest<<8;
    crctest |= string2[length-1];
    if(crctest==crc){
        return 1;
    }
    else{
        return 0;
    }


}

//_____________________________________________________________________

unsigned int request(char* requ_type)
{
    req_val=str_request(requ_type);
    if (req_val==1){
        for(k=0;k<30;k++){
            pure_string[k]=string_clean[k];
        }
        for(k=0;k<length-2;k++){
            pure_string[k]=string2[k];
        }

        return 1;
    }
    else{
        request(requ_type);
        return 1;
    }

}
//___________________________________________________________________

char * string_ret(char* requ_type)
{
    ret_val=request(requ_type);
    if(ret_val==1){
        return pure_string;
    }
    for(k=0;k<30;k++){
        pure_string[k]=string_clean[k];
    }
}

//__________________________________________________________________

unsigned char receive_data()
{
    if(rx_flag==1)
    {
        _delay_cycles(100000);
        return 1;
    }
    return 0;
}
//__________________________________________________________________________

void update_settings(short int* no_of_feeds,short int* feed_m,short int* feed_h,short int* feed_units)
{
    char* temp;
    unsigned int index;
    //decode protocol
    temp = strchr(string1,'f');
    index = temp-string1+1;
    *no_of_feeds = string1[index]-48;
    for(i=0; i<*no_of_feeds; i++)
    {
        temp = strchr(string1,'a'+i);
        index = temp-string1+1;
        *(feed_h+i) = (string1[index]-48)*10+(string1[index+1]-48);
        *(feed_m+i) = (string1[index+2]-48)*10+(string1[index+3]-48);
        feed_units[i] = string1[index+4]-48;
    }
    //prtocol decoded
    clear_buffer();
    rx_flag=0;
}
//___________________________________________________________________________

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    string1[i++] = UCA0RXBUF;
    _BIC_SR_IRQ(LPM0_bits);
    rx_flag=1;
}



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



______________________________________________lcd.c___________________________________________________________
_________________________________________________________________________________________________________


/* Build: msp430-gcc -mmcu=msp430g2231 -g -o lcd.elf lcd.c */

/* uC and LCD Connections
    TP1 - Vcc (+5v)
    TP3 - Vss (Gnd)
    P2.0 - D4
    P2.1 - D5
    P2.2 - D6
    P2.3 - D7
    P2.4 - EN
    P2.5 - RS
    Gnd  - RW
    Gnd  - Vee through 1K Resistor  - this value determines contrast - i.e. direct connection to Gnd means all dots displayed
    Gnd  - K (LED-)
    Vcc  - A (LED+) +5V
    Clock: 1MHz             */

#include <msp430.h>

// uC Port definitions
#define lcd_port        P2OUT
#define lcd_port_dir    P2DIR

// LCD Registers masks based on pin to which it is connected
#define LCD_EN      BIT4
#define LCD_RS      BIT5

void lcd_reset()
{
    lcd_port_dir = 0xff;    // output mode
    lcd_port = 0xff;
    __delay_cycles(20000);
    lcd_port = 0x03+LCD_EN;
    lcd_port = 0x03;
    __delay_cycles(10000);
    lcd_port = 0x03+LCD_EN;
    lcd_port = 0x03;
    __delay_cycles(1000);
    lcd_port = 0x03+LCD_EN;
    lcd_port = 0x03;
    __delay_cycles(1000);
    lcd_port = 0x02+LCD_EN;
    lcd_port = 0x02;
    __delay_cycles(1000);
}

void lcd_cmd (char cmd)
{
    // Send upper nibble
    lcd_port = ((cmd >> 4) & 0x0F)|LCD_EN;
    lcd_port = ((cmd >> 4) & 0x0F);

    // Send lower nibble
    lcd_port = (cmd & 0x0F)|LCD_EN;
    lcd_port = (cmd & 0x0F);

    __delay_cycles(4000);
}

void lcd_init ()
{
    lcd_reset();         // Call LCD reset
    lcd_cmd(0x28);       // 4-bit mode - 2 line - 5x7 font.
    lcd_cmd(0x0C);       // Display no cursor - no blink.
    lcd_cmd(0x06);       // Automatic Increment - No Display shift.
    lcd_cmd(0x80);       // Address DDRAM with 0 offset 80h.
    lcd_cmd(0x01);       // Clear screen
}


void lcd_data (unsigned char dat)
{
    // Send upper nibble
    lcd_port = (((dat >> 4) & 0x0F)|LCD_EN|LCD_RS);
    lcd_port = (((dat >> 4) & 0x0F)|LCD_RS);

    // Send lower nibble
    lcd_port = ((dat & 0x0F)|LCD_EN|LCD_RS);
    lcd_port = ((dat & 0x0F)|LCD_RS);

    __delay_cycles(4000); // a small delay may result in missing char display
}

void display_line(char *line)
{
    while (*line)
        lcd_data(*line++);
}



/*
int main()
{
    // Stop Watch Dog Timer
    WDTCTL = WDTPW + WDTHOLD;

    // Initialize LCD
    lcd_init();


    lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
    display_line("Black God");
    lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
    display_line("Bits & Bytes");

    while (1){
        lcd_init();


          lcd_cmd(0x80); // select 1st line (0x80 + addr) - here addr = 0x00
          display_line("Black God");
          lcd_cmd(0xc0); // select 2nd line (0x80 + addr) - here addr = 0x40
          display_line("Bits & Bytes");
          __delay_cycles(1000000);
    }
}

*/



_______________________________lcd.h_______________________
_____________________________________________________


/*
 * lcd.h
 *
 *  Created on: 12 Oca 2020
 *      Author: asus
 */

#ifndef LCD_H_
#define LCD_H_



#endif /* LCD_H_ */

___________________________________
