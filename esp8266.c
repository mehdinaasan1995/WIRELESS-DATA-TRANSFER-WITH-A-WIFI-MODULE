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

