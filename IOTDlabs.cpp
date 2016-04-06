 /**
 * @file IOTDlabs.cpp
 * @Xtension for database Connection of class ESP8266. 
 * @author Adrián Blasco<adrian.blascarr@gmail.com> 
 * @date 2016.03
 * 
 * @ Copyright:
 * Copyright (c) 2016 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "IOTDlabs.h"
#include <avr/pgmspace.h>

#define LOG_OUTPUT_DEBUG            (1)
#define LOG_OUTPUT_DEBUG_PREFIX     (1)

#define SSID        "SSID"
#define PASSWORD    "Password"

#define HOST_NAME "192.168.1.1"
#define HOST_PORT  (80)
 #define bufferchr 50

#define logDebug(arg)\
    do {\
        if (LOG_OUTPUT_DEBUG)\
        {\
            if (LOG_OUTPUT_DEBUG_PREFIX)\
            {\
                Serial.print("[LOG Debug: ");\
                Serial.print((const char*)__FILE__);\
                Serial.print(",");\
                Serial.print((unsigned int)__LINE__);\
                Serial.print(",");\
                Serial.print((const char*)__FUNCTION__);\
                Serial.print("] ");\
            }\
            Serial.print(arg);\
        }\
    } while(0)

    #ifdef IOTDlabs_USE_SOFTWARE_SERIAL
        IOTDlabs::IOTDlabs(SoftwareSerial &uart, uint32_t baud): m_puart(&uart)
        {
            m_puart->begin(baud);
            rx_empty();
            IOTDlabs::ssid = SSID;
            IOTDlabs::password = PASSWORD;
            IOTDlabs::_host = HOST_NAME;
            IOTDlabs::_porthost = HOST_PORT;
        }

        IOTDlabs::IOTDlabs(SoftwareSerial &uart, char* network, char* password, uint32_t baud): m_puart(&uart)
        {
            m_puart->begin(baud);
            rx_empty();
            IOTDlabs::ssid = network ;
            IOTDlabs::password =  password ;
            IOTDlabs::_host = HOST_NAME;
            IOTDlabs::_porthost = HOST_PORT;
        }
        IOTDlabs::IOTDlabs(SoftwareSerial &uart, char* network, char* password,char* host,int host_port, uint32_t baud):m_puart(&uart){
            m_puart->begin(baud);
            rx_empty();
            IOTDlabs::ssid = network;
            IOTDlabs::password = password;
            IOTDlabs::_host = host;
            IOTDlabs::_porthost = HOST_PORT;
        }
    #else
        IOTDlabs::IOTDlabs(HardwareSerial &uart, uint32_t baud): m_puart(&uart)
        {
            m_puart->begin(baud);
            rx_empty();
        }
    #endif

int IOTDlabs::init(){

    Serial.print("Connecting: ");

    int JAP = IOTDlabs::joinAP(IOTDlabs::ssid, IOTDlabs::password);
    if (JAP) {
        Serial.print("Join AP success\r\n");
        Serial.print("ESP8266 IP: ");       
        Serial.println(IOTDlabs::getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
    }

    IOTDlabs::_mux = IOTDlabs::disableMUX();
    if (IOTDlabs::_mux) {
        Serial.print("single OK\r\n");

    } else {
        Serial.print("single ERROR\r\n");
    }
    
    IOTDlabs::setMode(3);

    Serial.print("setup end\r\n");
    return JAP;
};

void IOTDlabs::setMode(int mode){
    switch (mode){
        case 1:

            if (IOTDlabs::setOprToStation()) {
                Serial.print("Station Mode OK\r\n");
            } else {
                Serial.print("Station Mode ERROR\r\n");
            };
            
        break;

        case 2:
            if (IOTDlabs::setOprToSoftAP()) {
                Serial.print("AP Mode OK\r\n");
            } else {
                Serial.print("AP Mode ERROR\r\n");
            };
        break;

        case 3:
            if (IOTDlabs::setOprToStationSoftAP()) {
                Serial.print("AP + Station Mode OK\r\n");
            } else {
                Serial.print("AP + Station Mode ERROR\r\n");
            };
        break;

        default:
            Serial.println("Only 1-3 values accepted");
        break;

    }    
}

void IOTDlabs::set_host(char* host){
        IOTDlabs::_host = host;
};

void IOTDlabs::set_port(int port){
        IOTDlabs::_porthost = port;
};

void IOTDlabs::set_webdir(char* webdir){
        IOTDlabs::_web_dir = webdir;
};

void IOTDlabs::set_webfile(char* webfile){
        IOTDlabs::_web_file = webfile;
};

void IOTDlabs::set_GET(bool bool_get){
        IOTDlabs::_GET_method = bool_get;
};

void IOTDlabs::set_POST(bool bool_post){
        IOTDlabs::_POST_method = bool_post;
};

void IOTDlabs::set_iduser(int iduser){
        IOTDlabs::_iduser = iduser;
}
void IOTDlabs::set_user(char* user){
        IOTDlabs::_user = user;
}
void IOTDlabs::set_idevice(int iddevice){
        IOTDlabs::_iddevice = iddevice;
}


void IOTDlabs::sniff(){
        Serial.println("Wifi List");
        Serial.println("AP Secure, AP Name, AP Strength, AP MAC, Channel");
        Serial.println("0--> OPEN, 1--> WEP, 2--> WPA-PSK, 3--> WPA2-PSK, 4-->WPA-WPA2-PSK\n");
        Serial.println(IOTDlabs::getAPList());
}

void IOTDlabs::ping(char* ping){
    if(IOTDlabs::setPing(ping)){
        Serial.print("Ping connected to ");
        Serial.print(ping);
        Serial.println(" Success");
    }else{
        Serial.print("Ping connected to ");
        Serial.print(ping);
        Serial.println(" Fail");
    }
}

void IOTDlabs::sendMSG(){


    if (IOTDlabs::createTCP(IOTDlabs::_host, IOTDlabs::_porthost)) {
        Serial.print("create tcp OK\r\n");
    } else {
        Serial.print("create tcp ERROR\r\n");
    }

    //uint8_t buffer[100] = {0};
    String cadena = "POST "+ IOTDlabs::_web_dir +"/"+ IOTDlabs::_web_file+" HTTP/1.1\r\nHost: "+IOTDlabs::_host+"\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: "+ 57+"\r\n\r\nid="+IOTDlabs::_iduser+"&user="+IOTDlabs::_user+"&idsensor="+IOTDlabs::_iddevice+"&valuesensor="+IOTDlabs::sensormsg+"&action=submit\r\n";
    Serial.println(cadena);
    
    char *hello;
    IOTDlabs::send((const uint8_t*)cadena.c_str(), strlen(cadena.c_str()));

    /*uint32_t len = IOTDlabs::recv(buffer, sizeof(buffer), 20000);
    Serial.println(len);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }*/

};

