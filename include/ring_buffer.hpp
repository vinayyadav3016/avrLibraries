#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include "cpluscplus_use.hpp"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

class RingBuffer
{
  private:
    uint8_t* _buffer;
    uint8_t _insert;
    uint8_t _pop;
    uint8_t _size;
  public:
    enum{
      NO_DATA_TO_READ=0x01,
      BUFFER_FULL=0x02
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
    inline void writeByte(const uint8_t& data)
    {
      _buffer[_insert] = data;
      _insert=(_insert+1)%_size;
    }
    inline void readByte(uint8_t& data)
    {
      data = _buffer[_pop];
      _pop = (_pop+1)%_size;
    }
    inline uint8_t getWriteBuffLength()
    {
      return (_size - _insert + _pop)%(_size+1);
    }
    inline uint8_t getReadBuffLength()
    {
      return ((int8_t)(_insert - _pop))%(_size+1);
    }
};
#endif
