/*! \file ring_buffer.hpp
 */
#ifndef __RING_BUFFER_HPP__
#define __RING_BUFFER_HPP__

#include "cpluscplus_use.hpp"

/*! \class RingBuffer
 */
class RingBuffer {
	private:
		/*!
		 */
		uint8_t* _buffer;
		/*!
		 */
		uint8_t _insert;
		/*!
		 */
		uint8_t _pop;
		/*!
		 */
		uint8_t _size;
	public:
		static enum {
			NO_DATA_TO_READ = 0x00,
			BUFFER_FULL     = 0x01
		} ERRORS;
		/*!
		 */
		RingBuffer(const uint8_t size):_size(size) {
			_buffer = new uint8_t[_size];
			_insert=0;
			_pop=0;//size-1;
		}
		/*!
		 */
		virtual ~RingBuffer() {
			delete[] _buffer;
		}
		/*!
		 */
		inline void writeByte(const uint8_t& data) __attribute__((always_inline)) {
			_buffer[_insert] = data;
			_insert++;
			if(_insert<_size) {
			} else {
				_insert%=_size;
			}
		}
		/*!
		 */
		inline void readByte(uint8_t& data) __attribute__((always_inline)) {
			data = _buffer[_pop];
			_pop++;
			if(_pop<_size) {
			} else {
				_pop %= _size;
			}
			//data = _buffer[_pop];
		}
		/*!
		 */
		inline uint8_t getWriteBuffLength() __attribute__((always_inline)) {
			return (_size - _insert + _pop)%(_size+1);
		}
		/*!
		 */
		inline uint8_t getReadBuffLength() __attribute__((always_inline)) {
			return (_size + _insert - _pop)%(_size);
		}
		/*!
		 */
		int16_t searchByte(const uint8_t& data,const uint8_t start=0) const {
			uint8_t pop = (_pop+start)%(_size);
			while(pop!=_insert) {
				if(_buffer[pop]==data) {
					return (_size - _pop + pop)%(_size);
				}
				pop++;
				pop %=_size;
			}
			return -1;
		}
		/*!
		 */
		inline void fastForwardRead(const uint8_t& count) __attribute__((always_inline)) {
			_pop = (_pop+count)%_size;
		}
		/*!
		 */
		inline uint8_t peek(uint8_t& count, const uint8_t start=0) __attribute__((always_inline)) {
			if(start<getReadBuffLength()) {
				count = _buffer[(_pop+start)%_size];
				return 1;
			}
			return NO_DATA_TO_READ;
		}
};
#endif//__RING_BUFFER_HPP__
