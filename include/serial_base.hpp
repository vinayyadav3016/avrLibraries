#ifndef SERIAL_BASE_HPP
#define SERIAL_BASE_HPP

#include "ring_buffer.hpp"
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class SerialBase
{
  protected:
    RingBuffer _reciever;
    RingBuffer _transmitter;
    ////////////////////////////////////////////////////////////
    /// Make protected to stop accidents
    SerialBase(uint8_t rec_size,uint8_t trans_size): _reciever(rec_size), _transmitter(trans_size)
    {
    }
    ///////////////////////////////////////////////////////////
    /// you must define these function in your class
    inline void initUSART(const int baudrate) const{};
    inline void startUSART() const{};
    inline void stopTransmission() const {};
    ///////////////////////////////////////////////////////////
    /// this is made to enable start transmission
    virtual void startTransmission() const{};
    ///////////////////////////////////////////////////////////
    // This is to destroy the UART module
    virtual void stopUSART(){};
    ///////////////////////////////////////////////////////////
    /// sender and reciever functions 
    uint8_t sendChunk(const uint8_t* data,const uint8_t length)
    {
      uint8_t len = _transmitter.getWriteBuffLength();
      len = MIN(len,length);
      for(uint8_t i=0;i<len;i++)
      {
        _transmitter.writeByte(data[i]);
      }
      startTransmission();
      return len;
    }
    uint8_t readChunk(uint8_t* data,const uint8_t length)
    {
      uint8_t len=_reciever.getReadBuffLength();
      len = MIN(len,length);
      for(uint8_t i=0;i<len;i++)
      {
        _reciever.readByte(data[i]);
      }
      return len;
    }
  public:
    virtual ~SerialBase()
    {
      stopUSART();
    }
    ///////////////////////////////////////////////////////////
    //// Recieving Functions
    inline uint8_t readByte(uint8_t& data) __attribute__((always_inline))
    {
      return readChunk(&data,1);
    }
    inline uint8_t readBuffer(uint8_t* data,const uint8_t length) __attribute__((always_inline))
    {
      return readChunk(data,length);
    }
    ///////////////////////////////////////////////////////////
    //// Transmission Functions
    inline uint8_t sendln() __attribute__((always_inline))
    {
      return sendString("\r\n");
    }
    inline uint8_t sendByte(const uint8_t& data) __attribute__((always_inline))
    {
      return sendChunk(&data,1);
    }
    inline uint8_t sendBuffer(const uint8_t* data,const uint8_t length) __attribute__((always_inline))
    {
      return sendChunk(data,length);
    }
    inline uint8_t sendString(const char* data) __attribute__((always_inline))
    {
      return sendChunk(reinterpret_cast<const uint8_t * >(data),strlen(data));
    }
    uint8_t sendInt(const int& data)
    {
      char str[10];
      uint8_t length = sprintf(str,"%d",data);
      return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
    }
    uint8_t sendFloat(const float& data,const uint8_t decimal=2)
    {
      char str[10];
      uint8_t length = sprintf(str,"%.*f",decimal,data);
      return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
    }
    ///////////////////////////////////////////////////////////
    /// you must overwrite these functions in your class
    inline void doUDRISR() __attribute__((always_inline))
    {
    }
    void doRXISR(const uint8_t& data)
    {
      if(_reciever.getWriteBuffLength()>0)
      {
        _reciever.writeByte(data);
      }
    }
    inline void doTXISR()
    {
    }
    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////
};

#endif
