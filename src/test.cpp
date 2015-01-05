#define F_CPU 16000000UL
#include "uart_lib.hpp"
#include <avr/delay.h>
#include <util/delay.h>
Serial serial_port(9600,32,32);

ISR(USART_UDRE_vect)
{
  serial_port.doUDRISR();
}

ISR(USART_RXC_vect)
{
  const uint8_t data = UDR;
  serial_port.doRXISR(data);
}

ISR(USART_TXC_vect)
{
  serial_port.doTXISR();
}

int main()
{
  DDRB=0xff;
  PORTB=0xff;
  cli();
  sei();
  PORTB=~PORTB;
  while(1)
  {
    _delay_ms(100);
     serial_port.sendString("Hi this is Vinay\n");
     //serial_port.sendByte('A');
     PORTB=~PORTB;
  }
  return 0;
}
