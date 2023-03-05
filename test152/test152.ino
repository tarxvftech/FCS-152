#define FREERTOS_CONFIG_XTENSA_H
#include "main.h"  
#include "input.h"
#include "view.h"
#include <stdlib.h>

#include <WiFi.h>
#include <AutoConnect.h>

#include <ArduinoJson.h>
WebServer http(80); //We can use this to support setting channels and such from a web client on the same network. modified by AutoConnect below but shouldn't interfere with our own use of it.
AutoConnect portal(http); //captive portal with dns capture to allow
//wifi clients to connect to us and allow easy configuration of wifi settings.
//should also allow setting from radio eventually, and from serial a little sooner
AutoConnectConfig portalconfig; 


//https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer/examples
void rootPage(){
    char content[] = "<html><head></head><body>"
    "<h1>FCS-152</h1>"
    "<a href='/_ac'>Wifi Settings Portal</a><br>"
    "<a href='/cps'>Program this radio</a><br>"
    "<a href='/clone'>Share from this radio</a><br>"
    "<a href='https://xvf.is/152'>Source Code and information</a>"
    "</body></html>";
    http.send(200, "text/html", content);
}
void cpsPage(){
    char content[] = "<html><head></head><body>"
    "<h1>FCS-152 embedded CPS</h1>"
    "<script>fetch('/api/channel?c=0',{ method: 'GET'}).then((r)=>r.text()).then(console.log)</script>"
    "</body></html>";
    http.send(200, "text/html", content);
}
void getChannel(){
    if( http.hasArg("c") ){
        String c = http.arg("c");
        long idx = strtol(c.c_str(), NULL, 10);
        StaticJsonDocument<250> jd;
        char buf[250];
        jd.clear(); //necessary?
        JsonObject obj = jd.createNestedObject();
        //load channel number indicated by c
        //fake data here for testing
        obj["idx"] = idx;
        obj["name"] = "ChannelName";
        obj["txf"] = 451.8;
        obj["rxf"] = 451.8;
        serializeJson(jd, buf);
        http.send(200, "application/json", buf);
    } else {
        http.send(200, "application/json", "{}");
    }
}
void setChannel(){
    String body = http.arg("plain");
    StaticJsonDocument<250> jd;
    deserializeJson(jd, body);
    int idx = jd["idx"];
    String name = jd["name"];
    float txf= jd["txf"];
    float rxf= jd["rxf"];
    char content[] = "{\"status\":\"ok\"}";
    http.send(200, "application/json", content);
}
void newChannel(){
    char content[] = "{}";
    http.send(200, "application/json", content);
}

unsigned long now = 0;
unsigned long last_draw_ts = 0;
void setup(){  
    Serial.begin(460800); 
    CH423_Init();       //IIC initialization-->CH423 initialization-->CH423 pin initialization

    SPK_SWITCH(IN, OFF); //limit startup noise

    Key_Init();         //Initialize button: initialize encoder button; detect whether to enter BOOT mode
    Standby_Init();     //Confirm whether the encoder is a normal long press; long press to enable the 3.3V control pin ////PO WER_EN_SET;//

    /*UART1_Init();       //init serial port*/ //skip because we're doing it ourselves up above with Serial.begin
    ADC_Init();         //检测电压使用 'detection voltage use', didn't translate well
    Timer_Init();       //Start timer processing ADC detection voltage program DAC output voltage timer interrupt

    //Initialize the control pin and set the state to avoid wrong IO state
    M62364_Init();      //m62364 initialization-->prohibit sound output/turn off FM power

    LCD_Init();
    PWM_Init();
    LCD_ShowPICALL(pic_XVF);
    Init_Storage();
    enterSecondSystem(); 
    LCD_ShowPICALL(pic_XVF);

    //anything in addition to the captive portal
    //order matters - more specific should be registered first.
    http.on("/api/channel", HTTP_POST, newChannel);  
    http.on("/api/channel", HTTP_PUT, setChannel); 
    http.on("/api/channel", getChannel); 
    http.on("/cps", cpsPage); 
    http.on("/", rootPage); 


    char hostname[32] = "";
    snprintf(hostname, 13, "FCS152-%llX", ESP.getEfuseMac()); //13 to get six characters
    String hostn = hostname;
    WiFi.setHostname(hostname); //the wifi stuff may not have hostname set in time by portal if it already has creds saved for a network to join
    portalconfig.hostName = hostn;

    portalconfig.apid = String(hostname);
    portalconfig.psk = String("thankyouforopensourcing!");
    portalconfig.portalTimeout = 1; //in milliseconds (ms) - low so it doesn't stick around and prevent boot - can't be 0 or it will wait forever to be configured
    portalconfig.channel = 1; //not sure if this'll cause issues with preserveAPMode
    portalconfig.beginTimeout = 10000;  //look for APs for this long (ms)

    portalconfig.autoReconnect = true; //connect to any already-saved wifi on first-start
    portalconfig.autoReset = false; //don't reset after wifi disconnect
    portalconfig.immediateStart = false; //start captive portal even before wifi.begin()
    portalconfig.autoRise = true; //if first wifi.begin fails to connect, do we start captive portal?
    portalconfig.retainPortal = true; //keep captive portal up after timeout?
    portalconfig.preserveAPMode = false; //keep the softAP up after connected in sta mode?
    portalconfig.ota = AC_OTA_BUILTIN; //enable the OTA (over the air) firmware update support in the portal
    portal.config(portalconfig);
    portal.begin();

    EN_GPIO_Init();  //POWER_EN_8_SET;

    SineWave_Data();
    VFO_Load_Data();
    A002_Init();
    /*VFO_Clear();*/
    input_init();
    view_init();
}
void loop(){
    now = millis();
    FeedDog(); //喂狗
    /*MY_GLOBAL_FUN();*/
    /*VFO_Refresh();*/
    /*Encoder_process(Encoder_Switch_Scan(0));*/
    /*Argument_process(Event_Matrix(Matrix_KEY_Scan(0)));	//matrix button event*/
    /*KDU_Processor();*/
    input_scan();
    if( now > last_draw_ts + 200 ){ 
        ui_draw(&root); 
        last_draw_ts = now;
    }
    portal.handleClient(); //also calls all the webserver handles and such
}