void IOTDlabs::simpleGET(String line){

    Serial.println(line);
    if (IOTDlabs::createTCP(IOTDlabs::_host, IOTDlabs::_porthost)) {
        Serial.print("create tcp OK\r\n");
    } else {
        Serial.print("create tcp ERROR\r\n");
    }

    uint8_t buffer[100] = {0};
    String cadena = "GET "+ IOTDlabs::_web_dir +"/"+ IOTDlabs::_web_file+"?p="+line+" HTTP/1.1\r\nHost: "+IOTDlabs::_host+"\r\nConnection: close\r\n\r\n";

    Serial.println(cadena);
    
    IOTDlabs::send((const uint8_t*)cadena.c_str(), strlen(cadena.c_str()));

    uint32_t len = IOTDlabs::recv(buffer, sizeof(buffer), 10000);
    Serial.println(len);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

};

void IOTDlabs::simplePOST(String line){

    if (IOTDlabs::createTCP(IOTDlabs::_host, IOTDlabs::_porthost)) {
        Serial.print("create tcp OK\r\n");
    } else {
        Serial.print("create tcp ERROR\r\n");
        return;
    }

    uint8_t buffer[100] = {0};
    
    char cadena[150]="POST ";
    strcat(cadena,IOTDlabs::_web_dir.c_str());
    strcat(cadena,"/");
    strcat(cadena,IOTDlabs::_web_file.c_str());
    strcat(cadena," HTTP/1.1\r\nHost: ");
    strcat(cadena,IOTDlabs::_host);
    strcat(cadena,"\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ");
    strcat(cadena,String(line.length()+2).c_str());
    strcat(cadena,"\r\n\r\np=");
    strcat(cadena,line.c_str());

    Serial.println(cadena);
    
    IOTDlabs::send((const uint8_t*)cadena, strlen(cadena));

    uint32_t len = IOTDlabs::recv(buffer, sizeof(buffer), 20000);
    Serial.println(len);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

};

/* Dynamic entry for HTTP Request*/
void IOTDlabs::sendhttpGET(){

    if (IOTDlabs::createTCP(IOTDlabs::_host, IOTDlabs::_porthost)) {
        Serial.print("create tcp OK\r\n");
    } else {
        Serial.print("create tcp ERROR\r\n");
        exit;
    }

    Serial.println("Escribe el numero de parametros de entrada");

    while (!Serial.available()){
        
    }
    String snvalues = Serial.readString();
    int nvalues = snvalues.toInt();

    uint8_t buffer[150] = {0};
    String cadena = "GET "+ IOTDlabs::_web_dir +"/"+ IOTDlabs::_web_file+"?" ;

    for(uint32_t i = 0; i < nvalues; i++) {
        Serial.println("Write the key: "+String(i+1));
        while (!Serial.available()){
            
        }
        String key = Serial.readString();
        char keychr[bufferchr];
        key.toCharArray(keychr, bufferchr) ;

        Serial.println("Write the value of that key: "+String(i+1));
        while (!Serial.available()){
            
        }
        String value = Serial.readString();
        char valuechr[bufferchr];
        value.toCharArray(valuechr, bufferchr) ;
        int numvalue = value.toFloat();

        key.replace(" ","%20");
        key.replace("ñ","%C3%B1");
        value.replace(" ","%20");
        value.replace("ñ","%C3%B1");
        cadena = cadena + key + "=" +value+"&";
    }

    cadena = cadena + " HTTP/1.1\r\nHost: "+IOTDlabs::_host+"\r\nConnection: close\r\n\r\n";
    
    Serial.println(cadena);
    
    IOTDlabs::send((const uint8_t*)cadena.c_str(), strlen(cadena.c_str()));

    uint32_t len = IOTDlabs::recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
}


