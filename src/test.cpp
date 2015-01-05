#define F_CPU 16000000UL
#include "uart_lib.hpp"
#include <avr/delay.h>
#include <util/delay.h>

#define SERIAL_NAME serial_port
#define BAUD_RATE 9600
#define RX_BUFF 32
#define TX_BUFF 32
#include "serial.hpp"

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
     PORTB=~PORTB;
  }
  return 0;
}
