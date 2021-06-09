# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
/*

***************************************************************************

**  Program  : DSMRloggerAPI (restAPI)

*/
# 6 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
/*

**  Copyright (c) 2020 Willem Aandewiel

**

**  TERMS OF USE: MIT License. See bottom of file.

***************************************************************************



  Arduino-IDE settings for DSMR-logger Version 4 (ESP-12):



    - Board: "Generic ESP8266 Module"

    - Builtin Led: "2"  // GPIO02 for Wemos and ESP-12

    - Flash mode: "DOUT" | "DIO"    // changes only after power-off and on again!

    - Flash size: "4MB (FS: 2MB OAT:~1019KB)"  << LET OP! 2MB SPIFFS

    - DebugT port: "Disabled"

    - DebugT Level: "None"

    - IwIP Variant: "v2 Lower Memory"

    - Reset Method: "none"   // but will depend on the programmer!

    - Crystal Frequency: "26 MHz"

    - VTables: "Flash"

    - Flash Frequency: "40MHz"

    - CPU Frequency: "80 MHz" (or 160MHz)

    - Upload Speed: "921600"

    - Erase Flash: "Only Sketch" (When switching from SPIFFS to LIttleFS, one time use "All Flash Contents")

    - Port: <select correct port>

*/
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
/*

**  You can find more info in the following links (all in Dutch):

**   https://willem.aandewiel.nl/index.php/2020/02/28/restapis-zijn-hip-nieuwe-firmware-voor-de-dsmr-logger/

**   https://mrwheel-docs.gitbook.io/dsmrloggerapi/

**   https://mrwheel.github.io/DSMRloggerWS/

*/
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
/******************** compiler options  ********************************************/
//#define USE_REQUEST_PIN           // define if it's a esp8266 with GPIO 12 connected to SM DTR pin

//#define USE_BELGIUM_PROTOCOL      // define if Slimme Meter is a Belgium Smart Meter
//#define USE_PRE40_PROTOCOL        // define if Slimme Meter is pre DSMR 4.0 (2.2 .. 3.0)
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//#define HAS_NO_SLIMMEMETER        // define for testing only!

//#define USE_MINDERGAS             // define if you want to update mindergas (configure through webinterface)
//#define USE_SYSLOGGER             // define if you want to use the sysLog library for debugging
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?

/******************** don't change anything below this comment **********************/

# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 2

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    TelnetStream.print("showValues: ");
    if (i.present())
    {
      TelnetStream.print(Item::name);
      TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
                        (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "59" "." "294" "\", \"aSM\", @progbits, 1 #"))) = (
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
                        ": "
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
                        ); &__pstr__[0];}))
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
                        ))));
      TelnetStream.print(i.val());
      TelnetStream.print(Item::unit());
      //} else
      //{
      //  TelnetStream.print(F("<no value>"));
    }
    TelnetStream.println();
  }
};


//===========================================================================================
void displayStatus()
{
  if (settingOledType > 0)
  {
    switch (msgMode) {
      case 1: snprintf(cMsg, sizeof(cMsg), "Up:%-15.15s", upTime().c_str());
        break;
      case 2: snprintf(cMsg, sizeof(cMsg), "WiFi RSSI:%4d dBm", WiFi.RSSI());
        break;
      case 3: snprintf(cMsg, sizeof(cMsg), "Heap:%7d Bytes", ESP.getFreeHeap());
        break;
      case 4: if (WiFi.status() != WL_CONNECTED)
          snprintf(cMsg, sizeof(cMsg), "**** NO  WIFI ****");
        else snprintf(cMsg, sizeof(cMsg), "IP %s", WiFi.localIP().toString().c_str());
        break;
      default: snprintf(cMsg, sizeof(cMsg), "Telgrms:%6d/%3d", telegramCount, telegramErrors);
        break;
    }

    oled_Print_Msg(3, cMsg, 0);
    msgMode = (msgMode + 1) % 5; //modular 5 = number of message displayed (hence it cycles thru the messages
  }
} // displayStatus()
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
//===========================================================================================
void setup()
{




  Serial.begin(115200, SERIAL_8N1);

  pinMode(2, 0x01);
  pinMode(0, 0x00);




  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(*((volatile uint32_t *)(0x3FF00000+(0x20E44))));
  snprintf(settingHostname, sizeof(settingHostname), "%s", "DSMR-API");
  Serial.printf("\n\nBooting....[%s]\r\n\r\n", String("v2.0.1 (17-04-2020)").c_str());

  if (settingOledType > 0)
  {
    oled_Init();
    oled_Clear(); // clear the screen so we can paint the menu.
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    int8_t sPos = String("v2.0.1 (17-04-2020)").indexOf(' ');
    snprintf(cMsg, sizeof(cMsg), "(c)2020 [%s]", String("v2.0.1 (17-04-2020)").substring(0, sPos).c_str());
    oled_Print_Msg(1, cMsg, 0);

      oled_Print_Msg(2, " LittleFS gebruikt!", 0);

    oled_Print_Msg(3, " >> Have fun!! <<", 1000);
    yield();
  }
  else // don't blink if oled-screen attatched
  {
    for (int I = 0; I < 8; I++)
    {
      digitalWrite(2, !digitalRead(2));
      delay(500);
    }
  }
  digitalWrite(2, 0x1); // HIGH is OFF
  lastReset = ESP.getResetReason();

  startTelnet();
  if (settingOledType > 0)
  {
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(3, "telnet (poort 23)", 2500);
  }

  //================ SPIFFS ===========================================
  if (LittleFS.begin())
  {
    ({ _debugBOL(__FUNCTION__, 195); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "195" "." "295" "\", \"aSM\", @progbits, 1 #"))) = (
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS Mount succesfull\r"
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "195" "." "295" "\", \"aSM\", @progbits, 1 #"))) = (
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS Mount succesfull\r"
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 195 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); }); });
    SPIFFSmounted = true;
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(3, "SPIFFS mounted", 1500);
    }
  } else {
    ({ _debugBOL(__FUNCTION__, 203); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "203" "." "296" "\", \"aSM\", @progbits, 1 #"))) = (
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS Mount failed\r"
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "203" "." "296" "\", \"aSM\", @progbits, 1 #"))) = (
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS Mount failed\r"
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); }); }); // Serious problem with SPIFFS
    SPIFFSmounted = false;
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(3, "SPIFFS FAILED!", 2000);
    }
  }

  //------ read status file for last Timestamp --------------------
  strcpy(actTimestamp, "040302010101X");
  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // place it in actTimestamp
  // set the time to actTimestamp!
  actT = epoch(actTimestamp, strlen(actTimestamp), true);
  ({ _debugBOL(__FUNCTION__, 220); ({ Serial.printf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp , nrReboots++ , slotErrors); TelnetStream.printf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp , nrReboots++ , slotErrors); }); })

                       ;
  readSettings(true);
  oled_Init();

  //=============start Networkstuff==================================
  if (settingOledType > 0)
  {
    if (settingOledFlip) oled_Init(); // only if true restart(init) oled screen
    oled_Clear(); // clear the screen
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(1, "Verbinden met WiFi", 500);
  }
  digitalWrite(2, 0x0);
  startWiFi(settingHostname, 240); // timeout 4 minuten

  if (settingOledType > 0)
  {
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(1, WiFi.SSID(), 0);
    snprintf(cMsg, sizeof(cMsg), "IP %s", WiFi.localIP().toString().c_str());
    oled_Print_Msg(2, cMsg, 1500);
  }
  digitalWrite(2, 0x1);

  ({ Serial.println(); TelnetStream.println(); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "247" "." "297" "\", \"aSM\", @progbits, 1 #"))) = (
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "Connected to "
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "247" "." "297" "\", \"aSM\", @progbits, 1 #"))) = (
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "Connected to "
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 247 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); }); ({ Serial.println(WiFi.SSID()); TelnetStream.println(WiFi.SSID()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "248" "." "298" "\", \"aSM\", @progbits, 1 #"))) = (
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "IP address: "
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "248" "." "298" "\", \"aSM\", @progbits, 1 #"))) = (
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "IP address: "
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 248 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); }); ({ Serial.println(WiFi.localIP()); TelnetStream.println(WiFi.localIP()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "249" "." "299" "\", \"aSM\", @progbits, 1 #"))) = (
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "IP gateway: "
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "249" "." "299" "\", \"aSM\", @progbits, 1 #"))) = (
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "IP gateway: "
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 249 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); }); ({ Serial.println(WiFi.gatewayIP()); TelnetStream.println(WiFi.gatewayIP()); });
  ({ Serial.println(); TelnetStream.println(); });

  for (int L = 0; L < 10; L++) {
    digitalWrite(2, !digitalRead(2));
    delay(200);
  }
  digitalWrite(2, 0x1);

  //-----------------------------------------------------------------
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
  startMDNS(settingHostname);
  if (settingOledType > 0)
  {
    oled_Print_Msg(3, "mDNS gestart", 1500);
  }

  //=============end Networkstuff======================================
# 305 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
  //================ end NTP =========================================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  ({ _debugBOL(__FUNCTION__, 308); ({ Serial.println(cMsg); TelnetStream.println(cMsg); }); });

  Serial.print("\nGebruik 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' voor verdere debugging\r\n");

  //=============now test if SPIFFS is correct populated!============
  if (DSMRfileExist(settingIndexPage, false) )
  {
    if (strcmp(settingIndexPage, "DSMRindex.html") != 0)
    {
      if (settingIndexPage[0] != '/')
      {
        char tempPage[50] = "/";
        strConcat(tempPage, 49, settingIndexPage);
        strCopy(settingIndexPage, sizeof(settingIndexPage), tempPage);
      }
      hasAlternativeIndex = true;
    }
    else hasAlternativeIndex = false;
  }
  if (!hasAlternativeIndex && !DSMRfileExist("/DSMRindex.html", false) )
  {
    spiffsNotPopulated = true;
  }
  if (!hasAlternativeIndex) //--- there's no alternative index.html
  {
    DSMRfileExist("/DSMRindex.js", false);
    DSMRfileExist("/DSMRindex.css", false);
    DSMRfileExist("/DSMRgraphics.js", false);
  }
  if (!DSMRfileExist("/FSexplorer.html", true))
  {
    spiffsNotPopulated = true;
  }
  if (!DSMRfileExist("/FSexplorer.css", true))
  {
    spiffsNotPopulated = true;
  }
  //=============end SPIFFS =========================================







  //=============now test if "convertPRD" file exists================

  if (LittleFS.exists("/!PRDconvert") )
  {
    convertPRD2RING();
  }

  //=================================================================
# 373 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
  if (settingOledType > 0)
  {
    snprintf(cMsg, sizeof(cMsg), "DT: %02d%02d%02d%02d0101W", thisYear
             , thisMonth, thisDay, thisHour);
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(3, cMsg, 1500);
  }

  //================ Start MQTT  ======================================


  connectMQTT(); //USE_MQTT
  if (settingOledType > 0) //USE_MQTT
  { //USE_MQTT
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0); //USE_MQTT
    oled_Print_Msg(3, "MQTT server set!", 1500); //USE_MQTT
  } //USE_MQTT


  //================ End of Start MQTT  ===============================


  //================ Start HTTP Server ================================

  if (!spiffsNotPopulated) {
    ({ _debugBOL(__FUNCTION__, 398); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "398" "." "300" "\", \"aSM\", @progbits, 1 #"))) = (
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS correct populated -> normal operation!\r"
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "398" "." "300" "\", \"aSM\", @progbits, 1 #"))) = (
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "SPIFFS correct populated -> normal operation!\r"
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); }); });
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(1, "OK, SPIFFS correct", 0);
      oled_Print_Msg(2, "Verder met normale", 0);
      oled_Print_Msg(3, "Verwerking ;-)", 2500);
    }
    if (hasAlternativeIndex)
    {
      httpServer.serveStatic("/", LittleFS, settingIndexPage);
      httpServer.serveStatic("/index", LittleFS, settingIndexPage);
      httpServer.serveStatic("/index.html", LittleFS, settingIndexPage);
      httpServer.serveStatic("/DSMRindex.html", LittleFS, settingIndexPage);
    }
    else
    {
      httpServer.serveStatic("/", LittleFS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.html", LittleFS, "/DSMRindex.html");
      httpServer.serveStatic("/index", LittleFS, "/DSMRindex.html");
      httpServer.serveStatic("/index.html", LittleFS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.css", LittleFS, "/DSMRindex.css");
      httpServer.serveStatic("/DSMRindex.js", LittleFS, "/DSMRindex.js");
      httpServer.serveStatic("/DSMRgraphics.js", LittleFS, "/DSMRgraphics.js");
    }
  } else {
    ({ _debugBOL(__FUNCTION__, 424); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "424" "." "301" "\", \"aSM\", @progbits, 1 #"))) = (
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "Oeps! not all files found on SPIFFS -> present FSexplorer!\r"
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "424" "." "301" "\", \"aSM\", @progbits, 1 #"))) = (
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   "Oeps! not all files found on SPIFFS -> present FSexplorer!\r"
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
   ); &__pstr__[0];}))
# 424 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
   )))); }); });
    spiffsNotPopulated = true;
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, "!OEPS! niet alle", 0);
      oled_Print_Msg(1, "files op SPIFFS", 0);
      oled_Print_Msg(2, "gevonden! (fout!)", 0);
      oled_Print_Msg(3, "Start FSexplorer", 2000);
    }
  }

  setupFSexplorer();
  httpServer.serveStatic("/FSexplorer.png", LittleFS, "/FSexplorer.png");

  httpServer.on("/api", HTTP_GET, processAPI);
  // all other api calls are catched in FSexplorer onNotFounD!

  httpServer.begin();
  ({ _debugBOL(__FUNCTION__, 442); ({ Serial.println("HTTP server gestart\r"); TelnetStream.println("HTTP server gestart\r"); }); });
  if (settingOledType > 0) //HAS_OLED
  { //HAS_OLED
    oled_Clear(); //HAS_OLED
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0); //HAS_OLED
    oled_Print_Msg(2, "HTTP server ..", 0); //HAS_OLED
    oled_Print_Msg(3, "gestart (poort 80)", 0); //HAS_OLED
  } //HAS_OLED

  for (int i = 0; i < 10; i++)
  {
    digitalWrite(2, !digitalRead(2));
    delay(250);
  }
  //================ Start HTTP Server ================================

  //test(); monthTabel





  ({ _debugBOL(__FUNCTION__, 464); ({ Serial.printf("Startup complete! actTimestamp[%s]\r\n", actTimestamp); TelnetStream.printf("Startup complete! actTimestamp[%s]\r\n", actTimestamp); }); });
  /* nothing*/;

  //================ End of Slimmer Meter ============================


  //================ The final part of the Setup =====================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  ({ _debugBOL(__FUNCTION__, 473); ({ Serial.println(cMsg); TelnetStream.println(cMsg); }); });

  if (settingOledType > 0)
  {
    oled_Print_Msg(0, "<DSMRloggerAPI>", 0);
    oled_Print_Msg(1, "Startup complete", 0);
    oled_Print_Msg(2, "Wait for first", 0);
    oled_Print_Msg(3, "telegram .....", 500);
  }

  //================ Start Slimme Meter ===============================

  ({ _debugBOL(__FUNCTION__, 485); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "485" "." "302" "\", \"aSM\", @progbits, 1 #"))) = (
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "Enable slimmeMeter..\r"
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "DSMRloggerAPI_LittleFS.ino" "." "485" "." "302" "\", \"aSM\", @progbits, 1 #"))) = (
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 "Enable slimmeMeter..\r"
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino" 3
 ); &__pstr__[0];}))
# 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
 )))); }); });







  delay(100);
  slimmeMeter.enable(true);

} // setup()


//===[ no-blocking delay with running background tasks in ms ]============================
static uint32_t timer_delay_ms_interval = (getParam(0, 1, 0)), timer_delay_ms_due = millis() +timer_delay_ms_interval +random(timer_delay_ms_interval / 3); static byte timer_delay_ms_type = getParam(1, 1, 0);;
void delayms(unsigned long delay_ms)
{
  timer_delay_ms_interval = (getParam(0, delay_ms, 0) ); timer_delay_ms_due = millis() +timer_delay_ms_interval;;
  ( timer_delay_ms_due = millis()+timer_delay_ms_interval );;
  while (!( __Due__(timer_delay_ms_due, timer_delay_ms_interval, timer_delay_ms_type) ))
  {
    doSystemTasks();
  }

} // delayms()

//========================================================================================

//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (Verbose1) ({ _debugBOL(__FUNCTION__, 517); ({ Serial.println("doTaskTelegram"); TelnetStream.println("doTaskTelegram"); }); });



  //-- enable DTR to read a telegram from the Slimme Meter
  slimmeMeter.enable(true);
  slimmeMeter.loop();
  handleSlimmemeter();

  if (WiFi.status() != WL_CONNECTED)
  {
    for (int b = 0; b < 10; b++) {
      digitalWrite(2, !digitalRead(2));
      delay(75);
    }
  }
  digitalWrite(2, !digitalRead(2));
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{

  slimmeMeter.loop();


  MQTTclient.loop();

  httpServer.handleClient();
  MDNS.update();
  handleKeyInput();
  if (settingOledType > 0)
  {
    checkFlashButton();
  }

  yield();

} // doSystemTasks()


void loop ()
{
  //--- do the tasks that has to be done
  //--- as often as possible
  doSystemTasks();

  loopCount++;

  //--- verwerk volgend telegram
  if ( __Due__(nextTelegram_due, nextTelegram_interval, nextTelegram_type) )
  {
    doTaskTelegram();
  }

  //--- update upTime counter
  if ( __Due__(updateSeconds_due, updateSeconds_interval, updateSeconds_type) )
  {
    upTimeSeconds++;
  }

  //--- if an OLED screen attached, display the status
  if (settingOledType > 0)
  {
    if ( __Due__(updateDisplay_due, updateDisplay_interval, updateDisplay_type) )
    {
      displayStatus();
    }
  }

  //--- if mindergas then check







  //--- if connection lost, try to reconnect to WiFi
  if ( ( __Due__(reconnectWiFi_due, reconnectWiFi_interval, reconnectWiFi_type) ) && (WiFi.status() != WL_CONNECTED) )
  {
    /* nothing*/;
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED)
      /* nothing*/;
    else {
      snprintf(cMsg, sizeof(cMsg), "IP:[%s], Gateway:[%s]", WiFi.localIP().toString().c_str()
               , WiFi.gatewayIP().toString().c_str());
      /* nothing*/;
    }
  }

  //--- if NTP set, see if it needs synchronizing
# 622 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
  yield();

} // loop()


/***************************************************************************



  Permission is hereby granted, free of charge, to any person obtaining a

  copy of this software and associated documentation files (the

  "Software"), to deal in the Software without restriction, including

  without limitation the rights to use, copy, modify, merge, pubFapilisth,

  distribute, sublicense, and/or sell copies of the Software, and to permit

  persons to whom the Software is furnished to do so, subject to the

  following conditions:



  The above copyright notice and this permission notice shall be included

  in all copies or substantial portions of the Software.



  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS

  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF

  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY

  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT

  OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR

  THE USE OR OTHER DEALINGS IN THE SOFTWARE.



***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
/* 
***************************************************************************  
**  Program  : FSexplorer, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Mostly stolen from https://www.arduinoforum.de/User-Fips
**  For more information visit: https://fipsok.de
**  See also https://www.arduinoforum.de/arduino-Thread-SPIFFS-DOWNLOAD-UPLOAD-DELETE-Esp8266-NodeMCU
**
***************************************************************************      
  Copyright (c) 2018 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************
*/

const char Helper[] = R"(
  <br>You first need to upload these two files:
  <ul>
    <li>FSexplorer.html</li>
    <li>FSexplorer.css</li>
  </ul>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
  <br/><b>or</b> you can use the <i>Flash Utility</i> to flash firmware or SPIFFS!
  <form action='/update' method='GET'>
    <input type='submit' name='SUBMIT' value='Flash Utility'/>
  </form>
)";
const char Header[] = "HTTP/1.1 303 OK\r\nLocation:FSexplorer.html\r\nCache-Control: no-cache\r\n";

//=====================================================================================
void setupFSexplorer() // Funktionsaufruf "spiffs();" muss im Setup eingebunden werden
{
  LittleFS.begin();

  if (LittleFS.exists("/FSexplorer.html"))
  {
    httpServer.serveStatic("/FSexplorer.html", LittleFS, "/FSexplorer.html");
    httpServer.serveStatic("/FSexplorer", LittleFS, "/FSexplorer.html");
  }
  else
  {
    httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  }
  httpServer.on("/api/listfiles", APIlistFiles);
  httpServer.on("/SPIFFSformat", formatSpiffs);
  httpServer.on(
      "/upload", HTTP_POST, []() {}, handleFileUpload);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/update", updateFirmware);
  httpServer.onNotFound([]() {
    if (Verbose2)
      ({ _debugBOL(__FUNCTION__, 63); ({ Serial.printf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str()); TelnetStream.printf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str()); }); });
    if (httpServer.uri().indexOf("/api/") == 0)
    {
      if (Verbose1)
        ({ _debugBOL(__FUNCTION__, 67); ({ Serial.printf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str()); TelnetStream.printf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str()); }); });
      processAPI();
    }
    else
    {
      ({ _debugBOL(__FUNCTION__, 72); ({ Serial.printf("next: handleFile(%s)\r\n", String(httpServer.urlDecode(httpServer.uri())).c_str()); TelnetStream.printf("next: handleFile(%s)\r\n", String(httpServer.urlDecode(httpServer.uri())).c_str()); }); });
      if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      {
        httpServer.send(404, "text/plain", "FileNotFound\r\n");
      }
    }
  });

} // setupFSexplorer()

//=====================================================================================
void APIlistFiles() // Senden aller Daten an den Client
{
  FSInfo SPIFFSinfo;

  typedef struct _fileMeta
  {
    char Name[30];
    int32_t Size;
  } fileMeta;

  _fileMeta dirMap[30];
  int fileNr = 0;

  Dir dir = LittleFS.openDir("/"); // List files on SPIFFS
  while (dir.next())
  {
    dirMap[fileNr].Name[0] = '\0';
    if (dir.fileName()[0] != '/')
    {
      strncat(dirMap[fileNr].Name, dir.fileName().substring(0).c_str(),30); // In LITTLEFS no leading "/"
    }
    else
    {
      strncat(dirMap[fileNr].Name, dir.fileName().substring(1).c_str(), 29); // remove leading '/'
    }
    dirMap[fileNr].Size = dir.fileSize();
    fileNr++;
  }

  // -- bubble sort dirMap op .Name--
  for (int8_t y = 0; y < fileNr; y++)
  {
    yield();
    for (int8_t x = y + 1; x < fileNr; x++)
    {
      //DebugTf("y[%d], x[%d] => seq[y][%s] / seq[x][%s] ", y, x, dirMap[y].Name, dirMap[x].Name);
      if (compare(String(dirMap[x].Name), String(dirMap[y].Name)))
      {
        //Debug(" !switch!");
        fileMeta temp = dirMap[y];
        dirMap[y] = dirMap[x];
        dirMap[x] = temp;
      } /* end if */
      //Debugln();
    } /* end for */
  } /* end for */

  for (int8_t x = 0; x < fileNr; x++)
  {
    ({ _debugBOL(__FUNCTION__, 132); ({ Serial.println(dirMap[x].Name); TelnetStream.println(dirMap[x].Name); }); });
  }

  String temp = "[";
  for (int f = 0; f < fileNr; f++)
  {
    if (temp != "[")
      temp += ",";
    temp += R"({"name":")" + String(dirMap[f].Name) + R"(","size":")" + formatBytes(dirMap[f].Size) + R"("})";
  }
  LittleFS.info(SPIFFSinfo);
  temp += R"(,{"usedBytes":")" + formatBytes(SPIFFSinfo.usedBytes * 1.05) + R"(",)" + // Berechnet den verwendeten Speicherplatz + 5% Sicherheitsaufschlag
          R"("totalBytes":")" + formatBytes(SPIFFSinfo.totalBytes) + R"(","freeBytes":")" + // Zeigt die Größe des Speichers
          (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + R"("}])"; // Berechnet den freien Speicherplatz + 5% Sicherheitsaufschlag
  httpServer.send(200, "application/json", temp);

} // APIlistFiles()

