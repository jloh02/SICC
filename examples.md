## SICC Examples
### Unidirection Sending and Receiving
Sends a message from A to B

Transmitter (A) Code:
``` cpp
#include <SICC.h>
SICC sicc(8,9);
void setup() {
  Serial.begin(9600);
}
char res[256];
void loop() {
  Serial.println("Start");
  res[0]=0;
  if(!sicc.receive(res,1000000)) Serial.println("Timeout");
  else Serial.println(res);    
  delay(80);
} 
```

Receiver (B) Code: 
``` cpp
#include <SICC.h>
SICC sicc(8,9,100);
void setup() {
  Serial.begin(9600);
}
void loop() {  
  if(!sicc.send("Hi Jon, this is is a really long sentence to tell you it works. Congratulations; Here are some chars @#$%^&*() - Enjoy!")) Serial.println("Send failed");
  else Serial.println("Sent");
  delay(100);
}

```

### Bidirection Sending and Receiving
Sends a message from A to B. A string is concatenated to the message and sent from B to A

Transmitter (A) Code: 
``` cpp
#include <SICC.h>
void setup() {
  Serial.begin(9600);
  SICC sicc(8,9);
  Serial.println("Start");
  sicc.send("Does it work?",1000000);
  char res[256];
  sicc.receive(res,1000000);
  Serial.println(res);
}
void loop() {}
```

Receiver (B) Code: 
``` cpp
#include <SICC.h>
void setup() {
  Serial.begin(9600);
  SICC sicc(8, 9);
  char res[256];
  sicc.receive(res,1000000);
  Serial.println(res);
  strcat(res, " Yes it does! Congrats!");
  sicc.send(res,1000000);
}
void loop() {}
```