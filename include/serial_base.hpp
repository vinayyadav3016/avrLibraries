/*! \file serial_base.hpp
 */
#ifndef __SERIAL_BASE_HPP__
#define __SERIAL_BASE_HPP__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ring_buffer.hpp>

/*! \class
 */
class SerialBase {
	protected:
		/*!
		 */
		RingBuffer _reciever;
		/*!
		 */
		RingBuffer _transmitter;
		////////////////////////////////////////////////////////////
		/// Make protected to stop accidents
		/*!
		 */
		SerialBase(uint8_t rec_size,uint8_t trans_size): _reciever(rec_size), _transmitter(trans_size) {
		}
		///////////////////////////////////////////////////////////
		/// you must define these function in your class
		/*!
		 */
		inline void initUSART(const int baudrate) const{};
		/*!
		 */
		inline void startUSART() const{};
		/*!
		 */
		inline void stopTransmission() const {};
		///////////////////////////////////////////////////////////
		/// this is made to enable start transmission
		/*!
		 */
		virtual void startTransmission() const{};
		///////////////////////////////////////////////////////////
		// This is to destroy the UART module
		/*!
		 */
		virtual void stopUSART(){};
		///////////////////////////////////////////////////////////
		/// sender and reciever functions 
		/*!
		 */
		uint8_t sendChunk(const uint8_t* data,const uint8_t length) {
			uint8_t len = _transmitter.getWriteBuffLength();
			len = MIN(len,length);
			for(uint8_t i=0;i<len;i++) {
				_transmitter.writeByte(data[i]);
			}
			startTransmission();
			return len;
		}
		/*!
		 */
		uint8_t readChunk(uint8_t* data,const uint8_t length) {
			uint8_t len=_reciever.getReadBuffLength();
			len = MIN(len,length);
			for(uint8_t i=0;i<len;i++) {
				_reciever.readByte(data[i]);
			}
			return len;
		}
	public:
		/*!
		 */
		virtual ~SerialBase() {
			stopUSART();
		}
		///////////////////////////////////////////////////////////
		//// Recieving Functions
		/*!
		 */
		inline uint8_t readByte(uint8_t& data) __attribute__((always_inline)) {
			return readChunk(&data,1);
		}
		/*!
		 */
		inline uint8_t readBuffer(uint8_t* data,const uint8_t length) __attribute__((always_inline)) {
			return readChunk(data,length);
		}
		/*!
		 */
		uint8_t readFrame(uint8_t* data, const uint8_t length,const uint8_t& start='0',const uint8_t& end='\n') {
			int16_t count = _reciever.searchByte('0');
			if(count!=-1) {
				int16_t count1 = _reciever.searchByte('\n',count);
				if(count1!=-1) {
					_reciever.fastForwardRead(count+1);
					uint8_t buff = readChunk(data,count1-count+1);
					return buff==0?buff:buff-1; 
				}
			}
			return 0;
		}
		///////////////////////////////////////////////////////////
		//// Transmission Functions
		/*!
		 */
		inline uint8_t send() __attribute__((always_inline)) {
			return send("\r\n");
		}
		inline uint8_t send(const uint8_t& data) __attribute__((always_inline)) {
			return sendChunk(&data,1);
		}
		/*!
		 */
		inline uint8_t send(const uint8_t* data,const uint8_t length) __attribute__((always_inline)) {
			return sendChunk(data,length);
		}
		/*!
		 */
		inline uint8_t send(const char* data) __attribute__((always_inline)) {
			return sendChunk(reinterpret_cast<const uint8_t * >(data),strlen(data));
		}
		/*!
		 */
		uint8_t send(const int& data) {
			char str[10];
			uint8_t length = sprintf(str,"%d",data);
			return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
		}
		/*!
		 */
		uint8_t send(const double& data,const uint8_t decimal=2) {
			char str[10];
			int d = fabs(data);
			double frac = fabs(data-d);
			int frc = static_cast<int>(frac*pow(10,decimal));
			uint8_t length=0;
			if(data<0) {
			length = sprintf(str,"-%d.%d",d,frc);
			} else {
			length = sprintf(str,"%d.%d",d,frc);
			}
			return sendChunk(reinterpret_cast<const uint8_t *>(str),length);
		}
		///////////////////////////////////////////////////////////
		/// you must overwrite these functions in your class
		/*!
		 */
		inline void doUDRISR() __attribute__((always_inline)) {
		}
		/*!
		 */
		void doRXISR(const uint8_t& data) {
			if(_reciever.getWriteBuffLength()>0) {
				_reciever.writeByte(data);
			}
		}
		/*!
		 */
		inline void doTXISR() {
		}
		///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////
};

#endif//__SERIAL_BASE_HPP__
