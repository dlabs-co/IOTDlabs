#include <SoftwareSerial.h>
#include "IOTDlabs.h"

#define SSID        "SSID"
#define PASSWORD    "PASSWORD"
#define HOST        "192.168.1.1"
SoftwareSerial RXTX(10,11);
IOTDlabs wifi(RXTX,SSID,PASSWORD,HOST);

char* cmd = "$";
String postdata = "holaguapo me gustaria conocerte moreno";
void setup(void)
{
    Serial.begin(9600);
    wifi.init();

    wifi.set_webdir("/");
    wifi.set_webfile("index.php");
}

void loop(void)
{
  while (RXTX.available()){
    Serial.write(RXTX.read());
  }
  delay(500);

  while (Serial.available()){
      String data = Serial.readString();

      //Hay que seleccionar No ajuste de linea en el Monitor Serial
      if (data == cmd){
         Serial.println("SendGET");
         wifi.simplePOST(postdata);
      }else{
         Serial.println("SendAT");
         RXTX.println(data);
      }
   }
      
  delay(500);
}

