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
class Serial
{
  protected:
    RingBuffer _reciever;
    RingBuffer _transmitter;
    inline void initUSART(const int baudrate) const
    {
      uint16_t  ubbr_value = (F_CPU/(baudrate*16UL))-1;
      UBRRL = ubbr_value;
      UBRRH = ubbr_value>>8;
      UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
    }
    inline void startUSART()
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
    Serial(int baudrate,uint8_t rec_size,uint8_t trans_size): _reciever(rec_size), _transmitter(trans_size)
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

#endif
