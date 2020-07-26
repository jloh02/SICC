#include <Arduino.h>

#ifndef SICC_h
#define SICC_h

#define CORRUPTION_CODE 255
#define TIMEOUT_CODE 254

#define BIT_TIMEOUT 12000    //Hardcoded timeout per bit (Possible bug if receive() consistently returns TIMEOUT_CODE)

class SICC
{
private:
    unsigned long startTime; //Used for timeouts
    int DELAY_TIME, clock_line, data_line;
    void isSending(bool state); //Set pinModes
    byte readByte(); //255 represents restart signal
    bool writeByte(byte c); //Returns ACK or NACK
    void sendRestart(); //Send restart signal

public:
    SICC(uint8_t SCL, uint8_t SDA, int delay=100); //Constructor
    bool send(char* s); //Send without timeout (timeout = (2^32-1)us ~ 71 mins)
    bool send(char* s, unsigned long timeout); //Send message
    bool receive(char* recvBuf, unsigned long timeout); //Receive message, timeout in nanosecs
    void setDelay(int delayMicros); //Set message delay in microseconds
};

#endif
