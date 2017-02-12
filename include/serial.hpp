#ifndef SERIAL_HPP
#define SERIAL_HPP

#include "ring_buffer.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "serial_base.hpp"

class Serial : public SerialBase {
	protected:
		inline void initUSART(const int baudrate) const __attribute__((always_inline)) {
			uint16_t	ubbr_value = (F_CPU/(baudrate*16UL))-1;
			UBRRL = ubbr_value;
			UBRRH = ubbr_value>>8;
			UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
		}
		inline void startUSART() const __attribute__((always_inline)) {
			UCSRB|=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
		}
		inline void stopTransmission() __attribute__((always_inline)) {
			UCSRB&=~(1<<UDRIE);
		}
		void stopUSART() {
			stopTransmission();
			UBRRH=0x00;
			UBRRL=0x00;
			UCSRB=0x00;
			UCSRC=0x00;
		}
		public:
		Serial(int baudrate,uint8_t rec_size,uint8_t trans_size):SerialBase(rec_size,trans_size) {
			initUSART(baudrate);
			startUSART();
		}
		inline void startTransmission() const __attribute__((always_inline)) {
			UCSRB|=(1<<UDRIE);
		}
		void doUDRISR() {
			if(_transmitter.getReadBuffLength()>0) {
				uint8_t data=0;
				_transmitter.readByte(data);
				UDR = data;
			} else {
				stopTransmission();
			}
		}
		void doRXISR(const uint8_t& data) {
			if(_reciever.getWriteBuffLength()>0) {
				_reciever.writeByte(data);
			}
		}
		void doTXISR() {
		}
};

Serial* SERIAL_NAME;//(BAUD_RATE,RX_BUFF,TX_BUFF);

Serial* getSerialPort(uint16_t baud_rate,uint16_t rx_buff,uint16_t tx_buff) {
	if(SERIAL_NAME==NULL)
		SERIAL_NAME = new Serial(baud_rate,rx_buff,tx_buff);
	return SERIAL_NAME;
}

void destroySerialPort() {
	if(SERIAL_NAME!=NULL)
		delete SERIAL_NAME;
	SERIAL_NAME = NULL;
}

ISR(USART_UDRE_vect) {
	SERIAL_NAME->doUDRISR();
}

ISR(USART_RXC_vect) {
	const uint8_t data = UDR;
	SERIAL_NAME->doRXISR(data);
}

ISR(USART_TXC_vect) {
	SERIAL_NAME->doTXISR();
}

#endif
