//
//  softSpi.hpp
//  
//
//  Created by Michael Hill on 4/8/17.
//
//

#ifndef softSpi_h
#define softSpi_h

class softSpi
{
public:
    softSpi() {};
    ~softSpi() {};
    
    void init();
    void send();
    unsigned char read();
};

#endif /* softSpi_hpp */
