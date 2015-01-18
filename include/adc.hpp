#ifndef ADC_HPP
#define ADC_HPP

#include <cpluscplus_use.hpp>
#include <avr/io.h>
class Adc
{
  protected:
    // enable ISR
    inline void enableISR() __attribute__((always_inline))
    {
      ADCSRA|=(1<<ADIE);
    }
    // disable ISR
    inline void disableISR() __attribute__((always_inline))
    {
      ADCSRA&=~(1<<ADIE);
    }
    // enable auto trigger
    inline void autoTrigger() __attribute__((always_inline))
    {
      ADCSRA|=(1<<ADATE);
    }
    // disable auto trigger
    inline void stopTrigger() __attribute__((always_inline))
    {
      ADCSRA&=~(1<<ADATE);
    }
    // initialize ADC
    inline void initADC() __attribute__((always_inline))
    {
      // AVCC selected put capacitor between VREF and GND
      ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR);
      // Operating frequency 250Khz;
      ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);
    }
    //
    inline void stopAdc() __attribute__((always_inline))
    {
      ADCSRA=0;
    }
    // method for autotrigger
    virtual void autoTriggerMethod(){};
  public:
    // constructor
    Adc()
    {
      initADC();
    }
    // main method to read Analog data
    inline uint16_t readAnalog(uint8_t mux) __attribute__((always_inline))
    {
      // Clear ADMUX flag
      ADMUX&=~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
      // Set ADMUX channel
      ADMUX|=mux;
      // start conversion
      ADCSRA|=(1<<ADSC);
      // wait for conversion to finish
      while(!(ADCSRA & (1<<ADIF)));
      // clear conversion flag
      ADCSRA|=(1<<ADIF);
      // return value
      return (ADC);
    }
    virtual ~Adc()
    {
      disableISR();
      stopTrigger();
      stopAdc();
    }
    // over ride this if using ISR
    virtual void doISR(uint16_t data)
    {
    }
};

Adc* _adc;

void setADC(Adc* adc)
{
  _adc = adc;
}

Adc* getADC()
{
  if(_adc==NULL)
  {
    _adc = new Adc();
  }
  return _adc;
}

void resetADC()
{
  delete _adc;
}

ISR(ADC_vect)
{
  _adc->doISR(ADC);
}

#endif