//=====================================================================================
bool handleFile(String &&path)
{
  if (httpServer.hasArg("delete"))
  {
    ({ _debugBOL(__FUNCTION__, 155); ({ Serial.printf("Delete -> [%s]\n\r", httpServer.arg("delete").c_str()); TelnetStream.printf("Delete -> [%s]\n\r", httpServer.arg("delete").c_str()); }); });
    LittleFS.remove(httpServer.arg("delete")); // Datei löschen
    httpServer.sendContent(Header);
    return true;
  }
  if (!LittleFS.exists("/FSexplorer.html"))
    httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  if (path.endsWith("/"))
    path += "index.html";
  return LittleFS.exists(path) ? ({File f = LittleFS.open(path, "r"); httpServer.streamFile(f, contentType(path)); f.close(); true; }) : false;

} // handleFile()

//=====================================================================================
void handleFileUpload()
{
  static File fsUploadFile;
  HTTPUpload &upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    if (upload.filename.length() > 30)
    {
      upload.filename = upload.filename.substring(upload.filename.length() - 30, upload.filename.length()); // Dateinamen auf 30 Zeichen kürzen
    }
    ({ Serial.println("FileUpload Name: " + upload.filename); TelnetStream.println("FileUpload Name: " + upload.filename); });
    fsUploadFile = LittleFS.open("/" + httpServer.urlDecode(upload.filename), "w");
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    ({ Serial.println("FileUpload Data: " + (String)upload.currentSize); TelnetStream.println("FileUpload Data: " + (String)upload.currentSize); });
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    ({ Serial.println("FileUpload Size: " + (String)upload.totalSize); TelnetStream.println("FileUpload Size: " + (String)upload.totalSize); });
    httpServer.sendContent(Header);
  }

} // handleFileUpload()

//=====================================================================================
void formatSpiffs()
{ //Formatiert den Speicher
  if (!LittleFS.exists("/!format"))
    return;
  ({ _debugBOL(__FUNCTION__, 203); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "203" "." "303" "\", \"aSM\", @progbits, 1 #"))) = (
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Format SPIFFS"
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "203" "." "303" "\", \"aSM\", @progbits, 1 #"))) = (
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Format SPIFFS"
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 203 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); }); });
  LittleFS.format();
  httpServer.sendContent(Header);

} // formatSpiffs()

//=====================================================================================
const String formatBytes(size_t const &bytes)
{
  return (bytes < 1024) ? String(bytes) + " Byte" : (bytes < (1024 * 1024)) ? String(bytes / 1024.0) + " KB"
                                                                            : String(bytes / 1024.0 / 1024.0) + " MB";

} //formatBytes()

//=====================================================================================
const String &contentType(String &filename)
{
  if (filename.endsWith(".htm") || filename.endsWith(".html"))
    filename = "text/html";
  else if (filename.endsWith(".css"))
    filename = "text/css";
  else if (filename.endsWith(".js"))
    filename = "application/javascript";
  else if (filename.endsWith(".json"))
    filename = "application/json";
  else if (filename.endsWith(".png"))
    filename = "image/png";
  else if (filename.endsWith(".gif"))
    filename = "image/gif";
  else if (filename.endsWith(".jpg"))
    filename = "image/jpeg";
  else if (filename.endsWith(".ico"))
    filename = "image/x-icon";
  else if (filename.endsWith(".xml"))
    filename = "text/xml";
  else if (filename.endsWith(".pdf"))
    filename = "application/x-pdf";
  else if (filename.endsWith(".zip"))
    filename = "application/x-zip";
  else if (filename.endsWith(".gz"))
    filename = "application/x-gzip";
  else
    filename = "text/plain";
  return filename;

} // &contentType()

//=====================================================================================
bool freeSpace(uint16_t const &printsize)
{
  FSInfo SPIFFSinfo;
  LittleFS.info(SPIFFSinfo);
  ({ Serial.println(formatBytes(SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + " im Spiffs frei"); TelnetStream.println(formatBytes(SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + " im Spiffs frei"); });
  return (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05) > printsize) ? true : false;

} // freeSpace()

//=====================================================================================
void updateFirmware()
{

  ({ _debugBOL(__FUNCTION__, 264); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "264" "." "304" "\", \"aSM\", @progbits, 1 #"))) = (
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Redirect to updateIndex .."
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "264" "." "304" "\", \"aSM\", @progbits, 1 #"))) = (
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Redirect to updateIndex .."
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); }); });
  doRedirect("wait ... ", 1, "/updateIndex", false);




} // updateFirmware()

//=====================================================================================
void reBootESP()
{
  ({ _debugBOL(__FUNCTION__, 275); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "275" "." "305" "\", \"aSM\", @progbits, 1 #"))) = (
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Redirect and ReBoot .."
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "FSexplorer.ino" "." "275" "." "305" "\", \"aSM\", @progbits, 1 #"))) = (
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 "Redirect and ReBoot .."
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino" 3
 ); &__pstr__[0];}))
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
 )))); }); });
  doRedirect("Reboot DSMR-logger ..", 60, "/", true);

} // reBootESP()

//=====================================================================================
void doRedirect(String msg, int wait, const char *URL, bool reboot)
{
  String redirectHTML =
      "<!DOCTYPE HTML><html lang='en-US'>"
      "<head>"
      "<meta charset='UTF-8'>"
      "<style type='text/css'>"
      "body {background-color: lightblue;}"
      "</style>"
      "<title>Redirect to Main Program</title>"
      "</head>"
      "<body><h1>FSexplorer</h1>"
      "<h3>" +
      msg + "</h3>"
            "<br><div style='width: 500px; position: relative; font-size: 25px;'>"
            "  <div style='float: left;'>Redirect over &nbsp;</div>"
            "  <div style='float: left;' id='counter'>" +
      String(wait) + "</div>"
                     "  <div style='float: left;'>&nbsp; seconden ...</div>"
                     "  <div style='float: right;'>&nbsp;</div>"
                     "</div>"
                     "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->"
                     "<br><br><hr>If you are not redirected automatically, click this <a href='/'>Main Program</a>."
                     "  <script>"
                     "      setInterval(function() {"
                     "          var div = document.querySelector('#counter');"
                     "          var count = div.textContent * 1 - 1;"
                     "          div.textContent = count;"
                     "          if (count <= 0) {"
                     "              window.location.replace('" +
      String(URL) + "'); "
                    "          } "
                    "      }, 1000); "
                    "  </script> "
                    "</body></html>\r\n";

  ({ _debugBOL(__FUNCTION__, 317); ({ Serial.println(msg); TelnetStream.println(msg); }); });
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot)
  {
    delay(5000);
    ESP.restart();
    delay(5000);
  }

} // doRedirect()
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
/* 
***************************************************************************  
**  Program  : MQTTstuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
**  RvdB  changed MQTT stuff to FSM 
**  AaW   simplified and restructured the FSM 
*/

// Declare some variables within global scope

  static IPAddress MQTTbrokerIP;
  static char MQTTbrokerIPchar[20];


//  https://github.com/knolleary/pubsubclient
//  #include <PubSubClient.h>           // MQTT client publish and subscribe functionality

//  static PubSubClient MQTTclient(wifiClient);
  int8_t reconnectAttempts = 0;
  char lastMQTTtimestamp[15] = "-";
  char mqttBuff[100];


  enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };
  enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

  String MQTTclientId;



//===========================================================================================
void connectMQTT()
{


  if (Verbose2) ({ _debugBOL(__FUNCTION__, 41); ({ Serial.printf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); TelnetStream.printf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); }); })

                                                                           ;

  if (settingMQTTinterval == 0) {
    mqttIsConnected = false;
    return;
  }

  if (!MQTTclient.connected() || stateMQTT != MQTT_STATE_IS_CONNECTED)
  {
    mqttIsConnected = false;
    stateMQTT = MQTT_STATE_INIT;
  }

  mqttIsConnected = connectMQTT_FSM();

  if (Verbose1) ({ _debugBOL(__FUNCTION__, 58); ({ Serial.printf("connected()[%d], mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); TelnetStream.printf("connected()[%d], mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); }); })

                                                                           ;

  reconnectMQTTtimer_interval = (getParam(0, 5, 0) *60*1000); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;;


}


//===========================================================================================
bool connectMQTT_FSM()
{


  switch(stateMQTT)
  {
    case MQTT_STATE_INIT:
          ({ _debugBOL(__FUNCTION__, 76); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "76" "." "306" "\", \"aSM\", @progbits, 1 #"))) = (
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT Initializing"
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "76" "." "306" "\", \"aSM\", @progbits, 1 #"))) = (
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT Initializing"
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          WiFi.hostByName(settingMQTTbroker, MQTTbrokerIP); // lookup the MQTTbroker convert to IP
          snprintf(MQTTbrokerIPchar, sizeof(MQTTbrokerIPchar), "%d.%d.%d.%d", MQTTbrokerIP[0]
                                                                            , MQTTbrokerIP[1]
                                                                            , MQTTbrokerIP[2]
                                                                            , MQTTbrokerIP[3]);
          if (!isValidIP(MQTTbrokerIP))
          {
            ({ _debugBOL(__FUNCTION__, 84); ({ Serial.printf("ERROR: [%s] => is not a valid URL\r\n", settingMQTTbroker); TelnetStream.printf("ERROR: [%s] => is not a valid URL\r\n", settingMQTTbroker); }); });
            settingMQTTinterval = 0;
            ({ _debugBOL(__FUNCTION__, 86); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "86" "." "307" "\", \"aSM\", @progbits, 1 #"))) = (
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "Next State: MQTT_STATE_ERROR"
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "86" "." "307" "\", \"aSM\", @progbits, 1 #"))) = (
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "Next State: MQTT_STATE_ERROR"
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); }); });
            stateMQTT = MQTT_STATE_ERROR;
            return false;
          }

          //MQTTclient.disconnect();
          //DebugTf("disconnect -> MQTT status, rc=%d \r\n", MQTTclient.state());
          ({ _debugBOL(__FUNCTION__, 93); ({ Serial.printf("[%s] => setServer(%s, %d) \r\n", settingMQTTbroker, MQTTbrokerIPchar, settingMQTTbrokerPort); TelnetStream.printf("[%s] => setServer(%s, %d) \r\n", settingMQTTbroker, MQTTbrokerIPchar, settingMQTTbrokerPort); }); });
          MQTTclient.setServer(MQTTbrokerIPchar, settingMQTTbrokerPort);
          ({ _debugBOL(__FUNCTION__, 95); ({ Serial.printf("setServer  -> MQTT status, rc=%d \r\n", MQTTclient.state()); TelnetStream.printf("setServer  -> MQTT status, rc=%d \r\n", MQTTclient.state()); }); });
          MQTTclientId = String(settingHostname) + "-" + WiFi.macAddress();
          stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
          ({ _debugBOL(__FUNCTION__, 98); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "98" "." "308" "\", \"aSM\", @progbits, 1 #"))) = (
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "Next State: MQTT_STATE_TRY_TO_CONNECT"
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "98" "." "308" "\", \"aSM\", @progbits, 1 #"))) = (
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "Next State: MQTT_STATE_TRY_TO_CONNECT"
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          reconnectAttempts = 0;

    case MQTT_STATE_TRY_TO_CONNECT:
          ({ _debugBOL(__FUNCTION__, 102); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "102" "." "309" "\", \"aSM\", @progbits, 1 #"))) = (
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT try to connect"
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "102" "." "309" "\", \"aSM\", @progbits, 1 #"))) = (
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT try to connect"
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 102 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          ({ _debugBOL(__FUNCTION__, 103); ({ Serial.printf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar); TelnetStream.printf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar); }); });

          ({ _debugBOL(__FUNCTION__, 105); ({ Serial.printf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str()); TelnetStream.printf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str()); }); });
          reconnectAttempts++;

          //--- If no username, then anonymous connection to broker, otherwise assume username/password.
          if (String(settingMQTTuser).length() == 0)
          {
            ({ _debugBOL(__FUNCTION__, 111); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "111" "." "310" "\", \"aSM\", @progbits, 1 #"))) = (
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "without a Username/Password "
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "111" "." "310" "\", \"aSM\", @progbits, 1 #"))) = (
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "without a Username/Password "
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 111 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); }); });
            MQTTclient.connect(MQTTclientId.c_str());
          }
          else
          {
            ({ _debugBOL(__FUNCTION__, 116); ({ Serial.printf("with Username [%s] and password ", settingMQTTuser); TelnetStream.printf("with Username [%s] and password ", settingMQTTuser); }); });
            MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd);
          }
          //--- If connection was made succesful, move on to next state...
          if (MQTTclient.connected())
          {
            reconnectAttempts = 0;
            ({ Serial.printf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state()); TelnetStream.printf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state()); });
            MQTTclient.loop();
            stateMQTT = MQTT_STATE_IS_CONNECTED;
            return true;
          }
          ({ Serial.printf(" -> MQTT status, rc=%d \r\n", MQTTclient.state()); TelnetStream.printf(" -> MQTT status, rc=%d \r\n", MQTTclient.state()); });

          //--- After 3 attempts... go wait for a while.
          if (reconnectAttempts >= 3)
          {
            ({ _debugBOL(__FUNCTION__, 133); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "133" "." "311" "\", \"aSM\", @progbits, 1 #"))) = (
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "3 attempts have failed. Retry wait for next reconnect in 10 minutes\r"
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "133" "." "311" "\", \"aSM\", @progbits, 1 #"))) = (
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "3 attempts have failed. Retry wait for next reconnect in 10 minutes\r"
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); }); });
            stateMQTT = MQTT_STATE_ERROR; // if the re-connect did not work, then return to wait for reconnect
            ({ _debugBOL(__FUNCTION__, 135); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "135" "." "312" "\", \"aSM\", @progbits, 1 #"))) = (
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "Next State: MQTT_STATE_ERROR"
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "135" "." "312" "\", \"aSM\", @progbits, 1 #"))) = (
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           "Next State: MQTT_STATE_ERROR"
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
           ); &__pstr__[0];}))
# 135 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
           )))); }); });
          }
          break;

    case MQTT_STATE_IS_CONNECTED:
          MQTTclient.loop();
          return true;

    case MQTT_STATE_ERROR:
          ({ _debugBOL(__FUNCTION__, 144); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "144" "." "313" "\", \"aSM\", @progbits, 1 #"))) = (
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "144" "." "313" "\", \"aSM\", @progbits, 1 #"))) = (
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 144 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          //--- next retry in 10 minutes.
          reconnectMQTTtimer_interval = (getParam(0, 10, 0) *60*1000); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;;
          break;

    default:
          ({ _debugBOL(__FUNCTION__, 150); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "150" "." "314" "\", \"aSM\", @progbits, 1 #"))) = (
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: default, this should NEVER happen!"
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "150" "." "314" "\", \"aSM\", @progbits, 1 #"))) = (
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "MQTT State: default, this should NEVER happen!"
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          //--- do nothing, this state should not happen
          stateMQTT = MQTT_STATE_INIT;
          reconnectMQTTtimer_interval = (getParam(0, 10, 0) *60*1000); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;;
          ({ _debugBOL(__FUNCTION__, 154); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "154" "." "315" "\", \"aSM\", @progbits, 1 #"))) = (
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "Next State: MQTT_STATE_INIT"
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "MQTTstuff.ino" "." "154" "." "315" "\", \"aSM\", @progbits, 1 #"))) = (
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         "Next State: MQTT_STATE_INIT"
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino" 3
         ); &__pstr__[0];}))
# 154 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
         )))); }); });
          break;
  }


  return false;

} // connectMQTT_FSM()

//===========================================================================================
String trimVal(char *in)
{
  String Out = in;
  Out.trim();
  return Out;
} // trimVal()


//=======================================================================
struct buildJsonMQTT {


    char topicId[100];

    template<typename Item>
    void apply(Item &i) {
      if (i.present())
      {
        String Name = Item::name;
        //-- for dsmr30 -----------------------------------------------



        String Unit = Item::unit();

        if (settingMQTTtopTopic[strlen(settingMQTTtopTopic)-1] == '/')
              snprintf(topicId, sizeof(topicId), "%s", settingMQTTtopTopic);
        else snprintf(topicId, sizeof(topicId), "%s/", settingMQTTtopTopic);
        strConcat(topicId, sizeof(topicId), Name.c_str());
        if (Verbose2) ({ _debugBOL(__FUNCTION__, 193); ({ Serial.printf("topicId[%s]\r\n", topicId); TelnetStream.printf("topicId[%s]\r\n", topicId); }); });

        if (Unit.length() > 0)
        {
          createMQTTjsonMessage(mqttBuff, Name.c_str(), typecastValue(i.val()), Unit.c_str());
        }
        else
        {
          createMQTTjsonMessage(mqttBuff, Name.c_str(), typecastValue(i.val()));
        }

        //snprintf(cMsg, sizeof(cMsg), "%s", jsonString.c_str());
        //DebugTf("topicId[%s] -> [%s]\r\n", topicId, mqttBuff);
        if (!MQTTclient.publish(topicId, mqttBuff) )
        {
          ({ _debugBOL(__FUNCTION__, 208); ({ Serial.printf("Error publish(%s) [%s] [%d bytes]\r\n", topicId, mqttBuff, (strlen(topicId) + strlen(mqttBuff))); TelnetStream.printf("Error publish(%s) [%s] [%d bytes]\r\n", topicId, mqttBuff, (strlen(topicId) + strlen(mqttBuff))); }); });
        }
      }
  }


}; // struct buildJsonMQTT


//===========================================================================================
void sendMQTTData()
{

  String dateTime, topicId, json;

  if (settingMQTTinterval == 0) return;

  if (ESP.getFreeHeap() < 9000) // to prevent firmware from crashing!
  {
    ({ _debugBOL(__FUNCTION__, 227); ({ Serial.printf("==> Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap()); TelnetStream.printf("==> Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap()); }); });
    /* nothing*/;
    return;
  }


  if (!MQTTclient.connected() || ! mqttIsConnected)
  {
    ({ _debugBOL(__FUNCTION__, 235); ({ Serial.printf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); TelnetStream.printf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n" , MQTTclient.connected() , mqttIsConnected, stateMQTT); }); })

                                                                           ;
  }
  if (!MQTTclient.connected())
  {
    if ( ( __Due__(reconnectMQTTtimer_due, reconnectMQTTtimer_interval, reconnectMQTTtimer_type) ) || mqttIsConnected)
    {
      mqttIsConnected = false;
      connectMQTT();
    }
    else
    {
      ({ _debugBOL(__FUNCTION__, 248); ({ Serial.printf("trying to reconnect in less than %d minutes\r\n", (( (( __TimeLeft__(reconnectMQTTtimer_due) ) ) / (60 * 1000)) +1)); TelnetStream.printf("trying to reconnect in less than %d minutes\r\n", (( (( __TimeLeft__(reconnectMQTTtimer_due) ) ) / (60 * 1000)) +1)); }); });
    }
    if ( !mqttIsConnected )
    {
      ({ _debugBOL(__FUNCTION__, 252); ({ Serial.println("no connection with a MQTT broker .."); TelnetStream.println("no connection with a MQTT broker .."); }); });
      return;
    }
  }

  ({ _debugBOL(__FUNCTION__, 257); ({ Serial.printf("Sending data to MQTT server [%s]:[%d]\r\n", settingMQTTbroker, settingMQTTbrokerPort); TelnetStream.printf("Sending data to MQTT server [%s]:[%d]\r\n", settingMQTTbroker, settingMQTTbrokerPort); }); });

  DSMRdata.applyEach(buildJsonMQTT());



} // sendMQTTData()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MinderGas.ino"
/*
**************************************************************************
**  Program  : MinderGas.ino
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Created by Robert van den Breemen (8 feb 2020)
*
*/


//=======================================================================
void handleMindergas()
{




} // handleMindergas()
# 437 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MinderGas.ino"
/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
/* 
***************************************************************************  
**  Program  : SPIFFSstuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

int16_t bytesWritten;

//static    FSInfo SPIFFSinfo;

//====================================================================
void readLastStatus()
{
  char buffer[50] = "";
  char dummy[50] = "";
  char spiffsTimestamp[20] = "";

  File _file = LittleFS.open("/DSMRstatus.csv", "r");
  if (!_file)
  {
    ({ _debugBOL(__FUNCTION__, 26); ({ Serial.println("read(): No /DSMRstatus.csv found .."); TelnetStream.println("read(): No /DSMRstatus.csv found .."); }); });
  }
  if (_file.available())
  {
    int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    buffer[l] = 0;
    ({ _debugBOL(__FUNCTION__, 32); ({ Serial.printf("read lastUpdate[%s]\r\n", buffer); TelnetStream.printf("read lastUpdate[%s]\r\n", buffer); }); });
    sscanf(buffer, "%[^;]; %u; %u; %[^;]", spiffsTimestamp, &nrReboots, &slotErrors, dummy);
    ({ _debugBOL(__FUNCTION__, 34); ({ Serial.printf("values timestamp[%s], nrReboots[%u], slotErrors[%u], dummy[%s]\r\n", spiffsTimestamp, nrReboots, slotErrors, dummy); TelnetStream.printf("values timestamp[%s], nrReboots[%u], slotErrors[%u], dummy[%s]\r\n", spiffsTimestamp, nrReboots, slotErrors, dummy); }); });
    yield();
  }
  _file.close();
  if (strlen(spiffsTimestamp) != 13)
  {
    strcpy(spiffsTimestamp, "010101010101X");
  }
  snprintf(actTimestamp, sizeof(actTimestamp), "%s", spiffsTimestamp);

} // readLastStatus()

//====================================================================
void writeLastStatus()
{
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    ({ _debugBOL(__FUNCTION__, 51); ({ Serial.printf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap()); TelnetStream.printf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap()); }); });
    /* nothing*/;
    return;
  }
  char buffer[50] = "";
  ({ _debugBOL(__FUNCTION__, 56); ({ Serial.printf("writeLastStatus() => %s; %u; %u;\r\n", actTimestamp, nrReboots, slotErrors); TelnetStream.printf("writeLastStatus() => %s; %u; %u;\r\n", actTimestamp, nrReboots, slotErrors); }); });
  /* nothing*/;
  File _file = LittleFS.open("/DSMRstatus.csv", "w");
  if (!_file)
  {
    ({ _debugBOL(__FUNCTION__, 61); ({ Serial.println("write(): No /DSMRstatus.csv found .."); TelnetStream.println("write(): No /DSMRstatus.csv found .."); }); });
  }
  snprintf(buffer, sizeof(buffer), "%-13.13s; %010u; %010u; %s;\n", actTimestamp, nrReboots, slotErrors, "meta data");
  _file.print(buffer);
  _file.flush();
  _file.close();

} // writeLastStatus()

//===========================================================================================
bool buildDataRecordFromSM(char *recIn)
{
  static float GG = 1;
  char record[75 + 1] = "";
  char key[10] = "";

  uint16_t recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
  strCopy(key, 10, actTimestamp, 0, 8);

  snprintf(record, sizeof(record), (char *)"%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;\n", key, (float)DSMRdata.energy_delivered_tariff1, (float)DSMRdata.energy_delivered_tariff2, (float)DSMRdata.energy_returned_tariff1, (float)DSMRdata.energy_returned_tariff2




           ,
           (float)DSMRdata.gas_delivered);

  // DATA + \n + \0
  fillRecord(record, 75);

  strcpy(recIn, record);

} // buildDataRecordFromSM()

//===========================================================================================
uint16_t buildDataRecordFromJson(char *recIn, String jsonIn)
{
  //static float GG = 1;
  char record[75 + 1] = "";
  String wOut[10];
  String wPair[5];
  char uKey[15] = "";
  float uEDT1 = 0.0;
  float uEDT2 = 0.0;
  float uERT1 = 0.0;
  float uERT2 = 0.0;
  float uGDT = 0.0;
  uint16_t recSlot;

  ({ _debugBOL(__FUNCTION__, 110); ({ Serial.println(jsonIn); TelnetStream.println(jsonIn); }); });

  jsonIn.replace("{", "");
  jsonIn.replace("}", "");
  jsonIn.replace("\"", "");
  int8_t wp = splitString(jsonIn.c_str(), ',', wOut, 9);
  for (int f = 0; f < wp; f++)
  {
    splitString(wOut[f].c_str(), ':', wPair, 4);
    if (Verbose2)
      ({ _debugBOL(__FUNCTION__, 120); ({ Serial.printf("[%d] -> [%s]\r\n", f, wOut[f].c_str()); TelnetStream.printf("[%d] -> [%s]\r\n", f, wOut[f].c_str()); }); });
    if (wPair[0].indexOf("recid") == 0)
      strCopy(uKey, 10, wPair[1].c_str());
    if (wPair[0].indexOf("edt1") == 0)
      uEDT1 = wPair[1].toFloat();
    if (wPair[0].indexOf("edt2") == 0)
      uEDT2 = wPair[1].toFloat();
    if (wPair[0].indexOf("ert1") == 0)
      uERT1 = wPair[1].toFloat();
    if (wPair[0].indexOf("ert2") == 0)
      uERT2 = wPair[1].toFloat();
    if (wPair[0].indexOf("gdt") == 0)
      uGDT = wPair[1].toFloat();
  }
  strConcat(uKey, 15, "0101X");
  recSlot = timestampToMonthSlot(uKey, strlen(uKey));

  ({ _debugBOL(__FUNCTION__, 137); ({ Serial.printf("MONTHS: Write [%s] to slot[%02d] in %s\r\n", uKey, recSlot, "/RINGmonths.csv"); TelnetStream.printf("MONTHS: Write [%s] to slot[%02d] in %s\r\n", uKey, recSlot, "/RINGmonths.csv"); }); });
  snprintf(record, sizeof(record), (char *)"%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;\n", uKey, (float)uEDT1, (float)uEDT2, (float)uERT1, (float)uERT2, (float)uGDT);

  // DATA + \n + \0
  fillRecord(record, 75);

  strcpy(recIn, record);

  return recSlot;

} // buildDataRecordFromJson()

//===========================================================================================
void writeDataToFile(const char *fileName, const char *record, uint16_t slot, int8_t fileType)
{
  uint16_t offset = 0;

  if (!isNumericp(record, 8))
  {
    ({ _debugBOL(__FUNCTION__, 156); ({ Serial.printf("timeStamp [%-13.13s] not valid\r\n", record); TelnetStream.printf("timeStamp [%-13.13s] not valid\r\n", record); }); });
    slotErrors++;
    return;
  }

  if (!LittleFS.exists(fileName))
  {
    switch (fileType)
    {
    case HOURS:
      createFile(fileName, (48 +1));
      break;
    case DAYS:
      createFile(fileName, (14 +1));
      break;
    case MONTHS:
      createFile(fileName, (24 +1));
      break;
    }
  }

  File dataFile = LittleFS.open(fileName, "r+"); // read and write ..
  if (!dataFile)
  {
    ({ _debugBOL(__FUNCTION__, 180); ({ Serial.printf("Error opening [%s]\r\n", fileName); TelnetStream.printf("Error opening [%s]\r\n", fileName); }); });
    return;
  }
  // slot goes from 0 to _NO_OF_SLOTS_
  // we need to add 1 to slot to skip header record!
  offset = ((slot + 1) * 75);
  dataFile.seek(offset, SeekSet);
  int32_t bytesWritten = dataFile.print(record);
  if (bytesWritten != 75)
  {
    ({ _debugBOL(__FUNCTION__, 190); ({ Serial.printf("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]\r\n", slot, bytesWritten, 75); TelnetStream.printf("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]\r\n", slot, bytesWritten, 75); }); });
    /* nothing*/;
  }
  dataFile.close();

} // writeDataToFile()

//===========================================================================================
void writeDataToFiles()
{
  char record[75 + 1] = "";
  uint16_t recSlot;

  buildDataRecordFromSM(record);
  ({ _debugBOL(__FUNCTION__, 204); ({ Serial.printf(">%s\r\n", record); TelnetStream.printf(">%s\r\n", record); }); }); // record ends in a \n

  // update HOURS
  recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 209); ({ Serial.printf("HOURS:  Write to slot[%02d] in %s\r\n", recSlot, "/RINGhours.csv"); TelnetStream.printf("HOURS:  Write to slot[%02d] in %s\r\n", recSlot, "/RINGhours.csv"); }); });
  writeDataToFile("/RINGhours.csv", record, recSlot, HOURS);
  /* nothing*/;

  // update DAYS
  recSlot = timestampToDaySlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 216); ({ Serial.printf("DAYS:   Write to slot[%02d] in %s\r\n", recSlot, "/RINGdays.csv"); TelnetStream.printf("DAYS:   Write to slot[%02d] in %s\r\n", recSlot, "/RINGdays.csv"); }); });
  writeDataToFile("/RINGdays.csv", record, recSlot, DAYS);

  // update MONTHS
  recSlot = timestampToMonthSlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 222); ({ Serial.printf("MONTHS: Write to slot[%02d] in %s\r\n", recSlot, "/RINGmonths.csv"); TelnetStream.printf("MONTHS: Write to slot[%02d] in %s\r\n", recSlot, "/RINGmonths.csv"); }); });
  writeDataToFile("/RINGmonths.csv", record, recSlot, MONTHS);

} // writeDataToFiles(fileType, dataStruct newDat, int8_t slotNr)

