//
//  ThreeWireSPI.h
//
//
//  Created by Michael Hill on 4/8/17.
//
//

#ifndef ThreeWireSPI_h
#define ThreeWireSPI_h

class ThreeWireSPI
{
public:
    ThreeWireSPI() {};
    ~ThreeWireSPI() {};

    void init();
    int send(unsigned char* buffer, int size);
    int receive(unsigned char* buffer, int size);

private:
    void setOutput();
    void setInput();
    void setMiso(unsigned char bit);
    void sendCommand(unsigned char cmd, int size);

    void sendBytes(unsigned char* bytes, int size);
    void recvBytes(unsigned char* bytes, int size);
};

#endif /* ThreeWireSPI_hpp */
