#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include "cpluscplus_use.hpp"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

class RingBuffer
{
  private:
    uint8_t* _buffer;
    uint8_t _insert;
    uint8_t _pop;
    uint8_t _size;
  public:
     static enum{
      NO_DATA_TO_READ=0x00,
      BUFFER_FULL=0x00
    } ERRORS;
    RingBuffer(const uint8_t size):_size(size)
    {
      _buffer = new uint8_t[_size];
      _insert=_pop=0;
    }
    virtual ~RingBuffer()
    {
      delete[] _buffer;
    }
    inline void writeByte(const uint8_t& data) __attribute__((always_inline))
    {
      _buffer[_insert] = data;
      _insert++;
      if(_insert<_size)
      {
      }
      else
      {
        _insert=(_insert)%_size;
      }
    }
    inline void readByte(uint8_t& data) __attribute__((always_inline))
    {
      data = _buffer[_pop];
      _pop++;
      if(_pop<_size)
      {

      }
      else
      {
        _pop = (_pop)%_size;
      }
    }
    inline uint8_t getWriteBuffLength() __attribute__((always_inline))
    {
      return (_size - _insert + _pop-1);
    }
    inline uint8_t getReadBuffLength() __attribute__((always_inline))
    {
      return ((int8_t)(_size + _insert - _pop));
    }
};
#endif