//===========================================================================================
void readOneSlot(int8_t fileType, const char *fileName, uint8_t recNr, uint8_t readSlot, bool doJson, const char *rName)
{
  uint16_t slot, maxSlots = 0, offset;
  char buffer[75 + 2] = "";
  char recID[10] = "";
  float EDT1, EDT2, ERT1, ERT2, GDT;

  switch (fileType)
  {
  case HOURS:
    maxSlots = (48 +1);
    break;
  case DAYS:
    maxSlots = (14 +1);
    break;
  case MONTHS:
    maxSlots = (24 +1);
    break;
  }

  if (!LittleFS.exists(fileName))
  {
    ({ _debugBOL(__FUNCTION__, 250); ({ Serial.printf("File [%s] does not excist!\r\n", fileName); TelnetStream.printf("File [%s] does not excist!\r\n", fileName); }); });
    return;
  }

  File dataFile = LittleFS.open(fileName, "r+"); // read and write ..
  if (!dataFile)
  {
    ({ _debugBOL(__FUNCTION__, 257); ({ Serial.printf("Error opening [%s]\r\n", fileName); TelnetStream.printf("Error opening [%s]\r\n", fileName); }); });
    return;
  }

  slot = (readSlot % maxSlots);
  // slot goes from 0 to _NO_OF_SLOTS_
  // we need to add 1 to slot to skip header record!
  offset = ((slot + 1) * 75);
  dataFile.seek(offset, SeekSet);
  int l = dataFile.readBytesUntil('\n', buffer, sizeof(buffer));
  buffer[l] = 0;
  if (l >= (75 - 1)) // '\n' is skipped by readBytesUntil()
  {
    if (!isNumericp(buffer, 8)) // first 8 bytes is YYMMDDHH
    {
      {
        ({ Serial.printf("slot[%02d]==>timeStamp [%-13.13s] not valid!!\r\n", slot, buffer); TelnetStream.printf("slot[%02d]==>timeStamp [%-13.13s] not valid!!\r\n", slot, buffer); });
        /* nothing*/;
      }
    }
    else
    {
      if (doJson)
      {
        sscanf(buffer, "%[^;];%f;%f;%f;%f;%f", recID, &EDT1, &EDT2, &ERT1, &ERT2, &GDT);
        sendNestedJsonObj(recNr++, recID, slot, EDT1, EDT2, ERT1, ERT2, GDT);
      }
      else
      {
        ({ Serial.printf("slot[%02d]->[%s]\r\n", slot, buffer); TelnetStream.printf("slot[%02d]->[%s]\r\n", slot, buffer); });
      }
    }
  }
  dataFile.close();

} // readOneSlot()

//===========================================================================================
void readSlotFromTimestamp(int8_t fileType, const char *fileName, const char *timeStamp, bool doJson, const char *rName)
{
  uint16_t firstSlot = 0, maxSlots = 0;

  ({ _debugBOL(__FUNCTION__, 299); ({ Serial.printf("timeStamp[%s]\r\n", timeStamp); TelnetStream.printf("timeStamp[%s]\r\n", timeStamp); }); });

  switch (fileType)
  {
  case HOURS:
    firstSlot = timestampToHourSlot(timeStamp, strlen(timeStamp));
    maxSlots = (48 +1);
    break;
  case DAYS:
    firstSlot = timestampToDaySlot(timeStamp, strlen(timeStamp));
    maxSlots = (14 +1);
    break;
  case MONTHS:
    firstSlot = timestampToMonthSlot(timeStamp, strlen(timeStamp));
    maxSlots = (24 +1);
    break;
  }

  firstSlot += maxSlots;
  ({ _debugBOL(__FUNCTION__, 318); ({ Serial.printf("firstSlot[%d] -> slot[%d]\r\n", firstSlot, (firstSlot % maxSlots)); TelnetStream.printf("firstSlot[%d] -> slot[%d]\r\n", firstSlot, (firstSlot % maxSlots)); }); });
  readOneSlot(fileType, fileName, firstSlot, 0, doJson, rName);

} // readSlotFromTimestamp()

//===========================================================================================
void readAllSlots(int8_t fileType, const char *fileName, const char *timeStamp, bool doJson, const char *rName)
{
  int16_t startSlot, endSlot, nrSlots, recNr = 0;

  switch (fileType)
  {
  case HOURS:
    startSlot = timestampToHourSlot(timeStamp, strlen(timeStamp));
    nrSlots = (48 +1);
    break;
  case DAYS:
    startSlot = timestampToDaySlot(timeStamp, strlen(timeStamp));
    nrSlots = (14 +1);
    break;
  case MONTHS:
    startSlot = timestampToMonthSlot(timeStamp, strlen(timeStamp));
    nrSlots = (24 +1);
    break;
  }

  endSlot = nrSlots + startSlot;
  //startSlot += nrSlots;
  ({ _debugBOL(__FUNCTION__, 346); ({ Serial.printf("start[%02d], endSlot[%02d]\r\n", (startSlot % nrSlots), endSlot); TelnetStream.printf("start[%02d], endSlot[%02d]\r\n", (startSlot % nrSlots), endSlot); }); });
  for (uint16_t s = startSlot; s < endSlot; s++)
  {
    readOneSlot(fileType, fileName, s, recNr++, false, "");
  }

} // readAllSlots()

//===========================================================================================
bool createFile(const char *fileName, uint16_t noSlots)
{
  ({ _debugBOL(__FUNCTION__, 357); ({ Serial.printf("fileName[%s], fileRecLen[%d]\r\n", fileName, 75); TelnetStream.printf("fileName[%s], fileRecLen[%d]\r\n", fileName, 75); }); });

  File dataFile = LittleFS.open(fileName, "a"); // create File
  // -- first write fileHeader ----------------------------------------
  snprintf(cMsg, sizeof(cMsg), "%s", "YYMMDDHH;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;"); // you cannot modify *fileHeader!!!
  fillRecord(cMsg, 75);
  ({ _debugBOL(__FUNCTION__, 363); ({ Serial.print(cMsg); TelnetStream.print(cMsg); }); });
  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "364" "." "316" "\", \"aSM\", @progbits, 1 #"))) = (
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r"
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "364" "." "316" "\", \"aSM\", @progbits, 1 #"))) = (
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r"
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 364 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); });
  bytesWritten = dataFile.print(cMsg);
  if (bytesWritten != 75)
  {
    ({ _debugBOL(__FUNCTION__, 368); ({ Serial.printf("ERROR!! slotNr[%d]: written [%d] bytes but should have been [%d] for Header\r\n", 0, bytesWritten, 75); TelnetStream.printf("ERROR!! slotNr[%d]: written [%d] bytes but should have been [%d] for Header\r\n", 0, bytesWritten, 75); }); });
  }
  ({ _debugBOL(__FUNCTION__, 370); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "370" "." "317" "\", \"aSM\", @progbits, 1 #"))) = (
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 ".. that went well! Now add next record ..\r"
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "370" "." "317" "\", \"aSM\", @progbits, 1 #"))) = (
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 ".. that went well! Now add next record ..\r"
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 370 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); }); });
  // -- as this file is empty, write one data record ------------
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d", 0, 0, 0, 0);

  snprintf(cMsg, sizeof(cMsg), "%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;\n", cMsg, 0.000, 0.000, 0.000, 0.000, 0.000);

  fillRecord(cMsg, 75);
  for (int r = 1; r <= noSlots; r++)
  {
    ({ _debugBOL(__FUNCTION__, 379); ({ Serial.printf("Write [%s] Data[%-9.9s]\r\n", fileName, cMsg); TelnetStream.printf("Write [%s] Data[%-9.9s]\r\n", fileName, cMsg); }); });
    dataFile.seek((r * 75), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != 75)
    {
      ({ _debugBOL(__FUNCTION__, 384); ({ Serial.printf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] \r\n", r, bytesWritten, 75); TelnetStream.printf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] \r\n", r, bytesWritten, 75); }); });
    }
  } // for ..

  dataFile.close();
  dataFile = LittleFS.open(fileName, "r+"); // open for Read & writing
  if (!dataFile)
  {
    ({ _debugBOL(__FUNCTION__, 392); ({ Serial.printf("Something is very wrong writing to [%s]\r\n", fileName); TelnetStream.printf("Something is very wrong writing to [%s]\r\n", fileName); }); });
    return false;
  }
  dataFile.close();

  return true;

} //  createFile()

//===========================================================================================
void fillRecord(char *record, int8_t len)
{
  int8_t s = 0, l = 0;
  while (record[s] != '\0' && record[s] != '\n')
  {
    s++;
  }
  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 410); ({ Serial.printf("Length of record is [%d] bytes\r\n", s); TelnetStream.printf("Length of record is [%d] bytes\r\n", s); }); });
  for (l = s; l < (len - 2); l++)
  {
    record[l] = ' ';
  }
  record[l] = ';';
  record[l + 1] = '\n';
  record[len] = '\0';

  while (record[l] != '\0')
  {
    l++;
  }
  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 424); ({ Serial.printf("Length of record is now [%d] bytes\r\n", l); TelnetStream.printf("Length of record is now [%d] bytes\r\n", l); }); });

} // fillRecord()

//====================================================================
uint16_t timestampToHourSlot(const char *TS, int8_t len)
{
  //char      aSlot[5];
  time_t t1 = epoch((char *)TS, strlen(TS), false);
  uint32_t nrHours = t1 / ((time_t)(3600UL));
  //sprintf(aSlot, "%d", ((nrDays % KEEP_DAYS_HOURS) *24) + hour(t1));
  //uint8_t   uSlot  = String(aSlot).toInt();
  uint8_t recSlot = (nrHours % (48 +1));

  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 439); ({ Serial.printf("===>>>>>  HOUR[%02d] => recSlot[%02d]\r\n", hour(t1), recSlot); TelnetStream.printf("===>>>>>  HOUR[%02d] => recSlot[%02d]\r\n", hour(t1), recSlot); }); });

  if (recSlot < 0 || recSlot >= (48 +1))
  {
    ({ _debugBOL(__FUNCTION__, 443); ({ Serial.printf("HOUR: Some serious error! Slot is [%d]\r\n", recSlot); TelnetStream.printf("HOUR: Some serious error! Slot is [%d]\r\n", recSlot); }); });
    recSlot = (48 +1);
    slotErrors++;
  }
  return recSlot;

} // timestampToHourSlot()

//====================================================================
uint16_t timestampToDaySlot(const char *TS, int8_t len)
{
  //char      aSlot[5];
  time_t t1 = epoch((char *)TS, strlen(TS), false);
  uint32_t nrDays = t1 / ((time_t)(((time_t)(3600UL)) * 24UL));
  uint16_t recSlot = (nrDays % (14 +1));

  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 460); ({ Serial.printf("===>>>>>   DAY[%02d] => recSlot[%02d]\r\n", day(t1), recSlot); TelnetStream.printf("===>>>>>   DAY[%02d] => recSlot[%02d]\r\n", day(t1), recSlot); }); });

  if (recSlot < 0 || recSlot >= (14 +1))
  {
    ({ _debugBOL(__FUNCTION__, 464); ({ Serial.printf("DAY: Some serious error! Slot is [%d]\r\n", recSlot); TelnetStream.printf("DAY: Some serious error! Slot is [%d]\r\n", recSlot); }); });
    recSlot = (14 +1);
    slotErrors++;
  }
  return recSlot;

} // timestampToDaySlot()

//====================================================================
uint16_t timestampToMonthSlot(const char *TS, int8_t len)
{
  //char      aSlot[5];
  time_t t1 = epoch((char *)TS, strlen(TS), false);
  uint32_t nrMonths = ((year(t1) - 1) * 12) + month(t1); // eg: year(2023) * 12 = 24276 + month(9) = 202309
  uint16_t recSlot = (nrMonths % (24 +1)); // eg: 24285 % _NO_MONTH_SLOT_

  if (Verbose1)
    ({ _debugBOL(__FUNCTION__, 481); ({ Serial.printf("===>>>>> MONTH[%02d] => recSlot[%02d]\r\n", month(t1), recSlot); TelnetStream.printf("===>>>>> MONTH[%02d] => recSlot[%02d]\r\n", month(t1), recSlot); }); });

  if (recSlot < 0 || recSlot >= (24 +1))
  {
    ({ _debugBOL(__FUNCTION__, 485); ({ Serial.printf("MONTH: Some serious error! Slot is [%d]\r\n", recSlot); TelnetStream.printf("MONTH: Some serious error! Slot is [%d]\r\n", recSlot); }); });
    recSlot = (24 +1);
    slotErrors++;
  }
  return recSlot;

} // timestampToMonthSlot()

//===========================================================================================
int32_t freeSpace()
{
  int32_t space;

  LittleFS.info(SPIFFSinfo);

  space = (int32_t)(SPIFFSinfo.totalBytes - SPIFFSinfo.usedBytes);

  return space;

} // freeSpace()

//===========================================================================================
void listSPIFFS()
{
  typedef struct _fileMeta
  {
    char Name[20];
    int32_t Size;
  } fileMeta;

  _fileMeta dirMap[30];
  int fileNr = 0;

  Dir dir = LittleFS.openDir("/"); // List files on SPIFFS
  while (dir.next())
  {
    dirMap[fileNr].Name[0] = '\0';
    if (dir.fileName()[0] != '/')
    {
      strncat(dirMap[fileNr].Name, dir.fileName().substring(0).c_str(), 30); // In LITTLEFS no leading "/"
    }
    else
    {
      strncat(dirMap[fileNr].Name, dir.fileName().substring(1).c_str(), 29); // remove leading '/'
    }
    dirMap[fileNr].Size = dir.fileSize();
    fileNr++;
  }

  // -- bubble sort dirMap op .Name--
  for (int8_t y = 0; y < fileNr; y++)
  {
    yield();
    for (int8_t x = y + 1; x < fileNr; x++)
    {
      //DebugTf("y[%d], x[%d] => seq[x][%s] ", y, x, dirMap[x].Name);
      if (compare(String(dirMap[x].Name), String(dirMap[y].Name)))
      {
        fileMeta temp = dirMap[y];
        dirMap[y] = dirMap[x];
        dirMap[x] = temp;
      } /* end if */
      //Debugln();
    } /* end for */
  } /* end for */

  ({ _debugBOL(__FUNCTION__, 551); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "551" "." "318" "\", \"aSM\", @progbits, 1 #"))) = (
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r\n"
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "551" "." "318" "\", \"aSM\", @progbits, 1 #"))) = (
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r\n"
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 551 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); }); });
  for (int f = 0; f < fileNr; f++)
  {
    ({ Serial.printf("%-25s %6d bytes \r\n", dirMap[f].Name, dirMap[f].Size); TelnetStream.printf("%-25s %6d bytes \r\n", dirMap[f].Name, dirMap[f].Size); });
    yield();
  }

  LittleFS.info(SPIFFSinfo);

  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "560" "." "319" "\", \"aSM\", @progbits, 1 #"))) = (
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r"
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "560" "." "319" "\", \"aSM\", @progbits, 1 #"))) = (
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 "\r"
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
 ); &__pstr__[0];}))
# 560 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
 )))); });
  if (freeSpace() < (10 * SPIFFSinfo.blockSize))
    ({ Serial.printf("Available SPIFFS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024)); TelnetStream.printf("Available SPIFFS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024)); });
  else
    ({ Serial.printf("Available SPIFFS space [%6d]kB\r\n", (freeSpace() / 1024)); TelnetStream.printf("Available SPIFFS space [%6d]kB\r\n", (freeSpace() / 1024)); });
  ({ Serial.printf("           SPIFFS Size [%6d]kB\r\n", (SPIFFSinfo.totalBytes / 1024)); TelnetStream.printf("           SPIFFS Size [%6d]kB\r\n", (SPIFFSinfo.totalBytes / 1024)); });
  ({ Serial.printf("     SPIFFS block Size [%6d]bytes\r\n", SPIFFSinfo.blockSize); TelnetStream.printf("     SPIFFS block Size [%6d]bytes\r\n", SPIFFSinfo.blockSize); });
  ({ Serial.printf("      SPIFFS page Size [%6d]bytes\r\n", SPIFFSinfo.pageSize); TelnetStream.printf("      SPIFFS page Size [%6d]bytes\r\n", SPIFFSinfo.pageSize); });
  ({ Serial.printf(" SPIFFS max.Open Files [%6d]\r\n\r\n", SPIFFSinfo.maxOpenFiles); TelnetStream.printf(" SPIFFS max.Open Files [%6d]\r\n\r\n", SPIFFSinfo.maxOpenFiles); });

} // listSPIFFS()

//===========================================================================================
bool eraseFile()
{
  char eName[30] = "";

  //--- erase buffer
  while (TelnetStream.available() > 0)
  {
    yield();
    (char)TelnetStream.read();
  }

  ({ Serial.print("Enter filename to erase: "); TelnetStream.print("Enter filename to erase: "); });
  TelnetStream.setTimeout(10000);
  TelnetStream.readBytesUntil('\n', eName, sizeof(eName));
  TelnetStream.setTimeout(1000);

  //--- remove control chars like \r and \n ----
  //--- and shift all char's one to the right --
  for (int i = strlen(eName); i > 0; i--)
  {
    eName[i] = eName[i - 1];
    if (eName[i] < ' ')
      eName[i] = '\0';
  }
  //--- add leading slash on position 0
  eName[0] = '/';

  if (LittleFS.exists(eName))
  {
    ({ Serial.printf("\r\nErasing [%s] from SPIFFS\r\n\n", eName); TelnetStream.printf("\r\nErasing [%s] from SPIFFS\r\n\n", eName); });
    LittleFS.remove(eName);
  }
  else
  {
    ({ Serial.printf("\r\nfile [%s] not found..\r\n\n", eName); TelnetStream.printf("\r\nfile [%s] not found..\r\n\n", eName); });
  }
  //--- empty buffer ---
  while (TelnetStream.available() > 0)
  {
    yield();
    (char)TelnetStream.read();
  }

} // eraseFile()

//===========================================================================================
bool DSMRfileExist(const char *fileName, bool doDisplay)
{
  char fName[30] = "";
  if (fileName[0] != '/')
  {
    strConcat(fName, 5, "/");
  }
  strConcat(fName, 29, fileName);

  ({ _debugBOL(__FUNCTION__, 628); ({ Serial.printf("check if [%s] exists .. ", fName); TelnetStream.printf("check if [%s] exists .. ", fName); }); });
  if (settingOledType > 0)
  {
    oled_Print_Msg(1, "Bestaat:", 10);
    oled_Print_Msg(2, fName, 10);
    oled_Print_Msg(3, "op SPIFFS?", 250);
  }

  if (!LittleFS.exists(fName))
  {
    if (doDisplay)
    {
      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "640" "." "320" "\", \"aSM\", @progbits, 1 #"))) = (
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     "NO! Error!!"
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     ); &__pstr__[0];}))
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "640" "." "320" "\", \"aSM\", @progbits, 1 #"))) = (
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     "NO! Error!!"
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     ); &__pstr__[0];}))
# 640 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     )))); });
      if (settingOledType > 0)
      {
        oled_Print_Msg(3, "Nee! FOUT!", 6000);
      }
      /* nothing*/;
      return false;
    }
    else
    {
      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "650" "." "321" "\", \"aSM\", @progbits, 1 #"))) = (
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     "NO! "
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     ); &__pstr__[0];}))
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "650" "." "321" "\", \"aSM\", @progbits, 1 #"))) = (
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     "NO! "
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
     ); &__pstr__[0];}))
