#define F_CPU 16000000UL
#include "uart_lib.hpp"
#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/delay.h>
//#include <util/delay.h>

////////////////////////////////////////
/////  UART defination start         ///
// type identifier                   ///
#define SERIAL_NAME serial_port      ///
#define BAUD_RATE 9600               ///
#define RX_BUFF 5                    ///
#define TX_BUFF 5                    ///
#include "serial.hpp"                ///
#undef SERIAL_NAME                   ///
#undef BAUD_RATE                     ///
#undef RX_BUFF                       ///
#undef TX_BUFF                       ///
////// UART defination ends here     ///
////////////////////////////////////////
#define PROMPT "Arduino:"
int main()
{
  //DDRB=0xff;
  //PORTB=0xff;
  //cli();
  //sei();
  //PORTB=~PORTB;
  const char* prompt="Arduino:";
  while(1)
  {
    //_delay_ms(100);
    serial_port.sendString(PROMPT);
    serial_port.sendln();
    serial_port.startTransmission();
    //PORTB=~PORTB;
  }
  return 0;
}
