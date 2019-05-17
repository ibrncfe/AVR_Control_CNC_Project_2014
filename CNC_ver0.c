#include <mega16.h>
#include <delay.h>

// Declare your global variables here
unsigned short control_array_half_MotZ[8]={9,1,5,4,6,2,10,8};
unsigned char control_array_half_MotY[8]={0x90,0x10,0x50,0x40,0x60,0x20,0xa0,0x80};
unsigned char control_array_half_MotX[8]={0x90,0x10,0x50,0x40,0x60,0x20,0xa0,0x80};

//float Min_Step=1.8;

void move_cw_motZ(unsigned int revolutions)
{
 int i=0;
 for (i=0;i<revolutions;i++)
 {
  PORTC=control_array_half_MotZ[i%8];
  delay_ms(3);
 }
}
void move_ccw_motZ(unsigned int revolutions)
{
 int i;
 for (i=revolutions;i>0;i--)
 {
  PORTC=control_array_half_MotZ[i%8];
  delay_ms(3);
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
  delay_ms(2);
 }
}
void move_ccw_motX(unsigned int revolutions)
{
 int i;
 for (i=revolutions;i>0;i--)
 {
  PORTA=control_array_half_MotX[i%8];
  delay_ms(2);
 }
}
////////////////////////////////////////
void main(void)
{
// Declare your local variables here
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0xFF;

// Port D initialization
// Func7=Out Func6=In Func5=Out Func4=Out Func3=In Func2=In Func1=In Func0=In 
// State7=1 State6=T State5=1 State4=1 State3=P State2=P State1=T State0=T 
PORTD=0xBC;
DDRD=0xB0;

TCCR0=0x00;
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

TIMSK=0x00;

// USART initialization
// USART disabled
UCSRB=0x00;

ACSR=0x80;
SFIOR=0x00;

ADCSRA=0x00;

SPCR=0x00;

TWCR=0x00;
PORTC=0xff;

while (1)
      {
       
       move_ccw_motY(530);   
       move_cw_motY(530);
       move_ccw_motX(530);   
       move_cw_motX(530);
       move_cw_motZ(30);
       move_ccw_motZ(30);
      }
}