# 650 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
     )))); });
      if (settingOledType > 0)
      {
        oled_Print_Msg(3, "Nee! ", 6000);
      }
      /* nothing*/;
      return false;
    }
  }
  else
  {
    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "661" "." "322" "\", \"aSM\", @progbits, 1 #"))) = (
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
   "Yes! OK!"
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
   ); &__pstr__[0];}))
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "SPIFFSstuff.ino" "." "661" "." "322" "\", \"aSM\", @progbits, 1 #"))) = (
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
   "Yes! OK!"
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino" 3
   ); &__pstr__[0];}))
# 661 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
   )))); });
    if (settingOledType > 0)
    {
      oled_Print_Msg(3, "JA! (OK!)", 250);
    }
  }
  return true;

} //  DSMRfileExist()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\convertPRD2RING.ino"
/* 
***************************************************************************  
**  Program  : convertPRD2RING, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=====================================================================
void convertPRD2RING()
{
    if (DSMRfileExist("PRDhours.csv", false) )
    {
      LittleFS.remove("/RINGhours.csv");
      convertPRDfile(HOURS);
    }
    if (DSMRfileExist("PRDdays.csv", false) )
    {
      LittleFS.remove("/RINGdays.csv");
      convertPRDfile(DAYS);
    }
    if (DSMRfileExist("PRDmonths.csv", false) )
    {
      LittleFS.remove("/RINGmonths.csv");
      convertPRDfile(MONTHS);
    }
    LittleFS.remove("/!PRDconvert");

} // convertPRD2RING()

//=====================================================================
void convertPRDfile(int8_t fileType)
{
  char PRDfileName[30];
  char buffer[200];
  char recKey[15];
  float EDT1, EDT2, ERT1, ERT2, GDT;
  int offSet = 0, maxRecs = 0;

  ({ Serial.println("convertPRDfile() =============================================\r\n"); TelnetStream.println("convertPRDfile() =============================================\r\n"); });

  switch(fileType)
  {
    case HOURS: strCopy(PRDfileName, sizeof(PRDfileName), "/PRDhours.csv");
                  maxRecs = 49;
                  break;
    case DAYS: strCopy(PRDfileName, sizeof(PRDfileName), "/PRDdays.csv");
                  maxRecs = 15;
                  break;
    case MONTHS: strCopy(PRDfileName, sizeof(PRDfileName), "/PRDmonths.csv");
                  maxRecs = 25;
                  break;

  } // switch()

  File PRDfile = LittleFS.open(PRDfileName, "r"); // open for Read 
  if (!PRDfile)
  {
    ({ _debugBOL(__FUNCTION__, 62); ({ Serial.printf("File [%s] does not exist, skip\r\n", PRDfileName); TelnetStream.printf("File [%s] does not exist, skip\r\n", PRDfileName); }); });
    return;
  } // if (!dataFile)

  int recLen = PRDfile.readBytesUntil('\n', buffer, sizeof(buffer)) +1;
  ({ _debugBOL(__FUNCTION__, 67); ({ Serial.printf("recLen[%02d]\r\n", recLen); TelnetStream.printf("recLen[%02d]\r\n", recLen); }); });

  for(int r=maxRecs; r>0; r--)
  {
      offSet = r * recLen;
      ({ _debugBOL(__FUNCTION__, 72); ({ Serial.printf("offSet[%4d] => ", offSet); TelnetStream.printf("offSet[%4d] => ", offSet); }); });
      PRDfile.seek(offSet, SeekSet);
      int l = PRDfile.readBytesUntil('\n', buffer, sizeof(buffer));
      buffer[l] = 0;
      sscanf(buffer, "%[^;]; %f; %f; %f; %f; %f;", recKey, &EDT1, &EDT2, &ERT1, &ERT2, &GDT);
      /*
      DebugTf("values key[%s], EDT1[%8.3f], ERT2[%8.3f], EDT1[%8.3f[, EDT2[%8.3f], GDT[%8.3f]\r\n"
                                                    , recKey
                                                    , EDT1, EDT2
                                                    , ERT1, ERT2
                                                    , GDT);
      */
      ({ Serial.printf("recKey[%s] --> \r\n", recKey); TelnetStream.printf("recKey[%s] --> \r\n", recKey); });
      if (isNumericp(recKey, strlen(recKey)))
      {
        writeToRINGfile(fileType, recKey, EDT1, EDT2, ERT1, ERT2, GDT);
      }
      yield();
  } // for r ..

  PRDfile.close();

} // convertPRDfile()

//=====================================================================
void writeToRINGfile(int8_t fileType, const char *key, float EDT1, float EDT2
                                      , float ERT1, float ERT2, float GDT)
{
  char record[75 + 1] = "";
  char newKey[15];
  uint16_t recSlot;

  // key is:
  //   hours:  YYMMDDHH concat mmssX
  //    days:  YYMMDD   concat HHmmssX
  //  months:  YYMM     concat DDHHmmssX
  strCopy(newKey, 14, key);

  switch(fileType)
  {
    case HOURS: strConcat(newKey, 14, "0101X");
                  recSlot = timestampToHourSlot(newKey, strlen(newKey));
                  break;
    case DAYS: strConcat(newKey, 14, "230101X");
                  recSlot = timestampToDaySlot(newKey, strlen(newKey));
                  break;
    case MONTHS: strConcat(newKey, 14, "01230101X");
                  recSlot = timestampToMonthSlot(newKey, strlen(newKey));
                  break;

  } // switch()

  snprintf(record, sizeof(record), (char*)"%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;\n", newKey , (float)EDT1
                                             , (float)EDT2
                                             , (float)ERT1
                                             , (float)ERT2
                                             , (float)GDT);

  // DATA + \n + \0                                        
  fillRecord(record, 75);

  if (Verbose2) ({ Serial.printf("key[%s], slot[%02d] %s\r", newKey, recSlot, record); TelnetStream.printf("key[%s], slot[%02d] %s\r", newKey, recSlot, record); });

  switch(fileType)
  {
    case HOURS: writeDataToFile("/RINGhours.csv", record, recSlot, HOURS);
                  break;
    case DAYS: writeDataToFile("/RINGdays.csv", record, recSlot, DAYS);
                  break;
    case MONTHS: writeDataToFile("/RINGmonths.csv", record, recSlot, MONTHS);
                  break;

  } // switch()

} // writeToRINGfile()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
/*
***************************************************************************  
**  Program  : handleSlimmeMeter - part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//==================================================================================
void handleSlimmemeter()
{
  //DebugTf("showRaw (%s)\r\n", showRaw ?"true":"false");
  if (showRaw) {
    //-- process telegrams in raw mode
    processSlimmemeterRaw();
  }
  else
  {
    processSlimmemeter();
  }

} // handleSlimmemeter()


//==================================================================================
void processSlimmemeterRaw()
{
  char tlgrm[1200] = "";

  ({ _debugBOL(__FUNCTION__, 34); ({ Serial.printf("handleSlimmerMeter RawCount=[%4d]\r\n", showRawCount); TelnetStream.printf("handleSlimmerMeter RawCount=[%4d]\r\n", showRawCount); }); });
  showRawCount++;
  showRaw = (showRawCount <= 20);
  if (!showRaw)
  {
    showRawCount = 0;
    return;
  }

  if (settingOledType > 0)
  {
    oled_Print_Msg(0, "<DSMRloggerAPI>", 0);
    oled_Print_Msg(1, "-------------------------",0);
    oled_Print_Msg(2, "Raw Format",0);
    snprintf(cMsg, sizeof(cMsg), "Raw Count %4d", showRawCount);
    oled_Print_Msg(3, cMsg, 0);
  }

  slimmeMeter.enable(true);
  Serial.setTimeout(5000); // 5 seconds must be enough ..
  memset(tlgrm, 0, sizeof(tlgrm));
  int l = 0;
  // The terminator character is discarded from the serial buffer.
  l = Serial.readBytesUntil('/', tlgrm, sizeof(tlgrm));
  // now read from '/' to '!'
  // The terminator character is discarded from the serial buffer.
  l = Serial.readBytesUntil('!', tlgrm, sizeof(tlgrm));
  Serial.setTimeout(1000); // seems to be the default ..
//  DebugTf("read [%d] bytes\r\n", l);
  if (l == 0)
  {
    ({ _debugBOL(__FUNCTION__, 65); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "handleSlimmeMeter.ino" "." "65" "." "323" "\", \"aSM\", @progbits, 1 #"))) = (
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   "RawMode: Timerout - no telegram received within 5 seconds"
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   ); &__pstr__[0];}))
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "handleSlimmeMeter.ino" "." "65" "." "323" "\", \"aSM\", @progbits, 1 #"))) = (
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   "RawMode: Timerout - no telegram received within 5 seconds"
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   ); &__pstr__[0];}))
# 65 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   )))); }); });
    return;
  }

  tlgrm[l++] = '!';

  // next 6 bytes are "<CRC>\r\n"
  for (int i=0; ( i<6 && (i<(sizeof(tlgrm)-7)) ); i++)
  {
    tlgrm[l++] = (char)Serial.read();
  }




  tlgrm[(l +1)] = '\0';
  // shift telegram 1 char to the right (make room at pos [0] for '/')
  for (int i=strlen(tlgrm); i>=0; i--) { tlgrm[i+1] = tlgrm[i]; yield(); }
  tlgrm[0] = '/';
  //Post result to Debug 
  ({ Serial.printf("Telegram (%d chars):\r\n/%s\r\n", strlen(tlgrm), tlgrm); TelnetStream.printf("Telegram (%d chars):\r\n/%s\r\n", strlen(tlgrm), tlgrm); });
  return;

} // processSlimmemeterRaw()


//==================================================================================
void processSlimmemeter()
{
  slimmeMeter.loop();
  if (slimmeMeter.available())
  {
    ({ _debugBOL(__FUNCTION__, 97); ({ Serial.printf("telegramCount=[%d] telegramErrors=[%d]\r\n", telegramCount, telegramErrors); TelnetStream.printf("telegramCount=[%d] telegramErrors=[%d]\r\n", telegramCount, telegramErrors); }); });
    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "handleSlimmeMeter.ino" "." "98" "." "324" "\", \"aSM\", @progbits, 1 #"))) = (
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   "\r\n[Time----][FreeHeap/mBlck][Function----(line):\r"
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   ); &__pstr__[0];}))
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "handleSlimmeMeter.ino" "." "98" "." "324" "\", \"aSM\", @progbits, 1 #"))) = (
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   "\r\n[Time----][FreeHeap/mBlck][Function----(line):\r"
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3
   ); &__pstr__[0];}))
# 98 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
   )))); });
    // Voorbeeld: [21:00:11][   9880/  8960] loop        ( 997): read telegram [28] => [140307210001S]
    telegramCount++;

    DSMRdata = {};
    String DSMRerror;

    if (slimmeMeter.parse(&DSMRdata, &DSMRerror)) // Parse succesful, print result
    {
      if (telegramCount > (
# 107 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino" 3 4
                          (0xffffffffUL) 
# 107 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
                                     - 10))
      {
        delay(1000);
        ESP.reset();
        delay(1000);
      }
      digitalWrite(2, 0x1);
      if (DSMRdata.identification_present)
      {
        //--- this is a hack! The identification can have a backslash in it
        //--- that will ruin javascript processing :-(
        for(int i=0; i<DSMRdata.identification.length(); i++)
        {
          if (DSMRdata.identification[i] == '\\') DSMRdata.identification[i] = '=';
          yield();
        }
      }
      if (!settingSmHasFaseInfo)
      {
        if (DSMRdata.power_delivered_present && !DSMRdata.power_delivered_l1_present)
        {
          DSMRdata.power_delivered_l1 = DSMRdata.power_delivered;
          DSMRdata.power_delivered_l1_present = true;
          DSMRdata.power_delivered_l2_present = true;
          DSMRdata.power_delivered_l3_present = true;
        }
        if (DSMRdata.power_returned_present && !DSMRdata.power_returned_l1_present)
        {
          DSMRdata.power_returned_l1 = DSMRdata.power_returned;
          DSMRdata.power_returned_l1_present = true;
          DSMRdata.power_returned_l2_present = true;
          DSMRdata.power_returned_l3_present = true;
        }
      } // No Fase Info
# 153 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
      processTelegram();
      if (Verbose2)
      {
        DSMRdata.applyEach(showValues());
      }

    }
    else // Parser error, print error
    {
      telegramErrors++;



      ({ _debugBOL(__FUNCTION__, 166); ({ Serial.printf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str()); TelnetStream.printf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str()); }); });
      //--- set DTR to get a new telegram as soon as possible
      slimmeMeter.enable(true);
      slimmeMeter.loop();
    }

    if ( (telegramCount > 25) && (telegramCount % (2100 / (settingTelegramInterval + 1)) == 0) )
    {
      ({ _debugBOL(__FUNCTION__, 174); ({ Serial.printf("Processed [%d] telegrams ([%d] errors)\r\n", telegramCount, telegramErrors); TelnetStream.printf("Processed [%d] telegrams ([%d] errors)\r\n", telegramCount, telegramErrors); }); });
      /* nothing*/;
    }

  } // if (slimmeMeter.available()) 

} // handleSlimmeMeter()



/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleTestdata.ino"
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
/* 
***************************************************************************  
**  Program  : helperStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//===========================================================================================
bool compare(String x, String y)
{
    for (int i = 0; i < min(x.length(), y.length()); i++) {
      if (x[i] != y[i])
      {
        return (bool)(x[i] < y[i]);
      }
    }
    return x.length() < y.length();

} // compare()


//===========================================================================================
boolean isValidIP(IPAddress ip)
{
 /* Works as follows:
   *  example: 
   *  127.0.0.1 
   *   1 => 127||0||0||1 = 128>0 = true 
   *   2 => !(false || false) = true
   *   3 => !(false || false || false || false ) = true
   *   4 => !(true && true && true && true) = false
   *   5 => !(false) = true
   *   true && true & true && false && true = false ==> correct, this is an invalid addres
   *   
   *   0.0.0.0
   *   1 => 0||0||0||0 = 0>0 = false 
   *   2 => !(true || true) = false
   *   3 => !(false || false || false || false) = true
   *   4 => !(true && true && true && tfalse) = true
   *   5 => !(false) = true
   *   false && false && true && true && true = false ==> correct, this is an invalid addres
   *   
   *   192.168.0.1 
   *   1 => 192||168||0||1 =233>0 = true 
   *   2 => !(false || false) = true
   *   3 +> !(false || false || false || false) = true
   *   4 => !(false && false && true && true) = true
   *   5 => !(false) = true
   *   true & true & true && true && true = true ==> correct, this is a valid address
   *   
   *   255.255.255.255
   *   1 => 255||255||255||255 =255>0 = true 
   *   2 => !(false || false ) = true
   *   3 +> !(true || true || true || true) = false
   *   4 => !(false && false && false && false) = true
   *   5 => !(true) = false
   *   true && true && false && true && false = false ==> correct, this is an invalid address
   *   
   *   0.123.12.1       => true && false && true && true && true = false  ==> correct, this is an invalid address 
   *   10.0.0.0         => true && false && true && true && true = false  ==> correct, this is an invalid address 
   *   10.255.0.1       => true && true && false && true && true = false  ==> correct, this is an invalid address 
   *   150.150.255.150  => true && true && false && true && true = false  ==> correct, this is an invalid address 
   *   
   *   123.21.1.99      => true && true && true && true && true = true    ==> correct, this is annvalid address 
   *   1.1.1.1          => true && true && true && true && true = true    ==> correct, this is annvalid address 
   *   
   *   Some references on valid ip addresses: 
   *   - https://www.quora.com/How-do-you-identify-an-invalid-IP-address
   *   
   */
  boolean _isValidIP = false;
  _isValidIP = ((ip[0] || ip[1] || ip[2] || ip[3])>0); // if any bits are set, then it is not 0.0.0.0
  _isValidIP &= !((ip[0]==0) || (ip[3]==0)); // if either the first or last is a 0, then it is invalid
  _isValidIP &= !((ip[0]==255) || (ip[1]==255) || (ip[2]==255) || (ip[3]==255)) ; // if any of the octets is 255, then it is invalid  
  _isValidIP &= !(ip[0]==127 && ip[1]==0 && ip[2]==0 && ip[3]==1); // if not 127.0.0.0 then it might be valid
  _isValidIP &= !(ip[0]>=224); // if ip[0] >=224 then reserved space  

  ({ _debugBOL(__FUNCTION__, 83); ({ Serial.printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]); TelnetStream.printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]); }); });
  if (_isValidIP)
    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "helperStuff.ino" "." "85" "." "325" "\", \"aSM\", @progbits, 1 #"))) = (
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   " = Valid IP"
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   ); &__pstr__[0];}))
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "helperStuff.ino" "." "85" "." "325" "\", \"aSM\", @progbits, 1 #"))) = (
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   " = Valid IP"
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   ); &__pstr__[0];}))
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   )))); });
  else
    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "helperStuff.ino" "." "87" "." "326" "\", \"aSM\", @progbits, 1 #"))) = (
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   " = Invalid IP!"
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   ); &__pstr__[0];}))
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "helperStuff.ino" "." "87" "." "326" "\", \"aSM\", @progbits, 1 #"))) = (
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   " = Invalid IP!"
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3
   ); &__pstr__[0];}))
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
   )))); });

  return _isValidIP;

} //  isValidIP()


//===========================================================================================
bool isNumericp(const char *timeStamp, int8_t len)
{
  for (int i=0; (i<len && i<12);i++)
  {
    if (timeStamp[i] < '0' || timeStamp[i] > '9')
    {
      return false;
    }
  }
  return true;

} // isNumericp()


//===========================================================================================
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords)
{
  int16_t inxS = 0, inxE = 0, wordCount = 0;

    inStrng.trim();
    while(inxE < inStrng.length() && wordCount < maxWords)
    {
      inxE = inStrng.indexOf(delimiter, inxS); //finds location of first ,
      wOut[wordCount] = inStrng.substring(inxS, inxE); //captures first data String
      wOut[wordCount].trim();
      //DebugTf("[%d] => [%c] @[%d] found[%s]\r\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
      inxS = inxE;
      inxS++;
      wordCount++;
    }
    // zero rest of the words
    for(int i=wordCount; i< maxWords; i++)
    {
      wOut[wordCount][0] = 0;
    }
    // if not whole string processed place rest in last word
    if (inxS < inStrng.length())
    {
      wOut[maxWords-1] = inStrng.substring(inxS, inStrng.length()); // store rest of String      
    }

    return wordCount;

} // splitString()


//===========================================================================================
String upTime()
{
  char calcUptime[20];

  snprintf(calcUptime, sizeof(calcUptime), "%d(d)-%02d:%02d(H:m)"
                                          , int((upTimeSeconds / (60 * 60 * 24)) % 365)
                                          , int((upTimeSeconds / (60 * 60)) % 24)
                                          , int((upTimeSeconds / (60)) % 60));

  return calcUptime;

} // upTime()


//===========================================================================================
void strConcat(char *dest, int maxLen, const char *src)
{
  if (strlen(dest) + strlen(src) < maxLen)
  {
    strcat(dest, src);
  }
  else
  {
    ({ _debugBOL(__FUNCTION__, 165); ({ Serial.printf("Combined string > %d chars\r\n", maxLen); TelnetStream.printf("Combined string > %d chars\r\n", maxLen); }); });
  }

} // strConcat()


//===========================================================================================
void strConcat(char *dest, int maxLen, float v, int dec)
{
  static char buff[25];
  if (dec == 0) sprintf(buff,"%.0f", v);
  else if (dec == 1) sprintf(buff,"%.1f", v);
  else if (dec == 2) sprintf(buff,"%.2f", v);
  else if (dec == 3) sprintf(buff,"%.3f", v);
  else if (dec == 4) sprintf(buff,"%.4f", v);
  else if (dec == 5) sprintf(buff,"%.5f", v);
  else sprintf(buff,"%f", v);

  if (strlen(dest) + strlen(buff) < maxLen)
  {
    strcat(dest, buff);
  }
  else
  {
    ({ _debugBOL(__FUNCTION__, 189); ({ Serial.printf("Combined string > %d chars\r\n", maxLen); TelnetStream.printf("Combined string > %d chars\r\n", maxLen); }); });
  }

} // strConcat()


//===========================================================================================
void strConcat(char *dest, int maxLen, int32_t v)
{
  static char buff[25];
  sprintf(buff,"%d", v);

  if (strlen(dest) + strlen(buff) < maxLen)
  {
    strcat(dest, buff);
  }
  else
  {
    ({ _debugBOL(__FUNCTION__, 207); ({ Serial.printf("Combined string > %d chars\r\n", maxLen); TelnetStream.printf("Combined string > %d chars\r\n", maxLen); }); });
  }

} // strConcat()


//===========================================================================================
void strToLower(char *src)
{
  for (int i = 0; i < strlen(src); i++)
  {
    if (src[i] == '\0') return;
    if (src[i] >= 'A' && src[i] <= 'Z')
        src[i] += 32;
  }
} // strToLower()

//===========================================================================================
// a 'save' string copy
void strCopy(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to)
{
  int d=0;
//DebugTf("dest[%s], src[%s] max[%d], frm[%d], to[%d] =>\r\n", dest, src, maxLen, frm, to);
  dest[0] = '\0';
  for (int i=0; i<=frm; i++)
  {
    if (src[i] == 0) return;
  }
  for (int i=frm; (src[i] != 0 && i<=to && d<maxLen); i++)
  {
    dest[d++] = src[i];
  }
  dest[d] = '\0';

} // strCopy()

//===========================================================================================
// a 'save' version of strncpy() that does not put a '\0' at
// the end of dest if src >= maxLen!
void strCopy(char *dest, int maxLen, const char *src)
{
  dest[0] = '\0';
  strcat(dest, src);

} // strCopy()


//===========================================================================================
int stricmp(const char *a, const char *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }

} // stricmp()

//===========================================================================================
char *intToStr(int32_t v)
{
  static char buff[25];
  sprintf(buff,"%d", v);
  return buff;

} // intToStr()

//===========================================================================================
char *floatToStr(float v, int dec)
{
  static char buff[25];
  if (dec == 0) sprintf(buff,"%.0f", v);
  else if (dec == 1) sprintf(buff,"%.1f", v);
  else if (dec == 2) sprintf(buff,"%.2f", v);
  else if (dec == 3) sprintf(buff,"%.3f", v);
  else if (dec == 4) sprintf(buff,"%.4f", v);
  else if (dec == 5) sprintf(buff,"%.5f", v);
  else sprintf(buff,"%f", v);
  return buff;

} // floattToStr()

//===========================================================================================
float formatFloat(float v, int dec)
{
  return (String(v, dec).toFloat());

} //  formatFloat()

//===========================================================================================
float strToFloat(const char *s, int dec)
{
  float r = 0.0;
  int p = 0;
  int d = -1;

  r = strtof(s, 
# 303 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino" 3 4
               __null
# 303 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
                   );
  p = (int)(r*pow(10, dec));
  r = p / pow(10, dec);
  //DebugTf("[%s][%d] => p[%d] -> r[%f]\r\n", s, dec, p, r);
  return r;

} //  strToFloat()


//=======================================================================        
template<typename Item>
Item& typecastValue(Item& i)
{
  return i;
}

//=======================================================================        
float typecastValue(TimestampedFixedValue i)
{
  return strToFloat(String(i).c_str(), 3);
}

