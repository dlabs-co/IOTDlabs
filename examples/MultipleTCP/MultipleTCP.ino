#include <SoftwareSerial.h>;
#include "IOTDlabs.h"
 
#define SSID        "MOVISTAR_58E8"
#define PASSWORD    "YNF3AWNC4FEJJ4AFKVKK"
#define HOST        "192.168.1.40"
SoftwareSerial RXTX(10,11);
IOTDlabs wifi(RXTX,SSID,PASSWORD,HOST);
char* cmd = "$";
 
void setup(void)
{
    Serial.begin(9600);
    wifi.init();
     
    //Establecer directorio y pagina del servidor
    wifi.set_webdir("/iot/get");
    wifi.set_webfile("get.php");
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
         Serial.println("SendMUXTCP");
 
         //Solo soporta un parametro en la Arduino UNO
         wifi.multipleTCP();
      }else{
         Serial.println("SendAT");
         RXTX.println(data);
      }
   }
       
  delay(500);
}
