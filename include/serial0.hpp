/*!
 */
#ifndef __SERIAL0_HPP__
#define __SERIAL0_HPP__

#include "ring_buffer.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "serial_base.hpp"

class Serial0 : public SerialBase {
	protected:
		inline void initUSART(const int baudrate) const {
			uint16_t	ubbr_value = (F_CPU/(baudrate*16UL))-1;
			UBRR0L = ubbr_value;
			UBRR0H = ubbr_value>>8;
			UCSR0C =(1<<UCSZ01)|(1<<UCSZ00);
		}
		inline void startUSART() const {
			UCSR0B|=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0)|(1<<UDRIE0);
		}
		inline void stopTransmission() {
			UCSR0B&=~(1<<UDRIE0);
		}
		void stopUSART() {
			stopTransmission();
			UBRR0H=0x00;
			UBRR0L=0x00;
			UCSR0B=0x00;
			UCSR0C=0x00;
		}
	public:
		Serial0(int baudrate,uint8_t rec_size,uint8_t trans_size):SerialBase(rec_size,trans_size) {
			initUSART(baudrate);
			startUSART();
		}
		~Serial0() {
			stopUSART();
		}
		inline void startTransmission() const {
			UCSR0B|=(1<<UDRIE0);
		}
		inline void doUDRISR() {
			if(_transmitter.getReadBuffLength()>0) {
				uint8_t data=0;
				_transmitter.readByte(data);
				UDR0 = data;
			}
			else {
				stopTransmission();
			}
		}
		inline void doRXISR(const uint8_t& data) {
			if(_reciever.getWriteBuffLength()>0) {
				_reciever.writeByte(data);
			}
		}
		inline void doTXISR() {
			
		}
};

Serial0* SERIAL_NAME;//(BAUD_RATE,RX_BUFF,TX_BUFF);

Serial0* getSerialPort0(uint16_t baud_rate,uint16_t rx_buff,uint16_t tx_buff) {
	if(SERIAL_NAME==NULL)
		SERIAL_NAME = new Serial0(baud_rate,rx_buff,tx_buff);
	return SERIAL_NAME;
}

void destroySerialPort0() {
	delete SERIAL_NAME;
}
#ifdef USART_UDRE_vect
ISR(USART_UDRE_vect) {
	SERIAL_NAME->doUDRISR();
}

ISR(USART_RX_vect) {
	const uint8_t data = UDR0;
	SERIAL_NAME->doRXISR(data);
}

ISR(USART_TX_vect) {
	SERIAL_NAME->doTXISR();
}
#elif USART0_UDRE_vect

ISR(USART0_UDRE_vect) {
	SERIAL_NAME->doUDRISR();
}

ISR(USART0_RX_vect) {
	const uint8_t data = UDR0;
	SERIAL_NAME->doRXISR(data);
}

ISR(USART0_TX_vect) {
	SERIAL_NAME->doTXISR();
}
#elif
Error "No Interrupt vector defined how"
#endif

#endif //__SERIAL0_HPP___