void IOTDlabs::sendhttpPOST(){

    if (IOTDlabs::createTCP(IOTDlabs::_host, IOTDlabs::_porthost)) {
        Serial.print("create tcp OK\r\n");
    } else {
        Serial.print("create tcp ERROR\r\n");
    }

    Serial.println("Escribe el numero de parametros de entrada");

    while (!Serial.available()){
        
    }
    String snvalues = Serial.readString();
    int nvalues = snvalues.toInt();

    String cadena = "POST "+ IOTDlabs::_web_dir +"/"+ IOTDlabs::_web_file ;
    cadena = cadena + " HTTP/1.1\r\nHost: "+IOTDlabs::_host+"\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
    Serial.println(cadena);
    String post_cadena;
    uint8_t buffer[200] = {0};
    for(uint32_t i = 0; i < nvalues; i++) {
        Serial.println("Write the key: "+i);
        while (!Serial.available()){
            
        }
        String key = Serial.readString();
        char keychr[bufferchr];
        key.toCharArray(keychr, bufferchr) ;

        Serial.println("Write the value of that key: "+i);
        while (!Serial.available()){
            
        }
        String value = Serial.readString();
        char valuechr[bufferchr];
        value.toCharArray(valuechr, bufferchr) ;

        key.replace(" ","%20");
        key.replace("ñ","%C3%B1");
        value.replace(" ","%20");
        value.replace("ñ","\%C3%B1");
        post_cadena = post_cadena + key + "=" +value+"&";
    }
    cadena = cadena + String(post_cadena.length()) + "\r\n\r\n"+ post_cadena + "\r\n";

    Serial.println(cadena);
    
    IOTDlabs::send((const uint8_t*)cadena.c_str(), strlen(cadena.c_str()));

    uint32_t len = IOTDlabs::recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
};


void IOTDlabs::multipleTCP(){
    char* cmd = "$";

    if (IOTDlabs::enableMUX()) {
        Serial.print("Multiple TCP OK\r\n");
    } else {
        Serial.print("Multiple TCP ERROR\r\n");
    }

    while (!Serial.available() ){
        
        Serial.println("Write module number to connect in network (max number : 0 ~ 4) or press $ to exit");

        while (!Serial.available()){
            
        }
        String sndevice = Serial.readString();

        if (sndevice == cmd){
            break;
        }
        int ndevice = sndevice.toInt();

        Serial.println("Write IP number module number to connect in network");

        while (!Serial.available()){
            
        }
        String snIP = Serial.readString();
        String IP = "\""+snIP+"\"";
        Serial.println("Write Port module number to connect in network");

        while (!Serial.available()){
            
        }
        String snPort = Serial.readString();
        int nPort = snPort.toInt();

        if (IOTDlabs::createTCP(ndevice, snIP, nPort)) {
            Serial.print("SUCCESS on TCP connection for IP: ");
            Serial.print(snIP);
            Serial.print(" on channel: ");
            Serial.println(ndevice);
            Serial.println("Write message to module: ");

            while (!Serial.available()){
                
            }
            String message = Serial.readString();
            IOTDlabs::sendTCP(ndevice, message);

        } else {
            Serial.print("FAIL on TCP connection for IP: ");
            Serial.print(snIP);
            Serial.print(" on channel: ");
            Serial.println(ndevice);
        }
    }
    Serial.println("Write any Command");
}

void IOTDlabs::sendTCP(uint8_t mux_id, String msg, uint32_t timeout ){

    uint8_t buffer[128] = {0};

    if (IOTDlabs::send(mux_id,(const uint8_t*)msg.c_str(), strlen(msg.c_str()))) {
        Serial.println("send OK");
    } else {
        Serial.println("send ERROR");
    }

    uint32_t len = IOTDlabs::recv(mux_id, buffer, sizeof(buffer), timeout);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
 
    if (IOTDlabs::releaseTCP(mux_id)) {
        Serial.print("release tcp ");
        Serial.print(mux_id);
        Serial.println(" OK");
    } else {
        Serial.print("release tcp ");
        Serial.print(mux_id);
        Serial.println(" ERROR");
    }
}

/*---------------------------------------------------------------------------------*/
/*--------------------------ITEADLIB_Arduino_WeeESP8266----------------------------*/
/*----------------https://github.com/itead/ITEADLIB_Arduino_WeeESP8266-------------*/

/**--------------------------------------------------------------------------------*/

bool IOTDlabs::kick(void)
{
    return eAT();
}

bool IOTDlabs::restart(void)
{
    unsigned long start;
    if (eATRST()) {
        delay(2000);
        start = millis();
        while (millis() - start < 3000) {
            if (eAT()) {
                delay(1500); /* Waiting for stable */
                return true;
            }
            delay(100);
        }
    }
    return false;
}

String IOTDlabs::getVersion(void)
{
    String version;
    eATGMR(version);
    return version;
}

bool IOTDlabs::setEcho(uint8_t mode)
{
    return eATE(mode);
}

bool IOTDlabs::restore(void)
{
    return eATRESTORE();
}
bool IOTDlabs::setUart(uint32_t baudrate,uint8_t pattern)
{
    return eATSETUART(baudrate,pattern);
}

bool IOTDlabs::deepSleep(uint32_t time)
{
    return eATGSLP(time);
}


