#include "bsp_wifi.h"
#include <esp_wifi.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "main.h"
#include <HTTPUpdate.h>

const char * host = "FCS152";
WebServer server(80);
const char * serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
String upUrl = "http://fcs.fun/filedownload/78941";

// true:  Automatic configuration connection is successful
// false: Automatic configuration connection failed
bool autoConfig() {
    const char * defaultWifiConfig = "FCS152WiFi";
    LCD_ShowString0608(0, 0, "Connecting           ", 1, 128);
    WiFi.disconnect(true, true);
    // WiFi.begin(ssid, password);
    WiFi.begin(defaultWifiConfig, defaultWifiConfig);

    for (size_t i = 0; i < 25; i++) {
        int wifiStatus = WiFi.status();
        if (wifiStatus == WL_CONNECTED) {
            D_printf("Auto Connecting Successfully!!!!!!!!");
            return true;
        }
        delay(200);
        D_printf("Connecting...");
    }
    D_printf("Cannot Connect~~~~~~~~~~~");
    return false;
}

// CLR2LAST:  Smart configuration failed (will not arrive)
// ENT2LAST:  Successful smart configuration
// BACK2MAIN: Cancel the smart configuration and return to the menu
int ConfigSmartWiFi() {
    Serial.println("Start SmartWIFI Config:");
    WiFi.beginSmartConfig();
    while (1) {
        Serial.print(".");
        delay(100);
        if (WiFi.smartConfigDone()) {
            Serial.println("SmartConfig Successfully!!!!!!!!!!\n");
            Serial.printf("SSID:%s\n", WiFi.SSID().c_str());
            Serial.printf("PSW:%s\n", WiFi.psk().c_str());
            return ENT2LAST;
        }
        switch (Encoder_Switch_Scan(0)) {
        // case key_double:
        //     return BACK2MAIN;
        //     break;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }
        if (Matrix_KEY_Scan(0) == MATRIX_RESULT_CLR) {
            return BACK2MAIN;
        }
    }
    return CLR2LAST; //Will not arrive
}

//Print wifi domain name and IP
void LCD_ShowAddressIP(void) {
    LCD_ShowString0608(0, 0, " http://FCS152.local ", 1, 128);
    char IP_SHOW[128] = {0};
    uint32_t ip_tmp = WiFi.localIP();
    sprintf(IP_SHOW, "%d.%d.%d.%d",
            (ip_tmp & 0xff),
            (ip_tmp >> 8) & 0xff,
            (ip_tmp >> 16) & 0xff,
            (ip_tmp >> 24) & 0xff);
    LCD_ShowString0608(0, 1, IP_SHOW, 1, 128);
}

// true : Successfully configured the server
// false: wifi disconnect
bool ConfigUpdateProcess() {
#if 01
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("*********Start Server*******\n");
        MDNS.begin(host);
        server.on("/", HTTP_GET, []() {
            server.sendHeader("Connection", "close");
            server.send(200, "text/html", serverIndex);
        });
        server.on(
        "/update", HTTP_POST, []() {
            server.sendHeader("Connection", "close");
            server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            delay_ms(1000);
            ResetSystem();
        },
        []() {
            HTTPUpload & upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.setDebugOutput(true);
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin()) {
                    // start with max available size
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) {
                    // true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                } else {
                    Update.printError(Serial);
                }
                Serial.setDebugOutput(false);
            } else {
                Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
            }
        });
        server.begin();
        MDNS.addService("http", "tcp", 80);

        Serial.printf("Ready! Open http://%s.local in your browser\n", host);
        LCD_ShowAddressIP();
        return true;
    }
#else
    Serial.println("start update");
    WiFiClient UpdateClient;
    t_httpUpdate_return ret = httpUpdate.update(UpdateClient, upUrl);
    switch (ret) {
    case HTTP_UPDATE_FAILED:                    //When the upgrade fails
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:                //When there is no upgrade
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:                        //When the upgrade is successful
        Serial.println("[update] Update ok.");
        LCD_Clear(GLOBAL32);
        LCD_ShowString0608(0, 2, "Update OK!", 1, 128);
        delay_ms(1000);
        ResetSystem();
        break;
    }
#endif

    D_printf("WiFi Failed");
    return false;
}

void StartServer(void) {
    if (!ConfigUpdateProcess()) {
        LCD_Clear(GLOBAL32);
        LCD_ShowString0608(0, 2, "WIFI DISCONNECTED    ", 1, 128);
        delay_ms(1500);
        return;
    }
    while (1) {
        switch (Encoder_Switch_Scan(0)) {
        case key_double:
            return;

        case key_long:
            D_printf("<<<<<<SHUTING>>>>>>\n");
            SHUT();
            break;
        }

        if (Matrix_KEY_Scan(0) == MATRIX_RESULT_CLR) {
            Serial.printf("Clean Config\n");
            LCD_ShowString0608(0, 0, "Need to reconfig     ", 1, 128);
            LCD_ShowString0608(0, 1, "                     ", 1, 128);
            if (WiFi.smartConfigDone()) {
                WiFi.stopSmartConfig();                    // Serial.printf("stopSmart:%d\n", WiFi.stopSmartConfig());
            }
            // WiFi.disconnect(false, false);              // Serial.printf("WIFI DISCON:%d\n", WiFi.disconnect(false, false));
            WiFi.disconnect(false, true);
            if (ConfigSmartWiFi() == BACK2MAIN) {          //Cancel the smart configuration and return to the menu
                WiFi.stopSmartConfig();
                return;
            } else {                                       //Successfully re-intelligent configuration
                if (!ConfigUpdateProcess()) {
                    LCD_Clear(GLOBAL32);
                    LCD_ShowString0608(0, 0, "WIFI DISCONNECTED    ", 1, 128);
                    delay_ms(1500);
                    return;
                }
            }
        }
        server.handleClient();
        delay(2);                                          //Allow the CPU to switch to other tasks
    }
}

void ConfigureToUpdate(void) {
    LCD_Clear(GLOBAL32);
    LCD_ShowString0608(43, 2, "PROGRAM", 1, 128);
#ifdef NEWBOOT
    LCD_ShowString0608(19, 3, VERSION_BOOT, 1, 128);
#endif
#ifdef UPBOOT
    LCD_ShowString0608(0, 3, VERSION_UPBOOT, 1, 128);
#endif
    if (!autoConfig()) {
        Serial.println("Need to reconnected!");
        LCD_ShowString0608(0, 0, "Need to reconfig     ", 1, 128);
        WiFi.mode(WIFI_AP_STA);
        if (ConfigSmartWiFi() == BACK2MAIN) {
            return;
        }
    }
    StartServer();
    WiFi.disconnect(true, false);

    // ESP.restart();
}
