#include <mega16.h>

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0)
      {
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
#endif
      rx_buffer_overflow=1;
      }
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
#endif
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif


#include <stdio.h>
#include <delay.h>
// Declare your global variables here
unsigned short control_array_half_MotZ[8]={9,1,5,4,6,2,10,8};
unsigned char control_array_half_MotY[8]={0x90,0x10,0x50,0x40,0x60,0x20,0xa0,0x80};
unsigned char control_array_half_MotX[8]={0x90,0x10,0x50,0x40,0x60,0x20,0xa0,0x80};


#define led1 PORTD.5
#define led2 PORTD.4
#define led3 PORTD.6

#define sw1 PIND.2
#define sw2 PIND.3

// Standard Input/Output functions
void move_cw_motZ(unsigned int revolutions)
{
 int i=0;
 for (i=0;i<revolutions;i++)
 {
  PORTC=control_array_half_MotZ[i%8];
  delay_ms(2);
 }
}
void move_ccw_motZ(unsigned int revolutions)
{
 int i;
 for (i=revolutions;i>0;i--)
 {
  PORTC=control_array_half_MotZ[i%8];
  delay_ms(2);
 }
}

////////////////////////////////////////
void move_cw_motY(unsigned int revolutions)
{
 int i=0;
 for (i=0;i<revolutions;i++)
 {
  PORTC=control_array_half_MotY[i%8];
  delay_ms(5);
 }
}
void move_ccw_motY(unsigned int revolutions)
{
 int i;
 for (i=revolutions;i>0;i--)
 {
  PORTC=control_array_half_MotY[i%8];
  delay_ms(5);
 }
}
////////////////////////////////////////
void move_cw_motX(unsigned int revolutions)
{
 int i=0;
 for (i=0;i<revolutions;i++)
 {
  PORTA=control_array_half_MotX[i%8];
  delay_ms(5);
 }
}
void move_ccw_motX(unsigned int revolutions)
{
 int i;
 for (i=revolutions;i>0;i--)
 {
  PORTA=control_array_half_MotX[i%8];
  delay_ms(5);
 }
}

void resetCNC(void)
{
    move_cw_motX(530);
    move_cw_motY(530);
}
// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Place your code here
if (!sw1) //reset 
{
    move_ccw_motX(530);
    move_ccw_motY(530);
}

}

// Declare your global variables here
unsigned char data=0; 
unsigned char DataX=0;
unsigned char DataY=0;
unsigned char DataZ=0;
unsigned char DataStep=13;// accuracy 13 pulses -> 1 mm

void main(void)
{
// Declare your local variables here

PORTA=0x00;
DDRA=0x00;

PORTB=0x00;
DDRB=0x00;

PORTC=0x00;
DDRC=0xFF;

PORTD=0xBC;
DDRD=0xB0;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=0x05;
TCNT0=0x00;
OCR0=0x00;

TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

MCUCR=0x00;
MCUCSR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

ACSR=0x80;
SFIOR=0x00;

ADCSRA=0x00;

SPCR=0x00;

TWCR=0x00;

// Global enable interrupts
#asm("sei")

resetCNC();

while (1)
      {

led1=1;
while (rx_counter==0);
data=getchar();

led1=0;
DataX=0;DataY=0;DataZ=0;

switch (data) {
    case 'a':
    {
//    DataX=getchar();
    move_cw_motX(DataStep);
    }
    break;
    
    case 'b':
    {
//    DataX=getchar();
    move_ccw_motX(DataStep);
    }
    break;
/////////////////////
    case 'c':
    {
//    DataY=getchar();
    move_cw_motY(DataStep);
    }
    break;
        
    case 'd':
    {
//    DataY=getchar();
    move_ccw_motY(DataStep);
    }
    break;    
/////////////////////    
    case 'e':
    {
//    DataZ=getchar();
    move_cw_motZ(DataStep);
    }
    break; 
           
    case 'f':
    {
//    DataZ=getchar();
    move_ccw_motZ(DataStep);
    }
    break;     

};


//       move_ccw_motY(530);   
//       move_cw_motY(530);
//       move_ccw_motX(530);   
//       move_cw_motX(530);
//       move_cw_motZ(30);
//       move_ccw_motZ(30);
      }
}
