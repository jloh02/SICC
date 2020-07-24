#include "SICC.h"

SICC::SICC(uint8_t SCL, uint8_t SDA, int delay/*=100*/){
    DELAY_TIME = delay;
    clock_line = SCL;
    data_line = SDA;
}
bool SICC::send(char* s, long timeout){
    startTime = micros();
    isSending(true);

    //Write 'start'
    digitalWrite(clock_line,1);
    digitalWrite(data_line,0);
    delayMicroseconds(DELAY_TIME);

    bool ack = writeByte(strlen(s));

    //Iterate through each character
    for (int i = 0; i < strlen(s) && ack; i++) {
        char c = s[i];
        ack = writeByte(c);
    }
    if(!ack) {
        sendRestart();
    }

    //End sequence
    digitalWrite(clock_line,0);
    digitalWrite(data_line,1);

    delayMicroseconds(DELAY_TIME);

    //Release wires
    digitalWrite(clock_line,1);
    digitalWrite(data_line,1);
    
    delayMicroseconds(DELAY_TIME);

    if(micros()-startTime>timeout) return false;

    if(!ack) {
        delay(10);
        return SICC::send(s, timeout-micros()+startTime);
    }

    return true;
}
void SICC::receive(char* recvBuf, long timeout){
    isSending(false);

    //Wait for start condition
    startTime = micros();
    while(digitalRead(data_line) || !digitalRead(clock_line)) {
        if(micros()-startTime>timeout){
            recvBuf[0] = TIMEOUT_CODE;
            recvBuf[1] = 0;
            return;
        }
    }
    while(digitalRead(clock_line)) {
        if(micros()-startTime>timeout){
            recvBuf[0] = TIMEOUT_CODE;
            recvBuf[1] = 0;
            return;
        }
    }

    //Receive size of data
    byte numChar = readByte();
    if(numChar == TIMEOUT_CODE) {
        recvBuf[0] = TIMEOUT_CODE;
        recvBuf[1] = 0;
        return;
    }
    bool corrupt = numChar == CORRUPTION_CODE;
    
    //Receive bytes
    for (int i = 0; i < numChar && !corrupt; i++) {
        recvBuf[i] = char(readByte());
        if(recvBuf[i] == TIMEOUT_CODE) {
            recvBuf[0] = TIMEOUT_CODE;
            recvBuf[1] = 0;
            return;
        }
        corrupt = recvBuf[i] == CORRUPTION_CODE;        
    }
    recvBuf[numChar] = 0;

    if(corrupt) {
        delay(5);
        SICC::receive(recvBuf, timeout-micros()+startTime);
    }
}

bool SICC::writeByte(byte c){
    for (int j = 0; j < 8; j++) {
        byte b = bitRead(c, 7-j);
        digitalWrite(clock_line, 0);
        digitalWrite(data_line, b);
        delayMicroseconds(DELAY_TIME);
        digitalWrite(clock_line, 1);
        delayMicroseconds(DELAY_TIME);
    }

    digitalWrite(clock_line,0);
    pinMode(data_line,INPUT_PULLUP); //Release data line to let it float for ACK/NACK   
    delayMicroseconds(DELAY_TIME);
    digitalWrite(clock_line,1);    
    bool ret = !digitalRead(data_line); //Read ACK
    delayMicroseconds(DELAY_TIME);   
    pinMode(data_line,OUTPUT);  //Return back to original state
    digitalWrite(clock_line,0);
    return ret;
}
byte SICC::readByte(){
    int a = 0;
    for (int i = 0; i < 8; i++) {
        startTime = micros();   //Timer for timeouts

        //Read bits when clock_line == 1
        while (!digitalRead(clock_line)) if(micros()-startTime>BIT_TIMEOUT) return TIMEOUT_CODE;
        bool origVal = digitalRead(data_line);
        bool prevVal = origVal;
        int numFlip = 0;
        while (digitalRead(clock_line)){
            if(micros()-startTime>BIT_TIMEOUT) return TIMEOUT_CODE;
            if(digitalRead(data_line) != prevVal) {
                prevVal = !prevVal;
                numFlip++;
            }
        }
        if(numFlip > 3) return CORRUPTION_CODE;
        a = (a<<1) | origVal;
    }

    startTime = micros();
    pinMode(data_line,OUTPUT); //Control data line to send ACK  
    digitalWrite(data_line,0); //Send ACK
    while(!digitalRead(clock_line))if(micros()-startTime>BIT_TIMEOUT) return TIMEOUT_CODE;
    while (digitalRead(clock_line))if(micros()-startTime>BIT_TIMEOUT) return TIMEOUT_CODE;    
    pinMode(data_line,INPUT_PULLUP); //Release data line to let it float for next byte of data

    return a;
}

void SICC::sendRestart(){
    pinMode(12,OUTPUT);
    digitalWrite(12,HIGH);

    bool state = false;
    digitalWrite(clock_line, 0);
    digitalWrite(data_line, state);
    delayMicroseconds(DELAY_TIME);
    digitalWrite(clock_line, 1);
    for(int i=0;i<10;i++){
        digitalWrite(data_line, state=!state);
        delayMicroseconds(DELAY_TIME/10);
    }
}
void SICC::isSending(bool state){
    if(state){  
        pinMode(clock_line, OUTPUT);
        pinMode(data_line, OUTPUT); 
        return;
    }
    pinMode(clock_line, INPUT_PULLUP);
    pinMode(data_line, INPUT_PULLUP); 
}


bool isByteChar(char c, byte b){
    return (byte)c == b;
}
bool isTimeout(char* recv){
    return isByteChar(recv[0],TIMEOUT_CODE);
}