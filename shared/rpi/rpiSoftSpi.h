class rpiSoftSPI
{
public:
    rpiSoftSPI(int clockPin,
               int chipSelectPin,
               int misoPin,
               int mosiPin,
               bool masterDrivesChipSelect = true)
    : _clockPin(clockPin)
    , _chipSelectPin(chipSelectPin)
    , _misoPin(misoPin)
    , _mosiPin(mosiPin)
    , _clockDelay(5)
    , _byteDelay(2)
    , _masterDrivesChipSelect(masterDrivesChipSelect)
    {};

    ~rpiSoftSPI() {};
    void init();
    bool isSelected();
    void transfer(unsigned char* buffer, int size);

private:
    int _clockPin;
    int _chipSelectPin;
    int _misoPin;
    int _mosiPin;
    int _clockDelay;
    int _byteDelay;
    bool _masterDrivesChipSelect;

    void setClock(int value);
    void setChipSelect(int value);
    int getChipSelect();
    void setMosi(int value);
    int getMiso();
};
