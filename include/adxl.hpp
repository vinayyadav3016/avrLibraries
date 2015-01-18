#ifndef ADXL_HPP
#define ADXL_HPP

#include <ring_buffer.hpp>
#include<cpluscplus_use.hpp>
#include <adc.hpp>

class ADXL : public Adc
{
  protected:
    float _x;
    float _y;
    float _z;
    uint8_t _c;
    inline void autoTriggerMethod() __attribute__((always_inline))
    {
      autoTrigger();
      enableISR();
      // SFIOR for mega32
      // Timer0 Compare Match
      //ADCSRB| = (0<<ADTS2)|(1<<ADTS1)|(1<<ADTS0);
      // Time0 Overflow
      ADCSRB|=(1<<ADTS2)|(0<<ADTS1)|(0<<ADTS0);
      // Timer1 Compare Match
      //ADCSRB| = (1<<ADTS2)|(0<<ADTS1)|(1<<ADTS0);
      // Time1 Overflow
      //ADCSRB| = (1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0);
      ADCSRA|=(1<<ADSC);
    }
  public:
    ADXL():_x(0),_y(0),_z(0),_c(0)
    {
      setADC(this);
      autoTriggerMethod();
    }
    virtual ~ADXL()
    {
    }
    inline float getX() __attribute__((always_inline))
    {
      return _x;
    }
    inline float getY() __attribute__((always_inline))
    {
      return _y;
    }
    inline float getZ() __attribute__((always_inline))
    {
      return _z;
    }
    void readSensor()
    {
    }
    void doISR(uint16_t data)
    {
      //TIFR0|=(1<<TOV0);
      switch(_c)
      {
        case 0:
          _c=1;
          _x = (data-395.0)/80.0;
          break;
        case 1:
          _c=2;
          _y = (data-395.0)/80.0;
          break;
        case 2:
          _c=0;
          _z = (data -400.0)/80.0;
          break;
      }
      ADMUX&=~((1<<MUX1)|(1<<MUX0));
      ADMUX|=_c;
      PORTB=~PORTB;
    }
};

#endif