bool IOTDlabs::setOprToStation(uint8_t pattern1,uint8_t pattern2)
{
    uint8_t mode;
    if (!qATCWMODE(&mode,pattern1)) {
        return false;
    }
    if (mode == 1) {
        return true;
    } else {
        if (sATCWMODE(1,pattern2)){
            return true;
        } else {
            return false;
        }
    }
}
String IOTDlabs::getWifiModeList(void)
{   
     String list;
     eATCWMODE(list);
     return list;
}
bool IOTDlabs::setOprToSoftAP(uint8_t pattern1,uint8_t pattern2)
{
    uint8_t mode;
    if (!qATCWMODE(&mode,pattern1)) {
        return false;
    }
    if (mode == 2) {
        return true;
    } else {
        if (sATCWMODE(2,pattern2) ){
            return true;
        } else {
            return false;
        }
    }
}

bool IOTDlabs::setOprToStationSoftAP(uint8_t pattern1,uint8_t pattern2)
{
    uint8_t mode;
    if (!qATCWMODE(&mode,pattern1)) {
        return false;
    }
    if (mode == 3) {
        return true;
    } else {
        if (sATCWMODE(3,pattern2) ){
            return true;
        } else {
            return false;
        }
    }
}

uint8_t IOTDlabs::getOprMode(uint8_t pattern1)
{
    uint8_t mode;
    if (!qATCWMODE(&mode,pattern1)) {
        return 0;
    } else {
        return mode;
    }
}

String IOTDlabs::getNowConecAp(uint8_t pattern)
{   
     String ssid;
     qATCWJAP(ssid,pattern);
     return ssid;
}


String IOTDlabs::getAPList(void)
{
    String list;
    eATCWLAP(list);
    return list;
}

bool IOTDlabs::joinAP(String ssid, String pwd,uint8_t pattern)
{
    return sATCWJAP(ssid, pwd,pattern);
}

bool IOTDlabs::leaveAP(void)
{
    return eATCWQAP();
}

String IOTDlabs::getSoftAPParam(uint8_t pattern)
{   
     String list;
     qATCWSAP(list,pattern);
     return list;
}



bool IOTDlabs::setSoftAPParam(String ssid, String pwd, uint8_t chl, uint8_t ecn,uint8_t pattern)
{
    return sATCWSAP(ssid, pwd, chl, ecn,pattern);
}

String IOTDlabs::getJoinedDeviceIP(void)
{
    String list;
    eATCWLIF(list);
    return list;
}

String IOTDlabs::getDHCP(uint8_t pattern)
{   
     String dhcp;
     qATCWDHCP(dhcp,pattern);
     return dhcp;
}
bool IOTDlabs::setDHCP(uint8_t mode, uint8_t en, uint8_t pattern)
{
    return sATCWDHCP(mode, en, pattern);
}

bool IOTDlabs::setAutoConnect(uint8_t en)
{
    return eATCWAUTOCONN(en);
}
String IOTDlabs::getStationMac(uint8_t pattern)
{
    String mac;
    qATCIPSTAMAC(mac,pattern);
    return mac;
}


bool IOTDlabs::setStationMac(String mac,uint8_t pattern)
{
   return eATCIPSTAMAC(mac,pattern);
}

String IOTDlabs::getStationIp(uint8_t pattern)
{
    String ip;
    qATCIPSTAIP(ip,pattern);
    return ip;
}

bool IOTDlabs::setStationIp(String ip,String gateway,String netmask,uint8_t pattern)
{
   return eATCIPSTAIP(ip,gateway,netmask,pattern);
}

String IOTDlabs::getAPIp(uint8_t pattern)
{
    String ip;
    qATCIPAP(ip,pattern);
    return ip;
}

bool IOTDlabs::setAPIp(String ip,uint8_t pattern)
{
   return eATCIPAP(ip,pattern);
}

bool IOTDlabs::startSmartConfig(uint8_t type)
{
    return eCWSTARTSMART(type);
}

bool IOTDlabs::stopSmartConfig(void)
{
    return eCWSTOPSMART();
}




String IOTDlabs::getIPStatus(void)
{
    String list;
    eATCIPSTATUS(list);
    return list;
}

String IOTDlabs::getLocalIP(void)
{
    String list;
    eATCIFSR(list);
    return list;
}

bool IOTDlabs::enableMUX(void)
{
    return sATCIPMUX(1);
}

bool IOTDlabs::disableMUX(void)
{
    return sATCIPMUX(0);
}

bool IOTDlabs::createTCP(String addr, uint32_t port)
{
    return sATCIPSTARTSingle("TCP", addr, port);
}

bool IOTDlabs::releaseTCP(void)
{
    return eATCIPCLOSESingle();
}

bool IOTDlabs::registerUDP(String addr, uint32_t port)
{
    return sATCIPSTARTSingle("UDP", addr, port);
}

bool IOTDlabs::unregisterUDP(void)
{
    return eATCIPCLOSESingle();
}

bool IOTDlabs::createTCP(uint8_t mux_id, String addr, uint32_t port)
{
    return sATCIPSTARTMultiple(mux_id, "TCP", addr, port);
}

bool IOTDlabs::releaseTCP(uint8_t mux_id)
{
    return sATCIPCLOSEMulitple(mux_id);
}

bool IOTDlabs::registerUDP(uint8_t mux_id, String addr, uint32_t port)
{
    return sATCIPSTARTMultiple(mux_id, "UDP", addr, port);
}

bool IOTDlabs::unregisterUDP(uint8_t mux_id)
{
    return sATCIPCLOSEMulitple(mux_id);
}

