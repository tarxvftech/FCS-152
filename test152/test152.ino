#define FREERTOS_CONFIG_XTENSA_H
#include "main.h"  
#include "input.h"
#include "view.h"

#include <AutoConnect.h>
WebServer http(80); //We can use this to support setting channels and such from a web client on the same network. modified by AutoConnect below but shouldn't interfere with our own use of it.
AutoConnect portal(http); //captive portal with dns capture to allow
//wifi clients to connect to us and allow easy configuration of wifi settings.
//should also allow setting from radio eventually, and from serial a little sooner
unsigned long portaltimeoutms = 100;
AutoConnectConfig portalconfig("FCS152", "thankyouforopensourcing!", portaltimeoutms); //no channel is set - unsure if it would cause problems to have a channel for the portal set when the STA mode is connected to something else
void rootPage(){
    char content[] = "<html><head></head><body>"
    "<h1>FCS-152</h2>"
    "<a href='/_ac'>Wifi Settings Portal</a>"
    "<p>Hello World!</p>"
    "</body></html>";
    http.send(200, "text/html", content);
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

    http.on("/", rootPage); //anything in addition to the captive portal
    LCD_ShowPICALL(pic_XVF);
    char hostname[32] = "";
    snprintf(hostname, 32, "FCS152-%llX", ESP.getEfuseMac());
    portalconfig.hostName = hostname;
    portalconfig.autoReconnect = true; //connect to any already-saved wifi on first-start
    portalconfig.autoReset = false; //don't reset after wifi disconnect
    portalconfig.autoRise = false; //keep the captive portal enabled on soft-ap when connected successfully in sta mode?
    portalconfig.retainPortal = false; //keep captive portal up after timeout?
    portalconfig.preserveAPMode = true; //keep the softAP up after connected in sta mode?
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
    MY_GLOBAL_FUN();
    /*VFO_Refresh();*/
    /*Encoder_process(Encoder_Switch_Scan(0));*/
    /*Argument_process(Event_Matrix(Matrix_KEY_Scan(0)));	//matrix button event*/
    /*KDU_Processor();*/
    input_scan();
    if( now > last_draw_ts + 100 ){ 
        ui_draw(&root); 
        last_draw_ts = now;
    }
    portal.handleClient(); //also calls all the webserver handles and such
}
