## SICC API
### Constructor
``` cpp
SICC(uint8_t SCL, uint8_t SDA, int delay=100);
```
Initialize clock line and data line ports. Delay is the time between oscillations on the clock line (Defaults to 100 microseconds(us)).

### Sending Data
```cpp
bool SICC::send(char* s, unsigned long timeout=(unsigned long)(-1));
```
Returns if message is acknowledged.

Send a string in the form of character array. Timeout defaults to limit of unsigned long = (2^32-1)us ~ 71 mins in Arduino. A hardcoded timeout for each bit is also given in `SICC.h`.

### Receiving Data
```cpp
bool SICC::receive(char* recvBuf, unsigned long timeout=(unsigned long)(-1));
```
Returns if message is received without timeout.

Receive a string in the form of a pointer to a character array. Timeout defaults to limit of unsigned long = (2^32-1)us ~ 71 mins in Arduino. A hardcoded timeout for each bit is also given in `SICC.h`.

Note: If consistently timing out with excessively long timeout, increase `BIT_TIMEOUT` in `SICC.h`.

### Changing Delay
```cpp
void SICC::setDelay(int delayMicros);
```
Changes the delay in microseconds (us) for clock line oscillation rate. Please ensure that both the receiving and transmitting end change delays before sending messages.