bool IOTDlabs::setTCPServerTimeout(uint32_t timeout)
{
    return sATCIPSTO(timeout);
}

bool IOTDlabs::startTCPServer(uint32_t port)
{
    if (sATCIPSERVER(1, port)) {
        return true;
    }
    return false;
}

bool IOTDlabs::stopTCPServer(void)
{
    sATCIPSERVER(0);
    restart();
    return false;
}

bool IOTDlabs::setCIPMODE(uint8_t mode)
{
    return sATCIPMODE(mode);
}

bool IOTDlabs::saveTransLink (uint8_t mode,String ip,uint32_t port)
{
    return eATSAVETRANSLINK(mode,ip,port);
}

bool IOTDlabs::setPing(String ip)
{
    return eATPING(ip);
}




bool IOTDlabs::startServer(uint32_t port)
{
    return startTCPServer(port);
}

bool IOTDlabs::stopServer(void)
{
    return stopTCPServer();
}

bool IOTDlabs::send(const uint8_t *buffer, uint32_t len)
{
    return sATCIPSENDSingle(buffer, len);
}

bool IOTDlabs::sendFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
    return sATCIPSENDMultipleFromFlash(mux_id, buffer, len);
}

bool IOTDlabs::sendFromFlash(const uint8_t *buffer, uint32_t len)
{
    return sATCIPSENDSingleFromFlash(buffer, len);
}

bool IOTDlabs::send(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
    return sATCIPSENDMultiple(mux_id, buffer, len);
}

uint32_t IOTDlabs::recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
    return recvPkg(buffer, buffer_size, NULL, timeout, NULL);
}

uint32_t IOTDlabs::recv(uint8_t mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
    uint8_t id;
    uint32_t ret;
    ret = recvPkg(buffer, buffer_size, NULL, timeout, &id);
    if (ret > 0 && id == mux_id) {
        return ret;
    }
    return 0;
}

uint32_t IOTDlabs::recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
    return recvPkg(buffer, buffer_size, NULL, timeout, coming_mux_id);
}

/*----------------------------------------------------------------------------*/
/* +IPD,<id>,<len>:<data> */
/* +IPD,<len>:<data> */

uint32_t IOTDlabs::recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
    String data;
    char a;
    int32_t index_PIPDcomma = -1;
    int32_t index_colon = -1; /* : */
    int32_t index_comma = -1; /* , */
    int32_t len = -1;
    int8_t id = -1;
    bool has_data = false;
    uint32_t ret;
    unsigned long start;
    uint32_t i;
    
    if (buffer == NULL) {
        return 0;
    }
    
    start = millis();
    while (millis() - start < timeout) {
        if(m_puart->available() > 0) {
            a = m_puart->read();
            data += a;
        }
        
        index_PIPDcomma = data.indexOf("+IPD,");
        if (index_PIPDcomma != -1) {
            index_colon = data.indexOf(':', index_PIPDcomma + 5);
            if (index_colon != -1) {
                index_comma = data.indexOf(',', index_PIPDcomma + 5);
                /* +IPD,id,len:data */
                if (index_comma != -1 && index_comma < index_colon) { 
                    id = data.substring(index_PIPDcomma + 5, index_comma).toInt();
                    if (id < 0 || id > 4) {
                        return 0;
                    }
                    len = data.substring(index_comma + 1, index_colon).toInt();
                    if (len <= 0) {
                        return 0;
                    }
                } else { /* +IPD,len:data */
                    len = data.substring(index_PIPDcomma + 5, index_colon).toInt();
                    if (len <= 0) {
                        return 0;
                    }
                }
                has_data = true;
                break;
            }
        }
    }
    
    if (has_data) {
        i = 0;
        ret = len > buffer_size ? buffer_size : len;
        start = millis();
        while (millis() - start < 3000) {
            while(m_puart->available() > 0 && i < ret) {
                a = m_puart->read();
                buffer[i++] = a;
            }
            if (i == ret) {
                rx_empty();
                if (data_len) {
                    *data_len = len;    
                }
                if (index_comma != -1 && coming_mux_id) {
                    *coming_mux_id = id;
                }
                return ret;
            }
        }
    }
    return 0;
}

void IOTDlabs::rx_empty(void) 
{
    while(m_puart->available() > 0) {
        m_puart->read();
    }
}

String IOTDlabs::recvString(String target, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_puart->available() > 0) {
            a = m_puart->read();
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target) != -1) {
            break;
        }   
    }
    
    return data;
}

String IOTDlabs::recvString(String target1, String target2, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_puart->available() > 0) {
            a = m_puart->read();
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        }
    }
    return data;
}

String IOTDlabs::recvString(String target1, String target2, String target3, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_puart->available() > 0) {
            a = m_puart->read();
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        } else if (data.indexOf(target3) != -1) {
            break;
        }
    }
    return data;
}

bool IOTDlabs::recvFind(String target, uint32_t timeout)
{
    String data_tmp;
    data_tmp = recvString(target, timeout);
    if (data_tmp.indexOf(target) != -1) {
        return true;
    }
    return false;
}

bool IOTDlabs::recvFindAndFilter(String target, String begin, String end, String &data, uint32_t timeout)
{
    String data_tmp;
    data_tmp = recvString(target, timeout);
    if (data_tmp.indexOf(target) != -1) {
        int32_t index1 = data_tmp.indexOf(begin);
        int32_t index2 = data_tmp.indexOf(end);
        if (index1 != -1 && index2 != -1) {
            index1 += begin.length();
            data = data_tmp.substring(index1, index2);
            return true;
        }
    }
    data = data_tmp;
    return false;
}

