//
//  xmodem.hpp
//  audioserialterminal
//
//  Created by Michael Hill on 2/28/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#ifndef xmodem_hpp
#define xmodem_hpp

#include "AudioSerial.hpp"

class Xmodem
{
public:
    Xmodem(AudioSerialPort *p);
    void send(const unsigned char *data, int len);
    int recv(char *buffer);
    
//private:
    static uint16_t calcrc(unsigned char *ptr, int count);
    
    AudioSerialPort *port = NULL;
};

#endif /* xmodem_hpp */
