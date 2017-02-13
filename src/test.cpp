#define F_CPU 16000000UL
#include <uart_lib.hpp>
#include <avr/io.h>
#include <serial0.hpp>				  ///
#include <adxl.hpp>
#include <util/delay.h>
int main()
{
	//DDRB=0xff;
	//PORTB=0xff;
	//cli();
	//sei();
	//PORTB=~PORTB;
	const char* prompt="Arduino:";
	Serial0* serial_port = getSerialPort0(9600,64,64);
	cli();
	sei();
	serial_port->send(prompt);
	while(1) {
		_delay_ms(1000);
		uint8_t data[25];
		uint8_t len = serial_port->readFrame(data, 25);
		if(len!=0)
		{
			//serial_port->send();
			serial_port->send("Echo:");
			serial_port->send(data,len);
			serial_port->send();
			serial_port->send(prompt);
		}
	}
	return 0;
}