bool IOTDlabs::eAT(void)
{
    rx_empty();
    m_puart->println(F("AT"));
    return recvFind("OK");
}

bool IOTDlabs::eATRST(void) 
{
    rx_empty();
    m_puart->println(F("AT+RST"));
    return recvFind("OK");
}

bool IOTDlabs::eATGMR(String &version)
{
    rx_empty();
    delay(3000);
    m_puart->println(F("AT+GMR"));
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", version,10000); 
}

bool IOTDlabs::eATGSLP(uint32_t time)
{
    rx_empty();
    m_puart->print(F("AT+GSLP="));
    m_puart->println(time);
    return recvFind("OK");
}


bool IOTDlabs::eATE(uint8_t mode)
{
    rx_empty();
    m_puart->print(F("ATE"));
    m_puart->println(mode);
    return recvFind("OK");
}

bool IOTDlabs::eATRESTORE(void)
{
    rx_empty();
    m_puart->println(F("AT+RESTORE"));
    return recvFind("OK");
}


bool IOTDlabs::eATSETUART(uint32_t baudrate,uint8_t pattern)
{
    rx_empty();
    if(pattern>3||pattern<1){
        return false;
        }
    switch(pattern){
        case 1:
            m_puart->print(F("AT+UART="));
            break;
        case 2:
            m_puart->print(F("AT+UART_CUR="));
            break;
        case 3:
             m_puart->print(F("AT+UART_DEF="));
             break;    
    }
    m_puart->print(baudrate);
    m_puart->print(F(","));
    m_puart->print(8);
    m_puart->print(F(","));
    m_puart->print(1);
    m_puart->print(F(","));
    m_puart->print(0);
    m_puart->print(F(","));
    m_puart->println(0);
    if(recvFind("OK",5000)){

    m_puart->begin(baudrate);
    return true;
    }
    else{
    return false;
    }
 
}


bool IOTDlabs::qATCWMODE(uint8_t *mode,uint8_t pattern) 
{
    String str_mode;
    bool ret;
    if (!mode||!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->println(F("AT+CWMODE_DEF?"));
            break;
        case 2:
            m_puart->println(F("AT+CWMODE_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CWMODE?"));
    }
    ret = recvFindAndFilter("OK", ":", "\r\n\r\nOK", str_mode); 
    if (ret) {
        *mode = (uint8_t)str_mode.toInt();       
        return true;
    } else {
        return false;
    }
}
bool IOTDlabs::eATCWMODE(String &list) 
{
    rx_empty();
    m_puart->println(F("AT+CWMODE=?"));
    return recvFindAndFilter("OK", "+CWMODE:(", ")\r\n\r\nOK", list);
}

bool IOTDlabs::sATCWMODE(uint8_t mode,uint8_t pattern)
{
    if(!pattern){
        return false;
        }
    String data;
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->print(F("AT+CWMODE_DEF="));
            break;
        case 2:
            m_puart->print(F("AT+CWMODE_CUR="));
            break;
        default:
            m_puart->print(F("AT+CWMODE="));
    }
    m_puart->println(mode);
    data = recvString("OK", "no change");

    if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1) {
        return true;
    }
    return false;
}


bool IOTDlabs::qATCWJAP(String &ssid,uint8_t pattern) 
{

    bool ret;
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->println(F("AT+CWJAP_DEF?"));
            break;
        case 2:
            m_puart->println(F("AT+CWJAP_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CWJAP?"));
    }
    ssid = recvString("OK", "No AP");
    if (ssid.indexOf("OK") != -1 || ssid.indexOf("No AP") != -1) {
        return true;
    }
    return false;
 
}

bool IOTDlabs::sATCWJAP(String ssid, String pwd,uint8_t pattern)
{
    String data;
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->print(F("AT+CWJAP_DEF=\""));

            break;
        case 2:
            m_puart->print(F("AT+CWJAP_CUR=\""));
            break;
        default:
            m_puart->print(F("AT+CWJAP=\""));
    }
    
    m_puart->print(ssid);
    m_puart->print(F("\",\""));
    m_puart->print(pwd);
    m_puart->println(F("\""));
    
    data = recvString("OK", "FAIL", 10000);
    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}

bool IOTDlabs::eATCWLAP(String &list) 
{
    String data;
    rx_empty();
    m_puart->println(F("AT+CWLAP"));
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list, 15000);
}




bool IOTDlabs::eATCWQAP(void)
{
    String data;
    rx_empty();
    m_puart->println(F("AT+CWQAP"));
    return recvFind("OK");
}


bool IOTDlabs::qATCWSAP(String &List,uint8_t pattern) 
{
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->println(F("AT+CWSAP_DEF?"));

            break;
        case 2:
            m_puart->println(F("AT+CWSAP_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CWSAP?"));
    }
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", List,10000);

}

