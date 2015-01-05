#ifndef SERIAL0_HPP
#define SERIAL0_HPP

#include "ring_buffer.hpp"
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class Serial0
{
  protected:
    RingBuffer _reciever;
    RingBuffer _transmitter;
    inline void initUSART(const int baudrate) const
    {
      uint16_t  ubbr_value = (F_CPU/(baudrate*16UL))-1;
      UBRR0L = ubbr_value;
      UBRR0H = ubbr_value>>8;
      UCSR0C =(1<<UCSZ01)|(1<<UCSZ00);
    }
    inline void startUSART()
    {
      UCSR0B|=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0)|(1<<UDRIE0);
    }
    inline void startTransmission() const
    {
      UCSR0B|=(1<<UDRIE0);
    }
    inline void stopTransmission()
    {
      UCSR0B&=~(1<<UDRIE0);
    } 
    inline uint8_t sendChunk(const uint8_t* data,const uint8_t length)
    {
      if(length<=_transmitter.getWriteBuffLength())
      {
        for(uint8_t i=0;i<length;i++)
        {
          _transmitter.writeByte(data[i]);
        }
        startTransmission();
        return 0;
      }
      else
      {
        return RingBuffer::BUFFER_FULL;
      }
    }
  public:
    Serial0(int baudrate,uint8_t rec_size,uint8_t trans_size): _reciever(rec_size), _transmitter(trans_size)
    {
      initUSART(baudrate);
      startUSART();
    }
    inline uint8_t sendln()
    {
      return sendByte('\n');
    }
    inline uint8_t sendByte(const uint8_t& data)
    {
      return sendChunk(&data,1);
    }
    inline uint8_t sendBuffer(const uint8_t* data,const uint8_t length)
    {
      return sendChunk(data,length);
    }
    inline uint8_t sendString(const char* data)
    {
      return sendChunk(reinterpret_cast<const uint8_t * >(data),strlen(data));
    }
    inline uint8_t sendInt(const int& data)
    {
      char str[10];
      uint8_t length = sprintf(str,"%d",data);
      return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
    }
    inline uint8_t sendFloat(const float& data,const uint8_t decimal=2)
    {
      char str[10];
      uint8_t length = sprintf(str,"%.*f",decimal,data);
      return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
    }
    inline void doUDRISR()
    {
      if(_transmitter.getReadBuffLength()>0)
      {
        uint8_t data=0;
        _transmitter.readByte(data);
        UDR0 = data;
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


Serial0 SERIAL0_NAME(BAUD_RATE0,RX0_BUFF,TX0_BUFF);

ISR(USART_UDRE_vect)
{
  SERIAL0_NAME.doUDRISR();
}

ISR(USART_RX_vect)
{
  const uint8_t data = UDR0;
  SERIAL0_NAME.doRXISR(data);
}

ISR(USART_TX_vect)
{
  SERIAL0_NAME.doTXISR();
}

#endif
