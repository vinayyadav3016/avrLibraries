#ifndef SERIAL_HPP
#define SERIAL_HPP

#include "ring_buffer.hpp"
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "serial_base.hpp"

class Serial : public SerialBase
{
  protected:
    inline void initUSART(const int baudrate) const
    {
      uint16_t  ubbr_value = (F_CPU/(baudrate*16UL))-1;
      UBRRL = ubbr_value;
      UBRRH = ubbr_value>>8;
      UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
    }
    inline void startUSART() const
    {
      UCSRB|=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
    }
    inline void startTransmission() const
    {
      UCSRB|=(1<<UDRIE);
    }
    inline void stopTransmission()
    {
      UCSRB&=~(1<<UDRIE);
    } 
  public:
    Serial(int baudrate,uint8_t rec_size,uint8_t trans_size):SerialBase(rec_size,trans_size)
    {
      initUSART(baudrate);
      startUSART();
    }
    inline void doUDRISR()
    {
      if(_transmitter.getReadBuffLength()>0)
      {
        uint8_t data=0;
        _transmitter.readByte(data);
        UDR = data;
      }
      else
      {
        stopTransmission();
      }
    }
    inline void doRXISR(const uint8_t& data)
    {
      if(_reciever.getWriteBuffLength()>0)
      {
        _reciever.writeByte(data);
      }
    }
    inline void doTXISR()
    {
    }
};

Serial SERIAL_NAME(BAUD_RATE,RX_BUFF,TX_BUFF);

ISR(USART_UDRE_vect)
{
  SERIAL_NAME.doUDRISR();
}

ISR(USART_RXC_vect)
{
  const uint8_t data = UDR;
  SERIAL_NAME.doRXISR(data);
}

ISR(USART_TXC_vect)
{
  SERIAL_NAME.doTXISR();
}

#endif
