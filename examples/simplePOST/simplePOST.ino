#include <SoftwareSerial.h>
#include "IOTDlabs.h"

#define SSID        "SSID"
#define PASSWORD    "PASSWORD"
#define HOST        "192.168.1.1"
SoftwareSerial RXTX(10,11);
IOTDlabs wifi(RXTX,SSID,PASSWORD,HOST);

char* cmd = "$";
String postdata = "valorPOST";

void setup(void)
{
    Serial.begin(9600);
    wifi.init();

    wifi.set_webdir("/");
    wifi.set_webfile("index.php");
    Serial.println(wifi._web_dir);
    Serial.println(wifi._web_file);
    delay(1000);

}

void loop(void)
{
  while (RXTX.available()){
    Serial.write(RXTX.read());
  }
  delay(500);
  //Hay que seleccionar No ajuste de linea desde el monitor serial
  while (Serial.available()){
      String data = Serial.readString();
      if (data == cmd){
         Serial.println("SendPOST");
         wifi.simplePOST(postdata);
      }else{
         Serial.println("SendAT");
         RXTX.println(data);
      }
   }
      
  delay(500);
}