//=======================================================================        
float typecastValue(FixedValue i)
{
  return i;
}

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
/* 
***************************************************************************  
**  Program  : jsonStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
static char objSprtr[10] = "";

//=======================================================================
void sendStartJsonObj(const char *objName)
{
  char sBuff[50] = "";
  objSprtr[0] = '\0';

  snprintf(sBuff, sizeof(sBuff), "{\"%s\":[\r\n", objName);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(((size_t) -1));
  httpServer.send(200, "application/json", sBuff);

} // sendStartJsonObj()


//=======================================================================
void sendEndJsonObj()
{
  httpServer.sendContent("\r\n]}\r\n");

  //httpServer.sendHeader( "Content-Length", "0");
  //httpServer.send ( 200, "application/json", "");

} // sendEndJsonObj()

//=======================================================================
void sendNestedJsonObj(uint8_t recNr, const char *recID, uint8_t slot, float EDT1, float EDT2, float ERT1, float ERT2, float GDT)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"recnr\": %d, \"recid\": \"%s\", \"slot\": %d,"
                          "\"edt1\": %.3f, \"edt2\": %.3f,"
                          "\"ert1\": %.3f, \"ert2\": %.3f,"
                          "\"gdt\": %.3f}"
                                      , objSprtr, recNr, recID, slot, EDT1, EDT2, ERT1, ERT2, GDT);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(int, *char, int, float, float, float, float, float)


//=======================================================================
void sendNestedJsonObj(const char *cName, const char *cValue, const char *cUnit)
{
  char jsonBuff[255] = "";

  if (strlen(cUnit) == 0)
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
                                      , objSprtr, cName, cValue);
  }
  else
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\", \"unit\": \"%s\"}"
                                      , objSprtr, cName, cValue, cUnit);
  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, *char, *char)

//---------------------------------------------------------------
void sendNestedJsonObj(const char *cName, const char *cValue)
{
  char noUnit[] = {'\0'};

  sendNestedJsonObj(cName, cValue, noUnit);

} // sendNestedJsonObj(*char, *char)


//=======================================================================
void sendNestedJsonObj(const char *cName, String sValue, const char *cUnit)
{
  char jsonBuff[255] = "";

  if (sValue.length() > (255 - 65) )
  {
    ({ _debugBOL(__FUNCTION__, 92); ({ Serial.printf("[2] sValue.length() [%d]\r\n", sValue.length()); TelnetStream.printf("[2] sValue.length() [%d]\r\n", sValue.length()); }); });
  }

  if (strlen(cUnit) == 0)
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
                                      , objSprtr, cName, sValue.c_str());
  }
  else
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\", \"unit\": \"%s\"}"
                                      , objSprtr, cName, sValue.c_str(), cUnit);
  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, String, *char)

//---------------------------------------------------------------
void sendNestedJsonObj(const char *cName, String sValue)
{
  char noUnit[] = {'\0'};

  if (sValue.length() > (255 - 60) )
  {
    sendNestedJsonObj(cName, sValue.substring(0,(255 - (strlen(cName) + 30))), noUnit);
  }
  else
  {
    sendNestedJsonObj(cName, sValue, noUnit);
  }

} // sendNestedJsonObj(*char, String)


//=======================================================================
void sendNestedJsonObj(const char *cName, int32_t iValue, const char *cUnit)
{
  char jsonBuff[200] = "";

  if (strlen(cUnit) == 0)
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d}"
                                      , objSprtr, cName, iValue);
  }
  else
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d, \"unit\": \"%s\"}"
                                      , objSprtr, cName, iValue, cUnit);
  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, int, *char)

//---------------------------------------------------------------
void sendNestedJsonObj(const char *cName, int32_t iValue)
{
  char noUnit[] = {'\0'};

  sendNestedJsonObj(cName, iValue, noUnit);

} // sendNestedJsonObj(*char, int)


//=======================================================================
void sendNestedJsonObj(const char *cName, uint32_t uValue, const char *cUnit)
{
  char jsonBuff[200] = "";

  if (strlen(cUnit) == 0)
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u}"
                                      , objSprtr, cName, uValue);
  }
  else
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u, \"unit\": \"%s\"}"
                                      , objSprtr, cName, uValue, cUnit);
  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, uint, *char)

//---------------------------------------------------------------
void sendNestedJsonObj(const char *cName, uint32_t uValue)
{
  char noUnit[] = {'\0'};

  sendNestedJsonObj(cName, uValue, noUnit);

} // sendNestedJsonObj(*char, uint)


//=======================================================================
void sendNestedJsonObj(const char *cName, float fValue, const char *cUnit)
{
  char jsonBuff[200] = "";

  if (strlen(cUnit) == 0)
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f}"
                                      , objSprtr, cName, fValue);
  }
  else
  {
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"unit\": \"%s\"}"
                                      , objSprtr, cName, fValue, cUnit);
  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, float, *char)

//---------------------------------------------------------------
void sendNestedJsonObj(const char *cName, float fValue)
{
  char noUnit[] = {'\0'};

  sendNestedJsonObj(cName, fValue, noUnit);

} // sendNestedJsonObj(*char, float)


//=======================================================================
//----- v0 api ----------------------------------------------------------
//=======================================================================
void sendNestedJsonV0Obj(const char *cName, uint32_t uValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s \"%s\": %u"
                                      , objSprtr, cName, uValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonV0Obj(*char, uint)

//---------------------------------------------------------------
void sendNestedJsonV0Obj(const char *cName, float fValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s \"%s\": %.3f"
                                      , objSprtr, cName, fValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonV0Obj(*char, float)

//---------------------------------------------------------------
void sendNestedJsonV0Obj(const char *cName, int32_t iValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s \"%s\": %u"
                                      , objSprtr, cName, iValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedV0Obj(*char, int)

//---------------------------------------------------------------
void sendNestedJsonV0Obj(const char *cName, String sValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s \"%s\": \"%s\""
                                      , objSprtr, cName
                                      , sValue.substring(0,(150 - (strlen(cName)))).c_str());

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonV0Obj(*char, String)


//=======================================================================
// ************ function to build Json Settings string ******************
//=======================================================================
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces)
{
  char jsonBuff[200] = "";

  switch(decPlaces) {
    case 0:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.0f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 2:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.2f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 5:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.5f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    default:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);

  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, iValue, iType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, int, *char, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\":\"%s\", \"type\": \"%s\", \"maxlen\": %d}"
                                      , objSprtr, cName, cValue, sType, maxLen);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, *char, *char, int, int)



//=========================================================================
// function to build MQTT Json string ** max message size is 128 bytes!! **
//=========================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, cValue);
  }
  else
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
                                      , cName, cValue, cUnit);
  }

} // createMQTTjsonMessage(*char, *char, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, cValue, noUnit);

} // createMQTTjsonMessage(*char, *char)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue, const char *cUnit)
{
  uint16_t hdrSize = (strlen(cName) * 2) +strlen(settingMQTTtopTopic) + 30;

  if (strlen(cUnit) == 0)
  {
    //DebugTf("sValue.lenght()[%d], strlen(%s)[%d]\r\n", sValue.length(), cName, strlen(cName));
    if ((hdrSize + sValue.length()) >= 128)
    {
      String tmp = sValue.substring(0, (128 - hdrSize));
      snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": \"%s\"}]}"
                         , cName, tmp.c_str());
    }
    else
    {
      snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, sValue.c_str());
    }
  }
  else
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
                                      , cName, sValue.c_str(), cUnit);
  }

} // createMQTTjsonMessage(*char, String, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, sValue, noUnit);

} // createMQTTjsonMessage(*char, String)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %d}]}"
                                      , cName, iValue);
  }
  else
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %d, \"unit\": \"%s\"}]}"
                                      , cName, iValue, cUnit);
  }

} // createMQTTjsonMessage(*char, int, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, iValue, noUnit);

} // createMQTTjsonMessage(char *mqttBuff, *char, int)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %u}]}"
                                      , cName, uValue);
  }
  else
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %u, \"unit\": \"%s\"}]}"
                                      , cName, uValue, cUnit);
  }

} // createMQTTjsonMessage(*char, uint, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, uValue, noUnit);

} // createMQTTjsonMessage(char *mqttBuff, *char, uint)


//=======================================================================
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue, const char *cUnit)
{
  if (strlen(cUnit) == 0)
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %.3f}]}"
                                      , cName, fValue);
  }
  else
  {
    snprintf(mqttBuff, 200, "{\"%s\": [{\"value\": %.3f, \"unit\": \"%s\"}]}"
                                      , cName, fValue, cUnit);
  }

} // createMQTTjsonMessage(*char, float, *char)

//---------------------------------------------------------------
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue)
{
  char noUnit[] = {'\0'};

  createMQTTjsonMessage(mqttBuff, cName, fValue, noUnit);

} // createMQTTjsonMessage(char *mqttBuff, *char, float)


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
/* 
***************************************************************************  
**  Program  : menuStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//===========================================================================================
void displayHoursHist(bool Telnet=true)
{
    readAllSlots(HOURS, "/RINGhours.csv", actTimestamp, false, "");

} // displayHoursHist()


//===========================================================================================
void displayDaysHist(bool Telnet=true)
{
    readAllSlots(DAYS, "/RINGdays.csv", actTimestamp, false, "");

} // displayDaysHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true)
{
    readAllSlots(MONTHS, "/RINGmonths.csv", actTimestamp, false, "");

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo()
{
  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "40" "." "327" "\", \"aSM\", @progbits, 1 #"))) = (
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "\r\n==================================================================\r"
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "40" "." "327" "\", \"aSM\", @progbits, 1 #"))) = (
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "\r\n==================================================================\r"
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "41" "." "328" "\", \"aSM\", @progbits, 1 #"))) = (
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n            (c)2019 by [Willem Aandewiel"
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "41" "." "328" "\", \"aSM\", @progbits, 1 #"))) = (
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n            (c)2019 by [Willem Aandewiel"
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "42" "." "329" "\", \"aSM\", @progbits, 1 #"))) = (
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Firmware Version ["
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "42" "." "329" "\", \"aSM\", @progbits, 1 #"))) = (
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Firmware Version ["
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print("v2.0.1 (17-04-2020)"); TelnetStream.print("v2.0.1 (17-04-2020)"); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "43" "." "330" "\", \"aSM\", @progbits, 1 #"))) = (
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              Compiled ["
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "43" "." "330" "\", \"aSM\", @progbits, 1 #"))) = (
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              Compiled ["
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print("Jun  8 2021"); TelnetStream.print("Jun  8 2021"); });
                                               ({ Serial.print("  "); TelnetStream.print("  "); });
                                               ({ Serial.print("14:15:37"); TelnetStream.print("14:15:37"); });



  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "49" "." "331" "\", \"aSM\", @progbits, 1 #"))) = (
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n         compiled with [dsmr.h"
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "49" "." "331" "\", \"aSM\", @progbits, 1 #"))) = (
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n         compiled with [dsmr.h"
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 49 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });

  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "51" "." "332" "\", \"aSM\", @progbits, 1 #"))) = (
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              #defines "
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "51" "." "332" "\", \"aSM\", @progbits, 1 #"))) = (
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              #defines "
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });




  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "56" "." "333" "\", \"aSM\", @progbits, 1 #"))) = (
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "[USE_UPDATE_SERVER]"
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "56" "." "333" "\", \"aSM\", @progbits, 1 #"))) = (
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "[USE_UPDATE_SERVER]"
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 56 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });


  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "59" "." "334" "\", \"aSM\", @progbits, 1 #"))) = (
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "[USE_MQTT]"
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "59" "." "334" "\", \"aSM\", @progbits, 1 #"))) = (
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "[USE_MQTT]"
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 59 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "77" "." "335" "\", \"aSM\", @progbits, 1 #"))) = (
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n   Telegrams Processed ["
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "77" "." "335" "\", \"aSM\", @progbits, 1 #"))) = (
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n   Telegrams Processed ["
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 77 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(telegramCount); TelnetStream.print(telegramCount); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "78" "." "336" "\", \"aSM\", @progbits, 1 #"))) = (
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n           With Errors ["
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "78" "." "336" "\", \"aSM\", @progbits, 1 #"))) = (
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n           With Errors ["
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 78 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(telegramErrors); TelnetStream.print(telegramErrors); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "79" "." "337" "\", \"aSM\", @progbits, 1 #"))) = (
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              FreeHeap ["
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "79" "." "337" "\", \"aSM\", @progbits, 1 #"))) = (
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              FreeHeap ["
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getFreeHeap()); TelnetStream.print(ESP.getFreeHeap()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "80" "." "338" "\", \"aSM\", @progbits, 1 #"))) = (
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             max.Block ["
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "80" "." "338" "\", \"aSM\", @progbits, 1 #"))) = (
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             max.Block ["
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getMaxFreeBlockSize()); TelnetStream.print(ESP.getMaxFreeBlockSize()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "81" "." "339" "\", \"aSM\", @progbits, 1 #"))) = (
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n               Chip ID ["
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "81" "." "339" "\", \"aSM\", @progbits, 1 #"))) = (
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n               Chip ID ["
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getChipId(), 16); TelnetStream.print(ESP.getChipId(), 16); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "82" "." "340" "\", \"aSM\", @progbits, 1 #"))) = (
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n          Core Version ["
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "82" "." "340" "\", \"aSM\", @progbits, 1 #"))) = (
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n          Core Version ["
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getCoreVersion()); TelnetStream.print(ESP.getCoreVersion()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "83" "." "341" "\", \"aSM\", @progbits, 1 #"))) = (
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n           SDK Version ["
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "83" "." "341" "\", \"aSM\", @progbits, 1 #"))) = (
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n           SDK Version ["
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getSdkVersion()); TelnetStream.print(ESP.getSdkVersion()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "84" "." "342" "\", \"aSM\", @progbits, 1 #"))) = (
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n        CPU Freq (MHz) ["
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "84" "." "342" "\", \"aSM\", @progbits, 1 #"))) = (
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n        CPU Freq (MHz) ["
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getCpuFreqMHz()); TelnetStream.print(ESP.getCpuFreqMHz()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "85" "." "343" "\", \"aSM\", @progbits, 1 #"))) = (
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Sketch Size (kB) ["
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "85" "." "343" "\", \"aSM\", @progbits, 1 #"))) = (
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Sketch Size (kB) ["
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 85 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getSketchSize() / 1024.0); TelnetStream.print(ESP.getSketchSize() / 1024.0); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "86" "." "344" "\", \"aSM\", @progbits, 1 #"))) = (
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\nFree Sketch Space (kB) ["
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "86" "." "344" "\", \"aSM\", @progbits, 1 #"))) = (
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\nFree Sketch Space (kB) ["
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getFreeSketchSpace() / 1024.0); TelnetStream.print(ESP.getFreeSketchSpace() / 1024.0); });

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85)
        snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());

  LittleFS.info(SPIFFSinfo);

  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "94" "." "345" "\", \"aSM\", @progbits, 1 #"))) = (
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n         Flash Chip ID ["
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "94" "." "345" "\", \"aSM\", @progbits, 1 #"))) = (
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n         Flash Chip ID ["
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 94 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(cMsg); TelnetStream.print(cMsg); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "95" "." "346" "\", \"aSM\", @progbits, 1 #"))) = (
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n  Flash Chip Size (kB) ["
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "95" "." "346" "\", \"aSM\", @progbits, 1 #"))) = (
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n  Flash Chip Size (kB) ["
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getFlashChipSize() / 1024); TelnetStream.print(ESP.getFlashChipSize() / 1024); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "96" "." "347" "\", \"aSM\", @progbits, 1 #"))) = (
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n   Chip Real Size (kB) ["
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "96" "." "347" "\", \"aSM\", @progbits, 1 #"))) = (
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n   Chip Real Size (kB) ["
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getFlashChipRealSize() / 1024); TelnetStream.print(ESP.getFlashChipRealSize() / 1024); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "97" "." "348" "\", \"aSM\", @progbits, 1 #"))) = (
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      SPIFFS Size (kB) ["
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "97" "." "348" "\", \"aSM\", @progbits, 1 #"))) = (
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      SPIFFS Size (kB) ["
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(SPIFFSinfo.totalBytes / 1024); TelnetStream.print(SPIFFSinfo.totalBytes / 1024); });

  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "99" "." "349" "\", \"aSM\", @progbits, 1 #"))) = (
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Flash Chip Speed ["
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "99" "." "349" "\", \"aSM\", @progbits, 1 #"))) = (
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n      Flash Chip Speed ["
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getFlashChipSpeed() / 1000 / 1000); TelnetStream.print(ESP.getFlashChipSpeed() / 1000 / 1000); });
  FlashMode_t ideMode = ESP.getFlashChipMode();
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "101" "." "350" "\", \"aSM\", @progbits, 1 #"))) = (
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n       Flash Chip Mode ["
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "101" "." "350" "\", \"aSM\", @progbits, 1 #"))) = (
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n       Flash Chip Mode ["
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(flashMode[ideMode]); TelnetStream.print(flashMode[ideMode]); });

  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "103" "." "351" "\", \"aSM\", @progbits, 1 #"))) = (
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "103" "." "351" "\", \"aSM\", @progbits, 1 #"))) = (
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 103 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });

  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "105" "." "352" "\", \"aSM\", @progbits, 1 #"))) = (
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r"
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "105" "." "352" "\", \"aSM\", @progbits, 1 #"))) = (
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r"
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 105 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "106" "." "353" "\", \"aSM\", @progbits, 1 #"))) = (
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n            Board type ["
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "106" "." "353" "\", \"aSM\", @progbits, 1 #"))) = (
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n            Board type ["
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 106 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "119" "." "354" "\", \"aSM\", @progbits, 1 #"))) = (
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n                  SSID ["
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "119" "." "354" "\", \"aSM\", @progbits, 1 #"))) = (
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n                  SSID ["
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 119 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(WiFi.SSID()); TelnetStream.print(WiFi.SSID()); });



  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "123" "." "355" "\", \"aSM\", @progbits, 1 #"))) = (
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n               PSK key [**********"
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "123" "." "355" "\", \"aSM\", @progbits, 1 #"))) = (
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n               PSK key [**********"
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 123 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });

  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "125" "." "356" "\", \"aSM\", @progbits, 1 #"))) = (
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n            IP Address ["
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "125" "." "356" "\", \"aSM\", @progbits, 1 #"))) = (
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n            IP Address ["
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 125 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(WiFi.localIP().toString()); TelnetStream.print(WiFi.localIP().toString()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "126" "." "357" "\", \"aSM\", @progbits, 1 #"))) = (
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              Hostname ["
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "126" "." "357" "\", \"aSM\", @progbits, 1 #"))) = (
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n              Hostname ["
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(settingHostname); TelnetStream.print(settingHostname); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "127" "." "358" "\", \"aSM\", @progbits, 1 #"))) = (
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n     Last reset reason ["
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "127" "." "358" "\", \"aSM\", @progbits, 1 #"))) = (
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n     Last reset reason ["
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 127 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(ESP.getResetReason()); TelnetStream.print(ESP.getResetReason()); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "128" "." "359" "\", \"aSM\", @progbits, 1 #"))) = (
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n                upTime ["
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "128" "." "359" "\", \"aSM\", @progbits, 1 #"))) = (
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n                upTime ["
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 128 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(upTime()); TelnetStream.print(upTime()); });
  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "129" "." "360" "\", \"aSM\", @progbits, 1 #"))) = (
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "129" "." "360" "\", \"aSM\", @progbits, 1 #"))) = (
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });


  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "132" "." "361" "\", \"aSM\", @progbits, 1 #"))) = (
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r"
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "132" "." "361" "\", \"aSM\", @progbits, 1 #"))) = (
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r"
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 132 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "133" "." "362" "\", \"aSM\", @progbits, 1 #"))) = (
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n           MQTT broker ["
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "133" "." "362" "\", \"aSM\", @progbits, 1 #"))) = (
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 " \r\n           MQTT broker ["
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 133 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(settingMQTTbroker); TelnetStream.print(settingMQTTbroker); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "134" "." "363" "\", \"aSM\", @progbits, 1 #"))) = (
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             MQTT User ["
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "134" "." "363" "\", \"aSM\", @progbits, 1 #"))) = (
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             MQTT User ["
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 134 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(settingMQTTuser); TelnetStream.print(settingMQTTuser); });



    ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "138" "." "364" "\", \"aSM\", @progbits, 1 #"))) = (
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
   "]\r\n         MQTT PassWord [**********"
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
   ); &__pstr__[0];}))
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "138" "." "364" "\", \"aSM\", @progbits, 1 #"))) = (
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
   "]\r\n         MQTT PassWord [**********"
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
   ); &__pstr__[0];}))
# 138 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
   )))); });

  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "140" "." "365" "\", \"aSM\", @progbits, 1 #"))) = (
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             Top Topic ["
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "140" "." "365" "\", \"aSM\", @progbits, 1 #"))) = (
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n             Top Topic ["
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 140 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(settingMQTTtopTopic); TelnetStream.print(settingMQTTtopTopic); });
  ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "141" "." "366" "\", \"aSM\", @progbits, 1 #"))) = (
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n       Update Interval ["
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "141" "." "366" "\", \"aSM\", @progbits, 1 #"))) = (
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r\n       Update Interval ["
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 141 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); }); ({ Serial.print(settingMQTTinterval); TelnetStream.print(settingMQTTinterval); });
  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "142" "." "367" "\", \"aSM\", @progbits, 1 #"))) = (
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "142" "." "367" "\", \"aSM\", @progbits, 1 #"))) = (
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "]\r"
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });
  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "143" "." "368" "\", \"aSM\", @progbits, 1 #"))) = (
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r\n\r"
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "143" "." "368" "\", \"aSM\", @progbits, 1 #"))) = (
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 "==================================================================\r\n\r"
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
 ); &__pstr__[0];}))
# 143 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
 )))); });


} // displayBoardInfo()



//===========================================================================================
void handleKeyInput()
{
  char inChar;

  while (TelnetStream.available() > 0)
  {
    yield();
    inChar = (char)TelnetStream.read();

    switch(inChar) {
      case 'b':
      case 'B': displayBoardInfo();
                    break;
      case 'l':
      case 'L': readSettings(true);
                    break;
      case 'd':
      case 'D': displayDaysHist(true);
                    break;
      case 'E': eraseFile();
                    break;





      case 'h':
      case 'H': displayHoursHist(true);
                    break;
      case 'm':
      case 'M': displayMonthsHist(true);
                    break;

      case 'W': ({ Serial.printf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", settingHostname); TelnetStream.printf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", settingHostname); });
                    delay(1000);
                    WiFi.disconnect(true); // deletes credentials !
                    //setupWiFi(true);
                    delay(2000);
                    ESP.reset();
                    delay(2000);
                    break;
      case 'i':
      case 'I': for(int I=0; I<10; I++)
                    {
                      digitalWrite(2, !digitalRead(2));
                      delay(1000);
                    }
                    break;





      case 'p':
      case 'P': showRaw = !showRaw;




                    showRawCount = 0;
                    break;
      case 'R': ({ _debugBOL(__FUNCTION__, 212); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "212" "." "369" "\", \"aSM\", @progbits, 1 #"))) = (
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "Reboot in 3 seconds ... \r\n"
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "212" "." "369" "\", \"aSM\", @progbits, 1 #"))) = (
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "Reboot in 3 seconds ... \r\n"
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); }); });
                    ({ Serial.flush(); TelnetStream.flush(); });
                    delay(3000);
                    ({ _debugBOL(__FUNCTION__, 215); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "215" "." "370" "\", \"aSM\", @progbits, 1 #"))) = (
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "now Rebooting. \r"
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "215" "." "370" "\", \"aSM\", @progbits, 1 #"))) = (
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "now Rebooting. \r"
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 215 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); }); });
                    ({ Serial.flush(); TelnetStream.flush(); });
                    ESP.reset();
                    break;
      case 's':
      case 'S': listSPIFFS();
                    break;
      case 'v':
      case 'V': if (Verbose2)
                    {
                      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "225" "." "371" "\", \"aSM\", @progbits, 1 #"))) = (
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose is OFF\r"
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "225" "." "371" "\", \"aSM\", @progbits, 1 #"))) = (
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose is OFF\r"
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 225 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); });
                      Verbose1 = false;
                      Verbose2 = false;
                    }
                    else if (Verbose1)
                    {
                      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "231" "." "372" "\", \"aSM\", @progbits, 1 #"))) = (
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose Level 2 is ON\r"
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "231" "." "372" "\", \"aSM\", @progbits, 1 #"))) = (
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose Level 2 is ON\r"
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 231 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); });
                      Verbose2 = true;
                    }
                    else
                    {
                      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "236" "." "373" "\", \"aSM\", @progbits, 1 #"))) = (
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose Level 1 is ON\r"
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "236" "." "373" "\", \"aSM\", @progbits, 1 #"))) = (
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "Verbose Level 1 is ON\r"
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 236 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); });
                      Verbose1 = true;
                      Verbose2 = false;
                    }
                    break;







      case 'Z': slotErrors = 0;
                    nrReboots = 0;
                    telegramCount = 0;
                    telegramErrors = 0;
                    writeLastStatus();




                    break;
      default: ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "258" "." "374" "\", \"aSM\", @progbits, 1 #"))) = (
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "\r\nCommands are:\r\n"
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "258" "." "374" "\", \"aSM\", @progbits, 1 #"))) = (
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "\r\nCommands are:\r\n"
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 258 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "259" "." "375" "\", \"aSM\", @progbits, 1 #"))) = (
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   B - Board Info\r"
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "259" "." "375" "\", \"aSM\", @progbits, 1 #"))) = (
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   B - Board Info\r"
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 259 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "260" "." "376" "\", \"aSM\", @progbits, 1 #"))) = (
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *E - erase file from SPIFFS\r"
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "260" "." "376" "\", \"aSM\", @progbits, 1 #"))) = (
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *E - erase file from SPIFFS\r"
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 260 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "261" "." "377" "\", \"aSM\", @progbits, 1 #"))) = (
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   L - list Settings\r"
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "261" "." "377" "\", \"aSM\", @progbits, 1 #"))) = (
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   L - list Settings\r"
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "262" "." "378" "\", \"aSM\", @progbits, 1 #"))) = (
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   D - Display Day table from SPIFFS\r"
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "262" "." "378" "\", \"aSM\", @progbits, 1 #"))) = (
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   D - Display Day table from SPIFFS\r"
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 262 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "263" "." "379" "\", \"aSM\", @progbits, 1 #"))) = (
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   H - Display Hour table from SPIFFS\r"
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "263" "." "379" "\", \"aSM\", @progbits, 1 #"))) = (
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   H - Display Hour table from SPIFFS\r"
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "264" "." "380" "\", \"aSM\", @progbits, 1 #"))) = (
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   M - Display Month table from SPIFFS\r"
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "264" "." "380" "\", \"aSM\", @progbits, 1 #"))) = (
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   M - Display Month table from SPIFFS\r"
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 264 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });



                    ({ Serial.printf("   I - Identify by blinking LED on GPIO[%02d]\r\n", 2); TelnetStream.printf("   I - Identify by blinking LED on GPIO[%02d]\r\n", 2); });
                    if (showRaw)
                    {
                      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "271" "." "381" "\", \"aSM\", @progbits, 1 #"))) = (
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "   P - Start Parsing again\r"
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "271" "." "381" "\", \"aSM\", @progbits, 1 #"))) = (
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "   P - Start Parsing again\r"
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); });
                    }
                    else
                    {
                      ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "275" "." "382" "\", \"aSM\", @progbits, 1 #"))) = (
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "   P - No Parsing (show RAW data from Smart Meter)\r"
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "275" "." "382" "\", \"aSM\", @progbits, 1 #"))) = (
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     "   P - No Parsing (show RAW data from Smart Meter)\r"
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                     ); &__pstr__[0];}))
# 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                     )))); });
                      showRawCount = 0;
                    }
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "278" "." "383" "\", \"aSM\", @progbits, 1 #"))) = (
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *W - Force Re-Config WiFi\r"
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "278" "." "383" "\", \"aSM\", @progbits, 1 #"))) = (
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *W - Force Re-Config WiFi\r"
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 278 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });



                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "282" "." "384" "\", \"aSM\", @progbits, 1 #"))) = (
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *R - Reboot\r"
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "282" "." "384" "\", \"aSM\", @progbits, 1 #"))) = (
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *R - Reboot\r"
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 282 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "283" "." "385" "\", \"aSM\", @progbits, 1 #"))) = (
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   S - File info on SPIFFS\r"
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "283" "." "385" "\", \"aSM\", @progbits, 1 #"))) = (
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "   S - File info on SPIFFS\r"
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 283 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "284" "." "386" "\", \"aSM\", @progbits, 1 #"))) = (
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *U - Update SPIFFS (save Data-files)\r"
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "284" "." "386" "\", \"aSM\", @progbits, 1 #"))) = (
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *U - Update SPIFFS (save Data-files)\r"
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 284 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "285" "." "387" "\", \"aSM\", @progbits, 1 #"))) = (
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *Z - Zero counters\r\n"
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "285" "." "387" "\", \"aSM\", @progbits, 1 #"))) = (
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   "  *Z - Zero counters\r\n"
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                   ); &__pstr__[0];}))
# 285 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                   )))); });
                    if (Verbose1 & Verbose2) ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "286" "." "388" "\", \"aSM\", @progbits, 1 #"))) = (
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose Off\r"
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "286" "." "388" "\", \"aSM\", @progbits, 1 #"))) = (
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose Off\r"
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); });
                    else if (Verbose1) ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "287" "." "389" "\", \"aSM\", @progbits, 1 #"))) = (
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose 2\r"
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "287" "." "389" "\", \"aSM\", @progbits, 1 #"))) = (
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose 2\r"
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 287 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); });
                    else ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "288" "." "390" "\", \"aSM\", @progbits, 1 #"))) = (
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose 1\r"
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "menuStuff.ino" "." "288" "." "390" "\", \"aSM\", @progbits, 1 #"))) = (
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             "   V - Toggle Verbose 1\r"
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino" 3
                                             ); &__pstr__[0];}))
# 288 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
                                             )))); });




    } // switch()
    while (TelnetStream.available() > 0)
    {
       yield();
       (char)TelnetStream.read();
    }
  }

} // handleKeyInput()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\ntpStuff.ino"
/*
***************************************************************************  
**  Program  : ntpStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
# 182 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\ntpStuff.ino"
/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\processTelegram.ino"
/* 
***************************************************************************  
**  Program  : processTelegram, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
//==================================================================================
void processTelegram()
{
  ({ _debugBOL(__FUNCTION__, 14); ({ Serial.printf("Telegram[%d]=>DSMRdata.timestamp[%s]\r\n", telegramCount , DSMRdata.timestamp.c_str()); TelnetStream.printf("Telegram[%d]=>DSMRdata.timestamp[%s]\r\n", telegramCount , DSMRdata.timestamp.c_str()); }); })
                                                                                 ;

//----- update OLED display ---------
  if (settingOledType > 0)
  {
    String DT = buildDateTimeString(DSMRdata.timestamp.c_str(), sizeof(DSMRdata.timestamp));

    snprintf(cMsg, sizeof(cMsg), "%s - %s", DT.substring(0, 10).c_str(), DT.substring(11, 16).c_str());
    oled_Print_Msg(0, cMsg, 0);
    snprintf(cMsg, sizeof(cMsg), "-Power%7d Watt", (int)(DSMRdata.power_delivered *1000));
    oled_Print_Msg(1, cMsg, 0);
    snprintf(cMsg, sizeof(cMsg), "+Power%7d Watt", (int)(DSMRdata.power_returned *1000));
    oled_Print_Msg(2, cMsg, 0);
  }

  strcpy(newTimestamp, DSMRdata.timestamp.c_str());
  //--- newTimestamp is the timestamp from the last telegram
  newT = epoch(newTimestamp, strlen(newTimestamp), true); // update system time
  //--- actTimestamp is the timestamp from the previous telegram
  actT = epoch(actTimestamp, strlen(actTimestamp), false);

  //--- Skip first 3 telegrams .. just to settle down a bit ;-)
  if ((int32_t)(telegramCount - telegramErrors) < 3)
  {
    strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
    actT = epoch(actTimestamp, strlen(actTimestamp), false); // update system time
    return;
  }

  ({ _debugBOL(__FUNCTION__, 44); ({ Serial.printf("actHour[%02d] -- newHour[%02d]\r\n", hour(actT), hour(newT)); TelnetStream.printf("actHour[%02d] -- newHour[%02d]\r\n", hour(actT), hour(newT)); }); });
  //--- if we have a new hour() update the previous hour
  if (hour(actT) != hour(newT)) {
    /* nothing*/;
  }
  //--- has the hour changed (or the day or month)  
  //--- in production testing on hour only would
  //--- suffice, but in testing I need all three
  //--- if we have a new hour() update the previous hour(actT)
  if ( (hour(actT) != hour(newT) )
       || (day(actT) != day(newT) )
       || (month(actT) != month(newT) ) )
  {
    /* nothing*/;
    writeDataToFiles();
    writeLastStatus();
    //--- now see if the day() has changed also
    if ( day(actT) != day(newT) )
    {
      //--- YES! actTimestamp := newTimestamp
      //--- and update the files with the actTimestamp
      strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
      writeDataToFiles();
    }
    else //--- NO, only the hour has changed
    {
      char record[75 + 1] = "";
      //--- actTimestamp := newTimestamp
      strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
      buildDataRecordFromSM(record);
      uint16_t recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
      //--- and update the files with the actTimestamp
      writeDataToFile("/RINGhours.csv", record, recSlot, HOURS);
      ({ _debugBOL(__FUNCTION__, 77); ({ Serial.printf(">%s\r\n", record); TelnetStream.printf(">%s\r\n", record); }); }); // record ends in a \n
    }
  }

  if ( ( __Due__(publishMQTTtimer_due, publishMQTTtimer_interval, publishMQTTtimer_type) ) )
  {
    sendMQTTData();
  }

  strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
  actT = epoch(actTimestamp, strlen(actTimestamp), true); // update system time

} // processTelegram()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
/* 

***************************************************************************  

**  Program  : restAPI, part of DSMRloggerAPI

**  Version  : v2.0.1

**

**  Copyright (c) 2020 Willem Aandewiel

**

**  TERMS OF USE: MIT License. See bottom of file.                                                            

***************************************************************************      

*/
# 12 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
// ******* Global Vars *******
uint32_t antiWearTimer = 0;