bool IOTDlabs::sATCWSAP(String ssid, String pwd, uint8_t chl, uint8_t ecn,uint8_t pattern)
{
    String data;
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern){
         case 1 :
            m_puart->print(F("AT+CWSAP_DEF=\""));

            break;
        case 2:
            m_puart->print(F("AT+CWSAP_CUR=\""));
            break;
        default:
            m_puart->print(F("AT+CWSAP=\""));

    }
    m_puart->print(ssid);
    m_puart->print(F("\",\""));
    m_puart->print(pwd);
    m_puart->print(F("\","));
    m_puart->print(chl);
    m_puart->print(F(","));
    m_puart->println(ecn);
    
    data = recvString("OK", "ERROR", 5000);
    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}

bool IOTDlabs::eATCWLIF(String &list)
{
    String data;
    rx_empty();
    m_puart->println(F("AT+CWLIF"));
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

bool IOTDlabs::qATCWDHCP(String &List,uint8_t pattern) 
{
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern)
    {
        case 1 :
            m_puart->println(F("AT+CWDHCP_DEF?"));
            break;
        case 2:
            m_puart->println(F("AT+CWDHCP_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CWDHCP?"));
    }

    return recvFindAndFilter("OK", "\r\r\n", "\r\nOK", List,10000);

}


bool IOTDlabs::sATCWDHCP(uint8_t mode, uint8_t en, uint8_t pattern)
{
    String data;
    if (!pattern) {
        return false;
    }
    rx_empty();
    switch(pattern){
         case 1 :
            m_puart->print(F("AT+CWDHCP_DEF="));

            break;
        case 2:
            m_puart->print(F("AT+CWDHCP_CUR="));
            break;
        default:
            m_puart->print(F("AT+CWDHCP="));

    }
    m_puart->print(mode);
    m_puart->print(F(","));
    m_puart->println(en);    
    data = recvString("OK", "ERROR", 2000);

    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}


bool IOTDlabs::eATCWAUTOCONN(uint8_t en)
{

    rx_empty();
    if(en>1||en<0){
        return false;
    }
    m_puart->print(F("AT+CWAUTOCONN="));
    m_puart->println(en);
    return recvFind("OK");

}

bool IOTDlabs::qATCIPSTAMAC(String &mac,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->println(F("AT+CIPSTAMAC_DEF?"));

            break;
        case 2:
            m_puart->println(F("AT+CIPSTAMAC_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CIPSTAMAC?"));

    }
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", mac,2000);

}



bool IOTDlabs::eATCIPSTAMAC(String mac,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->print(F("AT+CIPSTAMAC_DEF="));

            break;
        case 2:
            m_puart->print(F("AT+CIPSTAMAC_CUR="));
            break;
        default:
            m_puart->print(F("AT+CIPSTAMAC="));

    }
    m_puart->print(F("\""));
    m_puart->print(mac);
    m_puart->println(F("\""));
    return recvFind("OK");

}

bool IOTDlabs::qATCIPSTAIP(String &ip,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->println(F("AT+CIPSTA_DEF?"));

            break;
        case 2:
            m_puart->println(F("AT+CIPSTA_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CIPSTA?"));

    }
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", ip,2000);

}

bool IOTDlabs::eATCIPSTAIP(String ip,String gateway,String netmask,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->print(F("AT+CIPSTA_DEF="));

            break;
        case 2:
            m_puart->print(F("AT+CIPSTA_CUR="));
            break;
        default:
            m_puart->print(F("AT+CIPSTA="));

    }
    m_puart->print(F("\""));
    m_puart->print(ip);
    m_puart->print(F("\",\""));
    m_puart->print(gateway);
    m_puart->print(F("\",\""));
    m_puart->print(netmask);
    m_puart->println(F("\""));
    return recvFind("OK");

}


bool IOTDlabs::qATCIPAP(String &ip,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->println(F("AT+CIPAP_DEF?"));

            break;
        case 2:
            m_puart->println(F("AT+CIPAP_CUR?"));
            break;
        default:
            m_puart->println(F("AT+CIPAP?"));

    }
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", ip,2000);

}


bool IOTDlabs::eATCIPAP(String ip,uint8_t pattern)
{

    rx_empty();
    if (!pattern) {
        return false;
    }
    switch(pattern){
         case 1 :
            m_puart->print(F("AT+CIPAP_DEF="));

            break;
        case 2:
            m_puart->print(F("AT+CIPAP_CUR="));
            break;
        default:
            m_puart->print(F("AT+CIPAP="));

    }
    m_puart->print(F("\""));
    m_puart->print(ip);
    m_puart->println(F("\""));
    return recvFind("OK");

}


bool IOTDlabs::eCWSTARTSMART(uint8_t type)
{
    rx_empty();
    m_puart->print(F("AT+CWSTARTSMART="));
    m_puart->println(type);
    return recvFind("OK");
}

bool IOTDlabs::eCWSTOPSMART(void)
{
    rx_empty();
    m_puart->println(F("AT+CWSTOPSMART"));
    return recvFind("OK");
}

