#include <SoftwareSerial.h>
#include "IOTDlabs.h"

#define SSID        "SSID"
#define PASSWORD    "PASSWORD"
#define HOST        "192.168.1.1"
SoftwareSerial RXTX(10,11);
IOTDlabs wifi(RXTX,SSID,PASSWORD,HOST);
char* cmd = "$";

void setup(void)
{
    Serial.begin(9600);
    wifi.init();
    wifi.set_webdir("/");
    wifi.set_webfile("index.php");
}

void loop(void)
{
   //Modo depuración desde el puerto serie
   while (RXTX.available()){
    Serial.write(RXTX.read());
  }
  delay(500);

  while (Serial.available()){
      String data = Serial.readString();

      //Hay que seleccionar No ajuste de linea desde el monitor Serial para introducir commando
      if (data == cmd){
         Serial.println("SendGET");

         //Solo soporta un parametro en la Arduino UNO
         wifi.sendhttpGET();
      }else{
         Serial.println("SendAT");
         RXTX.println(data);
      }
   }
      
  delay(500);
}