char fieldName[40] = "";

char fieldsArray[50][35] = {{0}}; // to lookup fields 
int fieldsElements = 0;

int actualElements = 20;
char actualArray[][35] = { "timestamp"
                          ,"energy_delivered_tariff1","energy_delivered_tariff2"
                          ,"energy_returned_tariff1","energy_returned_tariff2"
                          ,"power_delivered","power_returned"
                          ,"voltage_l1","voltage_l2","voltage_l3"
                          ,"current_l1","current_l2","current_l3"
                          ,"power_delivered_l1","power_delivered_l2","power_delivered_l3"
                          ,"power_returned_l1","power_returned_l2","power_returned_l3"
                          ,"gas_delivered"
                          ,"\0"};
int infoElements = 7;
char infoArray[][35] = { "identification","p1_version","equipment_id","electricity_tariff"
                          ,"gas_device_type","gas_equipment_id"
                          , "\0" };

bool onlyIfPresent = false;

//=======================================================================
void processAPI()
{
  char fName[40] = "";
  char URI[50] = "";
  String words[10];

  strncpy( URI, httpServer.uri().c_str(), sizeof(URI) );

  if (httpServer.method() == HTTP_GET)
        ({ _debugBOL(__FUNCTION__, 48); ({ Serial.printf("from[%s] URI[%s] method[GET] \r\n" , httpServer.client().remoteIP().toString().c_str() , URI); TelnetStream.printf("from[%s] URI[%s] method[GET] \r\n" , httpServer.client().remoteIP().toString().c_str() , URI); }); })

                                              ;
  else ({ _debugBOL(__FUNCTION__, 51); ({ Serial.printf("from[%s] URI[%s] method[PUT] \r\n" , httpServer.client().remoteIP().toString().c_str() , URI); TelnetStream.printf("from[%s] URI[%s] method[PUT] \r\n" , httpServer.client().remoteIP().toString().c_str() , URI); }); })

                                              ;




  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!

  {
    ({ _debugBOL(__FUNCTION__, 61); ({ Serial.printf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap()); TelnetStream.printf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap()); }); });
    /* nothing*/


                                                       ;
    httpServer.send(500, "text/plain", "500: internal server error (low heap)\r\n");
    return;
  }

  int8_t wc = splitString(URI, '/', words, 10);

  if (Verbose2)
  {
    ({ _debugBOL(__FUNCTION__, 74); ({ Serial.print(">>"); TelnetStream.print(">>"); }); });
    for (int w=0; w<wc; w++)
    {
      ({ Serial.printf("word[%d] => [%s], ", w, words[w].c_str()); TelnetStream.printf("word[%d] => [%s], ", w, words[w].c_str()); });
    }
    ({ Serial.println(" "); TelnetStream.println(" "); });
  }

  if (words[1] != "api")
  {
    sendApiNotFound(URI);
    return;
  }

  if (words[2] == "v0" && words[3] == "sm" && words[4] == "actual")
  {
    //--- depreciated api. left here for backward compatibility
    onlyIfPresent = true;
    copyToFieldsArray(actualArray, actualElements);
    sendJsonV0Fields();
    return;
  }
  if (words[2] != "v1")
  {
    sendApiNotFound(URI);
    return;
  }

  if (words[3] == "dev")
  {
    handleDevApi(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
  }
  else if (words[3] == "hist")
  {
    handleHistApi(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
  }
  else if (words[3] == "sm")
  {
    handleSmApi(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
  }
  else sendApiNotFound(URI);

} // processAPI()


//====================================================
void handleDevApi(const char *URI, const char *word4, const char *word5, const char *word6)
{
  //DebugTf("word4[%s], word5[%s], word6[%s]\r\n", word4, word5, word6);
  if (strcmp(word4, "info") == 0)
  {
    sendDeviceInfo();
  }
  else if (strcmp(word4, "time") == 0)
  {
    sendDeviceTime();
  }
  else if (strcmp(word4, "settings") == 0)
  {
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------ 
      // json string: {"name":"settingInterval","value":9}  
      // json string: {"name":"settingTelegramInterval","value":123.45}  
      // json string: {"name":"settingTelegramInterval","value":"abc"}  
      //------------------------------------------------------------ 
      // so, why not use ArduinoJSON library?
      // I say: try it yourself ;-) It won't be easy
      String wOut[5];
      String wPair[5];
      String jsonIn = httpServer.arg(0).c_str();
      char field[25] = "";
      char newValue[101]="";
      jsonIn.replace("{", "");
      jsonIn.replace("}", "");
      jsonIn.replace("\"", "");
      int8_t wp = splitString(jsonIn.c_str(), ',', wPair, 5) ;
      for (int i=0; i<wp; i++)
      {
        //DebugTf("[%d] -> pair[%s]\r\n", i, wPair[i].c_str());
        int8_t wc = splitString(wPair[i].c_str(), ':', wOut, 5) ;
        //DebugTf("==> [%s] -> field[%s]->val[%s]\r\n", wPair[i].c_str(), wOut[0].c_str(), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("name")) strCopy(field, sizeof(field), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("value")) strCopy(newValue, sizeof(newValue), wOut[1].c_str());
      }
      //DebugTf("--> field[%s] => newValue[%s]\r\n", field, newValue);
      updateSetting(field, newValue);
      httpServer.send(200, "application/json", httpServer.arg(0));
      /* nothing*/;
    }
    else
    {
      sendDeviceSettings();
    }
  }
  else if (strcmp(word4, "debug") == 0)
  {
    sendDeviceDebug(URI, word5);
  }
  else sendApiNotFound(URI);

} // handleDevApi()


//====================================================
void handleHistApi(const char *URI, const char *word4, const char *word5, const char *word6)
{
  int8_t fileType = 0;
  char fileName[20] = "";

  //DebugTf("word4[%s], word5[%s], word6[%s]\r\n", word4, word5, word6);
  if ( strcmp(word4, "hours") == 0 )
  {
    fileType = HOURS;
    strCopy(fileName, sizeof(fileName), "/RINGhours.csv");
  }
  else if (strcmp(word4, "days") == 0 )
  {
    fileType = DAYS;
    strCopy(fileName, sizeof(fileName), "/RINGdays.csv");
  }
  else if (strcmp(word4, "months") == 0)
  {
    fileType = MONTHS;
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------ 
      // json string: {"recid":"29013023"
      //               ,"edt1":2601.146,"edt2":"9535.555"
      //               ,"ert1":378.074,"ert2":208.746
      //               ,"gdt":3314.404}
      //------------------------------------------------------------ 
      char record[75 + 1] = "";
      uint16_t recSlot;

      String jsonIn = httpServer.arg(0).c_str();
      ({ _debugBOL(__FUNCTION__, 210); ({ Serial.println(jsonIn); TelnetStream.println(jsonIn); }); });

      recSlot = buildDataRecordFromJson(record, jsonIn);

      //--- update MONTHS
      writeDataToFile("/RINGmonths.csv", record, recSlot, MONTHS);
      //--- send OK response --
      httpServer.send(200, "application/json", httpServer.arg(0));

      return;
    }
    else
    {
      strCopy(fileName, sizeof(fileName), "/RINGmonths.csv");
    }
  }
  else
  {
    sendApiNotFound(URI);
    return;
  }
  if (strcmp(word5, "desc") == 0)
        sendJsonHist(fileType, fileName, actTimestamp, true);
  else sendJsonHist(fileType, fileName, actTimestamp, false);

} // handleHistApi()


//====================================================
void handleSmApi(const char *URI, const char *word4, const char *word5, const char *word6)
{
  char tlgrm[1200] = "";
  uint8_t p=0;
  bool stopParsingTelegram = false;

  //DebugTf("word4[%s], word5[%s], word6[%s]\r\n", word4, word5, word6);
  if (strcmp(word4, "info") == 0)
  {
    //sendSmInfo();
    onlyIfPresent = false;
    copyToFieldsArray(infoArray, infoElements);
    sendJsonFields(word4);
  }
  else if (strcmp(word4, "actual") == 0)
  {
    //sendSmActual();
    onlyIfPresent = true;
    copyToFieldsArray(actualArray, actualElements);
    sendJsonFields(word4);
  }
  else if (strcmp(word4, "fields") == 0)
  {
    fieldsElements = 0;
    onlyIfPresent = false;

    if (strlen(word5) > 0)
    {
       memset(fieldsArray,0,sizeof(fieldsArray));
       strCopy(fieldsArray[0], 34,"timestamp");
       strCopy(fieldsArray[1], 34, word5);
       fieldsElements = 2;
    }
    sendJsonFields(word4);
  }
  else if (strcmp(word4, "telegram") == 0)
  {
    showRaw = true;
    slimmeMeter.enable(true);
    Serial.setTimeout(5000); // 5 seconds must be enough ..
    memset(tlgrm, 0, sizeof(tlgrm));
    int l = 0;
    // The terminator character is discarded from the serial buffer.
    l = Serial.readBytesUntil('/', tlgrm, sizeof(tlgrm));
    // now read from '/' to '!'
    // The terminator character is discarded from the serial buffer.
    l = Serial.readBytesUntil('!', tlgrm, sizeof(tlgrm));
    Serial.setTimeout(1000); // seems to be the default ..
    ({ _debugBOL(__FUNCTION__, 287); ({ Serial.printf("read [%d] bytes\r\n", l); TelnetStream.printf("read [%d] bytes\r\n", l); }); });
    if (l == 0)
    {
      httpServer.send(200, "application/plain", "no telegram received");
      showRaw = false;
      return;
    }

    tlgrm[l++] = '!';

    // next 6 bytes are "<CRC>\r\n"
    for (int i=0; ( i<6 && (i<(sizeof(tlgrm)-7)) ); i++)
    {
      tlgrm[l++] = (char)Serial.read();
    }




    tlgrm[(l +1)] = '\0';
    // shift telegram 1 char to the right (make room at pos [0] for '/')
    for (int i=strlen(tlgrm); i>=0; i--) { tlgrm[i+1] = tlgrm[i]; yield(); }
    tlgrm[0] = '/';
    showRaw = false;
    if (Verbose1) ({ Serial.printf("Telegram (%d chars):\r\n/%s", strlen(tlgrm), tlgrm); TelnetStream.printf("Telegram (%d chars):\r\n/%s", strlen(tlgrm), tlgrm); });
    httpServer.send(200, "application/plain", tlgrm);

  }
  else sendApiNotFound(URI);

} // handleSmApi()


//=======================================================================
void sendDeviceInfo()
{
  char compileOptions[200] = "";
# 333 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
    strConcat(compileOptions, sizeof(compileOptions), "[USE_DUTCH_PROTOCOL]");


    strConcat(compileOptions, sizeof(compileOptions), "[USE_UPDATE_SERVER]");


    strConcat(compileOptions, sizeof(compileOptions), "[USE_MQTT]");
# 351 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
  sendStartJsonObj("devinfo");

  sendNestedJsonObj("author", "Willem Aandewiel (www.aandewiel.nl)");
  sendNestedJsonObj("fwversion", "v2.0.1 (17-04-2020)");

  snprintf(cMsg, sizeof(cMsg), "%s %s", "Jun  8 2021", "14:15:37");
  sendNestedJsonObj("compiled", cMsg);
  sendNestedJsonObj("hostname", settingHostname);
  sendNestedJsonObj("ipaddress", WiFi.localIP().toString().c_str());
  sendNestedJsonObj("macaddress", WiFi.macAddress().c_str());
  sendNestedJsonObj("indexfile", settingIndexPage);
  sendNestedJsonObj("freeheap", ESP.getFreeHeap(), "bytes");
  sendNestedJsonObj("maxfreeblock", ESP.getMaxFreeBlockSize(), "bytes");
  sendNestedJsonObj("chipid", String( ESP.getChipId(), 16 ).c_str());
  sendNestedJsonObj("coreversion", String( ESP.getCoreVersion() ).c_str() );
  sendNestedJsonObj("sdkversion", String( ESP.getSdkVersion() ).c_str());
  sendNestedJsonObj("cpufreq", ESP.getCpuFreqMHz(), "MHz");
  sendNestedJsonObj("sketchsize", formatFloat( (ESP.getSketchSize() / 1024.0), 3), "kB");
  sendNestedJsonObj("freesketchspace", formatFloat( (ESP.getFreeSketchSpace() / 1024.0), 3), "kB");

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85)
        snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());
  sendNestedJsonObj("flashchipid", cMsg); // flashChipId
  sendNestedJsonObj("flashchipsize", formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3), "MB");
  sendNestedJsonObj("flashchiprealsize", formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3), "MB");

  LittleFS.info(SPIFFSinfo);
  sendNestedJsonObj("spiffssize", formatFloat( (SPIFFSinfo.totalBytes / (1024.0 * 1024.0)), 0), "MB");

  sendNestedJsonObj("flashchipspeed", formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0), "MHz");

  FlashMode_t ideMode = ESP.getFlashChipMode();
  sendNestedJsonObj("flashchipmode ", flashMode[ideMode]);
  sendNestedJsonObj("boardtype",
# 398 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
  );
  sendNestedJsonObj("compileoptions", compileOptions);
  sendNestedJsonObj("ssid", WiFi.SSID().c_str());



  sendNestedJsonObj("wifirssi", WiFi.RSSI());
  sendNestedJsonObj("uptime", upTime());
  sendNestedJsonObj("oled_type", (int)settingOledType);
  sendNestedJsonObj("oled_flip_screen", (int)settingOledFlip);
  sendNestedJsonObj("smhasfaseinfo", (int)settingSmHasFaseInfo);
  sendNestedJsonObj("telegraminterval", (int)settingTelegramInterval);
  sendNestedJsonObj("telegramcount", (int)telegramCount);
  sendNestedJsonObj("telegramerrors", (int)telegramErrors);


  snprintf(cMsg, sizeof(cMsg), "%s:%04d", settingMQTTbroker, settingMQTTbrokerPort);
  sendNestedJsonObj("mqttbroker", cMsg);
  sendNestedJsonObj("mqttinterval", settingMQTTinterval);
  if (mqttIsConnected)
        sendNestedJsonObj("mqttbroker_connected", "yes");
  else sendNestedJsonObj("mqttbroker_connected", "no");







  sendNestedJsonObj("reboots", (int)nrReboots);
  sendNestedJsonObj("lastreset", lastReset);

  httpServer.sendContent("\r\n]}\r\n");

} // sendDeviceInfo()


//=======================================================================
void sendDeviceTime()
{
  sendStartJsonObj("devtime");
  sendNestedJsonObj("timestamp", actTimestamp);
  sendNestedJsonObj("time", buildDateTimeString(actTimestamp, sizeof(actTimestamp)).c_str());
  sendNestedJsonObj("epoch", (int)now());

  sendEndJsonObj();

} // sendDeviceTime()