bool IOTDlabs::eATCIPSTATUS(String &list)
{
    String data;
    delay(100);
    rx_empty();
    m_puart->println(F("AT+CIPSTATUS"));
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}
bool IOTDlabs::sATCIPSTARTSingle(String type, String addr, uint32_t port)
{
    String data;
    rx_empty();
    m_puart->print(F("AT+CIPSTART=\""));
    m_puart->print(type);
    m_puart->print(F("\",\""));
    m_puart->print(addr);
    m_puart->print(F("\","));
    m_puart->println(port);
    
    data = recvString("OK", "ERROR", "ALREADY CONNECT", 10000);
    if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
        return true;
    }
    return false;
}
bool IOTDlabs::sATCIPSTARTMultiple(uint8_t mux_id, String type, String addr, uint32_t port)
{
    String data;
    rx_empty();
    m_puart->print(F("AT+CIPSTART="));
    m_puart->print(mux_id);
    m_puart->print(F(",\""));
    m_puart->print(type);
    m_puart->print(F("\",\""));
    m_puart->print(addr);
    m_puart->print(F("\","));
    m_puart->println(port);
    
    data = recvString("OK", "ERROR", "ALREADY CONNECT", 10000);
    if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
        return true;
    }
    return false;
}
bool IOTDlabs::sATCIPSENDSingle(const uint8_t *buffer, uint32_t len)
{
    rx_empty();
    m_puart->print(F("AT+CIPSEND="));
    m_puart->println(len);
    if (recvFind(">", 5000)) {
        rx_empty();
        for (uint32_t i = 0; i < len; i++) {
            m_puart->write(buffer[i]);
        }
        return recvFind("SEND OK", 10000);
    }
    return false;
}
bool IOTDlabs::sATCIPSENDMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
    rx_empty();
    m_puart->print(F("AT+CIPSEND="));
    m_puart->print(mux_id);
    m_puart->print(F(","));
    m_puart->println(len);
    if (recvFind(">", 5000)) {
        rx_empty();
        for (uint32_t i = 0; i < len; i++) {
            m_puart->write(buffer[i]);
        }
        return recvFind("SEND OK", 10000);
    }
    return false;
}
bool IOTDlabs::sATCIPSENDSingleFromFlash(const uint8_t *buffer, uint32_t len)
{
    rx_empty();
    m_puart->print(F("AT+CIPSEND="));
    m_puart->println(len);
    if (recvFind(">", 5000)) {
        rx_empty();
        for (uint32_t i = 0; i < len; i++) {
            m_puart->write((char) pgm_read_byte(&buffer[i]));
        }
        return recvFind("SEND OK", 10000);
    }
    return false;
}
bool IOTDlabs::sATCIPSENDMultipleFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
    rx_empty();
    m_puart->print(F("AT+CIPSEND="));
    m_puart->print(mux_id);
    m_puart->print(F(","));
    m_puart->println(len);
    if (recvFind(">", 5000)) {
        rx_empty();
        for (uint32_t i = 0; i < len; i++) {
            m_puart->write((char) pgm_read_byte(&buffer[i]));
        }
        return recvFind("SEND OK", 10000);
    }
    return false;
}
bool IOTDlabs::sATCIPCLOSEMulitple(uint8_t mux_id)
{
    String data;
    rx_empty();
    m_puart->print(F("AT+CIPCLOSE="));
    m_puart->println(mux_id);
    
    data = recvString("OK", "link is not", 5000);
    if (data.indexOf("OK") != -1 || data.indexOf("link is not") != -1) {
        return true;
    }
    return false;
}
bool IOTDlabs::eATCIPCLOSESingle(void)
{
    rx_empty();
    m_puart->println(F("AT+CIPCLOSE"));
    return recvFind("OK", 5000);
}
bool IOTDlabs::eATCIFSR(String &list)
{
    rx_empty();
    m_puart->println(F("AT+CIFSR"));
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}
bool IOTDlabs::sATCIPMUX(uint8_t mode)
{
    String data;
    rx_empty();
    m_puart->print(F("AT+CIPMUX="));
    m_puart->println(mode);
    
    data = recvString("OK", "Link is builded");
    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}
bool IOTDlabs::sATCIPSERVER(uint8_t mode, uint32_t port)
{
    String data;
    if (mode) {
        rx_empty();
        m_puart->print(F("AT+CIPSERVER=1,"));
        m_puart->println(port);
        
        data = recvString("OK", "no change");
        if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1) {
            return true;
        }
        return false;
    } else {
        rx_empty();
        m_puart->println(F("AT+CIPSERVER=0"));
        return recvFind("\r\r\n");
    }
}


bool IOTDlabs::sATCIPMODE(uint8_t mode)
{
    String data;
    if(mode>1||mode<0){
        return false;
        }
    rx_empty();
    m_puart->print(F("AT+CIPMODE="));
    m_puart->println(mode);
    
    data = recvString("OK", "Link is builded",2000);
    if (data.indexOf("OK") != -1 ) {
        return true;
    }
    return false;
}




bool IOTDlabs::eATSAVETRANSLINK(uint8_t mode,String ip,uint32_t port)
{

    String data;
    rx_empty();
    m_puart->print(F("AT+SAVETRANSLINK="));
    m_puart->print(mode);
    m_puart->print(F(",\""));
    m_puart->print(ip);
    m_puart->print(F("\","));
    m_puart->println(port);
    data = recvString("OK", "ERROR",2000);
    if (data.indexOf("OK") != -1 ) {
        return true;
    }
    return false;
}



bool IOTDlabs::eATPING(String ip)
{
    rx_empty();
    m_puart->print(F("AT+PING="));
    m_puart->print(F("\""));
    m_puart->print(ip);
    m_puart->println(F("\""));
    return recvFind("OK",2000);
}



bool IOTDlabs::sATCIPSTO(uint32_t timeout)
{
    rx_empty();
    m_puart->print(F("AT+CIPSTO="));
    m_puart->println(timeout);
    return recvFind("OK");
}

