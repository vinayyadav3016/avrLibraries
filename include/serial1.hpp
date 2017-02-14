/*! \file serial1.hpp
 */
#ifndef __SERIAL1_HPP__
#define __SERIAL1_HPP__

#include <serial_base.hpp>

/*! \class
 */
class Serial1 : public SerialBase {
	/*!
	 */
	protected:
		/*!
		 */
		inline void initUSART(const int baudrate) const {
			uint16_t	ubbr_value = (F_CPU/(baudrate*16UL))-1;
			UBRR1L = ubbr_value;
			UBRR1H = ubbr_value>>8;
			UCSR1C =(1<<UCSZ11)|(1<<UCSZ10);
		}
		/*!
		 */
		inline void startUSART() const {
			UCSR1B|=(1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(1<<TXCIE1)|(1<<UDRIE1);
		}
		/*!
		 */
		inline void stopTransmission() {
			UCSR1B&=~(1<<UDRIE1);
		}
		/*!
		 */
		void stopUSART() {
			stopTransmission();
			UBRR1H=0x00;
			UBRR1L=0x00;
			UCSR1B=0x00;
			UCSR1C=0x00;
		}
	public:
		/*!
		 */
		Serial1(int baudrate,uint8_t rec_size,uint8_t trans_size):SerialBase(rec_size,trans_size) {
			initUSART(baudrate);
			startUSART();
		}
		/*!
		 */
		~Serial1() {
			stopUSART();
		}
		/*!
		 */
		inline void startTransmission() const {
			UCSR1B|=(1<<UDRIE1);
		}
		/*!
		 */
		inline void doUDRISR() {
			if(_transmitter.getReadBuffLength()>0) {
				uint8_t data=0;
				_transmitter.readByte(data);
				UDR1 = data;
			}
			else {
				stopTransmission();
			}
		}
		/*!
		 */
		inline void doRXISR(const uint8_t& data) {
			if(_reciever.getWriteBuffLength()>0) {
				_reciever.writeByte(data);
			}
		}
		/*!
		 */
		inline void doTXISR() {
		}
};

/*!
 */
Serial1* SERIAL_NAME;//(BAUD_RATE,RX_BUFF,TX_BUFF);

/*!
 */
Serial1* getSerialPort1(uint16_t baud_rate,uint16_t rx_buff,uint16_t tx_buff) {
	if(SERIAL_NAME==NULL)
		SERIAL_NAME = new Serial1(baud_rate,rx_buff,tx_buff);
	return SERIAL_NAME;
}

/*!
 */
void destroySerialPort0() {
	delete SERIAL_NAME;
}

#ifdef USART1_UDRE_vect

/*!
 */
ISR(USART1_UDRE_vect) {
	SERIAL_NAME->doUDRISR();
}

/*!
 */
ISR(USART1_RX_vect) {
	const uint8_t data = UDR1;
	SERIAL_NAME->doRXISR(data);
}

/*!
 */
ISR(USART1_TX_vect) {
	SERIAL_NAME->doTXISR();
}
#elif
Error "No Interrupt vector defined how"
#endif

#endif //__SERIAL1_HPP___
