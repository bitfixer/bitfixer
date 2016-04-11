#ifndef __SID_H
#define __SID_H

#include <inttypes.h>
#include <math.h>
#include "SerialPort.hpp"

/* Define a structure with the sid register offsets */
struct __sid_voice {
    //uint16_t   	    freq;	/* Frequency */
    //uint16_t 		pw;		/* Pulse width */
    
    unsigned char   freq_low;
    unsigned char   freq_high;
    unsigned char   pw_low;
    unsigned char   pw_high;
    unsigned char	ctrl;	/* Control register */
    unsigned char	ad;		/* Attack/decay */
    unsigned char	sr;		/* Sustain/release */
};
struct __sid {
    //struct __sid_voice	v1;     	/* Voice 1 */
    //struct __sid_voice 	v2;     	/* Voice 2 */
    //struct __sid_voice	v3;     	/* Voice 3 */
    struct __sid_voice v[3];
    //uint16_t		flt_freq;   	/* Filter frequency */
    unsigned char   flt_freq_low;
    unsigned char   flt_freq_high;
    unsigned char	flt_ctrl;	/* Filter control register */
    unsigned char	amp;		/* Amplitude */
    unsigned char	ad1;		/* A/D converter 1 */
    unsigned char   ad2;		/* A/D converter 2 */
    unsigned char	noise;		/* Noise generator */
    unsigned char	read3;		/* Value of voice 3 */
};

class Sid
{
public:
    Sid() {}
    virtual void init();
    
    struct __sid *getData()
    {
        return &s;
    }
    
    typedef enum
    {
        NOISE = 0x80,
        PULSE = 0x40,
        SAWTOOTH = 0x20,
        TRIANGLE = 0x10
    } waveForm;

    void setVolume(float volume);
    void setFrequency(int voice, float freq);
    void setAttack(int voice, float attack);
    void setDecay(int voice, float decay);
    void setSustain(int voice, float sustain);
    void setRelease(int voice, float release);
    
    void setWaveform(int voice, waveForm wf);
    void noteOn(int voice);
    void noteOff(int voice);
    
    virtual void update() {};
    
private:
    struct __sid s;
    
    void bytesForFrequency(float freq, unsigned char &low, unsigned char &high)
    {
        float fn = freq / 0.06097;
        uint16_t fn_int = (uint16_t)floor(fn);
        low = fn_int & 0x00FF;
        high = (fn_int & 0xFF00) >> 8;
    }
    
    waveForm wfForVoice[3];
};

void send_control_packet(SerialPort *port, unsigned char type, unsigned char offset, unsigned char val);

class SidController : public Sid
{
public:
    SidController(SerialPort *p);
    void init();
    void update();
    
private:
    unsigned char type = 1;
    SerialPort *port = NULL;
    struct __sid sid_state;
};


/* End of _sid.h */
#endif