//=======================================================================
void sendDeviceSettings()
{
  ({ _debugBOL(__FUNCTION__, 451); ({ Serial.println("sending device settings ...\r"); TelnetStream.println("sending device settings ...\r"); }); });

  sendStartJsonObj("settings");

  sendJsonSettingObj("hostname", settingHostname, "s", sizeof(settingHostname) -1);
  sendJsonSettingObj("ed_tariff1", settingEDT1, "f", 0, 10, 5);
  sendJsonSettingObj("ed_tariff2", settingEDT2, "f", 0, 10, 5);
  sendJsonSettingObj("er_tariff1", settingERT1, "f", 0, 10, 5);
  sendJsonSettingObj("er_tariff2", settingERT2, "f", 0, 10, 5);
  sendJsonSettingObj("gd_tariff", settingGDT, "f", 0, 10, 5);
  sendJsonSettingObj("electr_netw_costs", settingENBK, "f", 0, 100, 2);
  sendJsonSettingObj("gas_netw_costs", settingGNBK, "f", 0, 100, 2);
  sendJsonSettingObj("sm_has_fase_info", settingSmHasFaseInfo, "i", 0, 1);
  sendJsonSettingObj("tlgrm_interval", settingTelegramInterval,"i", 2, 60);
  sendJsonSettingObj("oled_type", settingOledType, "i", 0, 2);
  sendJsonSettingObj("oled_screen_time", settingOledSleep, "i", 1, 300);
  sendJsonSettingObj("oled_flip_screen", settingOledFlip, "i", 0, 1);
  sendJsonSettingObj("index_page", settingIndexPage, "s", sizeof(settingIndexPage) -1);
  sendJsonSettingObj("mqtt_broker", settingMQTTbroker, "s", sizeof(settingMQTTbroker) -1);
  sendJsonSettingObj("mqtt_broker_port", settingMQTTbrokerPort, "i", 1, 9999);
  sendJsonSettingObj("mqtt_user", settingMQTTuser, "s", sizeof(settingMQTTuser) -1);
  sendJsonSettingObj("mqtt_passwd", settingMQTTpasswd, "s", sizeof(settingMQTTpasswd) -1);
  sendJsonSettingObj("mqtt_toptopic", settingMQTTtopTopic, "s", sizeof(settingMQTTtopTopic) -1);
  sendJsonSettingObj("mqtt_interval", settingMQTTinterval, "i", 0, 600);




  sendEndJsonObj();

} // sendDeviceSettings()


//=======================================================================
void sendDeviceDebug(const char *URI, String tail)
{
# 510 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
  sendApiNotFound(URI);


} // sendDeviceDebug()

//=======================================================================
struct buildJsonApiV0SmActual
{
    bool skip = false;

    template<typename Item>
    void apply(Item &i) {
      skip = false;
      String Name = Item::name;
      //-- for dsmr30 -----------------------------------------------



      if (!isInFieldsArray(Name.c_str(), fieldsElements))
      {
        skip = true;
      }
      if (!skip)
      {
        if (i.present())
        {
          //String Unit = Item::unit();
          sendNestedJsonV0Obj(Name.c_str(), typecastValue(i.val()));
        }
      }
  }

}; // buildJsonApiV0SmActual()


//=======================================================================
void sendJsonV0Fields()
{
  objSprtr[0] = '\0';

  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(((size_t) -1));
  httpServer.send(200, "application/json", "{\r\n");
  DSMRdata.applyEach(buildJsonApiV0SmActual());
  httpServer.sendContent("\r\n}\r\n");

} // sendJsonV0Fields()


//=======================================================================
struct buildJsonApi
{
    bool skip = false;

    template<typename Item>
    void apply(Item &i) {
      skip = false;
      String Name = Item::name;
      //-- for dsmr30 -----------------------------------------------



      if (!isInFieldsArray(Name.c_str(), fieldsElements))
      {
        skip = true;
      }
      if (!skip)
      {
        if (i.present())
        {
          String Unit = Item::unit();

          if (Unit.length() > 0)
          {
            sendNestedJsonObj(Name.c_str(), typecastValue(i.val()), Unit.c_str());
          }
          else
          {
            sendNestedJsonObj(Name.c_str(), typecastValue(i.val()));
          }
        }
        else if (!onlyIfPresent)
        {
          sendNestedJsonObj(Name.c_str(), "-");
        }
      }
  }

}; // buildJsonApi()


//=======================================================================
void sendJsonFields(const char *Name)
{
  sendStartJsonObj(Name);
  DSMRdata.applyEach(buildJsonApi());
  sendEndJsonObj();

} // sendJsonFields()


//=======================================================================
void sendJsonHist(int8_t fileType, const char *fileName, const char *timeStamp, bool desc)
{
  uint8_t startSlot, nrSlots, recNr = 0;
  char typeApi[10];


  if (( __Due__(antiWearTimer_due, antiWearTimer_interval, antiWearTimer_type) ))
  {
    writeDataToFiles();
    writeLastStatus();
  }

  switch(fileType) {
    case HOURS: startSlot = timestampToHourSlot(timeStamp, strlen(timeStamp));
                  nrSlots = (48 +1);
                  strCopy(typeApi, 9, "hours");
                  break;
    case DAYS: startSlot = timestampToDaySlot(timeStamp, strlen(timeStamp));
                  nrSlots = (14 +1);
                  strCopy(typeApi, 9, "days");
                  break;
    case MONTHS: startSlot = timestampToMonthSlot(timeStamp, strlen(timeStamp));
                  nrSlots = (24 +1);
                  strCopy(typeApi, 9, "months");
                  break;
  }

  sendStartJsonObj(typeApi);

  if (desc)
        startSlot += nrSlots +1; // <==== voorbij actuele slot!
  else startSlot += nrSlots; // <==== start met actuele slot!

  ({ _debugBOL(__FUNCTION__, 645); ({ Serial.printf("sendJsonHist startSlot[%02d]\r\n", (startSlot % nrSlots)); TelnetStream.printf("sendJsonHist startSlot[%02d]\r\n", (startSlot % nrSlots)); }); });

  for (uint8_t s = 0; s < nrSlots; s++)
  {
    if (desc)
          readOneSlot(fileType, fileName, s, (s +startSlot), true, "hist") ;
    else readOneSlot(fileType, fileName, s, (startSlot -s), true, "hist") ;
  }
  sendEndJsonObj();

} // sendJsonHist()


bool isInFieldsArray(const char* lookUp, int elemts)
{
  if (elemts == 0) return true;

  for (int i=0; i<elemts; i++)
  {
    //if (Verbose2) DebugTf("[%2d] Looking for [%s] in array[%s]\r\n", i, lookUp, fieldsArray[i]); 
    if (strcmp(lookUp, fieldsArray[i]) == 0) return true;
  }
  return false;

} // isInFieldsArray()


void copyToFieldsArray(const char inArray[][35], int elemts)
{
  int i = 0;
  memset(fieldsArray,0,sizeof(fieldsArray));
  //if (Verbose2) DebugTln("start copying ....");

  for ( i=0; i<elemts; i++)
  {
    strncpy(fieldsArray[i], inArray[i], 34);
    //if (Verbose1) DebugTf("[%2d] => inArray[%s] fieldsArray[%s]\r\n", i, inArray[i], fieldsArray[i]); 

  }
  fieldsElements = i;

} // copyToFieldsArray()


bool listFieldsArray(char inArray[][35])
{
  int i = 0;

  for ( i=0; strlen(inArray[i]) == 0; i++)
  {
    ({ _debugBOL(__FUNCTION__, 695); ({ Serial.printf("[%2d] => inArray[%s]\r\n", i, inArray[i]); TelnetStream.printf("[%2d] => inArray[%s]\r\n", i, inArray[i]); }); });
  }

} // listFieldsArray()


//====================================================
void sendApiNotFound(const char *URI)
{
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(((size_t) -1));
  httpServer.send ( 404, "text/html", "<!DOCTYPE HTML><html><head>");

  strCopy(cMsg, sizeof(cMsg), "<style>body { background-color: lightgray; font-size: 15pt;}");
  strConcat(cMsg, sizeof(cMsg), "</style></head><body>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg, sizeof(cMsg), "<h1>DSMR-logger</h1><b1>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg, sizeof(cMsg), "<br>[<b>");
  strConcat(cMsg, sizeof(cMsg), URI);
  strConcat(cMsg, sizeof(cMsg), "</b>] is not a valid ");
  httpServer.sendContent(cMsg);

  strCopy(cMsg, sizeof(cMsg), "<a href=");
  strConcat(cMsg, sizeof(cMsg), "\"https://mrwheel-docs.gitbook.io/dsmrloggerapi/beschrijving-restapis\">");
  strConcat(cMsg, sizeof(cMsg), "restAPI</a> call.");
  httpServer.sendContent(cMsg);

  strCopy(cMsg, sizeof(cMsg), "</body></html>\r\n");
  httpServer.sendContent(cMsg);

  /* nothing*/;

} // sendApiNotFound()



/***************************************************************************

*

* Permission is hereby granted, free of charge, to any person obtaining a

* copy of this software and associated documentation files (the

* "Software"), to deal in the Software without restriction, including

* without limitation the rights to use, copy, modify, merge, publish,

* distribute, sublicense, and/or sell copies of the Software, and to permit

* persons to whom the Software is furnished to do so, subject to the

* following conditions:

*

* The above copyright notice and this permission notice shall be included

* in all copies or substantial portions of the Software.

*

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS

* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF

* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY

* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT

* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR

* THE USE OR OTHER DEALINGS IN THE SOFTWARE.

* 

***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
/*
***************************************************************************  
**  Program  : settingsStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* 1.0.11 added Mindergas Authtoken setting
*/

//=======================================================================
void writeSettings()
{
  yield();
  ({ _debugBOL(__FUNCTION__, 17); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "17" "." "391" "\", \"aSM\", @progbits, 1 #"))) = (
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Writing to ["
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "17" "." "391" "\", \"aSM\", @progbits, 1 #"))) = (
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Writing to ["
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); }); }); ({ Serial.print("/DSMRsettings.ini"); TelnetStream.print("/DSMRsettings.ini"); }); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                  (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "17" "." "392" "\", \"aSM\", @progbits, 1 #"))) = (
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                  "] ..."
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                  ); &__pstr__[0];}))
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                  )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                  (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "17" "." "392" "\", \"aSM\", @progbits, 1 #"))) = (
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                  "] ..."
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                  ); &__pstr__[0];}))
# 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                  )))); });
  File file = LittleFS.open("/DSMRsettings.ini", "w"); // open for reading and writing
  if (!file)
  {
    ({ _debugBOL(__FUNCTION__, 21); ({ Serial.printf("open(%s, 'w') FAILED!!! --> Bailout\r\n", "/DSMRsettings.ini"); TelnetStream.printf("open(%s, 'w') FAILED!!! --> Bailout\r\n", "/DSMRsettings.ini"); }); });
    return;
  }
  yield();

  if (strlen(settingIndexPage) < 7) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), "DSMRindex.html");
  if (settingTelegramInterval < 2) settingTelegramInterval = 10;
  if (settingMQTTbrokerPort < 1) settingMQTTbrokerPort = 1883;

  ({ _debugBOL(__FUNCTION__, 30); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "30" "." "393" "\", \"aSM\", @progbits, 1 #"))) = (
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Start writing setting data "
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "30" "." "393" "\", \"aSM\", @progbits, 1 #"))) = (
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Start writing setting data "
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); }); });

  file.print("Hostname = "); file.println(settingHostname); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "32" "." "394" "\", \"aSM\", @progbits, 1 #"))) = (
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "32" "." "394" "\", \"aSM\", @progbits, 1 #"))) = (
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 32 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("EnergyDeliveredT1 = "); file.println(String(settingEDT1, 5)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "33" "." "395" "\", \"aSM\", @progbits, 1 #"))) = (
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "33" "." "395" "\", \"aSM\", @progbits, 1 #"))) = (
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 33 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("EnergyDeliveredT2 = "); file.println(String(settingEDT2, 5)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "34" "." "396" "\", \"aSM\", @progbits, 1 #"))) = (
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "34" "." "396" "\", \"aSM\", @progbits, 1 #"))) = (
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 34 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("EnergyReturnedT1 = "); file.println(String(settingERT1, 5)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "35" "." "397" "\", \"aSM\", @progbits, 1 #"))) = (
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "35" "." "397" "\", \"aSM\", @progbits, 1 #"))) = (
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("EnergyReturnedT2 = "); file.println(String(settingERT2, 5)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "36" "." "398" "\", \"aSM\", @progbits, 1 #"))) = (
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "36" "." "398" "\", \"aSM\", @progbits, 1 #"))) = (
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 36 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("GASDeliveredT = "); file.println(String(settingGDT, 5)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "37" "." "399" "\", \"aSM\", @progbits, 1 #"))) = (
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "37" "." "399" "\", \"aSM\", @progbits, 1 #"))) = (
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("EnergyVasteKosten = "); file.println(String(settingENBK, 2)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "38" "." "400" "\", \"aSM\", @progbits, 1 #"))) = (
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "38" "." "400" "\", \"aSM\", @progbits, 1 #"))) = (
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("GasVasteKosten = "); file.println(String(settingGNBK, 2)); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "39" "." "401" "\", \"aSM\", @progbits, 1 #"))) = (
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "39" "." "401" "\", \"aSM\", @progbits, 1 #"))) = (
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("OledType = "); file.println(settingOledType); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "40" "." "402" "\", \"aSM\", @progbits, 1 #"))) = (
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "40" "." "402" "\", \"aSM\", @progbits, 1 #"))) = (
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 40 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("OledSleep = "); file.println(settingOledSleep); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "41" "." "403" "\", \"aSM\", @progbits, 1 #"))) = (
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "41" "." "403" "\", \"aSM\", @progbits, 1 #"))) = (
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 41 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("OledFlip = "); file.println(settingOledFlip); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "42" "." "404" "\", \"aSM\", @progbits, 1 #"))) = (
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "42" "." "404" "\", \"aSM\", @progbits, 1 #"))) = (
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("SmHasFaseInfo = "); file.println(settingSmHasFaseInfo); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "43" "." "405" "\", \"aSM\", @progbits, 1 #"))) = (
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "43" "." "405" "\", \"aSM\", @progbits, 1 #"))) = (
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 43 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });

  file.print("TelegramInterval = "); file.println(settingTelegramInterval); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "45" "." "406" "\", \"aSM\", @progbits, 1 #"))) = (
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "45" "." "406" "\", \"aSM\", @progbits, 1 #"))) = (
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 45 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("IndexPage = "); file.println(settingIndexPage); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "46" "." "407" "\", \"aSM\", @progbits, 1 #"))) = (
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "46" "." "407" "\", \"aSM\", @progbits, 1 #"))) = (
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });


  //sprintf(settingMQTTbroker, "%s:%d", MQTTbroker, MQTTbrokerPort);
  file.print("MQTTbroker = "); file.println(settingMQTTbroker); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "50" "." "408" "\", \"aSM\", @progbits, 1 #"))) = (
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "50" "." "408" "\", \"aSM\", @progbits, 1 #"))) = (
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 50 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("MQTTbrokerPort = "); file.println(settingMQTTbrokerPort); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "51" "." "409" "\", \"aSM\", @progbits, 1 #"))) = (
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "51" "." "409" "\", \"aSM\", @progbits, 1 #"))) = (
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 51 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("MQTTUser = "); file.println(settingMQTTuser); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "52" "." "410" "\", \"aSM\", @progbits, 1 #"))) = (
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "52" "." "410" "\", \"aSM\", @progbits, 1 #"))) = (
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 52 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("MQTTpasswd = "); file.println(settingMQTTpasswd); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "53" "." "411" "\", \"aSM\", @progbits, 1 #"))) = (
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "53" "." "411" "\", \"aSM\", @progbits, 1 #"))) = (
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 53 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("MQTTinterval = "); file.println(settingMQTTinterval); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "54" "." "412" "\", \"aSM\", @progbits, 1 #"))) = (
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "54" "." "412" "\", \"aSM\", @progbits, 1 #"))) = (
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 54 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });
  file.print("MQTTtopTopic = "); file.println(settingMQTTtopTopic); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "55" "." "413" "\", \"aSM\", @progbits, 1 #"))) = (
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "55" "." "413" "\", \"aSM\", @progbits, 1 #"))) = (
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               "."
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                                                                               ); &__pstr__[0];}))
# 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                                                                               )))); });






file.close();

  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "64" "." "414" "\", \"aSM\", @progbits, 1 #"))) = (
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 " done"
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "64" "." "414" "\", \"aSM\", @progbits, 1 #"))) = (
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 " done"
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 64 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); });
  if (Verbose1)
  {
    ({ _debugBOL(__FUNCTION__, 67); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "67" "." "415" "\", \"aSM\", @progbits, 1 #"))) = (
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "Wrote this:"
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "67" "." "415" "\", \"aSM\", @progbits, 1 #"))) = (
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "Wrote this:"
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 67 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); });
    ({ _debugBOL(__FUNCTION__, 68); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "68" "." "416" "\", \"aSM\", @progbits, 1 #"))) = (
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyDeliveredT1 = "
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "68" "." "416" "\", \"aSM\", @progbits, 1 #"))) = (
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyDeliveredT1 = "
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingEDT1, 5)); TelnetStream.println(String(settingEDT1, 5)); });
    ({ _debugBOL(__FUNCTION__, 69); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "69" "." "417" "\", \"aSM\", @progbits, 1 #"))) = (
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyDeliveredT2 = "
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "69" "." "417" "\", \"aSM\", @progbits, 1 #"))) = (
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyDeliveredT2 = "
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingEDT2, 5)); TelnetStream.println(String(settingEDT2, 5)); });
    ({ _debugBOL(__FUNCTION__, 70); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "70" "." "418" "\", \"aSM\", @progbits, 1 #"))) = (
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyReturnedT1 = "
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "70" "." "418" "\", \"aSM\", @progbits, 1 #"))) = (
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyReturnedT1 = "
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 70 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingERT1, 5)); TelnetStream.println(String(settingERT1, 5)); });
    ({ _debugBOL(__FUNCTION__, 71); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "71" "." "419" "\", \"aSM\", @progbits, 1 #"))) = (
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyReturnedT2 = "
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "71" "." "419" "\", \"aSM\", @progbits, 1 #"))) = (
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyReturnedT2 = "
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingERT2, 5)); TelnetStream.println(String(settingERT2, 5)); });
    ({ _debugBOL(__FUNCTION__, 72); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "72" "." "420" "\", \"aSM\", @progbits, 1 #"))) = (
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "GASDeliveredT = "
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "72" "." "420" "\", \"aSM\", @progbits, 1 #"))) = (
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "GASDeliveredT = "
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingGDT, 5)); TelnetStream.println(String(settingGDT, 5)); });
    ({ _debugBOL(__FUNCTION__, 73); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "73" "." "421" "\", \"aSM\", @progbits, 1 #"))) = (
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyVasteKosten = "
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "73" "." "421" "\", \"aSM\", @progbits, 1 #"))) = (
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "EnergyVasteKosten = "
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 73 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingENBK, 2)); TelnetStream.println(String(settingENBK, 2)); });
    ({ _debugBOL(__FUNCTION__, 74); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "74" "." "422" "\", \"aSM\", @progbits, 1 #"))) = (
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "GasVasteKosten = "
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "74" "." "422" "\", \"aSM\", @progbits, 1 #"))) = (
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "GasVasteKosten = "
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 74 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(String(settingGNBK, 2)); TelnetStream.println(String(settingGNBK, 2)); });
    ({ _debugBOL(__FUNCTION__, 75); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "75" "." "423" "\", \"aSM\", @progbits, 1 #"))) = (
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledType = "
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "75" "." "423" "\", \"aSM\", @progbits, 1 #"))) = (
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledType = "
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 75 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); });
    if (settingOledType == 1) ({ Serial.println("SDD1306"); TelnetStream.println("SDD1306"); });
    else if (settingOledType == 2) ({ Serial.println("SH1306"); TelnetStream.println("SH1306"); });
    else ({ Serial.println("None"); TelnetStream.println("None"); });
    ({ _debugBOL(__FUNCTION__, 79); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "79" "." "424" "\", \"aSM\", @progbits, 1 #"))) = (
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledSleep = "
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "79" "." "424" "\", \"aSM\", @progbits, 1 #"))) = (
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledSleep = "
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingOledSleep); TelnetStream.println(settingOledSleep); });
    ({ _debugBOL(__FUNCTION__, 80); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "80" "." "425" "\", \"aSM\", @progbits, 1 #"))) = (
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledFlip = "
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "80" "." "425" "\", \"aSM\", @progbits, 1 #"))) = (
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "OledFlip = "
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 80 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); });
    if (settingOledFlip) ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "81" "." "426" "\", \"aSM\", @progbits, 1 #"))) = (
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         "Yes"
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         ); &__pstr__[0];}))
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "81" "." "426" "\", \"aSM\", @progbits, 1 #"))) = (
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         "Yes"
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         ); &__pstr__[0];}))
# 81 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         )))); });
    else ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "82" "." "427" "\", \"aSM\", @progbits, 1 #"))) = (
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         "No"
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         ); &__pstr__[0];}))
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "82" "." "427" "\", \"aSM\", @progbits, 1 #"))) = (
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         "No"
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                         ); &__pstr__[0];}))
# 82 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                         )))); });

    ({ _debugBOL(__FUNCTION__, 84); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "84" "." "428" "\", \"aSM\", @progbits, 1 #"))) = (
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "SmHasFaseInfo"
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "84" "." "428" "\", \"aSM\", @progbits, 1 #"))) = (
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "SmHasFaseInfo"
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 84 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); });
    if (settingSmHasFaseInfo == 1) ({ Serial.println("Yes"); TelnetStream.println("Yes"); });
    else ({ Serial.println("No"); TelnetStream.println("No"); });
    ({ _debugBOL(__FUNCTION__, 87); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "87" "." "429" "\", \"aSM\", @progbits, 1 #"))) = (
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "TelegramInterval = "
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "87" "." "429" "\", \"aSM\", @progbits, 1 #"))) = (
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "TelegramInterval = "
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 87 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingTelegramInterval); TelnetStream.println(settingTelegramInterval); });
    ({ _debugBOL(__FUNCTION__, 88); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "88" "." "430" "\", \"aSM\", @progbits, 1 #"))) = (
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "IndexPage = "
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "88" "." "430" "\", \"aSM\", @progbits, 1 #"))) = (
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "IndexPage = "
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 88 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingIndexPage); TelnetStream.println(settingIndexPage); });


    ({ _debugBOL(__FUNCTION__, 91); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "91" "." "431" "\", \"aSM\", @progbits, 1 #"))) = (
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTbroker = "
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "91" "." "431" "\", \"aSM\", @progbits, 1 #"))) = (
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTbroker = "
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 91 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingMQTTbroker); TelnetStream.println(settingMQTTbroker); });
    ({ _debugBOL(__FUNCTION__, 92); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "92" "." "432" "\", \"aSM\", @progbits, 1 #"))) = (
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTbrokerPort = "
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "92" "." "432" "\", \"aSM\", @progbits, 1 #"))) = (
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTbrokerPort = "
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingMQTTbrokerPort); TelnetStream.println(settingMQTTbrokerPort); });
    ({ _debugBOL(__FUNCTION__, 93); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "93" "." "433" "\", \"aSM\", @progbits, 1 #"))) = (
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTUser = "
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "93" "." "433" "\", \"aSM\", @progbits, 1 #"))) = (
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTUser = "
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 93 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingMQTTuser); TelnetStream.println(settingMQTTuser); });



      ({ _debugBOL(__FUNCTION__, 97); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "97" "." "434" "\", \"aSM\", @progbits, 1 #"))) = (
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
     "MQTTpasswd = ********"
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
     ); &__pstr__[0];}))
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
     )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
     (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "97" "." "434" "\", \"aSM\", @progbits, 1 #"))) = (
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
     "MQTTpasswd = ********"
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
     ); &__pstr__[0];}))
