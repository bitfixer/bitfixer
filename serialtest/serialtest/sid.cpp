//
//  sid.cpp
//  serialtest
//
//  Created by Michael Hill on 4/8/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "sid.h"

void send_control_packet(SerialPort *port, unsigned char type, unsigned char offset, unsigned char val)
{
    printf("sending: %d %d %d\n", type, offset, val);
    
    port->send(&type, 1);
    port->send(&offset, 1);
    port->send(&val, 1);
}

SidController::SidController(SerialPort *p) :
port(p)
{
    // create sid class
}

void SidController::init()
{
    Sid::init();
    
    memset((void *)&sid_state, 255, sizeof(struct __sid));
    update();
}

void SidController::update()
{
    int numBytes = sizeof(struct __sid);
    unsigned char *siddata = (unsigned char *)getData();
    unsigned char *state = (unsigned char *)&sid_state;
    
    for (int i = 0; i < numBytes; i++)
    {
        if (state[i] != siddata[i])
        {
            send_control_packet(port, type, i, siddata[i]);
        }
        state[i] = siddata[i];
    }
}

void Sid::init()
{
    memset((void *)&s, 0, sizeof(struct __sid));
    
    for (int i = 0; i < 3; i++)
    {
        wfForVoice[i] = TRIANGLE;
    }
}

void Sid::setVolume(float volume)
{
    unsigned char v = (unsigned char)float(volume * 16.0);
    if (v == 16)
        v = 15;
    
    s.amp = v;
    update();
}

void Sid::setFrequency(int voice, float freq)
{
    bytesForFrequency(freq, s.v[voice].freq_low, s.v[voice].freq_high);
    update();
}

void Sid::setAttack(int voice, float attack)
{
    unsigned char ad = s.v[voice].ad;
    // remove attack value
    ad &= 0x0F;
 
    unsigned char att = (unsigned char)floor(attack * 16.0);
    if (att == 16)
        att = 15; // cap at 15 (2^4)
    
    att <<= 4;
    ad |= att;
    
    s.v[voice].ad = ad;
    update();
}

void Sid::setDecay(int voice, float decay)
{
    unsigned char ad = s.v[voice].ad;
    // remove decay value
    ad &= 0xF0;
    
    unsigned char dec = (unsigned char)floor(decay * 16.0);
    if (dec == 16)
        dec = 15; // cap at 15 (2^4)
    
    ad |= dec;
    
    s.v[voice].ad = ad;
    update();
}

void Sid::setSustain(int voice, float sustain)
{
    unsigned char sr = s.v[voice].sr;
    
    sr &= 0x0F;
    
    unsigned char sus = (unsigned char)floor(sustain * 16.0);
    if (sus == 16)
        sus = 15; // cap at 15 (2^4)
    
    sus <<= 4;
    sr |= sus;
    
    s.v[voice].sr = sr;
    update();
}

void Sid::setRelease(int voice, float release)
{
    unsigned char sr = s.v[voice].sr;
    sr &= 0xF0;
    
    unsigned char rel = (unsigned char)floor(release * 16.0);
    if (rel == 16)
        rel = 15; // cap at 15 (2^4)
    
    sr |= rel;
    
    s.v[voice].sr = sr;
    update();
}

void Sid::setWaveform(int voice, waveForm wf)
{
    wfForVoice[voice] = wf;
}

void Sid::noteOn(int voice)
{
    unsigned char val = wfForVoice[voice] + 1;
    s.v[voice].ctrl = val;
    update();
}

void Sid::noteOff(int voice)
{
    unsigned char val = wfForVoice[voice];
    s.v[voice].ctrl = val;
    update();
}


