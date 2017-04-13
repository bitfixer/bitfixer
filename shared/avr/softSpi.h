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
    softSpi(bool driveChipSelect)
    : _driveChipSelect(driveChipSelect)
    {};
    
    ~softSpi() {};
    
    void init();
    void send(unsigned char sendbyte);
    unsigned char read();
private:
    bool _driveChipSelect;
    
    void setMiso(unsigned char bit);
};

#endif /* softSpi_hpp */
