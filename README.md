# IOTDlabs
IOTDlabs es una librería de Arduino para establecer comunicaciones WiFi con un módulo ESP8266.

Esta librería simplifica algunos aspectos de la comunicación por WiFi para la impartición de talleres en el Arduino Day Zgz 2016.


class IOTDlabs {
 
  /*--------Properties-------------*/
    int _baud;

    char* ssid;
    char* password;
    char* _host;
    int  _porthost;

    int _iduser;
    char* _user;
    int _iddevice;
    char* sensormsg;

    String _web_dir = "/";
    String _web_file = "index.php";



    /**----------------------------------------------------------------------
    -------------------------Properties Definition
    --------------------------------------------------------------------------
    */

    void set_host(char* host);
    void set_port(int port);

    void set_webdir(char* webdir);
    void set_webfile(char* webfile);

    void set_iduser(int iduser);
    void set_user(char* user);
    void set_idevice(int iddevice);

    void set_GET(bool bool_get);
    void set_POST(bool bool_post);

    /*-----------Methods--------------*/
    /** ---------------------------------------------------------------------
    -----------------------Basic IOTDlabs Functions Definition
    -----------------------------------------------------------------------
    */

    void sniff();
    void ping(char* ping);
    /** ---------------------------------------------------------------------
    -----------------------Complex IOTDlabs Functions Definition
    -----------------------------------------------------------------------
    */
    int init();

    void setMode(int mode);

    void simpleGET(String line);
    void simplePOST(String line);
    void sendMSG();
    void sendhttpGET();
    void sendhttpPOST();
    
    
    
Esta librería es una modificación de ITEADLIB_Arduino_WeeESP8266.

https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
