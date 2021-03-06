//
//  softSpi.h
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
    softSpi(bool driveChipSelect = true)
    : _driveChipSelect(driveChipSelect)
    {};

    ~softSpi() {};

    void init();
    int transfer(unsigned char* buffer, int maxsize);
    int transfer_asm(unsigned char* buffer, int maxsize);
private:
    bool _driveChipSelect;
    void setMiso(unsigned char bit);
};

#endif /* softSpi_hpp */
