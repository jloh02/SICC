#include <Arduino.h>

#ifndef SICC_h
#define SICC_h

#define CORRUPTION_CODE 255
#define TIMEOUT_CODE 254

#define BIT_TIMEOUT (DELAY_TIME*3)    //Hardcoded timeout per bit

class SICC
{
private:
    long startTime; //Used for timeouts
    int DELAY_TIME, clock_line, data_line;
    void isSending(bool state); //Set pinModes
    byte readByte(); //255 represents restart signal
    bool writeByte(byte c); //Returns ACK or NACK
    void sendRestart(); //Send restart signal

public:
    SICC(uint8_t SCL, uint8_t SDA, int delay=100); //Constructor
    bool send(char* s); //Send message
    void receive(char* recvBuf, long timeout); //Receive message, timeout in nanosecs
    void setDelay(int delayMicros); //Set message delay in microseconds
};

bool isTimeout(char* recv); //Utility function to check if string is timeout response (Uses isByteChar(char c, byte b) utility function)
#endif