# 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
     )))); }); });

    ({ _debugBOL(__FUNCTION__, 99); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "99" "." "435" "\", \"aSM\", @progbits, 1 #"))) = (
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTinterval = "
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "99" "." "435" "\", \"aSM\", @progbits, 1 #"))) = (
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTinterval = "
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 99 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingMQTTinterval); TelnetStream.println(settingMQTTinterval); });
    ({ _debugBOL(__FUNCTION__, 100); ({ Serial.print(((reinterpret_cast<const __FlashStringHelper *>(
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "100" "." "436" "\", \"aSM\", @progbits, 1 #"))) = (
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTtopTopic = "
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.print(((reinterpret_cast<const __FlashStringHelper *>(
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "100" "." "436" "\", \"aSM\", @progbits, 1 #"))) = (
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   "MQTTtopTopic = "
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 100 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); }); ({ Serial.println(settingMQTTtopTopic); TelnetStream.println(settingMQTTtopTopic); });
# 110 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
  } // Verbose1

} // writeSettings()


//=======================================================================
void readSettings(bool show)
{
  String sTmp, nColor;
  String words[10];

  File file;

  ({ _debugBOL(__FUNCTION__, 123); ({ Serial.printf(" %s ..\r\n", "/DSMRsettings.ini"); TelnetStream.printf(" %s ..\r\n", "/DSMRsettings.ini"); }); });

  snprintf(settingHostname, sizeof(settingHostname), "%s", "DSMR-API");
  settingEDT1 = 0.1;
  settingEDT2 = 0.2;
  settingERT1 = 0.3;
  settingERT2 = 0.4;
  settingGDT = 0.5;
  settingENBK = 15.15;
  settingGNBK = 11.11;
  settingSmHasFaseInfo = 1; // default: it does
  settingTelegramInterval = 10; // seconds
  settingOledType = 1; // 0=None, 1=SDD1306, 2=SH1106
  settingOledSleep = 0; // infinite
  settingOledFlip = 0; // Don't flip
  strCopy(settingIndexPage, sizeof(settingIndexPage), "DSMRindex.html");
  settingMQTTbroker[0] = '\0';
  settingMQTTbrokerPort = 1883;
  settingMQTTuser[0] = '\0';
  settingMQTTpasswd[0] = '\0';
  settingMQTTinterval = 0;
  snprintf(settingMQTTtopTopic, sizeof(settingMQTTtopTopic), "%s", settingHostname);





  if (!LittleFS.exists("/DSMRsettings.ini"))
  {
    ({ _debugBOL(__FUNCTION__, 152); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "152" "." "437" "\", \"aSM\", @progbits, 1 #"))) = (
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   " .. file not found! --> created file!"
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "152" "." "437" "\", \"aSM\", @progbits, 1 #"))) = (
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   " .. file not found! --> created file!"
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
   ); &__pstr__[0];}))
# 152 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
   )))); }); });
    writeSettings();
  }

  for (int T = 0; T < 2; T++)
  {
    file = LittleFS.open("/DSMRsettings.ini", "r");
    if (!file)
    {
      if (T == 0) ({ _debugBOL(__FUNCTION__, 161); ({ Serial.printf(" .. something went wrong opening [%s]\r\n", "/DSMRsettings.ini"); TelnetStream.printf(" .. something went wrong opening [%s]\r\n", "/DSMRsettings.ini"); }); });
      else ({ _debugBOL(__FUNCTION__, 162); ({ Serial.print(T); TelnetStream.print(T); }); });
      delay(100);
    }
  } // try T times ..

  ({ _debugBOL(__FUNCTION__, 167); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "167" "." "438" "\", \"aSM\", @progbits, 1 #"))) = (
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Reading settings:\r"
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "167" "." "438" "\", \"aSM\", @progbits, 1 #"))) = (
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "Reading settings:\r"
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 167 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); }); });
  while(file.available())
  {
    sTmp = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //DebugTf("[%s] (%d)\r\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();
    nColor = words[1].substring(0,15);

    if (words[0].equalsIgnoreCase("Hostname")) strCopy(settingHostname, 29, words[1].c_str());
    if (words[0].equalsIgnoreCase("EnergyDeliveredT1")) settingEDT1 = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyDeliveredT2")) settingEDT2 = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyReturnedT1")) settingERT1 = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyReturnedT2")) settingERT2 = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("GasDeliveredT")) settingGDT = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyVasteKosten")) settingENBK = strToFloat(words[1].c_str(), 2);
    if (words[0].equalsIgnoreCase("GasVasteKosten")) settingGNBK = strToFloat(words[1].c_str(), 2);
    if (words[0].equalsIgnoreCase("SmHasFaseInfo"))
    {
      settingSmHasFaseInfo = words[1].toInt();
      if (settingSmHasFaseInfo != 0) settingSmHasFaseInfo = 1;
      else settingSmHasFaseInfo = 0;
    }

    if (words[0].equalsIgnoreCase("OledType"))
    {
      settingOledType = words[1].toInt();
      if (settingOledType > 2) settingOledType = 1;
    }
    if (words[0].equalsIgnoreCase("OledSleep"))
    {
      settingOledSleep = words[1].toInt();
      oledSleepTimer_interval = (getParam(0, settingOledSleep, 0) *60*1000); oledSleepTimer_due = millis() +oledSleepTimer_interval;;
    }
    if (words[0].equalsIgnoreCase("OledFlip")) settingOledFlip = words[1].toInt();
    if (settingOledFlip != 0) settingOledFlip = 1;
    else settingOledFlip = 0;

    if (words[0].equalsIgnoreCase("TelegramInterval"))
    {
      settingTelegramInterval = words[1].toInt();
      nextTelegram_interval = (getParam(0, settingTelegramInterval, 0) *1000); nextTelegram_due = millis() +nextTelegram_interval;;
    }

    if (words[0].equalsIgnoreCase("IndexPage")) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), words[1].c_str());






    if (words[0].equalsIgnoreCase("MQTTbroker"))
    {
      memset(settingMQTTbroker, '\0', sizeof(settingMQTTbroker));
      strCopy(settingMQTTbroker, 100, words[1].c_str());
    }
    if (words[0].equalsIgnoreCase("MQTTbrokerPort")) settingMQTTbrokerPort = words[1].toInt();
    if (words[0].equalsIgnoreCase("MQTTuser")) strCopy(settingMQTTuser ,35 ,words[1].c_str());
    if (words[0].equalsIgnoreCase("MQTTpasswd")) strCopy(settingMQTTpasswd ,25, words[1].c_str());
    if (words[0].equalsIgnoreCase("MQTTinterval")) settingMQTTinterval = words[1].toInt();
    if (words[0].equalsIgnoreCase("MQTTtopTopic")) strCopy(settingMQTTtopTopic, 20, words[1].c_str());

    publishMQTTtimer_interval = (getParam(0, settingMQTTinterval, 0) *1000); publishMQTTtimer_due = millis() +publishMQTTtimer_interval;;
    reconnectMQTTtimer_interval = (getParam(0, 1, 0) *60*1000); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;;


  } // while available()

  file.close();

  //--- this will take some time to settle in
  //--- probably need a reboot before that to happen :-(
  MDNS.setHostname(settingHostname); // start advertising with new(?) settingHostname

  ({ _debugBOL(__FUNCTION__, 242); ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "242" "." "439" "\", \"aSM\", @progbits, 1 #"))) = (
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 " .. done\r"
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "242" "." "439" "\", \"aSM\", @progbits, 1 #"))) = (
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 " .. done\r"
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 242 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); }); });


  if (strlen(settingIndexPage) < 7) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), "DSMRindex.html");
  if (settingTelegramInterval < 2) settingTelegramInterval = 10;
  if (settingMQTTbrokerPort < 1) settingMQTTbrokerPort = 1883;

  if (!show) return;

  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "251" "." "440" "\", \"aSM\", @progbits, 1 #"))) = (
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "\r\n==== Settings ===================================================\r"
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "251" "." "440" "\", \"aSM\", @progbits, 1 #"))) = (
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "\r\n==== Settings ===================================================\r"
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); });
  ({ Serial.printf("                    Hostname : %s\r\n", settingHostname); TelnetStream.printf("                    Hostname : %s\r\n", settingHostname); });
  ({ Serial.printf("   Energy Delivered Tarief 1 : %9.7f\r\n", settingEDT1); TelnetStream.printf("   Energy Delivered Tarief 1 : %9.7f\r\n", settingEDT1); });
  ({ Serial.printf("   Energy Delivered Tarief 2 : %9.7f\r\n", settingEDT2); TelnetStream.printf("   Energy Delivered Tarief 2 : %9.7f\r\n", settingEDT2); });
  ({ Serial.printf("   Energy Delivered Tarief 1 : %9.7f\r\n", settingERT1); TelnetStream.printf("   Energy Delivered Tarief 1 : %9.7f\r\n", settingERT1); });
  ({ Serial.printf("   Energy Delivered Tarief 2 : %9.7f\r\n", settingERT2); TelnetStream.printf("   Energy Delivered Tarief 2 : %9.7f\r\n", settingERT2); });
  ({ Serial.printf("        Gas Delivered Tarief : %9.7f\r\n", settingGDT); TelnetStream.printf("        Gas Delivered Tarief : %9.7f\r\n", settingGDT); });
  ({ Serial.printf("     Energy Netbeheer Kosten : %9.2f\r\n", settingENBK); TelnetStream.printf("     Energy Netbeheer Kosten : %9.2f\r\n", settingENBK); });
  ({ Serial.printf("        Gas Netbeheer Kosten : %9.2f\r\n", settingGNBK); TelnetStream.printf("        Gas Netbeheer Kosten : %9.2f\r\n", settingGNBK); });
  ({ Serial.printf("  SM Fase Info (0=No, 1=Yes) : %d\r\n", settingSmHasFaseInfo); TelnetStream.printf("  SM Fase Info (0=No, 1=Yes) : %d\r\n", settingSmHasFaseInfo); });
  ({ Serial.printf("   Telegram Process Interval : %d\r\n", settingTelegramInterval); TelnetStream.printf("   Telegram Process Interval : %d\r\n", settingTelegramInterval); });
  ({ Serial.printf("         OLED Type (0, 1, 2) : %d\r\n", settingOledType); TelnetStream.printf("         OLED Type (0, 1, 2) : %d\r\n", settingOledType); });
  ({ Serial.printf("OLED Sleep Min. (0=oneindig) : %d\r\n", settingOledSleep); TelnetStream.printf("OLED Sleep Min. (0=oneindig) : %d\r\n", settingOledSleep); });
  ({ Serial.printf("     Flip Oled (0=No, 1=Yes) : %d\r\n", settingOledFlip); TelnetStream.printf("     Flip Oled (0=No, 1=Yes) : %d\r\n", settingOledFlip); });
  ({ Serial.printf("                  Index Page : %s\r\n", settingIndexPage); TelnetStream.printf("                  Index Page : %s\r\n", settingIndexPage); });


  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "268" "." "441" "\", \"aSM\", @progbits, 1 #"))) = (
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "\r\n==== MQTT settings ==============================================\r"
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "268" "." "441" "\", \"aSM\", @progbits, 1 #"))) = (
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "\r\n==== MQTT settings ==============================================\r"
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 268 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); });
  ({ Serial.printf("          MQTT broker URL/IP : %s:%d", settingMQTTbroker, settingMQTTbrokerPort); TelnetStream.printf("          MQTT broker URL/IP : %s:%d", settingMQTTbroker, settingMQTTbrokerPort); });
  if (MQTTclient.connected()) ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "270" "." "442" "\", \"aSM\", @progbits, 1 #"))) = (
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                             " (is Connected!)\r"
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                             ); &__pstr__[0];}))
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                             )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                             (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "270" "." "442" "\", \"aSM\", @progbits, 1 #"))) = (
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                             " (is Connected!)\r"
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                             ); &__pstr__[0];}))
# 270 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                             )))); });
  else ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                      (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "271" "." "443" "\", \"aSM\", @progbits, 1 #"))) = (
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                      " (NOT Connected!)\r"
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                      ); &__pstr__[0];}))
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                      )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                      (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "271" "." "443" "\", \"aSM\", @progbits, 1 #"))) = (
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                      " (NOT Connected!)\r"
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
                      ); &__pstr__[0];}))
# 271 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                      )))); });
  ({ Serial.printf("                   MQTT user : %s\r\n", settingMQTTuser); TelnetStream.printf("                   MQTT user : %s\r\n", settingMQTTuser); });



  ({ Serial.print("               MQTT password : *************\r\n"); TelnetStream.print("               MQTT password : *************\r\n"); });

  ({ Serial.printf("          MQTT send Interval : %d\r\n", settingMQTTinterval); TelnetStream.printf("          MQTT send Interval : %d\r\n", settingMQTTinterval); });
  ({ Serial.printf("              MQTT top Topic : %s\r\n", settingMQTTtopTopic); TelnetStream.printf("              MQTT top Topic : %s\r\n", settingMQTTtopTopic); });






  ({ Serial.println(((reinterpret_cast<const __FlashStringHelper *>(
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "286" "." "444" "\", \"aSM\", @progbits, 1 #"))) = (
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "-\r"
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); TelnetStream.println(((reinterpret_cast<const __FlashStringHelper *>(
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "settingsStuff.ino" "." "286" "." "444" "\", \"aSM\", @progbits, 1 #"))) = (
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 "-\r"
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3
 ); &__pstr__[0];}))
# 286 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
 )))); });

} // readSettings()


//=======================================================================
void updateSetting(const char *field, const char *newValue)
{
  ({ _debugBOL(__FUNCTION__, 294); ({ Serial.printf("-> field[%s], newValue[%s]\r\n", field, newValue); TelnetStream.printf("-> field[%s], newValue[%s]\r\n", field, newValue); }); });

  if (!stricmp(field, "Hostname")) {
    strCopy(settingHostname, 29, newValue);
    if (strlen(settingHostname) < 1) strCopy(settingHostname, 29, "DSMR-API");
    char *dotPntr = strchr(settingHostname, '.') ;
    if (dotPntr != 
# 300 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino" 3 4
                  __null
# 300 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
                      )
    {
      byte dotPos = (dotPntr-settingHostname);
      if (dotPos > 0) settingHostname[dotPos] = '\0';
    }
    ({ Serial.println(); TelnetStream.println(); });
    ({ _debugBOL(__FUNCTION__, 306); ({ Serial.printf("Need reboot before new %s.local will be available!\r\n\n", settingHostname); TelnetStream.printf("Need reboot before new %s.local will be available!\r\n\n", settingHostname); }); });
  }
  if (!stricmp(field, "ed_tariff1")) settingEDT1 = String(newValue).toFloat();
  if (!stricmp(field, "ed_tariff2")) settingEDT2 = String(newValue).toFloat();
  if (!stricmp(field, "er_tariff1")) settingERT1 = String(newValue).toFloat();
  if (!stricmp(field, "er_tariff2")) settingERT2 = String(newValue).toFloat();
  if (!stricmp(field, "electr_netw_costs")) settingENBK = String(newValue).toFloat();

  if (!stricmp(field, "gd_tariff")) settingGDT = String(newValue).toFloat();
  if (!stricmp(field, "gas_netw_costs")) settingGNBK = String(newValue).toFloat();

  if (!stricmp(field, "sm_has_fase_info"))
  {
    settingSmHasFaseInfo = String(newValue).toInt();
    if (settingSmHasFaseInfo != 0) settingSmHasFaseInfo = 1;
    else settingSmHasFaseInfo = 0;
  }

  if (!stricmp(field, "oled_type"))
  {
    settingOledType = String(newValue).toInt();
    if (settingOledType > 2) settingOledType = 1;
    oled_Init();
  }
  if (!stricmp(field, "oled_screen_time"))
  {
    settingOledSleep = String(newValue).toInt();
    oledSleepTimer_interval = (getParam(0, settingOledSleep, 0) *60*1000); oledSleepTimer_due = millis() +oledSleepTimer_interval;
  }
  if (!stricmp(field, "oled_flip_screen"))
  {
    settingOledFlip = String(newValue).toInt();
    if (settingOledFlip != 0) settingOledFlip = 1;
    else settingOledFlip = 0;
    oled_Init();
  }

  if (!stricmp(field, "tlgrm_interval"))
  {
    settingTelegramInterval = String(newValue).toInt();
    nextTelegram_interval = (getParam(0, settingTelegramInterval, 0) *1000); nextTelegram_due = millis() +nextTelegram_interval;
  }

  if (!stricmp(field, "index_page")) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), newValue);






  if (!stricmp(field, "mqtt_broker")) {
    ({ _debugBOL(__FUNCTION__, 357); ({ Serial.print("settingMQTTbroker! to : "); TelnetStream.print("settingMQTTbroker! to : "); }); });
    memset(settingMQTTbroker, '\0', sizeof(settingMQTTbroker));
    strCopy(settingMQTTbroker, 100, newValue);
    ({ Serial.printf("[%s]\r\n", settingMQTTbroker); TelnetStream.printf("[%s]\r\n", settingMQTTbroker); });
    mqttIsConnected = false;
    reconnectMQTTtimer_interval = (getParam(0, 100, 0) ); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;; // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_broker_port")) {
    settingMQTTbrokerPort = String(newValue).toInt();
    mqttIsConnected = false;
    reconnectMQTTtimer_interval = (getParam(0, 100, 0) ); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;; // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_user")) {
    strCopy(settingMQTTuser ,35, newValue);
    mqttIsConnected = false;
    reconnectMQTTtimer_interval = (getParam(0, 100, 0) ); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;; // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_passwd")) {
    strCopy(settingMQTTpasswd ,25, newValue);
    mqttIsConnected = false;
    reconnectMQTTtimer_interval = (getParam(0, 100, 0) ); reconnectMQTTtimer_due = millis() +reconnectMQTTtimer_interval;; // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_interval")) {
    settingMQTTinterval = String(newValue).toInt();
    publishMQTTtimer_interval = (getParam(0, settingMQTTinterval, 0) *1000); publishMQTTtimer_due = millis() +publishMQTTtimer_interval;;
  }
  if (!stricmp(field, "mqtt_toptopic")) strCopy(settingMQTTtopTopic, 20, newValue);


  writeSettings();

} // updateSetting()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
# 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
/*
***************************************************************************  
**  Program  : timeStuff, part of DSMRloggerAPI
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//static time_t ntpTimeSav;

//===========================================================================================
String buildDateTimeString(const char* timeStamp, int len)
{
  String tmpTS = String(timeStamp);
  String DateTime = "";
  if (len < 12) return String(timeStamp);
  DateTime = "20" + tmpTS.substring(0, 2); // YY
  DateTime += "-" + tmpTS.substring(2, 4); // MM
  DateTime += "-" + tmpTS.substring(4, 6); // DD
  DateTime += " " + tmpTS.substring(6, 8); // HH
  DateTime += ":" + tmpTS.substring(8, 10); // MM
  DateTime += ":" + tmpTS.substring(10, 12); // SS
  return DateTime;

} // buildDateTimeString()

//===========================================================================================
void epochToTimestamp(time_t t, char *ts, int8_t len)
{
  if (len < 12) {
    strcpy(ts, "Error");
    return;
  }
  //------------yy  mm  dd  hh  mm  ss
  sprintf(ts, "%02d%02d%02d%02d%02d%02d", year(t)-2000, month(t), day(t)
                                        , hour(t), minute(t), second(t));

  //DebugTf("epochToTimestamp() => [%s]\r\n", ts);

} // epochToTimestamp()

//===========================================================================================
int8_t SecondFromTimestamp(const char *timeStamp)
{
  char aSS[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss SS = 4-5
  strCopy(aSS, 4, timeStamp, 10, 11);
  return String(aSS).toInt();

} // SecondFromTimestamp()

//===========================================================================================
int8_t MinuteFromTimestamp(const char *timeStamp)
{
  char aMM[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss MM = 8-9
  strCopy(aMM, 4, timeStamp, 8, 9);
  return String(aMM).toInt();

} // MinuteFromTimestamp()

//===========================================================================================
int8_t HourFromTimestamp(const char *timeStamp)
{
  char aHH[4] = "";
  //DebugTf("timeStamp[%s] => \r\n", timeStamp); // YYMMDDHHmmss HH = 5-6
  strCopy(aHH, 4, timeStamp, 6, 7);
  //Debugf("aHH[%s], nHH[%02d]\r\n", aHH, String(aHH).toInt()); 
  return String(aHH).toInt();

} // HourFromTimestamp()

//===========================================================================================
int8_t DayFromTimestamp(const char *timeStamp)
{
  char aDD[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss DD = 4-5
  strCopy(aDD, 4, timeStamp, 4, 5);
  return String(aDD).toInt();

} // DayFromTimestamp()

//===========================================================================================
int8_t MonthFromTimestamp(const char *timeStamp)
{
  char aMM[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss MM = 2-3
  strCopy(aMM, 4, timeStamp, 2, 3);
  return String(aMM).toInt();

} // MonthFromTimestamp()

//===========================================================================================
int8_t YearFromTimestamp(const char *timeStamp)
{
  char aYY[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss YY = 0-1
  strCopy(aYY, 4, timeStamp, 0, 1);
  return String(aYY).toInt();

} // YearFromTimestamp()

//===========================================================================================
int32_t HoursKeyTimestamp(const char *timeStamp)
{
  char aHK[10] = "";
  // 0123456789ab
  // YYMMDDHHmmss YY = 0-1
  strCopy(aHK, 4, timeStamp, 0, 7);
  //return timeStamp.substring(0, 8).toInt();
  return String(aHK).toInt();

} // HourFromTimestamp()

//===========================================================================================
// calculate epoch from timeStamp
// if syncTime is true, set system time to calculated epoch-time
time_t epoch(const char *timeStamp, int8_t len, bool syncTime)
{
  char fullTimeStamp[16] = "";

  strConcat(fullTimeStamp, 15, timeStamp);
  if (Verbose2) ({ _debugBOL(__FUNCTION__, 131); ({ Serial.printf("epoch(%s) strlen([%d])\r\n", fullTimeStamp, strlen(fullTimeStamp)); TelnetStream.printf("epoch(%s) strlen([%d])\r\n", fullTimeStamp, strlen(fullTimeStamp)); }); });
  switch(strlen(fullTimeStamp)) {
    case 4: //--- timeStamp is YYMM
              strConcat(fullTimeStamp, 15, "01010101X");
              break;
    case 6: //--- timeStamp is YYMMDD
              strConcat(fullTimeStamp, 15, "010101X");
              break;
    case 8: //--- timeStamp is YYMMDDHH
              strConcat(fullTimeStamp, 15, "0101X");
              break;
    case 10: //--- timeStamp is YYMMDDHHMM
              strConcat(fullTimeStamp, 15, "01X");
              break;
    case 12: //--- timeStamp is YYMMDDHHMMSS
              strConcat(fullTimeStamp, 15, "X");
              break;
    //default:  return now();
  }

  if (strlen(fullTimeStamp) < 13) return now();

  if (Verbose2) ({ _debugBOL(__FUNCTION__, 153); ({ Serial.printf("DateTime: [%02d]-[%02d]-[%02d] [%02d]:[%02d]:[%02d]\r\n" ,DayFromTimestamp(timeStamp) ,MonthFromTimestamp(timeStamp) ,YearFromTimestamp(timeStamp) ,HourFromTimestamp(timeStamp) ,MinuteFromTimestamp(timeStamp) ,0); TelnetStream.printf("DateTime: [%02d]-[%02d]-[%02d] [%02d]:[%02d]:[%02d]\r\n" ,DayFromTimestamp(timeStamp) ,MonthFromTimestamp(timeStamp) ,YearFromTimestamp(timeStamp) ,HourFromTimestamp(timeStamp) ,MinuteFromTimestamp(timeStamp) ,0); }); })






                        ;


  time_t nT;
  time_t savEpoch = now();

  setTime(HourFromTimestamp(fullTimeStamp)
         ,MinuteFromTimestamp(fullTimeStamp)
         ,SecondFromTimestamp(fullTimeStamp)
         ,DayFromTimestamp(fullTimeStamp)
         ,MonthFromTimestamp(fullTimeStamp)
         ,YearFromTimestamp(fullTimeStamp));

  nT = now();
  if (!syncTime)
  {
    setTime(savEpoch);
  }
  return nT;

} // epoch()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
