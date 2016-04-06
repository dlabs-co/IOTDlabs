#include <SoftwareSerial.h>

long bps=9600;
SoftwareSerial mySerial(10, 11); // RX, TX
void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(bps);
  mySerial.begin(bps);
  
  Serial.println("Goodnight moon!");
  
  mySerial.println("AT");

}

void loop() // run over and over
{
  while (mySerial.available()){
    Serial.write(mySerial.read());
  }
  delay(500);
  
  while (Serial.available()){
    mySerial.write(Serial.read());
  }
  delay(500);
}
