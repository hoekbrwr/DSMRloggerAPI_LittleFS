#include <Arduino.h>
#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
/*
***************************************************************************
**  Program  : DSMRloggerAPI (restAPI)
*/
#define _FW_VERSION "v2.0.1 (17-04-2020)"
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
/*
**  You can find more info in the following links (all in Dutch):
**   https://willem.aandewiel.nl/index.php/2020/02/28/restapis-zijn-hip-nieuwe-firmware-voor-de-dsmr-logger/
**   https://mrwheel-docs.gitbook.io/dsmrloggerapi/
**   https://mrwheel.github.io/DSMRloggerWS/
*/
/******************** compiler options  ********************************************/
//#define USE_REQUEST_PIN           // define if it's a esp8266 with GPIO 12 connected to SM DTR pin
#define USE_UPDATE_SERVER         // define if there is enough memory and updateServer to be used
//#define USE_BELGIUM_PROTOCOL      // define if Slimme Meter is a Belgium Smart Meter
//#define USE_PRE40_PROTOCOL        // define if Slimme Meter is pre DSMR 4.0 (2.2 .. 3.0)
//#define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//#define HAS_NO_SLIMMEMETER        // define for testing only!
#define USE_MQTT                  // define if you want to use MQTT (configure through webinterface)
//#define USE_MINDERGAS             // define if you want to update mindergas (configure through webinterface)
//#define USE_SYSLOGGER             // define if you want to use the sysLog library for debugging
//#define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
#define USE_LITTLEFS              // Use new Flash Filesystem instead of SPIFFS(reload of files mandotory with LittleFS upload tool)
/******************** don't change anything below this comment **********************/

#include "DSMRloggerAPI_LittleFS.h"

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    TelnetStream.print("showValues: ");
    if (i.present())
    {
      TelnetStream.print(Item::name);
      TelnetStream.print(F(": "));
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
#line 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void displayStatus();
#line 139 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void setup();
#line 501 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void delayms(unsigned long delay_ms);
#line 515 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void doTaskTelegram();
#line 537 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void doSystemTasks();
#line 558 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void loop();
#line 42 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void setupFSexplorer();
#line 83 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void APIlistFiles();
#line 151 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
bool handleFile(String &&path);
#line 169 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void handleFileUpload();
#line 199 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void formatSpiffs();
#line 210 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
const String formatBytes(size_t const &bytes);
#line 218 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
const String& contentType(String &filename);
#line 251 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
bool freeSpace(uint16_t const &printsize);
#line 261 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void updateFirmware();
#line 273 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void reBootESP();
#line 281 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
void doRedirect(String msg, int wait, const char *URL, bool reboot);
#line 37 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
void connectMQTT();
#line 69 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
bool connectMQTT_FSM();
#line 164 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
String trimVal(char *in);
#line 218 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
void sendMQTTData();
#line 16 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MinderGas.ino"
void handleMindergas();
#line 17 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void readLastStatus();
#line 47 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void writeLastStatus();
#line 71 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
bool buildDataRecordFromSM(char *recIn);
#line 96 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
uint16_t buildDataRecordFromJson(char *recIn, String jsonIn);
#line 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void writeDataToFile(const char *fileName, const char *record, uint16_t slot, int8_t fileType);
#line 198 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void writeDataToFiles();
#line 228 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void readOneSlot(int8_t fileType, const char *fileName, uint8_t recNr, uint8_t readSlot, bool doJson, const char *rName);
#line 295 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void readSlotFromTimestamp(int8_t fileType, const char *fileName, const char *timeStamp, bool doJson, const char *rName);
#line 324 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void readAllSlots(int8_t fileType, const char *fileName, const char *timeStamp, bool doJson, const char *rName);
#line 355 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
bool createFile(const char *fileName, uint16_t noSlots);
#line 402 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void fillRecord(char *record, int8_t len);
#line 429 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
uint16_t timestampToHourSlot(const char *TS, int8_t len);
#line 452 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
uint16_t timestampToDaySlot(const char *TS, int8_t len);
#line 473 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
uint16_t timestampToMonthSlot(const char *TS, int8_t len);
#line 494 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
int32_t freeSpace();
#line 507 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
void listSPIFFS();
#line 573 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
bool eraseFile();
#line 619 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
bool DSMRfileExist(const char *fileName, bool doDisplay);
#line 13 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\convertPRD2RING.ino"
void convertPRD2RING();
#line 35 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\convertPRD2RING.ino"
void convertPRDfile(int8_t fileType);
#line 97 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\convertPRD2RING.ino"
void writeToRINGfile(int8_t fileType, const char *key, float EDT1, float EDT2 , float ERT1, float ERT2, float GDT);
#line 14 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
void handleSlimmemeter();
#line 30 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
void processSlimmemeterRaw();
#line 92 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
void processSlimmemeter();
#line 14 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
bool compare(String x, String y);
#line 28 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
boolean isValidIP(IPAddress ip);
#line 95 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
bool isNumericp(const char *timeStamp, int8_t len);
#line 110 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords);
#line 142 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
String upTime();
#line 157 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strConcat(char *dest, int maxLen, const char *src);
#line 172 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strConcat(char *dest, int maxLen, float v, int dec);
#line 196 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strConcat(char *dest, int maxLen, int32_t v);
#line 214 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strToLower(char *src);
#line 226 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strCopy(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to);
#line 246 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
void strCopy(char *dest, int maxLen, const char *src);
#line 255 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
int stricmp(const char *a, const char *b);
#line 266 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
char * intToStr(int32_t v);
#line 275 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
char * floatToStr(float v, int dec);
#line 290 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
float formatFloat(float v, int dec);
#line 297 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
float strToFloat(const char *s, int dec);
#line 312 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
template<typename Item>Item& typecastValue(Item& i);
#line 320 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
float typecastValue(TimestampedFixedValue i);
#line 326 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
float typecastValue(FixedValue i);
#line 14 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendStartJsonObj(const char *objName);
#line 28 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendEndJsonObj();
#line 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(uint8_t recNr, const char *recID, uint8_t slot, float EDT1, float EDT2, float ERT1, float ERT2, float GDT);
#line 55 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, const char *cValue, const char *cUnit);
#line 76 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, const char *cValue);
#line 86 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, String sValue, const char *cUnit);
#line 112 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, String sValue);
#line 129 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, int32_t iValue, const char *cUnit);
#line 150 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, int32_t iValue);
#line 160 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, uint32_t uValue, const char *cUnit);
#line 181 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, uint32_t uValue);
#line 191 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, float fValue, const char *cUnit);
#line 212 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonObj(const char *cName, float fValue);
#line 224 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonV0Obj(const char *cName, uint32_t uValue);
#line 237 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonV0Obj(const char *cName, float fValue);
#line 250 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonV0Obj(const char *cName, int32_t iValue);
#line 263 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendNestedJsonV0Obj(const char *cName, String sValue);
#line 280 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue);
#line 294 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces);
#line 324 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue);
#line 338 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen);
#line 355 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue, const char *cUnit);
#line 371 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, const char *cValue);
#line 381 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue, const char *cUnit);
#line 409 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, String sValue);
#line 419 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue, const char *cUnit);
#line 435 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, int32_t iValue);
#line 445 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue, const char *cUnit);
#line 461 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, uint32_t uValue);
#line 471 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue, const char *cUnit);
#line 487 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
void createMQTTjsonMessage(char *mqttBuff, const char *cName, float fValue);
#line 38 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
void displayBoardInfo();
#line 151 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
void handleKeyInput();
#line 12 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\processTelegram.ino"
void processTelegram();
#line 39 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void processAPI();
#line 120 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void handleDevApi(const char *URI, const char *word4, const char *word5, const char *word6);
#line 179 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void handleHistApi(const char *URI, const char *word4, const char *word5, const char *word6);
#line 239 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void handleSmApi(const char *URI, const char *word4, const char *word5, const char *word6);
#line 321 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendDeviceInfo();
#line 436 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendDeviceTime();
#line 449 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendDeviceSettings();
#line 485 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendDeviceDebug(const char *URI, String tail);
#line 546 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendJsonV0Fields();
#line 602 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendJsonFields(const char *Name);
#line 612 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendJsonHist(int8_t fileType, const char *fileName, const char *timeStamp, bool desc);
#line 658 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
bool isInFieldsArray(const char* lookUp, int elemts);
#line 672 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void copyToFieldsArray(const char inArray[][35], int elemts);
#line 689 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
bool listFieldsArray(char inArray[][35]);
#line 702 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
void sendApiNotFound(const char *URI);
#line 14 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
void writeSettings();
#line 116 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
void readSettings(bool show);
#line 292 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
void updateSetting(const char *field, const char *newValue);
#line 15 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
String buildDateTimeString(const char* timeStamp, int len);
#line 31 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
void epochToTimestamp(time_t t, char *ts, int8_t len);
#line 46 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t SecondFromTimestamp(const char *timeStamp);
#line 57 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t MinuteFromTimestamp(const char *timeStamp);
#line 68 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t HourFromTimestamp(const char *timeStamp);
#line 79 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t DayFromTimestamp(const char *timeStamp);
#line 90 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t MonthFromTimestamp(const char *timeStamp);
#line 101 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int8_t YearFromTimestamp(const char *timeStamp);
#line 112 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
int32_t HoursKeyTimestamp(const char *timeStamp);
#line 126 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
time_t epoch(const char *timeStamp, int8_t len, bool syncTime);
#line 72 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\DSMRloggerAPI_LittleFS.ino"
void displayStatus()
{
  if (settingOledType > 0)
  {
    switch (msgMode) {
      case 1:   snprintf(cMsg, sizeof(cMsg), "Up:%-15.15s", upTime().c_str());
        break;
      case 2:   snprintf(cMsg, sizeof(cMsg), "WiFi RSSI:%4d dBm", WiFi.RSSI());
        break;
      case 3:   snprintf(cMsg, sizeof(cMsg), "Heap:%7d Bytes", ESP.getFreeHeap());
        break;
      case 4:   if (WiFi.status() != WL_CONNECTED)
          snprintf(cMsg, sizeof(cMsg), "**** NO  WIFI ****");
        else  snprintf(cMsg, sizeof(cMsg), "IP %s", WiFi.localIP().toString().c_str());
        break;
      default:  snprintf(cMsg, sizeof(cMsg), "Telgrms:%6d/%3d", telegramCount, telegramErrors);
        break;
    }

    oled_Print_Msg(3, cMsg, 0);
    msgMode = (msgMode + 1) % 5; //modular 5 = number of message displayed (hence it cycles thru the messages
  }
} // displayStatus()


#ifdef USE_SYSLOGGER
//===========================================================================================
void openSysLog(bool empty)
{
  if (sysLog.begin(500, 100, empty))  // 500 lines use existing sysLog file
  {
    DebugTln("Succes opening sysLog!");
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(3, "Syslog OK!", 500);
    }
  }
  else
  {
    DebugTln("Error opening sysLog!");
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(3, "Error Syslog", 1500);
    }
  }

  sysLog.setDebugLvl(1);
  sysLog.setOutput(&TelnetStream);
  sysLog.status();
  sysLog.write("\r\n");
  for (int q = 0; q < 3; q++)
  {
    sysLog.write("******************************************************************************************************");
  }
  writeToSysLog("Last Reset Reason [%s]", ESP.getResetReason().c_str());
  writeToSysLog("actTimestamp[%s], nrReboots[%u], Errors[%u]", actTimestamp
                , nrReboots
                , slotErrors);

  sysLog.write(" ");

} // openSysLog()
#endif

//===========================================================================================
void setup()
{
#ifdef USE_PRE40_PROTOCOL                                                         //PRE40
  //Serial.begin(115200);                                                           //DEBUG
  Serial.begin(9600, SERIAL_7E1);                                                 //PRE40
#else   // not use_dsmr_30                                                        //PRE40
  Serial.begin(115200, SERIAL_8N1);
#endif  // use_dsmr_30
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_BUTTON, INPUT);
#ifdef DTR_ENABLE
  pinMode(DTR_ENABLE, OUTPUT);
#endif

  //--- setup randomseed the right way
  //--- This is 8266 HWRNG used to seed the Random PRNG
  //--- Read more: https://config9.com/arduino/getting-a-truly-random-number-in-arduino/
  randomSeed(RANDOM_REG32);
  snprintf(settingHostname, sizeof(settingHostname), "%s", _DEFAULT_HOSTNAME);
  Serial.printf("\n\nBooting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());

  if (settingOledType > 0)
  {
    oled_Init();
    oled_Clear();  // clear the screen so we can paint the menu.
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    int8_t sPos = String(_FW_VERSION).indexOf(' ');
    snprintf(cMsg, sizeof(cMsg), "(c)2020 [%s]", String(_FW_VERSION).substring(0, sPos).c_str());
    oled_Print_Msg(1, cMsg, 0);
    #ifdef USE_LITTLEFS
      oled_Print_Msg(2, " LittleFS gebruikt!", 0);
    #endif
    oled_Print_Msg(3, " >> Have fun!! <<", 1000);
    yield();
  }
  else  // don't blink if oled-screen attatched
  {
    for (int I = 0; I < 8; I++)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(500);
    }
  }
  digitalWrite(LED_BUILTIN, LED_OFF);  // HIGH is OFF
  lastReset     = ESP.getResetReason();

  startTelnet();
  if (settingOledType > 0)
  {
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(3, "telnet (poort 23)", 2500);
  }

  //================ SPIFFS ===========================================
  if (SPIFFS.begin())
  {
    DebugTln(F("SPIFFS Mount succesfull\r"));
    SPIFFSmounted = true;
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(3, "SPIFFS mounted", 1500);
    }
  } else {
    DebugTln(F("SPIFFS Mount failed\r"));   // Serious problem with SPIFFS
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
  DebugTf("===> actTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n", actTimestamp
          , nrReboots++
          , slotErrors);
  readSettings(true);
  oled_Init();

  //=============start Networkstuff==================================
  if (settingOledType > 0)
  {
    if (settingOledFlip)  oled_Init();  // only if true restart(init) oled screen
    oled_Clear();                       // clear the screen
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(1, "Verbinden met WiFi", 500);
  }
  digitalWrite(LED_BUILTIN, LED_ON);
  startWiFi(settingHostname, 240);  // timeout 4 minuten

  if (settingOledType > 0)
  {
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(1, WiFi.SSID(), 0);
    snprintf(cMsg, sizeof(cMsg), "IP %s", WiFi.localIP().toString().c_str());
    oled_Print_Msg(2, cMsg, 1500);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);

  Debugln();
  Debug (F("Connected to " )); Debugln (WiFi.SSID());
  Debug (F("IP address: " ));  Debugln (WiFi.localIP());
  Debug (F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

  for (int L = 0; L < 10; L++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);

  //-----------------------------------------------------------------
#ifdef USE_SYSLOGGER
  openSysLog(false);
  snprintf(cMsg, sizeof(cMsg), "SSID:[%s],  IP:[%s], Gateway:[%s]", WiFi.SSID().c_str()
           , WiFi.localIP().toString().c_str()
           , WiFi.gatewayIP().toString().c_str());
  writeToSysLog("%s", cMsg);

#endif

  startMDNS(settingHostname);
  if (settingOledType > 0)
  {
    oled_Print_Msg(3, "mDNS gestart", 1500);
  }

  //=============end Networkstuff======================================

#if defined(USE_NTP_TIME)                                   //USE_NTP
  //================ startNTP =========================================
  if (settingOledType > 0)                                  //USE_NTP
  { //USE_NTP
    oled_Print_Msg(3, "setup NTP server", 100);             //USE_NTP
  }                                                         //USE_NTP
  //USE_NTP
  if (!startNTP())                                          //USE_NTP
  { //USE_NTP
    DebugTln(F("ERROR!!! No NTP server reached!\r\n\r"));   //USE_NTP
    if (settingOledType > 0)                                //USE_NTP
    { //USE_NTP
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);              //USE_NTP
      oled_Print_Msg(2, "geen reactie van", 100);           //USE_NTP
      oled_Print_Msg(2, "NTP server's", 100);               //USE_NTP
      oled_Print_Msg(3, "Reboot DSMR-logger", 2000);        //USE_NTP
    }                                                       //USE_NTP
    delay(2000);                                            //USE_NTP
    ESP.restart();                                          //USE_NTP
    delay(3000);                                            //USE_NTP
  }                                                         //USE_NTP
  if (settingOledType > 0)                                  //USE_NTP
  { //USE_NTP
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);                //USE_NTP
    oled_Print_Msg(3, "NTP gestart", 1500);                 //USE_NTP
  }                                                         //USE_NTP
  prevNtpHour = hour();                                     //USE_NTP
  //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP
  //================ end NTP =========================================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

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
      hasAlternativeIndex        = true;
    }
    else  hasAlternativeIndex    = false;
  }
  if (!hasAlternativeIndex && !DSMRfileExist("/DSMRindex.html", false) )
  {
    spiffsNotPopulated = true;
  }
  if (!hasAlternativeIndex)    //--- there's no alternative index.html
  {
    DSMRfileExist("/DSMRindex.js",    false);
    DSMRfileExist("/DSMRindex.css",   false);
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
#ifdef USE_SYSLOGGER
  if (spiffsNotPopulated)
  {
    sysLog.write("SPIFFS is not correct populated (files are missing)");
  }
#endif

  //=============now test if "convertPRD" file exists================

  if (SPIFFS.exists("/!PRDconvert") )
  {
    convertPRD2RING();
  }

  //=================================================================

#if defined(USE_NTP_TIME)                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d%02d%02dW\0\0"                      //USE_NTP
           , (year(t) - 2000), month(t), day(t) //USE_NTP
           , hour(t), minute(t), second(t));    //USE_NTP
  pTimestamp = cMsg;                                                                //USE_NTP
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
#endif  // use_dsmr_30

  if (settingOledType > 0)
  {
    snprintf(cMsg, sizeof(cMsg), "DT: %02d%02d%02d%02d0101W", thisYear
             , thisMonth, thisDay, thisHour);
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
    oled_Print_Msg(3, cMsg, 1500);
  }

  //================ Start MQTT  ======================================

#ifdef USE_MQTT                                                 //USE_MQTT
  connectMQTT();                                                //USE_MQTT
  if (settingOledType > 0)                                      //USE_MQTT
  { //USE_MQTT
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);                    //USE_MQTT
    oled_Print_Msg(3, "MQTT server set!", 1500);                //USE_MQTT
  }                                                             //USE_MQTT
#endif                                                          //USE_MQTT

  //================ End of Start MQTT  ===============================


  //================ Start HTTP Server ================================

  if (!spiffsNotPopulated) {
    DebugTln(F("SPIFFS correct populated -> normal operation!\r"));
    if (settingOledType > 0)
    {
      oled_Print_Msg(0, " <DSMRloggerAPI>", 0);
      oled_Print_Msg(1, "OK, SPIFFS correct", 0);
      oled_Print_Msg(2, "Verder met normale", 0);
      oled_Print_Msg(3, "Verwerking ;-)", 2500);
    }
    if (hasAlternativeIndex)
    {
      httpServer.serveStatic("/",                 SPIFFS, settingIndexPage);
      httpServer.serveStatic("/index",            SPIFFS, settingIndexPage);
      httpServer.serveStatic("/index.html",       SPIFFS, settingIndexPage);
      httpServer.serveStatic("/DSMRindex.html",   SPIFFS, settingIndexPage);
    }
    else
    {
      httpServer.serveStatic("/",                 SPIFFS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.html",   SPIFFS, "/DSMRindex.html");
      httpServer.serveStatic("/index",            SPIFFS, "/DSMRindex.html");
      httpServer.serveStatic("/index.html",       SPIFFS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.css",    SPIFFS, "/DSMRindex.css");
      httpServer.serveStatic("/DSMRindex.js",     SPIFFS, "/DSMRindex.js");
      httpServer.serveStatic("/DSMRgraphics.js",  SPIFFS, "/DSMRgraphics.js");
    }
  } else {
    DebugTln(F("Oeps! not all files found on SPIFFS -> present FSexplorer!\r"));
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
  httpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");

  httpServer.on("/api", HTTP_GET, processAPI);
  // all other api calls are catched in FSexplorer onNotFounD!

  httpServer.begin();
  DebugTln( "HTTP server gestart\r" );
  if (settingOledType > 0)                                  //HAS_OLED
  { //HAS_OLED
    oled_Clear();                                           //HAS_OLED
    oled_Print_Msg(0, " <DSMRloggerAPI>", 0);                //HAS_OLED
    oled_Print_Msg(2, "HTTP server ..", 0);                 //HAS_OLED
    oled_Print_Msg(3, "gestart (poort 80)", 0);             //HAS_OLED
  }                                                         //HAS_OLED

  for (int i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }
  //================ Start HTTP Server ================================

  //test(); monthTabel

#ifdef USE_MINDERGAS
  handleMindergas();
#endif

  DebugTf("Startup complete! actTimestamp[%s]\r\n", actTimestamp);
  writeToSysLog("Startup complete! actTimestamp[%s]", actTimestamp);

  //================ End of Slimmer Meter ============================


  //================ The final part of the Setup =====================

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

  if (settingOledType > 0)
  {
    oled_Print_Msg(0, "<DSMRloggerAPI>", 0);
    oled_Print_Msg(1, "Startup complete", 0);
    oled_Print_Msg(2, "Wait for first", 0);
    oled_Print_Msg(3, "telegram .....", 500);
  }

  //================ Start Slimme Meter ===============================

  DebugTln(F("Enable slimmeMeter..\r"));

#if defined( USE_REQUEST_PIN ) && !defined( HAS_NO_SLIMMEMETER )
  DebugTf("Swapping serial port to Smart Meter, debug output will continue on telnet\r\n");
  DebugFlush();
  Serial.swap();
#endif // is_esp12

  delay(100);
  slimmeMeter.enable(true);

} // setup()


//===[ no-blocking delay with running background tasks in ms ]============================
DECLARE_TIMER_MS(timer_delay_ms, 1);
void delayms(unsigned long delay_ms)
{
  CHANGE_INTERVAL_MS(timer_delay_ms, delay_ms);
  RESTART_TIMER(timer_delay_ms);
  while (!DUE(timer_delay_ms))
  {
    doSystemTasks();
  }

} // delayms()

//========================================================================================

//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (Verbose1) DebugTln("doTaskTelegram");
#if defined(HAS_NO_SLIMMEMETER)
  handleTestdata();
#else
  //-- enable DTR to read a telegram from the Slimme Meter
  slimmeMeter.enable(true);
  slimmeMeter.loop();
  handleSlimmemeter();
#endif
  if (WiFi.status() != WL_CONNECTED)
  {
    for (int b = 0; b < 10; b++) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(75);
    }
  }
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
#ifndef HAS_NO_SLIMMEMETER
  slimmeMeter.loop();
#endif
#ifdef USE_MQTT
  MQTTclient.loop();
#endif
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
  if DUE(nextTelegram)
  {
    doTaskTelegram();
  }

  //--- update upTime counter
  if DUE(updateSeconds)
  {
    upTimeSeconds++;
  }

  //--- if an OLED screen attached, display the status
  if (settingOledType > 0)
  {
    if DUE(updateDisplay)
    {
      displayStatus();
    }
  }

  //--- if mindergas then check
#ifdef USE_MINDERGAS
  if ( DUE(minderGasTimer) )
  {
    handleMindergas();
  }
#endif

  //--- if connection lost, try to reconnect to WiFi
  if ( DUE(reconnectWiFi) && (WiFi.status() != WL_CONNECTED) )
  {
    writeToSysLog("Restart wifi with [%s]...", settingHostname);
    startWiFi(settingHostname, 10);
    if (WiFi.status() != WL_CONNECTED)
      writeToSysLog("%s", "Wifi still not connected!");
    else {
      snprintf(cMsg, sizeof(cMsg), "IP:[%s], Gateway:[%s]", WiFi.localIP().toString().c_str()
               , WiFi.gatewayIP().toString().c_str());
      writeToSysLog("%s", cMsg);
    }
  }

  //--- if NTP set, see if it needs synchronizing
#if defined(USE_NTP_TIME)                                           //USE_NTP
  if DUE(synchrNTP)                                                 //USE_NTP
  {
    //if (timeStatus() == timeNeedsSync || prevNtpHour != hour())     //USE_NTP
    //{
    //prevNtpHour = hour();                                         //USE_NTP
    setSyncProvider(getNtpTime);                                  //USE_NTP
    setSyncInterval(600);                                         //USE_NTP
    //}
  }
#endif                                                              //USE_NTP

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\FSexplorer.ino"
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
  SPIFFS.begin();

  if (SPIFFS.exists("/FSexplorer.html"))
  {
    httpServer.serveStatic("/FSexplorer.html", SPIFFS, "/FSexplorer.html");
    httpServer.serveStatic("/FSexplorer", SPIFFS, "/FSexplorer.html");
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
      DebugTf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());
    if (httpServer.uri().indexOf("/api/") == 0)
    {
      if (Verbose1)
        DebugTf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      processAPI();
    }
    else
    {
      DebugTf("next: handleFile(%s)\r\n", String(httpServer.urlDecode(httpServer.uri())).c_str());
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

  Dir dir = SPIFFS.openDir("/"); // List files on SPIFFS
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
  }   /* end for */

  for (int8_t x = 0; x < fileNr; x++)
  {
    DebugTln(dirMap[x].Name);
  }

  String temp = "[";
  for (int f = 0; f < fileNr; f++)
  {
    if (temp != "[")
      temp += ",";
    temp += R"({"name":")" + String(dirMap[f].Name) + R"(","size":")" + formatBytes(dirMap[f].Size) + R"("})";
  }
  SPIFFS.info(SPIFFSinfo);
  temp += R"(,{"usedBytes":")" + formatBytes(SPIFFSinfo.usedBytes * 1.05) + R"(",)" +       // Berechnet den verwendeten Speicherplatz + 5% Sicherheitsaufschlag
          R"("totalBytes":")" + formatBytes(SPIFFSinfo.totalBytes) + R"(","freeBytes":")" + // Zeigt die Größe des Speichers
          (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + R"("}])";               // Berechnet den freien Speicherplatz + 5% Sicherheitsaufschlag
  httpServer.send(200, "application/json", temp);

} // APIlistFiles()

//=====================================================================================
bool handleFile(String &&path)
{
  if (httpServer.hasArg("delete"))
  {
    DebugTf("Delete -> [%s]\n\r", httpServer.arg("delete").c_str());
    SPIFFS.remove(httpServer.arg("delete")); // Datei löschen
    httpServer.sendContent(Header);
    return true;
  }
  if (!SPIFFS.exists("/FSexplorer.html"))
    httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  if (path.endsWith("/"))
    path += "index.html";
  return SPIFFS.exists(path) ? ({File f = SPIFFS.open(path, "r"); httpServer.streamFile(f, contentType(path)); f.close(); true; }) : false;

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
    Debugln("FileUpload Name: " + upload.filename);
    fsUploadFile = SPIFFS.open("/" + httpServer.urlDecode(upload.filename), "w");
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    Debugln("FileUpload Data: " + (String)upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    Debugln("FileUpload Size: " + (String)upload.totalSize);
    httpServer.sendContent(Header);
  }

} // handleFileUpload()

//=====================================================================================
void formatSpiffs()
{ //Formatiert den Speicher
  if (!SPIFFS.exists("/!format"))
    return;
  DebugTln(F("Format SPIFFS"));
  SPIFFS.format();
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
  SPIFFS.info(SPIFFSinfo);
  Debugln(formatBytes(SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05)) + " im Spiffs frei");
  return (SPIFFSinfo.totalBytes - (SPIFFSinfo.usedBytes * 1.05) > printsize) ? true : false;

} // freeSpace()

//=====================================================================================
void updateFirmware()
{
#ifdef USE_UPDATE_SERVER
  DebugTln(F("Redirect to updateIndex .."));
  doRedirect("wait ... ", 1, "/updateIndex", false);
#else
  doRedirect("UpdateServer not available", 10, "/", false);
#endif

} // updateFirmware()

//=====================================================================================
void reBootESP()
{
  DebugTln(F("Redirect and ReBoot .."));
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

  DebugTln(msg);
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot)
  {
    delay(5000);
    ESP.restart();
    delay(5000);
  }

} // doRedirect()

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MQTTstuff.ino"
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

  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerIPchar[20];

#ifdef USE_MQTT
//  https://github.com/knolleary/pubsubclient
//  #include <PubSubClient.h>           // MQTT client publish and subscribe functionality
  
//  static PubSubClient MQTTclient(wifiClient);
  int8_t              reconnectAttempts = 0;
  char                lastMQTTtimestamp[15] = "-";
  char                mqttBuff[100];


  enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };
  enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

  String            MQTTclientId;

#endif

//===========================================================================================
void connectMQTT() 
{
#ifdef USE_MQTT
  
  if (Verbose2) DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);

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
  
  if (Verbose1) DebugTf("connected()[%d], mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);

  CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 5);

#endif
}


//===========================================================================================
bool connectMQTT_FSM() 
{
#ifdef USE_MQTT
  
  switch(stateMQTT) 
  {
    case MQTT_STATE_INIT:  
          DebugTln(F("MQTT State: MQTT Initializing"));
          WiFi.hostByName(settingMQTTbroker, MQTTbrokerIP);  // lookup the MQTTbroker convert to IP
          snprintf(MQTTbrokerIPchar, sizeof(MQTTbrokerIPchar), "%d.%d.%d.%d", MQTTbrokerIP[0]
                                                                            , MQTTbrokerIP[1]
                                                                            , MQTTbrokerIP[2]
                                                                            , MQTTbrokerIP[3]);
          if (!isValidIP(MQTTbrokerIP))  
          {
            DebugTf("ERROR: [%s] => is not a valid URL\r\n", settingMQTTbroker);
            settingMQTTinterval = 0;
            DebugTln(F("Next State: MQTT_STATE_ERROR"));
            stateMQTT = MQTT_STATE_ERROR;
            return false;
          }
          
          //MQTTclient.disconnect();
          //DebugTf("disconnect -> MQTT status, rc=%d \r\n", MQTTclient.state());
          DebugTf("[%s] => setServer(%s, %d) \r\n", settingMQTTbroker, MQTTbrokerIPchar, settingMQTTbrokerPort);
          MQTTclient.setServer(MQTTbrokerIPchar, settingMQTTbrokerPort);
          DebugTf("setServer  -> MQTT status, rc=%d \r\n", MQTTclient.state());
          MQTTclientId  = String(settingHostname) + "-" + WiFi.macAddress();
          stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
          DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
          reconnectAttempts = 0;

    case MQTT_STATE_TRY_TO_CONNECT:
          DebugTln(F("MQTT State: MQTT try to connect"));
          DebugTf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar);
      
          DebugTf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str());
          reconnectAttempts++;

          //--- If no username, then anonymous connection to broker, otherwise assume username/password.
          if (String(settingMQTTuser).length() == 0) 
          {
            DebugT(F("without a Username/Password "));
            MQTTclient.connect(MQTTclientId.c_str());
          } 
          else 
          {
            DebugTf("with Username [%s] and password ", settingMQTTuser);
            MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd);
          }
          //--- If connection was made succesful, move on to next state...
          if (MQTTclient.connected())
          {
            reconnectAttempts = 0;  
            Debugf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state());
            MQTTclient.loop();
            stateMQTT = MQTT_STATE_IS_CONNECTED;
            return true;
          }
          Debugf(" -> MQTT status, rc=%d \r\n", MQTTclient.state());
      
          //--- After 3 attempts... go wait for a while.
          if (reconnectAttempts >= 3)
          {
            DebugTln(F("3 attempts have failed. Retry wait for next reconnect in 10 minutes\r"));
            stateMQTT = MQTT_STATE_ERROR;  // if the re-connect did not work, then return to wait for reconnect
            DebugTln(F("Next State: MQTT_STATE_ERROR"));
          }   
          break;
          
    case MQTT_STATE_IS_CONNECTED:
          MQTTclient.loop();
          return true;

    case MQTT_STATE_ERROR:
          DebugTln(F("MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"));
          //--- next retry in 10 minutes.
          CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
          break;

    default:
          DebugTln(F("MQTT State: default, this should NEVER happen!"));
          //--- do nothing, this state should not happen
          stateMQTT = MQTT_STATE_INIT;
          CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
          DebugTln(F("Next State: MQTT_STATE_INIT"));
          break;
  }
#endif

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
#ifdef USE_MQTT

    char topicId[100];

    template<typename Item>
    void apply(Item &i) {
      if (i.present()) 
      {
        String Name = Item::name;
        //-- for dsmr30 -----------------------------------------------
  #if defined( USE_PRE40_PROTOCOL )
        if (Name.indexOf("gas_delivered2") == 0) Name = "gas_delivered";
  #endif
        String Unit = Item::unit();

        if (settingMQTTtopTopic[strlen(settingMQTTtopTopic)-1] == '/')
              snprintf(topicId, sizeof(topicId), "%s",  settingMQTTtopTopic);
        else  snprintf(topicId, sizeof(topicId), "%s/", settingMQTTtopTopic);
        strConcat(topicId, sizeof(topicId), Name.c_str());
        if (Verbose2) DebugTf("topicId[%s]\r\n", topicId);
        
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
          DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", topicId, mqttBuff, (strlen(topicId) + strlen(mqttBuff)));
        }
      }
  }
#endif

};  // struct buildJsonMQTT


//===========================================================================================
void sendMQTTData() 
{
#ifdef USE_MQTT
  String dateTime, topicId, json;

  if (settingMQTTinterval == 0) return;

  if (ESP.getFreeHeap() < 9000) // to prevent firmware from crashing!
  {
    DebugTf("==> Bailout due to low heap (%d bytes)\r\n",   ESP.getFreeHeap() );
    writeToSysLog("==> Bailout low heap (%d bytes)", ESP.getFreeHeap() );
    return;
  }


  if (!MQTTclient.connected() || ! mqttIsConnected)
  {
    DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
                                              , MQTTclient.connected()
                                              , mqttIsConnected, stateMQTT);
  }
  if (!MQTTclient.connected())  
  {
    if ( DUE( reconnectMQTTtimer) || mqttIsConnected)
    {
      mqttIsConnected = false;
      connectMQTT();
    }
    else
    {
      DebugTf("trying to reconnect in less than %d minutes\r\n", (TIME_LEFT_MIN(reconnectMQTTtimer) +1) );
    }
    if ( !mqttIsConnected ) 
    {
      DebugTln("no connection with a MQTT broker ..");
      return;
    }
  }

  DebugTf("Sending data to MQTT server [%s]:[%d]\r\n", settingMQTTbroker, settingMQTTbrokerPort);
  
  DSMRdata.applyEach(buildJsonMQTT());

#endif

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\MinderGas.ino"
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
#define MG_FILENAME         "/Mindergas.post"

//=======================================================================
void handleMindergas()
{
  #ifdef USE_MINDERGAS
    processMindergas_FSM();
  #endif

} // handleMindergas()


#ifdef USE_MINDERGAS

enum states_of_MG { MG_INIT, MG_WAIT_FOR_FIRST_TELEGRAM, MG_WAIT_FOR_NEXT_DAY
                           , MG_WRITE_TO_FILE, MG_DO_COUNTDOWN
                           , MG_SEND_MINDERGAS, MG_NO_AUTHTOKEN, MG_ERROR };
                           
enum  states_of_MG stateMindergas   = MG_INIT;
bool  sendMindergasPostFile();
void  writePostToFile();

int8_t    MG_Day                    = -1;
bool      validToken                = false;
bool      handleMindergasSemaphore  = false;
int8_t    MGminuten                 = 0;
int8_t    retryCounter = 0;
bool      bDoneResponse             = false;

//=======================================================================
//force mindergas update, by skipping states
void forceMindergasUpdate()
{
  snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());

  validToken = true;

  if (SPIFFS.exists(MG_FILENAME))
  {
    writeToSysLog("found [%s] at day#[%d]", MG_FILENAME, day());
    MG_Day = day();   // make it thisDay...
    strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "force Mindergas countdown");
    DebugTln(F("Force send data to mindergas.nl in ~1 minute"));
    MGminuten=1;
    CHANGE_INTERVAL_MIN(minderGasTimer, 1);
    stateMindergas = MG_DO_COUNTDOWN;
    processMindergas_FSM();
  }
  else
  {
    strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Force Write Mindergas.post");
    DebugTln(F("Force Write data to post file now!"));
    writeToSysLog("Force Write Data to [%s]", MG_FILENAME);
    CHANGE_INTERVAL_MIN(minderGasTimer, 1);
    stateMindergas = MG_WRITE_TO_FILE;  // write file is next state
    processMindergas_FSM();
  }
  
} // forceMindergasUpdate()


//=======================================================================
// handle finite state machine of mindergas
void processMindergas_FSM()
{
  if (handleMindergasSemaphore) // if already running ? then return...
  {
    DebugTln(F("already running .. bailing out!"));
    writeToSysLog("already running .. bailing out!");
    return; //--- you may only enter this once
  } 
  //signal that we are busy...
  handleMindergasSemaphore = true;
  
  yield(); 
  
  switch(stateMindergas) 
  {
    case MG_INIT: //--- only after reboot
          DebugTln(F("Mindergas State: MG_INIT"));
          writeToSysLog("Mindergas State: MG_INIT");
          snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day() , hour(), minute());
          if (intStatuscodeMindergas == 0)
          {
            strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "INITIAL STATE");
          }
          if (SPIFFS.exists(MG_FILENAME))
          {
            strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "found Mindergas.post");
            writeToSysLog(txtResponseMindergas);
            validToken     = true;
            stateMindergas = MG_SEND_MINDERGAS;
            CHANGE_INTERVAL_MIN(minderGasTimer, 1);
            break;
          } 
          //--- check to see if there is a authtoken
          //--- Assume there is a valid token, if there is a string. To be proven later.
          validToken = (strlen(settingMindergasToken) > 5); 
          if  (validToken) 
          {
            CHANGE_INTERVAL_MIN(minderGasTimer, 1);
            //--- Next state is wait for first telegram
            stateMindergas = MG_WAIT_FOR_FIRST_TELEGRAM; 
          }
          else
          {
            //--- No AuthToken
            DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
            writeToSysLog("MinderGas Authtoken is not set, no update can be done.");
            strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "NO_AUTHTOKEN");
            stateMindergas = MG_NO_AUTHTOKEN; // no token, no mindergas
          } // end-if 
          break; 
      
    case MG_WAIT_FOR_FIRST_TELEGRAM: 
          DebugTln(F("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM"));
          writeToSysLog("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM");
          //--- if you received at least one telegram, then wait for midnight
          if ((telegramCount - telegramErrors) > 1) 
          {
            //--- Now you know what day it is, do setup MG_Day. This to enable day change detection.
            MG_Day = day(); 
            stateMindergas = MG_WAIT_FOR_NEXT_DAY;
          }
          break; 
      
    case MG_WAIT_FOR_NEXT_DAY: 
          DebugTln(F("Mindergas State: MG_WAIT_FOR_NEXT_DAY"));
          writeToSysLog("Mindergas State: MG_WAIT_FOR_NEXT_DAY");
          if (intStatuscodeMindergas == 0)
          {
            snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());
            strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "WAIT_FOR_NEXT_DAY");
          }
          CHANGE_INTERVAL_MIN(minderGasTimer, 30);
          //--- Detect day change at midnight, then...
          if (MG_Day != day())                  // It is no longer the same day, so it must be past midnight
          {
            MG_Day = day();                     // make it thisDay...
            writeToSysLog("a new day has become .. next: Write to file");
            snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());
            strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "WRITE_TO_FILE");
            CHANGE_INTERVAL_MIN(minderGasTimer, 1);
            stateMindergas = MG_WRITE_TO_FILE;  // write file is next state
          }
          break; 
      
    case MG_WRITE_TO_FILE: 
          DebugTln(F("Mindergas State: MG_WRITE_TO_FILE"));
          writeToSysLog("Mindergas State: MG_WRITE_TO_FILE");
          writePostToFile();
          CHANGE_INTERVAL_MIN(minderGasTimer, 1);
          break; 
      
    case MG_DO_COUNTDOWN: 
          DebugTf("Mindergas State: MG_DO_COUNTDOWN (%d minuten te gaan)\r\n", MGminuten);
          writeToSysLog("Mindergas State: MG_DO_COUNTDOWN (%d minuten te gaan)", MGminuten);
          snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());
          snprintf(txtResponseMindergas, sizeof(txtResponseMindergas), "Send DATA in %d minutes", MGminuten );
          DebugTf("MinderGas update in less than [%d] minutes\r\n", MGminuten );
          MGminuten--; 
          intStatuscodeMindergas = MGminuten;

          if (MGminuten <= 0)
          {
            //--- when waitime is done, it's time to send the POST string
            intStatuscodeMindergas = 0;
            stateMindergas = MG_SEND_MINDERGAS;
          }
          break; 
      
    case MG_SEND_MINDERGAS: 
          DebugTln(F("Mindergas State: MG_SEND_MINDERGAS"));
          writeToSysLog("Mindergas State: MG_SEND_MINDERGAS");
        
          snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());
          strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "SEND_MINDERGAS");

          //--- if POST response for Mindergas exists, then send it... btw it should exist by now :)
          if ((validToken) && SPIFFS.exists(MG_FILENAME)) 
          {
            if (!sendMindergasPostFile())
            {
              Debugln();
              retryCounter++;
              if (retryCounter < 3)
              {
                //--- start countdown -- again
                MGminuten = random(1,10);
                DebugTf("Try[%d] MinderGas update in [%d] minute(s)\r\n", retryCounter, MGminuten);
                writeToSysLog("Try[%d] MinderGas update in [%d] minute(s)", retryCounter, MGminuten);
                //--- Lets'do the countdown
                strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "DO_COUNTDOWN");
                stateMindergas = MG_DO_COUNTDOWN;
                break;
              }
            }
            Debugln();
            //--- delete POST file from SPIFFS
            if (SPIFFS.remove(MG_FILENAME)) 
            {
              DebugTln(F("POST Mindergas file succesfully deleted!"));
              writeToSysLog("Deleted Mindergas.post !");
            } 
            else 
            {
              //--- help, this should just not happen, but if it does, it 
              //--- will not influence behaviour in a negative way
              DebugTln(F("Failed to delete POST Mindergas file"));
              writeToSysLog("Failed to delete Mindergas.post");
            } 
//          bDoneResponse = true; 
          }   
          CHANGE_INTERVAL_MIN(minderGasTimer, 30);
          break; 
      
    case MG_NO_AUTHTOKEN:
          if (Verbose2) DebugTln(F("Mindergas State: MG_NO_AUTHTOKEN"));
          if (validToken)
          {
            stateMindergas = MG_INIT;   
          }
          CHANGE_INTERVAL_MIN(minderGasTimer, 60);
          break; 
      
    case MG_ERROR: 
          DebugTln(F("Mindergas State: MG_ERROR"));
          writeToSysLog("Mindergas State: MG_ERROR");
          CHANGE_INTERVAL_MIN(minderGasTimer, 30); 
          break;
      
    default: 
          DebugTln(F("Mindergas State: Impossible, default state!")); 
          writeToSysLog("Mindergas State: Impossible, default state!"); 
          CHANGE_INTERVAL_MIN(minderGasTimer, 10);
          stateMindergas = MG_INIT; 
          break;  
          
  } // switch(..)

  //on exit, allow next handle state event
  handleMindergasSemaphore = false;
  
} // processMindergas_FSM()


//=======================================================================
boolean sendMindergasPostFile()
{
  WiFiClient  MGclient;   
  const char *MGhost = "www.mindergas.nl";
  File        minderGasFile;
  boolean     bMgClientReply = false;
  
  bDoneResponse = false; 

  //--- create a string with the date and the meter value
  DebugTln(F("Reading POST from file:"));
  minderGasFile = SPIFFS.open(MG_FILENAME, "r");
  String sBuffer;
  sBuffer = "";
  while(minderGasFile.available()) 
  { 
    char ltr = minderGasFile.read();
    sBuffer += ltr;
  }
  minderGasFile.close();
  Debugln(sBuffer);

  //--- try to connect to minderGas
  DebugTf("Connecting to %s ...\r\n", MGhost);
  //--- connect over http with mindergas
  if (MGclient.connect(MGhost, 80)) 
  {
    //--- then post to mindergas...
    DebugTln(F("Send to Mindergas.nl..."));
    writeToSysLog("Send to Mindergas.nl...");
    MGclient.println(sBuffer);
    //--- read response from mindergas.nl
    snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d >> ", day(), hour(), minute());
    DebugTf("[%s] Mindergas response: ", timeLastResponse);

    MGclient.setTimeout(1000);

    while (!bDoneResponse && (MGclient.connected() || MGclient.available())) 
    {
      Debug(".");
      if (MGclient.available()) 
      {
        Debug(".");
        //--- skip to find HTTP/1.1
        //--- then parse response code
        if (MGclient.find("HTTP/1.1"))
        {
          intStatuscodeMindergas = MGclient.parseInt(); // parse status code
          //Debugln();
          writeToSysLog("Mindergas response: [%d]", intStatuscodeMindergas);
          Debugf("Statuscode: [%d]\r\n", intStatuscodeMindergas);
          switch (intStatuscodeMindergas) {
            case 201:  
                validToken = true;
                bMgClientReply = true;
                //--- report error back to see in settings page
                strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Created entry");
                Debugln(F("Succes, the gas delivered has been added to your mindergas.nl account"));
                writeToSysLog("Succes, the gas delivered has been added to your mindergas.nl account");
                DebugTln(F("Next State: MG_WAIT_FOR_NEXT_DAY"));
                stateMindergas = MG_WAIT_FOR_NEXT_DAY;               
                break;
          
            case 401:
                validToken = false;
                bMgClientReply = true;
                strCopy(settingMindergasToken, sizeof(settingMindergasToken), "Invalid token"); 
                strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unauthorized, token invalid!"); // report error back to see in settings page
                Debugln(F("Invalid Mindergas Authenication Token"));
                writeToSysLog("Invalid Mindergas Authenication Token");
                stateMindergas = MG_NO_AUTHTOKEN;
                break;
          
            case 422:
                validToken = true;
                bMgClientReply = true;
                //--- report error back to see in settings page
                strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unprocessed entity");
                Debugln(F("Unprocessed entity, goto website mindergas for more information")); 
                writeToSysLog("Unprocessed entity, goto website mindergas for more information");
                stateMindergas = MG_WAIT_FOR_NEXT_DAY; 
                break;
          
            default:
                validToken = true;
                bMgClientReply = true;
                //--- report error back to see in settings page
                strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unknown response code");
                Debugln(F("Unknown responsecode, goto mindergas for information"));
                stateMindergas = MG_WAIT_FOR_NEXT_DAY;           
                break;
          } // end switch-case             
        }  // end-if find HTTP/1.1
    
        //--- close HTTP connection
        MGclient.stop();
        DebugTln(F("Disconnected from mindergas.nl"));
        writeToSysLog("Disconnected from mindergas.nl");
        bDoneResponse = true; 
    
      } // end-if client.available() 
      else 
      {
        //--- wait for connections, just keep trying...
        delay(100); 
      } // end-else
    
    } // while ..

  } //   connect to mindergas.nl
  else
  {
    DebugTln(".. not connected (ERROR!)");
  }
  return bMgClientReply;
  
} // sendMindergasPostFile()



//=======================================================================
void writePostToFile()
{
  //--- create POST and write to file, so it will survive a reset within the countdown period
  DebugTf("Writing to [%s] ..\r\n", MG_FILENAME);
  writeToSysLog("Writing to [%s] ..", MG_FILENAME);
  File minderGasFile = SPIFFS.open(MG_FILENAME, "a"); //  create File
  if (!minderGasFile) 
  {
    //--- cannot create file, thus error
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", MG_FILENAME);
    //--- now in failure mode
    //DebugTln(F("Next State: MG_ERROR"));
    strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "ERROR CREATE FILE");
    writeToSysLog(txtResponseMindergas);
    stateMindergas = MG_ERROR;
    return;
  } 
  //--- write POST respons into file
  yield();
  DebugTln(F("Start writing POST data "));
  //--- we want to upload the gas usage of yesterday so rewind the clock for 1 day
  time_t t = now() - SECS_PER_DAY;  
  char dataString[80];
  snprintf(dataString, sizeof(dataString),"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }"
                                                          , year(t)
                                                          , month(t)
                                                          , day(t)
                                                          , DSMRdata.gas_delivered.val());
  //--- write the POST to a file...
  minderGasFile.println(F("POST /api/gas_meter_readings HTTP/1.1"));
  minderGasFile.print(F("AUTH-TOKEN:")); minderGasFile.println(settingMindergasToken);
  minderGasFile.println(F("Host: mindergas.nl"));
  minderGasFile.println(F("User-Agent: DSMRAPI"));
  minderGasFile.println(F("Content-Type: application/json"));
  minderGasFile.println(F("Accept: application/json"));
  minderGasFile.print(F("Content-Length: ")); minderGasFile.println(strlen(dataString));
  minderGasFile.println();
  minderGasFile.println(dataString);        

  minderGasFile.close();
  snprintf(timeLastResponse, sizeof(timeLastResponse), "@%02d|%02d:%02d -> ", day(), hour(), minute());
  strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Mindergas.post aangemaakt");
  writeToSysLog(txtResponseMindergas);

  //--- start countdown
  MGminuten = random(10,120);
  DebugTf("MinderGas update in [%d] minute(s)\r\n", MGminuten);
  writeToSysLog("MinderGas update in [%d] minute(s)", MGminuten);
  //--- Lets'do the countdown
  strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "DO_COUNTDOWN");
  stateMindergas = MG_DO_COUNTDOWN;
  
} // writePostToFile()

#endif

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\SPIFFSstuff.ino"
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

  File _file = SPIFFS.open("/DSMRstatus.csv", "r");
  if (!_file)
  {
    DebugTln("read(): No /DSMRstatus.csv found ..");
  }
  if (_file.available())
  {
    int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    buffer[l] = 0;
    DebugTf("read lastUpdate[%s]\r\n", buffer);
    sscanf(buffer, "%[^;]; %u; %u; %[^;]", spiffsTimestamp, &nrReboots, &slotErrors, dummy);
    DebugTf("values timestamp[%s], nrReboots[%u], slotErrors[%u], dummy[%s]\r\n", spiffsTimestamp, nrReboots, slotErrors, dummy);
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
    DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    writeToSysLog("Bailout low heap (%d bytes)", ESP.getFreeHeap());
    return;
  }
  char buffer[50] = "";
  DebugTf("writeLastStatus() => %s; %u; %u;\r\n", actTimestamp, nrReboots, slotErrors);
  writeToSysLog("writeLastStatus() => %s; %u; %u;", actTimestamp, nrReboots, slotErrors);
  File _file = SPIFFS.open("/DSMRstatus.csv", "w");
  if (!_file)
  {
    DebugTln("write(): No /DSMRstatus.csv found ..");
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
  char record[DATA_RECLEN + 1] = "";
  char key[10] = "";

  uint16_t recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
  strCopy(key, 10, actTimestamp, 0, 8);

  snprintf(record, sizeof(record), (char *)DATA_FORMAT, key, (float)DSMRdata.energy_delivered_tariff1, (float)DSMRdata.energy_delivered_tariff2, (float)DSMRdata.energy_returned_tariff1, (float)DSMRdata.energy_returned_tariff2
#ifdef USE_PRE40_PROTOCOL
           ,
           (float)DSMRdata.gas_delivered2);
#else
           ,
           (float)DSMRdata.gas_delivered);
#endif
  // DATA + \n + \0
  fillRecord(record, DATA_RECLEN);

  strcpy(recIn, record);

} // buildDataRecordFromSM()

//===========================================================================================
uint16_t buildDataRecordFromJson(char *recIn, String jsonIn)
{
  //static float GG = 1;
  char record[DATA_RECLEN + 1] = "";
  String wOut[10];
  String wPair[5];
  char uKey[15] = "";
  float uEDT1 = 0.0;
  float uEDT2 = 0.0;
  float uERT1 = 0.0;
  float uERT2 = 0.0;
  float uGDT = 0.0;
  uint16_t recSlot;

  DebugTln(jsonIn);

  jsonIn.replace("{", "");
  jsonIn.replace("}", "");
  jsonIn.replace("\"", "");
  int8_t wp = splitString(jsonIn.c_str(), ',', wOut, 9);
  for (int f = 0; f < wp; f++)
  {
    splitString(wOut[f].c_str(), ':', wPair, 4);
    if (Verbose2)
      DebugTf("[%d] -> [%s]\r\n", f, wOut[f].c_str());
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

  DebugTf("MONTHS: Write [%s] to slot[%02d] in %s\r\n", uKey, recSlot, MONTHS_FILE);
  snprintf(record, sizeof(record), (char *)DATA_FORMAT, uKey, (float)uEDT1, (float)uEDT2, (float)uERT1, (float)uERT2, (float)uGDT);

  // DATA + \n + \0
  fillRecord(record, DATA_RECLEN);

  strcpy(recIn, record);

  return recSlot;

} // buildDataRecordFromJson()

//===========================================================================================
void writeDataToFile(const char *fileName, const char *record, uint16_t slot, int8_t fileType)
{
  uint16_t offset = 0;

  if (!isNumericp(record, 8))
  {
    DebugTf("timeStamp [%-13.13s] not valid\r\n", record);
    slotErrors++;
    return;
  }

  if (!SPIFFS.exists(fileName))
  {
    switch (fileType)
    {
    case HOURS:
      createFile(fileName, _NO_HOUR_SLOTS_);
      break;
    case DAYS:
      createFile(fileName, _NO_DAY_SLOTS_);
      break;
    case MONTHS:
      createFile(fileName, _NO_MONTH_SLOTS_);
      break;
    }
  }

  File dataFile = SPIFFS.open(fileName, "r+"); // read and write ..
  if (!dataFile)
  {
    DebugTf("Error opening [%s]\r\n", fileName);
    return;
  }
  // slot goes from 0 to _NO_OF_SLOTS_
  // we need to add 1 to slot to skip header record!
  offset = ((slot + 1) * DATA_RECLEN);
  dataFile.seek(offset, SeekSet);
  int32_t bytesWritten = dataFile.print(record);
  if (bytesWritten != DATA_RECLEN)
  {
    DebugTf("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]\r\n", slot, bytesWritten, DATA_RECLEN);
    writeToSysLog("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]", slot, bytesWritten, DATA_RECLEN);
  }
  dataFile.close();

} // writeDataToFile()

//===========================================================================================
void writeDataToFiles()
{
  char record[DATA_RECLEN + 1] = "";
  uint16_t recSlot;

  buildDataRecordFromSM(record);
  DebugTf(">%s\r\n", record); // record ends in a \n

  // update HOURS
  recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    DebugTf("HOURS:  Write to slot[%02d] in %s\r\n", recSlot, HOURS_FILE);
  writeDataToFile(HOURS_FILE, record, recSlot, HOURS);
  writeToSysLog("HOURS: actTimestamp[%s], recSlot[%d]", actTimestamp, recSlot);

  // update DAYS
  recSlot = timestampToDaySlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    DebugTf("DAYS:   Write to slot[%02d] in %s\r\n", recSlot, DAYS_FILE);
  writeDataToFile(DAYS_FILE, record, recSlot, DAYS);

  // update MONTHS
  recSlot = timestampToMonthSlot(actTimestamp, strlen(actTimestamp));
  if (Verbose1)
    DebugTf("MONTHS: Write to slot[%02d] in %s\r\n", recSlot, MONTHS_FILE);
  writeDataToFile(MONTHS_FILE, record, recSlot, MONTHS);

} // writeDataToFiles(fileType, dataStruct newDat, int8_t slotNr)

//===========================================================================================
void readOneSlot(int8_t fileType, const char *fileName, uint8_t recNr, uint8_t readSlot, bool doJson, const char *rName)
{
  uint16_t slot, maxSlots = 0, offset;
  char buffer[DATA_RECLEN + 2] = "";
  char recID[10] = "";
  float EDT1, EDT2, ERT1, ERT2, GDT;

  switch (fileType)
  {
  case HOURS:
    maxSlots = _NO_HOUR_SLOTS_;
    break;
  case DAYS:
    maxSlots = _NO_DAY_SLOTS_;
    break;
  case MONTHS:
    maxSlots = _NO_MONTH_SLOTS_;
    break;
  }

  if (!SPIFFS.exists(fileName))
  {
    DebugTf("File [%s] does not excist!\r\n", fileName);
    return;
  }

  File dataFile = SPIFFS.open(fileName, "r+"); // read and write ..
  if (!dataFile)
  {
    DebugTf("Error opening [%s]\r\n", fileName);
    return;
  }

  slot = (readSlot % maxSlots);
  // slot goes from 0 to _NO_OF_SLOTS_
  // we need to add 1 to slot to skip header record!
  offset = ((slot + 1) * DATA_RECLEN);
  dataFile.seek(offset, SeekSet);
  int l = dataFile.readBytesUntil('\n', buffer, sizeof(buffer));
  buffer[l] = 0;
  if (l >= (DATA_RECLEN - 1)) // '\n' is skipped by readBytesUntil()
  {
    if (!isNumericp(buffer, 8)) // first 8 bytes is YYMMDDHH
    {
      {
        Debugf("slot[%02d]==>timeStamp [%-13.13s] not valid!!\r\n", slot, buffer);
        writeToSysLog("slot[%02d]==>timeStamp [%-13.13s] not valid!!", slot, buffer);
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
        Debugf("slot[%02d]->[%s]\r\n", slot, buffer);
      }
    }
  }
  dataFile.close();

} // readOneSlot()

//===========================================================================================
void readSlotFromTimestamp(int8_t fileType, const char *fileName, const char *timeStamp, bool doJson, const char *rName)
{
  uint16_t firstSlot = 0, maxSlots = 0;

  DebugTf("timeStamp[%s]\r\n", timeStamp);

  switch (fileType)
  {
  case HOURS:
    firstSlot = timestampToHourSlot(timeStamp, strlen(timeStamp));
    maxSlots = _NO_HOUR_SLOTS_;
    break;
  case DAYS:
    firstSlot = timestampToDaySlot(timeStamp, strlen(timeStamp));
    maxSlots = _NO_DAY_SLOTS_;
    break;
  case MONTHS:
    firstSlot = timestampToMonthSlot(timeStamp, strlen(timeStamp));
    maxSlots = _NO_MONTH_SLOTS_;
    break;
  }

  firstSlot += maxSlots;
  DebugTf("firstSlot[%d] -> slot[%d]\r\n", firstSlot, (firstSlot % maxSlots));
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
    nrSlots = _NO_HOUR_SLOTS_;
    break;
  case DAYS:
    startSlot = timestampToDaySlot(timeStamp, strlen(timeStamp));
    nrSlots = _NO_DAY_SLOTS_;
    break;
  case MONTHS:
    startSlot = timestampToMonthSlot(timeStamp, strlen(timeStamp));
    nrSlots = _NO_MONTH_SLOTS_;
    break;
  }

  endSlot = nrSlots + startSlot;
  //startSlot += nrSlots;
  DebugTf("start[%02d], endSlot[%02d]\r\n", (startSlot % nrSlots), endSlot);
  for (uint16_t s = startSlot; s < endSlot; s++)
  {
    readOneSlot(fileType, fileName, s, recNr++, false, "");
  }

} // readAllSlots()

//===========================================================================================
bool createFile(const char *fileName, uint16_t noSlots)
{
  DebugTf("fileName[%s], fileRecLen[%d]\r\n", fileName, DATA_RECLEN);

  File dataFile = SPIFFS.open(fileName, "a"); // create File
  // -- first write fileHeader ----------------------------------------
  snprintf(cMsg, sizeof(cMsg), "%s", DATA_CSV_HEADER); // you cannot modify *fileHeader!!!
  fillRecord(cMsg, DATA_RECLEN);
  DebugT(cMsg);
  Debugln(F("\r"));
  bytesWritten = dataFile.print(cMsg);
  if (bytesWritten != DATA_RECLEN)
  {
    DebugTf("ERROR!! slotNr[%d]: written [%d] bytes but should have been [%d] for Header\r\n", 0, bytesWritten, DATA_RECLEN);
  }
  DebugTln(F(".. that went well! Now add next record ..\r"));
  // -- as this file is empty, write one data record ------------
  snprintf(cMsg, sizeof(cMsg), "%02d%02d%02d%02d", 0, 0, 0, 0);

  snprintf(cMsg, sizeof(cMsg), DATA_FORMAT, cMsg, 0.000, 0.000, 0.000, 0.000, 0.000);

  fillRecord(cMsg, DATA_RECLEN);
  for (int r = 1; r <= noSlots; r++)
  {
    DebugTf("Write [%s] Data[%-9.9s]\r\n", fileName, cMsg);
    dataFile.seek((r * DATA_RECLEN), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != DATA_RECLEN)
    {
      DebugTf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] \r\n", r, bytesWritten, DATA_RECLEN);
    }
  } // for ..

  dataFile.close();
  dataFile = SPIFFS.open(fileName, "r+"); // open for Read & writing
  if (!dataFile)
  {
    DebugTf("Something is very wrong writing to [%s]\r\n", fileName);
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
    DebugTf("Length of record is [%d] bytes\r\n", s);
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
    DebugTf("Length of record is now [%d] bytes\r\n", l);

} // fillRecord()

//====================================================================
uint16_t timestampToHourSlot(const char *TS, int8_t len)
{
  //char      aSlot[5];
  time_t t1 = epoch((char *)TS, strlen(TS), false);
  uint32_t nrHours = t1 / SECS_PER_HOUR;
  //sprintf(aSlot, "%d", ((nrDays % KEEP_DAYS_HOURS) *24) + hour(t1));
  //uint8_t   uSlot  = String(aSlot).toInt();
  uint8_t recSlot = (nrHours % _NO_HOUR_SLOTS_);

  if (Verbose1)
    DebugTf("===>>>>>  HOUR[%02d] => recSlot[%02d]\r\n", hour(t1), recSlot);

  if (recSlot < 0 || recSlot >= _NO_HOUR_SLOTS_)
  {
    DebugTf("HOUR: Some serious error! Slot is [%d]\r\n", recSlot);
    recSlot = _NO_HOUR_SLOTS_;
    slotErrors++;
  }
  return recSlot;

} // timestampToHourSlot()

//====================================================================
uint16_t timestampToDaySlot(const char *TS, int8_t len)
{
  //char      aSlot[5];
  time_t t1 = epoch((char *)TS, strlen(TS), false);
  uint32_t nrDays = t1 / SECS_PER_DAY;
  uint16_t recSlot = (nrDays % _NO_DAY_SLOTS_);

  if (Verbose1)
    DebugTf("===>>>>>   DAY[%02d] => recSlot[%02d]\r\n", day(t1), recSlot);

  if (recSlot < 0 || recSlot >= _NO_DAY_SLOTS_)
  {
    DebugTf("DAY: Some serious error! Slot is [%d]\r\n", recSlot);
    recSlot = _NO_DAY_SLOTS_;
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
  uint16_t recSlot = (nrMonths % _NO_MONTH_SLOTS_);      // eg: 24285 % _NO_MONTH_SLOT_

  if (Verbose1)
    DebugTf("===>>>>> MONTH[%02d] => recSlot[%02d]\r\n", month(t1), recSlot);

  if (recSlot < 0 || recSlot >= _NO_MONTH_SLOTS_)
  {
    DebugTf("MONTH: Some serious error! Slot is [%d]\r\n", recSlot);
    recSlot = _NO_MONTH_SLOTS_;
    slotErrors++;
  }
  return recSlot;

} // timestampToMonthSlot()

//===========================================================================================
int32_t freeSpace()
{
  int32_t space;

  SPIFFS.info(SPIFFSinfo);

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

  Dir dir = SPIFFS.openDir("/"); // List files on SPIFFS
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
  }   /* end for */

  DebugTln(F("\r\n"));
  for (int f = 0; f < fileNr; f++)
  {
    Debugf("%-25s %6d bytes \r\n", dirMap[f].Name, dirMap[f].Size);
    yield();
  }

  SPIFFS.info(SPIFFSinfo);

  Debugln(F("\r"));
  if (freeSpace() < (10 * SPIFFSinfo.blockSize))
    Debugf("Available SPIFFS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024));
  else
    Debugf("Available SPIFFS space [%6d]kB\r\n", (freeSpace() / 1024));
  Debugf("           SPIFFS Size [%6d]kB\r\n", (SPIFFSinfo.totalBytes / 1024));
  Debugf("     SPIFFS block Size [%6d]bytes\r\n", SPIFFSinfo.blockSize);
  Debugf("      SPIFFS page Size [%6d]bytes\r\n", SPIFFSinfo.pageSize);
  Debugf(" SPIFFS max.Open Files [%6d]\r\n\r\n", SPIFFSinfo.maxOpenFiles);

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

  Debug("Enter filename to erase: ");
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

  if (SPIFFS.exists(eName))
  {
    Debugf("\r\nErasing [%s] from SPIFFS\r\n\n", eName);
    SPIFFS.remove(eName);
  }
  else
  {
    Debugf("\r\nfile [%s] not found..\r\n\n", eName);
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

  DebugTf("check if [%s] exists .. ", fName);
  if (settingOledType > 0)
  {
    oled_Print_Msg(1, "Bestaat:", 10);
    oled_Print_Msg(2, fName, 10);
    oled_Print_Msg(3, "op SPIFFS?", 250);
  }

  if (!SPIFFS.exists(fName))
  {
    if (doDisplay)
    {
      Debugln(F("NO! Error!!"));
      if (settingOledType > 0)
      {
        oled_Print_Msg(3, "Nee! FOUT!", 6000);
      }
      writeToSysLog("Error! File [%s] not found!", fName);
      return false;
    }
    else
    {
      Debugln(F("NO! "));
      if (settingOledType > 0)
      {
        oled_Print_Msg(3, "Nee! ", 6000);
      }
      writeToSysLog("File [%s] not found!", fName);
      return false;
    }
  }
  else
  {
    Debugln(F("Yes! OK!"));
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\convertPRD2RING.ino"
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
    if (DSMRfileExist("PRDhours.csv",  false) )
    {
      SPIFFS.remove(HOURS_FILE);
      convertPRDfile(HOURS);
    }
    if (DSMRfileExist("PRDdays.csv",   false) )
    {
      SPIFFS.remove(DAYS_FILE);
      convertPRDfile(DAYS);
    }
    if (DSMRfileExist("PRDmonths.csv", false) )
    {
      SPIFFS.remove(MONTHS_FILE);
      convertPRDfile(MONTHS);
    }
    SPIFFS.remove("/!PRDconvert");

} // convertPRD2RING()

//=====================================================================
void convertPRDfile(int8_t fileType)
{
  char  PRDfileName[30];
  char  buffer[200];
  char  recKey[15];
  float EDT1, EDT2, ERT1, ERT2, GDT;
  int   offSet = 0, maxRecs = 0;

  Debugln("convertPRDfile() =============================================\r\n");
  
  switch(fileType)
  {
    case HOURS:   strCopy(PRDfileName, sizeof(PRDfileName), "/PRDhours.csv");
                  maxRecs = 49;
                  break;
    case DAYS:    strCopy(PRDfileName, sizeof(PRDfileName), "/PRDdays.csv");
                  maxRecs = 15;
                  break;
    case MONTHS:  strCopy(PRDfileName, sizeof(PRDfileName), "/PRDmonths.csv");
                  maxRecs = 25;
                  break;
                  
  } // switch()

  File PRDfile  = SPIFFS.open(PRDfileName, "r");    // open for Read 
  if (!PRDfile) 
  {
    DebugTf("File [%s] does not exist, skip\r\n", PRDfileName);
    return;
  } // if (!dataFile)

  int recLen = PRDfile.readBytesUntil('\n', buffer, sizeof(buffer)) +1;
  DebugTf("recLen[%02d]\r\n", recLen);

  for(int r=maxRecs; r>0; r--)
  {
      offSet = r * recLen;
      DebugTf("offSet[%4d] => ", offSet);
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
      Debugf("recKey[%s] --> \r\n", recKey);
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
  char record[DATA_RECLEN + 1] = "";
  char newKey[15];
  uint16_t recSlot;

  // key is:
  //   hours:  YYMMDDHH concat mmssX
  //    days:  YYMMDD   concat HHmmssX
  //  months:  YYMM     concat DDHHmmssX
  strCopy(newKey, 14, key);
  
  switch(fileType)
  {
    case HOURS:   strConcat(newKey, 14, "0101X");
                  recSlot = timestampToHourSlot(newKey,  strlen(newKey));
                  break;
    case DAYS:    strConcat(newKey, 14, "230101X");
                  recSlot = timestampToDaySlot(newKey,   strlen(newKey));
                  break;
    case MONTHS:  strConcat(newKey, 14, "01230101X");
                  recSlot = timestampToMonthSlot(newKey, strlen(newKey));
                  break;
                  
  } // switch()

  snprintf(record, sizeof(record), (char*)DATA_FORMAT, newKey , (float)EDT1
                                             , (float)EDT2
                                             , (float)ERT1
                                             , (float)ERT2
                                             , (float)GDT);

  // DATA + \n + \0                                        
  fillRecord(record, DATA_RECLEN);

  if (Verbose2) Debugf("key[%s], slot[%02d] %s\r", newKey, recSlot, record);
  
  switch(fileType)
  {
    case HOURS:   writeDataToFile(HOURS_FILE,  record, recSlot, HOURS);
                  break;
    case DAYS:    writeDataToFile(DAYS_FILE,   record, recSlot, DAYS);
                  break;
    case MONTHS:  writeDataToFile(MONTHS_FILE, record, recSlot, MONTHS);
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleSlimmeMeter.ino"
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

#if !defined(HAS_NO_SLIMMEMETER)
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
  char    tlgrm[1200] = "";
   
  DebugTf("handleSlimmerMeter RawCount=[%4d]\r\n", showRawCount);
  showRawCount++;
  showRaw = (showRawCount <= 20);
  if (!showRaw)
  {
    showRawCount  = 0;
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
  Serial.setTimeout(5000);  // 5 seconds must be enough ..
  memset(tlgrm, 0, sizeof(tlgrm));
  int l = 0;
  // The terminator character is discarded from the serial buffer.
  l = Serial.readBytesUntil('/', tlgrm, sizeof(tlgrm));
  // now read from '/' to '!'
  // The terminator character is discarded from the serial buffer.
  l = Serial.readBytesUntil('!', tlgrm, sizeof(tlgrm));
  Serial.setTimeout(1000);  // seems to be the default ..
//  DebugTf("read [%d] bytes\r\n", l);
  if (l == 0) 
  {
    DebugTln(F("RawMode: Timerout - no telegram received within 5 seconds"));
    return;
  }

  tlgrm[l++] = '!';
#if !defined( USE_PRE40_PROTOCOL )
  // next 6 bytes are "<CRC>\r\n"
  for (int i=0; ( i<6 && (i<(sizeof(tlgrm)-7)) ); i++)
  {
    tlgrm[l++] = (char)Serial.read();
  }
#else
  tlgrm[l++]    = '\r';
  tlgrm[l++]    = '\n';
#endif
  tlgrm[(l +1)] = '\0';
  // shift telegram 1 char to the right (make room at pos [0] for '/')
  for (int i=strlen(tlgrm); i>=0; i--) { tlgrm[i+1] = tlgrm[i]; yield(); }
  tlgrm[0] = '/'; 
  //Post result to Debug 
  Debugf("Telegram (%d chars):\r\n/%s\r\n", strlen(tlgrm), tlgrm); 
  return;
  
} // processSlimmemeterRaw()


//==================================================================================
void processSlimmemeter()
{
  slimmeMeter.loop();
  if (slimmeMeter.available()) 
  {
    DebugTf("telegramCount=[%d] telegramErrors=[%d]\r\n", telegramCount, telegramErrors);
    Debugln(F("\r\n[Time----][FreeHeap/mBlck][Function----(line):\r"));
    // Voorbeeld: [21:00:11][   9880/  8960] loop        ( 997): read telegram [28] => [140307210001S]
    telegramCount++;
        
    DSMRdata = {};
    String    DSMRerror;
        
    if (slimmeMeter.parse(&DSMRdata, &DSMRerror))   // Parse succesful, print result
    {
      if (telegramCount > (UINT32_MAX - 10)) 
      {
        delay(1000);
        ESP.reset();
        delay(1000);
      }
      digitalWrite(LED_BUILTIN, LED_OFF);
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

#ifdef USE_NTP_TIME
      if (!DSMRdata.timestamp_present)                        //USE_NTP
      {                                                       //USE_NTP
        sprintf(cMsg, "%02d%02d%02d%02d%02d%02dW\0\0"         //USE_NTP
                        , (year() - 2000), month(), day()     //USE_NTP
                        , hour(), minute(), second());        //USE_NTP
        DSMRdata.timestamp         = cMsg;                    //USE_NTP
        DSMRdata.timestamp_present = true;                    //USE_NTP
      }                                                       //USE_NTP
#endif

      processTelegram();
      if (Verbose2) 
      {
        DSMRdata.applyEach(showValues());
      }
          
    } 
    else                  // Parser error, print error
    {
      telegramErrors++;
      #ifdef USE_SYSLOGGER
        sysLog.writef("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str());
      #endif
      DebugTf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str());
      //--- set DTR to get a new telegram as soon as possible
      slimmeMeter.enable(true);
      slimmeMeter.loop();
    }

    if ( (telegramCount > 25) && (telegramCount % (2100 / (settingTelegramInterval + 1)) == 0) )
    {
      DebugTf("Processed [%d] telegrams ([%d] errors)\r\n", telegramCount, telegramErrors);
      writeToSysLog("Processed [%d] telegrams ([%d] errors)", telegramCount, telegramErrors);
    }
        
  } // if (slimmeMeter.available()) 
  
} // handleSlimmeMeter()

#endif

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\handleTestdata.ino"


#if defined(HAS_NO_SLIMMEMETER)

#define   MAXLINELENGTH     128   // longest normal line is 47 char (+3 for \r\n\0)

enum runStates { SInit, SMonth, SDay, SHour, SNormal };
enum runStates runMode = SNormal;

char        telegramLine[MAXLINELENGTH] = "";
char        telegram[1000] = "";
uint16_t    currentCRC; 
int16_t     calcCRC;
uint32_t    actInterval = 5, nextMinute = 0, nextESPcheck = 0, nextGuiUpdate;
int8_t      State;
int16_t     actSec, actMinute, actHour, actDay, actMonth, actYear, actSpeed;
char        actDSMR[3] = "40", savDSMR[3] = "40";
double      ED_T1=0, ED_T2=0, ER_T1=0, ER_T2=0, V_l1=0, V_l2=0, V_l3=0, C_l1=0, C_l2=0, C_l3=0;
uint8_t     ETariffInd=1;
float       PDelivered, PReturned;
float       IPD_l1, IPD_l2, IPD_l3, IPR_l1, IPR_l2, IPR_l3;
float       GDelivered;
bool        forceBuildRingFiles = false;
int16_t     forceBuildRecs;

#define   TELEGRAM_INTERVAL   5 //seconds
DECLARE_TIMER_SEC(telegramTimer, TELEGRAM_INTERVAL);

//==================================================================================================
void handleTestdata()
{
  time_t nt;
  int16_t slot;
 
  if (!forceBuildRingFiles && ( !DUE( telegramTimer) ) )  return;   // not yet time for new Telegram

//  DebugTf("Time for a new Telegram ..");
  if (forceBuildRingFiles)
  {
    switch(runMode) {
      case SInit:   runMode = SMonth;
                    forceBuildRecs = (_NO_MONTH_SLOTS_ *2) +5;
                    runMode = SMonth;
                    nt = epoch(actTimestamp, sizeof(actTimestamp), true);
                    break;
                    
      case SMonth:  if (forceBuildRecs <= 0)
                    {
                      forceBuildRecs = (_NO_DAY_SLOTS_ *2) +4;
                      nt = epoch(actTimestamp, sizeof(actTimestamp), true);
                      runMode = SDay;
                      break;
                    }    
                    nt = now() + (15 * SECS_PER_DAY);
                    DebugTf("Force build RING file for months -> rec[%2d]\r\n\n", forceBuildRecs);
                    forceBuildRecs--;
                    break;
                    
      case SDay:    if (forceBuildRecs <= 0)
                    {
                      forceBuildRecs = (_NO_HOUR_SLOTS_ * 2) +5;
                      nt = epoch(actTimestamp, sizeof(actTimestamp), true);
                      runMode = SHour;
                      break;
                    }
                    nt = now() + (SECS_PER_DAY / 2);
                    DebugTf("Force build RING file for days -> rec[%2d]\r\n\n", forceBuildRecs);
                    forceBuildRecs--;
                    break;
                    
      case SHour:   if (forceBuildRecs <= 0)
                    {
                      forceBuildRingFiles= false;
                      nt = epoch(actTimestamp, sizeof(actTimestamp), true);
                      DebugTln("Force build RING file back to normal operation\r\n\n");
                      runMode = SNormal;
                      break;
                    }
                    nt = now() + (SECS_PER_HOUR / 2);
                    //epochToTimestamp(nt, newTimestamp, sizeof(newTimestamp));
                    //slot = timestampToHourSlot(newTimestamp, sizeof(newTimestamp));
                    DebugTf("Force build RING file for hours -> rec[%2d]\r\n\n", forceBuildRecs);
                    forceBuildRecs--;
                    break;
                    
      default:      runMode = SNormal;
                    forceBuildRingFiles = false;

    } // switch()
  }
  else  // normal operation mode
  {
    nt = now() + (SECS_PER_HOUR / 2);
  }

  epochToTimestamp(nt, newTimestamp, sizeof(newTimestamp));
  Debugf("==>> new date/time [%s] is [%s]\r\n", newTimestamp, buildDateTimeString(newTimestamp, sizeof(newTimestamp)).c_str());
  
  updateMeterValues(SNormal);
  
  currentCRC = 0;
  memset(telegram,0,sizeof(telegram));
  
#if defined( USE_PRE40_PROTOCOL )
    for (int16_t line = 0; line < 20; line++) {
      yield();
      int16_t len = buildTelegram30(line, telegramLine);  // also: prints to DSMRsend
//    calcCRC = decodeTelegram(len);  // why??
    }
    if (Verbose2) Debugf("!\r\n");
    strConcat(telegram, sizeof(telegram), "!\r\n");

#else
    for (int16_t line = 0; line < 38; line++) {
      yield();
      int16_t len = buildTelegram40(line, telegramLine);  // also: prints to DSMRsend
      calcCRC = decodeTelegram(len);
    } 
    snprintf(cMsg, sizeof(cMsg), "!%04X\r\n\r\n", (calcCRC & 0xFFFF));
    if (Verbose2) Debug(cMsg);
    strConcat(telegram, sizeof(telegram), cMsg);
    
#endif

  DebugFlush();
  telegramCount++;

  DSMRdata = {};
  ParseResult<void> res = P1Parser::parse(&DSMRdata, telegram, lengthof(telegram));
  if (res.err) 
  {
    // Parsing error, show it
    Debugln(res.fullError(telegram, telegram + lengthof(telegram)));
  } 
  else if (!DSMRdata.all_present()) 
  {
    if (Verbose2) DebugTln("DSMR: Some fields are missing");
  } 
  // Succesfully parsed, now process the data!

  processTelegram();
  
  Debugf("==>> act date/time [%s] is [%s]\r\n\n", actTimestamp, buildDateTimeString(actTimestamp, sizeof(actTimestamp)).c_str());

} // handleTestdata()


//==================================================================================================
int16_t buildTelegram40(int16_t line, char telegramLine[]) 
{
  int16_t len = 0;

  float val;

  switch (line) {
                                     //XMX5LGBBLB2410065887
    case 0:   sprintf(telegramLine, "/XMX5LGBBLB2410065887\r\n");
              break;
    case 1:   sprintf(telegramLine, "\r\n");    
              break;
    case 2:   sprintf(telegramLine, "1-3:0.2.8(50)\r\n");
              break;
    case 3:   sprintf(telegramLine, "0-0:1.0.0(%12.12sS)\r\n", newTimestamp);
              break;
    case 4:   sprintf(telegramLine, "0-0:96.1.1(4530303336303000000000000000000040)\r\n", val);
              break;
    case 5:   // Energy Delivered
              sprintf(telegramLine, "1-0:1.8.1(%s*kWh)\r\n", Format(ED_T1, 10, 3).c_str());
              break;
    case 6:   sprintf(telegramLine, "1-0:1.8.2(%s*kWh)\r\n", Format(ED_T2, 10, 3).c_str());
              break;
    case 7:   // Energy Returned
              sprintf(telegramLine, "1-0:2.8.1(%s*kWh)\r\n", Format(ER_T1, 10, 3).c_str());
              break;
    case 8:   sprintf(telegramLine, "1-0:2.8.2(%s*kWh)\r\n", Format(ER_T2, 10, 3).c_str());
              break;
    case 9:   // Tariff indicator electricity
              sprintf(telegramLine, "0-0:96.14.0(%04d)\r\n", ETariffInd);
              break;
    case 10:  // Actual electricity power delivered (+P) in 1 Watt resolution
              sprintf(telegramLine, "1-0:1.7.0(%s*kW)\r\n", Format(PDelivered, 6, 2).c_str());
              break;
    case 11:  // Actual electricity power received (-P) in 1 Watt resolution
              sprintf(telegramLine, "1-0:2.7.0(%s*kW)\r\n", Format(PReturned, 6, 2).c_str());
              break;
    case 12:  // Number of power failures in any phase
              sprintf(telegramLine, "0-0:96.7.21(00010)\r\n", val);
              break;
    case 13:  // Number of long power failures in any phase
              sprintf(telegramLine, "0-0:96.7.9(00000)\r\n", val);
              break;
    case 14:  // Power Failure Event Log (long power failures)
              sprintf(telegramLine, "1-0:99.97.0(0)(0-0:96.7.19)\r\n", val);
              break;
    case 15:  // Number of voltage sags in phase L1
              sprintf(telegramLine, "1-0:32.32.0(00002)\r\n", val);
              break;
    case 16:  // Number of voltage sags in phase L2 (polyphase meters only)
              sprintf(telegramLine, "1-0:52.32.0(00003)\r\n", val);
              break;
    case 17:  // Number of voltage sags in phase L3 (polyphase meters only)
              sprintf(telegramLine, "1-0:72.32.0(00003)\r\n", val);
              break;
    case 18:  // Number of voltage swells in phase L1
              sprintf(telegramLine, "1-0:32.36.0(00000)\r\n", val);
              break;
    case 19:  // Number of voltage swells in phase L2
              sprintf(telegramLine, "1-0:52.36.0(00000)\r\n", val);
              break;
    case 20:  // Number of voltage swells in phase L3
              sprintf(telegramLine, "1-0:72.36.0(00000)\r\n", val);
              break;
    case 21:  // Text message max 2048 characters
              sprintf(telegramLine, "0-0:96.13.0()\r\n", val);
              break;
    case 22:  // Instantaneous voltage L1 in 0.1V resolution
              sprintf(telegramLine, "1-0:32.7.0(%03d.0*V)\r\n", (240 + random(-3,3)));
              break;
    case 23:  // Instantaneous voltage L1 in 0.1V resolution
              sprintf(telegramLine, "1-0:52.7.0(%03d.0*V)\r\n", (238 + random(-3,3)));
              break;
    case 24:  // Instantaneous voltage L1 in 0.1V resolution
              sprintf(telegramLine, "1-0:72.7.0(%03d.0*V)\r\n", (236 + random(-3,3)));
              break;
    case 25:  // Instantaneous current L1 in A resolution
              sprintf(telegramLine, "1-0:31.7.0(%03d*A)\r\n", random(0,4));
              break;
    case 26:  // Instantaneous current L2 in A resolution
              sprintf(telegramLine, "1-0:51.7.0(%03d*A)\r\n",  random(0,4));
              break;
    case 27:  // Instantaneous current L3 in A resolution
              sprintf(telegramLine, "1-0:71.7.0(000*A)\r\n", val);
              break;
    case 28:  // Instantaneous active power L1 (+P) in W resolution
              sprintf(telegramLine, "1-0:21.7.0(%s*kW)\r\n", Format(IPD_l1, 6, 3).c_str());
              break;
    case 29:  // Instantaneous active power L2 (+P) in W resolution
              sprintf(telegramLine, "1-0:41.7.0(%s*kW)\r\n", Format(IPD_l2, 6, 3).c_str());
              break;
    case 30:  // Instantaneous active power L3 (+P) in W resolution
              sprintf(telegramLine, "1-0:61.7.0(%s*kW)\r\n", Format(IPD_l3, 6, 3).c_str());
              break;
    case 31:  // Instantaneous active power L1 (-P) in W resolution
              sprintf(telegramLine, "1-0:22.7.0(%s*kW)\r\n", Format(IPR_l1, 6, 3).c_str());
              break;
    case 32:  // Instantaneous active power L2 (-P) in W resolution
              sprintf(telegramLine, "1-0:42.7.0(%s*kW)\r\n", Format(IPR_l2, 6, 3).c_str());
              break;
    case 33:  // Instantaneous active power L3 (-P) in W resolution
              sprintf(telegramLine, "1-0:62.7.0(%s*kW)\r\n", Format(IPR_l3, 6, 3).c_str());
              break;
    case 34:  // Gas Device-Type
              sprintf(telegramLine, "0-1:24.1.0(003)\r\n", val);
              break;
    case 35:  // Equipment identifier (Gas)
              sprintf(telegramLine, "0-1:96.1.0(4730303339303031363532303530323136)\r\n", val);
              break;
    case 36:  // Last 5-minute value (temperature converted), gas delivered to client
              // in m3, including decimal values and capture time (Note: 4.x spec has
              sprintf(telegramLine, "0-1:24.2.1(%02d%02d%02d%02d%02d01S)(%s*m3)\r\n", (year() - 2000), month(), day(), hour(), minute(), 
                                                                            Format(GDelivered, 9, 3).c_str());
              break;
    case 37:  sprintf(telegramLine, "!xxxx\r\n");   
              break;
              
  } // switch(line)

  if (line < 37) {
    if (Verbose2) Debug(telegramLine); 
    strConcat(telegram, sizeof(telegram), telegramLine);
  }

  for(len = 0; len < MAXLINELENGTH, telegramLine[len] != '\0'; len++) {}    
  
  return len;

} // buildTelegram40()


//==================================================================================================
int16_t buildTelegram30(int16_t line, char telegramLine[]) 
{
/*
**  /KMP5 KA6U001585575011                - Telegram begin-marker + manufacturer + serial number
**  
**  0-0:96.1.1(204B413655303031353835353735303131)    -  Serial number in hex
**  1-0:1.8.1(08153.408*kWh)                          -  +T1: Energy input, low tariff (kWh)
**  1-0:1.8.2(05504.779*kWh)                          -  +T2: Energy input, normal tariff (kWh)
**  1-0:2.8.1(00000.000*kWh)                          -  -T3: Energy output, low tariff (kWh)
**  1-0:2.8.2(00000.000*kWh)                          -  -T4: Energy output, normal tariff (kWh)
**  0-0:96.14.0(0002)                                 -  Current tariff (1=low, 2=normal)
**  1-0:1.7.0(0000.30*kW)                             -  Actual power input (kW)
**  1-0:2.7.0(0000.00*kW)                             -  Actual power output (kW)
**  0-0:17.0.0(999*A)                                 -  Max current per phase (999=no max)
**  0-0:96.3.10(1)                                    -  Switch position
**  0-0:96.13.1()                                     -  Message code
**  0-0:96.13.0()                                     -  Message text
**  0-1:24.1.0(3)                                     -  Attached device type identifier
**  0-1:96.1.0(3238313031353431303031333733353131)    -  Serial number of gas meter
**  0-1:24.3.0(190718190000)(08)(60)(1)(0-1:24.2.1)(m3) -  Time of last gas meter update
**  (04295.190)                                       -  Gas meter value (m³)
**  0-1:24.4.0(1)                                     -  Gas valve position
**  !                                                 -  Telegram end-marker
**  
*/
//==================================================================================================
  int16_t len = 0;

  float val;

  switch (line) {
                                //KMP5 KA6U001585575011
    case 0:   sprintf(telegramLine, "/KMP5 KA6U001585575011\r\n");
              break;
    case 1:   sprintf(telegramLine, "\r\n");    
              break;
    case 2:   sprintf(telegramLine, "0-0:96.1.1(4530303336303000000000000000000000)\r\n", val);
              break;
    case 3:   // Energy Delivered
              sprintf(telegramLine, "1-0:1.8.1(%s*kWh)\r\n", Format(ED_T1, 10, 3).c_str());
              break;
    case 4:   sprintf(telegramLine, "1-0:1.8.2(%s*kWh)\r\n", Format(ED_T2, 10, 3).c_str());
              break;
    case 5:   // Energy Returned
              sprintf(telegramLine, "1-0:2.8.1(%s*kWh)\r\n", Format(ER_T1, 10, 3).c_str());
              break;
    case 6:   sprintf(telegramLine, "1-0:2.8.2(%s*kWh)\r\n", Format(ER_T2, 10, 3).c_str());
              break;
    case 7:   // Tariff indicator electricity
              sprintf(telegramLine, "0-0:96.14.0(%04d)\r\n", ETariffInd);
              break;
    case 8:   // Actual electricity power delivered (+P) in 1 Watt resolution
              sprintf(telegramLine, "1-0:1.7.0(%s*kW)\r\n", Format(PDelivered, 6, 2).c_str());
              break;
    case 9:   // Actual electricity power received (-P) in 1 Watt resolution
              sprintf(telegramLine, "1-0:2.7.0(%s*kW)\r\n", Format(PReturned, 6, 2).c_str());
              break;
    case 10:  // Max current per phase (999=no max)
              sprintf(telegramLine, "0-0:17.0.0(016*A)\r\n", val);
              break;
    case 11:  // Switch position (?)
              sprintf(telegramLine, "0-0:96.3.10(1)\r\n", val);
              break;
    case 12:  // Text message code
              sprintf(telegramLine, "0-0:96.13.1()\r\n", val);
              break;
    case 13:  // Text message text
              sprintf(telegramLine, "0-0:96.13.0()\r\n", val);
              break;
    case 14:  // Gas Device-Type
              sprintf(telegramLine, "0-1:24.1.0(3)\r\n", val);
              break;
    case 15:  // Equipment identifier (Gas)
              sprintf(telegramLine, "0-1:96.1.0(4730303339303031363500000000000000)\r\n", val);
              break;
    case 16:  // Last 5-minute value (temperature converted), gas delivered to client
              // in m3, including decimal values and capture time 
              sprintf(telegramLine, "0-1:24.3.0(%02d%02d%02d%02d%02d00)(08)(60)(1)(0-1:24.2.1)(m3)\r\n", (year() - 2000), month(), day(), hour(), minute());
              break;
    case 17:  sprintf(telegramLine, "(%s)\r\n", Format(GDelivered, 9, 3).c_str());
              break;
    case 18:  // Gas valve position
              sprintf(telegramLine, "0-1:24.4.0(1)\r\n", val);
              break;
    case 19:  sprintf(telegramLine, "!\r\n\r\n");     // just for documentation 
              break;
              
  } // switch(line)

  if (line < 19) {
    if (Verbose2) Debug(telegramLine); 
    strConcat(telegram, sizeof(telegram), telegramLine);
  }

  for(len = 0; len < MAXLINELENGTH, telegramLine[len] != '\0'; len++) {}    
  
  return len;

} // buildTelegram30()



//==================================================================================================
void updateMeterValues(uint8_t period) 
{
  float  Factor = 1.098;
  String wsString = "";
  /*
  switch(period) {
    case SMonth:  Factor = 30.0 * 24.0; break;
    case SDay:    Factor = 24.0;        break;
    case SHour:   Factor = 1.0;         break;
    default:      Factor = 1.0;
  }
  **/
  ED_T1      += (float)((random(200,2200) / 3600000.0) * actInterval) * Factor;
  ED_T2      += (float)((random(100,9000) / 3600000.0) * actInterval) * Factor;
  if (hour(actT) >= 4 && hour(actT) <= 20) {
    ER_T1      += (float)((random(0,400)  / 3600000.0) * actInterval) * Factor;
    ER_T2      += (float)((random(0,200)  / 3600000.0) * actInterval) * Factor;
    ETariffInd  = 1;
  } else {
    ETariffInd  = 2;
  }
  GDelivered += (float)(random(2,25) / 10000.0) * Factor;     // Gas Delevered
  V_l1        = (float)(random(220,240) * 1.01);      // Voltages
  V_l2        = (float)(random(220,240) * 1.02);
  V_l3        = (float)(random(220,240) * 1.03);
  C_l1        = (float)(random(1,20) * 1.01);          // Current
  C_l2        = (float)(random(1,15) * 1.02);
  C_l3        = (float)(random(1,10) * 1.03);   
  IPD_l1      = (float)(random(1,1111) * 0.001102);
  IPD_l2      = (float)(random(1,892)  * 0.001015);
  IPD_l3      = (float)(random(1,773)  * 0.001062);
  if (hour(actT) >= 4 && hour(actT) <= 20) {
    IPR_l1    = (float)(random(1,975) * 0.01109);
    IPR_l2    = (float)(random(1,754) * 0.01031);
    IPR_l3    = (float)(random(1,613) * 0.01092);
    
  } else {    // 's-nachts geen opwekking van energy!
    IPR_l1    = 0.0;
    IPR_l2    = 0.0;
    IPR_l3    = 0.0;
  }
  PDelivered  = (float)(IPD_l1 + IPD_l2 + IPD_l3) / 1.0;       // Power Delivered
  PReturned   = (float)(IPR_l1 + IPR_l2 + IPR_l3) / 1.0;       // Power Returned

  if (Verbose2) Debugf("l1[%5d], l2[%5d], l3[%5d] ==> l1+l2+l3[%9.3f]\n"
                                                          , (int)(IPD_l1 * 1000)
                                                          , (int)(IPD_l2 * 1000)
                                                          , (int)(IPD_l3 * 1000)
                                                          , PDelivered);
  
} // updateMeterValues()


//==================================================================================================
String Format(double x, int len, int d) 
{
  String r;
  int rl;
  
  r = String(x, d);
//Debugf("Format(%s, %d, %d)\n", r.c_str(), len, d);
  while (r.length() < len) r = "0" + r;
  rl = r.length();
  if (rl > len) {
    return r.substring((rl - len));
  }
  return r;

} // Format()



//==================================================================================================
int FindCharInArrayRev(unsigned char array[], char c, int len) 
{
  for (int16_t i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}


//==================================================================================================
int16_t decodeTelegram(int len) 
{
  //need to check for start
  int startChar = FindCharInArrayRev((unsigned char*)telegramLine, '/', len);
  int endChar   = FindCharInArrayRev((unsigned char*)telegramLine, '!', len);
  
  bool validCRCFound = false;
  if(startChar>=0) {
    //start found. Reset CRC calculation
    currentCRC=CRC16(0x0000,(unsigned char *) telegramLine+startChar, len-startChar);
    
  } else if(endChar>=0) {
    //add to crc calc 
    currentCRC=CRC16(currentCRC,(unsigned char*)telegramLine+endChar, 1);
  
  } else {
    currentCRC=CRC16(currentCRC, (unsigned char*)telegramLine, len);
  }

  //return validCRCFound;
  return currentCRC;
  
} // decodeTelegram()


//==================================================================================================
unsigned int CRC16(unsigned int crc, unsigned char *buf, int len)
{
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}

#endif

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\helperStuff.ino"
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
  _isValidIP = ((ip[0] || ip[1] || ip[2] || ip[3])>0);                             // if any bits are set, then it is not 0.0.0.0
  _isValidIP &= !((ip[0]==0) || (ip[3]==0));                                       // if either the first or last is a 0, then it is invalid
  _isValidIP &= !((ip[0]==255) || (ip[1]==255) || (ip[2]==255) || (ip[3]==255)) ;  // if any of the octets is 255, then it is invalid  
  _isValidIP &= !(ip[0]==127 && ip[1]==0 && ip[2]==0 && ip[3]==1);                 // if not 127.0.0.0 then it might be valid
  _isValidIP &= !(ip[0]>=224);                                                     // if ip[0] >=224 then reserved space  
  
  DebugTf( "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if (_isValidIP) 
    Debugln(F(" = Valid IP")); 
  else 
    Debugln(F(" = Invalid IP!"));
    
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
      inxE  = inStrng.indexOf(delimiter, inxS);         //finds location of first ,
      wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
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
      wOut[maxWords-1] = inStrng.substring(inxS, inStrng.length());  // store rest of String      
    }

    return wordCount;
    
} // splitString()


//===========================================================================================
String upTime() 
{
  char    calcUptime[20];

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
    DebugTf("Combined string > %d chars\r\n", maxLen);
  }
  
} // strConcat()


//===========================================================================================
void strConcat(char *dest, int maxLen, float v, int dec)
{
  static char buff[25];
  if (dec == 0)       sprintf(buff,"%.0f", v);
  else if (dec == 1)  sprintf(buff,"%.1f", v);
  else if (dec == 2)  sprintf(buff,"%.2f", v);
  else if (dec == 3)  sprintf(buff,"%.3f", v);
  else if (dec == 4)  sprintf(buff,"%.4f", v);
  else if (dec == 5)  sprintf(buff,"%.5f", v);
  else                sprintf(buff,"%f",   v);

  if (strlen(dest) + strlen(buff) < maxLen) 
  {
    strcat(dest, buff);
  } 
  else
  {
    DebugTf("Combined string > %d chars\r\n", maxLen);
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
    DebugTf("Combined string > %d chars\r\n", maxLen);
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
  for (int i=frm; (src[i] != 0  && i<=to && d<maxLen); i++)
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
  if (dec == 0)       sprintf(buff,"%.0f", v);
  else if (dec == 1)  sprintf(buff,"%.1f", v);
  else if (dec == 2)  sprintf(buff,"%.2f", v);
  else if (dec == 3)  sprintf(buff,"%.3f", v);
  else if (dec == 4)  sprintf(buff,"%.4f", v);
  else if (dec == 5)  sprintf(buff,"%.5f", v);
  else                sprintf(buff,"%f",   v);
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
  float r =  0.0;
  int   p =  0;
  int   d = -1;
  
  r = strtof(s, NULL);
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\jsonStuff.ino"
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
  objSprtr[0]    = '\0';

  snprintf(sBuff, sizeof(sBuff), "{\"%s\":[\r\n", objName);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
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
  char jsonBuff[JSON_BUFF_MAX] = "";
  
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
  char jsonBuff[JSON_BUFF_MAX] = "";
  
  if (sValue.length() > (JSON_BUFF_MAX - 65) )
  {
    DebugTf("[2] sValue.length() [%d]\r\n", sValue.length());
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

  if (sValue.length() > (JSON_BUFF_MAX - 60) )
  {
    sendNestedJsonObj(cName, sValue.substring(0,(JSON_BUFF_MAX - (strlen(cName) + 30))), noUnit);
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
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, cValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
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
      snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                         , cName, tmp.c_str());
    }
    else
    {
      snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\"}]}"
                                      , cName, sValue.c_str());
    }
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": \"%s\", \"unit\": \"%s\"}]}"
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
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %d}]}"
                                      , cName, iValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %d, \"unit\": \"%s\"}]}"
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
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %u}]}"
                                      , cName, uValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %u, \"unit\": \"%s\"}]}"
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
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %.3f}]}"
                                      , cName, fValue);
  }
  else
  {
    snprintf(mqttBuff, MQTT_BUFF_MAX, "{\"%s\": [{\"value\": %.3f, \"unit\": \"%s\"}]}"
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\menuStuff.ino"
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
    readAllSlots(HOURS, HOURS_FILE, actTimestamp, false, "");

} // displayHoursHist()


//===========================================================================================
void displayDaysHist(bool Telnet=true) 
{
    readAllSlots(DAYS, DAYS_FILE, actTimestamp, false, "");

} // displayDaysHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true) 
{
    readAllSlots(MONTHS, MONTHS_FILE, actTimestamp, false, "");

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo() 
{
  Debugln(F("\r\n==================================================================\r"));
  Debug(F(" \r\n            (c)2019 by [Willem Aandewiel"));
  Debug(F("]\r\n      Firmware Version ["));  Debug( _FW_VERSION );
  Debug(F("]\r\n              Compiled ["));  Debug( __DATE__ ); 
                                               Debug( "  " );
                                               Debug( __TIME__ );
#ifdef USE_PRE40_PROTOCOL
  Debug(F("]\r\n         compiled with [dsmr30.h] [USE_PRE40_PROTOCOL"));
#else
  Debug(F("]\r\n         compiled with [dsmr.h"));
#endif
  Debug(F("]\r\n              #defines "));
#ifdef USE_REQUEST_PIN
  Debug(F("[USE_REQUEST_PIN]"));
#endif
#ifdef USE_UPDATE_SERVER
  Debug(F("[USE_UPDATE_SERVER]"));
#endif
#ifdef USE_MQTT
  Debug(F("[USE_MQTT]"));
#endif
#ifdef USE_MINDERGAS
  Debug(F("[USE_MINDERGAS]"));
#endif
#ifdef USE_SYSLOGGER
  Debug(F("[USE_SYSLOGGER]"));
#endif
#ifdef USE_NTP_TIME
  Debug(F("[USE_NTP_TIME]"));
#endif
#ifdef USE_BELGIUM_PROTOCOL
  Debug(F("[USE_BELGIUM_PROTOCOL]"));
#endif
#ifdef SHOW_PASSWRDS
  Debug(F("[SHOW_PASSWRDS]"));
#endif

  Debug(F(" \r\n   Telegrams Processed ["));  Debug( telegramCount );
  Debug(F("]\r\n           With Errors ["));  Debug( telegramErrors );
  Debug(F("]\r\n              FreeHeap ["));  Debug( ESP.getFreeHeap() );
  Debug(F("]\r\n             max.Block ["));  Debug( ESP.getMaxFreeBlockSize() );
  Debug(F("]\r\n               Chip ID ["));  Debug( ESP.getChipId(), HEX );
  Debug(F("]\r\n          Core Version ["));  Debug( ESP.getCoreVersion() );
  Debug(F("]\r\n           SDK Version ["));  Debug( ESP.getSdkVersion() );
  Debug(F("]\r\n        CPU Freq (MHz) ["));  Debug( ESP.getCpuFreqMHz() );
  Debug(F("]\r\n      Sketch Size (kB) ["));  Debug( ESP.getSketchSize() / 1024.0 );
  Debug(F("]\r\nFree Sketch Space (kB) ["));  Debug( ESP.getFreeSketchSpace() / 1024.0 );

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else  snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());

  SPIFFS.info(SPIFFSinfo);

  Debug(F("]\r\n         Flash Chip ID ["));  Debug( cMsg );
  Debug(F("]\r\n  Flash Chip Size (kB) ["));  Debug( ESP.getFlashChipSize() / 1024 );
  Debug(F("]\r\n   Chip Real Size (kB) ["));  Debug( ESP.getFlashChipRealSize() / 1024 );
  Debug(F("]\r\n      SPIFFS Size (kB) ["));  Debug( SPIFFSinfo.totalBytes / 1024 );

  Debug(F("]\r\n      Flash Chip Speed ["));  Debug( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Debug(F("]\r\n       Flash Chip Mode ["));  Debug( flashMode[ideMode] );

  Debugln(F("]\r"));

  Debugln(F("==================================================================\r"));
  Debug(F(" \r\n            Board type ["));
#ifdef ARDUINO_ESP8266_NODEMCU
    Debug(F("ESP8266_NODEMCU"));
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    Debug(F("ESP8266_GENERIC"));
#endif
#ifdef ESP8266_ESP01
    Debug(F("ESP8266_ESP01"));
#endif
#ifdef ESP8266_ESP12
    Debug(F("ESP8266_ESP12"));
#endif
  Debug(F("]\r\n                  SSID ["));  Debug( WiFi.SSID() );
#ifdef SHOW_PASSWRDS
  Debug(F("]\r\n               PSK key ["));  Debug( WiFi.psk() );
#else
  Debug(F("]\r\n               PSK key [**********"));
#endif
  Debug(F("]\r\n            IP Address ["));  Debug( WiFi.localIP().toString() );
  Debug(F("]\r\n              Hostname ["));  Debug( settingHostname );
  Debug(F("]\r\n     Last reset reason ["));  Debug( ESP.getResetReason() );
  Debug(F("]\r\n                upTime ["));  Debug( upTime() );
  Debugln(F("]\r"));

#ifdef USE_MQTT
  Debugln(F("==================================================================\r"));
  Debug(F(" \r\n           MQTT broker ["));  Debug( settingMQTTbroker );
  Debug(F("]\r\n             MQTT User ["));  Debug( settingMQTTuser );
  #ifdef SHOW_PASSWRDS
    Debug(F("]\r\n         MQTT PassWord ["));  Debug( settingMQTTpasswd );
  #else
    Debug(F("]\r\n         MQTT PassWord [**********"));
  #endif
  Debug(F("]\r\n             Top Topic ["));  Debug(settingMQTTtopTopic );
  Debug(F("]\r\n       Update Interval ["));  Debug(settingMQTTinterval);
  Debugln(F("]\r"));
  Debugln(F("==================================================================\r\n\r"));
#endif

} // displayBoardInfo()



//===========================================================================================
void handleKeyInput() 
{
  char    inChar;

  while (TelnetStream.available() > 0) 
  {
    yield();
    inChar = (char)TelnetStream.read();
    
    switch(inChar) {
      case 'b':
      case 'B':     displayBoardInfo();
                    break;
      case 'l':
      case 'L':     readSettings(true);
                    break;
      case 'd':
      case 'D':     displayDaysHist(true);
                    break;
      case 'E':     eraseFile();
                    break;
#if defined(HAS_NO_SLIMMEMETER)
      case 'F':     forceBuildRingFiles = true;
                    runMode = SInit;
                    break;
#endif
      case 'h':
      case 'H':     displayHoursHist(true);
                    break;
      case 'm':
      case 'M':     displayMonthsHist(true);
                    break;
                    
      case 'W':     Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", settingHostname);
                    delay(1000);
                    WiFi.disconnect(true);  // deletes credentials !
                    //setupWiFi(true);
                    delay(2000);
                    ESP.reset();
                    delay(2000);
                    break;
      case 'i':
      case 'I':     for(int I=0; I<10; I++) 
                    {
                      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                      delay(1000);
                    }
                    break;
#ifdef USE_MINDERGAS
      case 't':
      case 'T':     forceMindergasUpdate();  //skip waiting for (midnight||countdown) 
                    break;
#endif
      case 'p':
      case 'P':     showRaw = !showRaw;
                 #ifdef USE_REQUEST_PIN
                    if (showRaw)  digitalWrite(DTR_ENABLE, HIGH);
                    else          digitalWrite(DTR_ENABLE, LOW);
                 #endif
                    showRawCount = 0;
                    break;
      case 'R':     DebugT(F("Reboot in 3 seconds ... \r\n"));
                    DebugFlush();
                    delay(3000);
                    DebugTln(F("now Rebooting. \r"));
                    DebugFlush();
                    ESP.reset();
                    break;
      case 's':
      case 'S':     listSPIFFS();
                    break;
      case 'v':
      case 'V':     if (Verbose2) 
                    {
                      Debugln(F("Verbose is OFF\r"));
                      Verbose1 = false;
                      Verbose2 = false;
                    } 
                    else if (Verbose1) 
                    {
                      Debugln(F("Verbose Level 2 is ON\r"));
                      Verbose2 = true;
                    } 
                    else 
                    {
                      Debugln(F("Verbose Level 1 is ON\r"));
                      Verbose1 = true;
                      Verbose2 = false;
                    }
                    break;
#ifdef USE_SYSLOGGER
      case 'q':
      case 'Q':     sysLog.setDebugLvl(0);
                    sysLog.dumpLogFile();
                    sysLog.setDebugLvl(1);
                    break;
#endif
      case 'Z':     slotErrors      = 0;
                    nrReboots       = 0;
                    telegramCount   = 0;
                    telegramErrors  = 0;
                    writeLastStatus();
                    #ifdef USE_SYSLOGGER
                      sysLog = {};
                      openSysLog(true);
                    #endif
                    break;
      default:      Debugln(F("\r\nCommands are:\r\n"));
                    Debugln(F("   B - Board Info\r"));
                    Debugln(F("  *E - erase file from SPIFFS\r"));
                    Debugln(F("   L - list Settings\r"));
                    Debugln(F("   D - Display Day table from SPIFFS\r"));
                    Debugln(F("   H - Display Hour table from SPIFFS\r"));
                    Debugln(F("   M - Display Month table from SPIFFS\r"));
                  #if defined(HAS_NO_SLIMMEMETER)
                    Debugln(F("  *F - Force build RING files\r"));
                  #endif
                    Debugf ("   I - Identify by blinking LED on GPIO[%02d]\r\n", LED_BUILTIN);
                    if (showRaw) 
                    {
                      Debugln(F("   P - Start Parsing again\r"));
                    } 
                    else 
                    {
                      Debugln(F("   P - No Parsing (show RAW data from Smart Meter)\r"));
                      showRawCount = 0;
                    }
                    Debugln(F("  *W - Force Re-Config WiFi\r"));
#ifdef USE_SYSLOGGER
                    Debugln(F("   Q - dump sysLog file\r"));
#endif
                    Debugln(F("  *R - Reboot\r"));
                    Debugln(F("   S - File info on SPIFFS\r"));
                    Debugln(F("  *U - Update SPIFFS (save Data-files)\r"));
                    Debugln(F("  *Z - Zero counters\r\n"));
                    if (Verbose1 & Verbose2)  Debugln(F("   V - Toggle Verbose Off\r"));
                    else if (Verbose1)        Debugln(F("   V - Toggle Verbose 2\r"));
                    else                      Debugln(F("   V - Toggle Verbose 1\r"));
                    #ifdef USE_MINDERGAS
                    Debugln(F("   T - Force update mindergas.nl\r"));
                    #endif

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\ntpStuff.ino"
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
#if defined(USE_NTP_TIME)

#include <WiFiUdp.h>            // - part of ESP8266 Core https://github.com/esp8266/Arduino
WiFiUDP           Udp;

const int         timeZone = 1;       // Central European (Winter) Time
unsigned int      localPort = 8888;   // local port to listen for UDP packets

// NTP Servers:
static const char ntpPool[][30] = { "time.google.com",
                                    "nl.pool.ntp.org",
                                    "0.nl.pool.ntp.org",
                                    "1.nl.pool.ntp.org",
                                    "3.nl.pool.ntp.org"
                                   };
static int        ntpPoolIndx = 0;

char              ntpServerName[50];

const int         NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte              packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

static int        ntpServerNr = 0;
static bool       externalNtpTime = false;
static IPAddress  ntpServerIP; // NTP server's ip address


//=======================================================================
bool startNTP() 
{
  DebugTln(F("Starting UDP"));
  Udp.begin(localPort);
  DebugT(F("Local port: "));
  DebugTln(String(Udp.localPort()));
  DebugTln(F("waiting for NTP sync"));
  setSyncProvider(getNtpTime);
  setSyncInterval(60);  // seconds!
  if (timeStatus() == timeSet)      // time is set,
  {
    return true;                    // exit with time set
  }
  return false;

} // startNTP()


//=======================================================================
time_t getNtpTime() 
{
  DECLARE_TIMER_MS(waitForPackage, 1500);   

  while(true) 
  {
    yield;
    ntpPoolIndx++;
    if ( ntpPoolIndx > (sizeof(ntpPool) / sizeof(ntpPool[0]) -1) ) 
    {
      ntpPoolIndx = 0;
    }
    snprintf(ntpServerName, sizeof(ntpServerName), "%s", String(ntpPool[ntpPoolIndx]).c_str());

    while (Udp.parsePacket() > 0) ; // discard any previously received packets
    TelnetStream.println("Transmit NTP Request");
    // get a random server from the pool
    WiFi.hostByName(ntpServerName, ntpServerIP);
    TelnetStream.print(ntpServerName);
    TelnetStream.print(": ");
    TelnetStream.println(ntpServerIP);
    TelnetStream.flush();
    sendNTPpacket(ntpServerIP);
    RESTART_TIMER(waitForPackage);
    while (!DUE(waitForPackage))
    {
      int size = Udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) 
      {
        //TelnetStream.print("Receive NTP Response: ");
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        time_t t = (secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
        snprintf(cMsg, sizeof(cMsg), "%02d:%02d:%02d", hour(t), minute(t), second(t));   
        TelnetStream.printf("[%s] Received NTP Response => new time [%s]  (Winter)\r\n", cMsg, cMsg);
        // return epoch ..
        return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
      }
    }
    TelnetStream.println("No NTP Response :-(");

  } // while true ..
  
  return 0; // return 0 if unable to get the time

} // getNtpTime()


// send an NTP request to the time server at the given address
//=======================================================================
void sendNTPpacket(IPAddress &address) 
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  
} // sendNTPpacket()


//=======================================================================
time_t dateTime2Epoch(char const *date, char const *time) 
{
    char s_month[5];
    int year, day, h, m, s;
    tmElements_t t;

    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    if (sscanf(date, "%s %d %d", s_month, &day, &year) != 3) 
    {
      DebugTf("Not a valid date string [%s]\r\n", date);
      return 0;
    }
    t.Day  = day;
    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;
    
  //DebugTf("date=>[%d-%02d-%02d]\r\n", t.Year, t.Month, t.Day);
    
    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;

    if (sscanf(time, "%2d:%2d:%2d", &h, &m, &s) != 3) 
    {
      DebugTf("Not a valid time string [%s] (time set to '0')\r\n", time);
      h = 0;
      m = 0;
      s = 0;
    }
    t.Hour    = h;
    t.Minute  = m;
    t.Second  = s;
    
  //DebugTf("time=>[%02d:%02d:%02d]\r\n", t.Hour, t.Minute, t.Second);

    t.Year = CalendarYrToTm(year);
    
  //DebugTf("converted=>[%d-%02d-%02d @ %02d:%02d:%02d]\r\n", t.Year, t.Month, t.Day, t.Hour, t.Minute, t.Second);

    return makeTime(t);
    
} // dateTime2Epoch()

#endif
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\processTelegram.ino"
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
  DebugTf("Telegram[%d]=>DSMRdata.timestamp[%s]\r\n", telegramCount
                                                    , DSMRdata.timestamp.c_str());

//----- update OLED display ---------
  if (settingOledType > 0)
  {
    String DT   = buildDateTimeString(DSMRdata.timestamp.c_str(), sizeof(DSMRdata.timestamp));

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
    actT = epoch(actTimestamp, strlen(actTimestamp), false);   // update system time
    return;
  }
  
  DebugTf("actHour[%02d] -- newHour[%02d]\r\n", hour(actT), hour(newT));
  //--- if we have a new hour() update the previous hour
  if (hour(actT) != hour(newT)) {
    writeToSysLog("actHour[%02d] -- newHour[%02d]", hour(actT), hour(newT));
  }
  //--- has the hour changed (or the day or month)  
  //--- in production testing on hour only would
  //--- suffice, but in testing I need all three
  //--- if we have a new hour() update the previous hour(actT)
  if (     (hour(actT) != hour(newT)  ) 
       ||   (day(actT) != day(newT)   ) 
       || (month(actT) != month(newT) ) )
  {
    writeToSysLog("Update RING-files");
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
    else  //--- NO, only the hour has changed
    {
      char      record[DATA_RECLEN + 1] = "";
      //--- actTimestamp := newTimestamp
      strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
      buildDataRecordFromSM(record);
      uint16_t recSlot = timestampToHourSlot(actTimestamp, strlen(actTimestamp));
      //--- and update the files with the actTimestamp
      writeDataToFile(HOURS_FILE, record, recSlot, HOURS);
      DebugTf(">%s\r\n", record); // record ends in a \n
    }
  }

  if ( DUE(publishMQTTtimer) )
  {
    sendMQTTData();      
  }    

  strCopy(actTimestamp, sizeof(actTimestamp), newTimestamp);
  actT = epoch(actTimestamp, strlen(actTimestamp), true);   // update system time

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\restAPI.ino"
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

// ******* Global Vars *******
uint32_t  antiWearTimer = 0;

char fieldName[40] = "";

char fieldsArray[50][35] = {{0}}; // to lookup fields 
int  fieldsElements      = 0;

int  actualElements = 20;
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
int  infoElements = 7;
char infoArray[][35]   = { "identification","p1_version","equipment_id","electricity_tariff"
                          ,"gas_device_type","gas_equipment_id"
                          , "\0" };
  
bool onlyIfPresent  = false;

//=======================================================================
void processAPI() 
{
  char fName[40] = "";
  char URI[50]   = "";
  String words[10];

  strncpy( URI, httpServer.uri().c_str(), sizeof(URI) );

  if (httpServer.method() == HTTP_GET)
        DebugTf("from[%s] URI[%s] method[GET] \r\n"
                                  , httpServer.client().remoteIP().toString().c_str()
                                        , URI); 
  else  DebugTf("from[%s] URI[%s] method[PUT] \r\n" 
                                  , httpServer.client().remoteIP().toString().c_str()
                                        , URI); 

#ifdef USE_SYSLOGGER
  if (ESP.getFreeHeap() < 5000) // to prevent firmware from crashing!
#else
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
#endif
  {
    DebugTf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap() );
    writeToSysLog("from[%s][%s] Bailout low heap (%d bytes)"
                                  , httpServer.client().remoteIP().toString().c_str()
                                  , URI
                                  , ESP.getFreeHeap() );
    httpServer.send(500, "text/plain", "500: internal server error (low heap)\r\n"); 
    return;
  }

  int8_t wc = splitString(URI, '/', words, 10);
  
  if (Verbose2) 
  {
    DebugT(">>");
    for (int w=0; w<wc; w++)
    {
      Debugf("word[%d] => [%s], ", w, words[w].c_str());
    }
    Debugln(" ");
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
      String jsonIn  = httpServer.arg(0).c_str();
      char field[25] = "";
      char newValue[101]="";
      jsonIn.replace("{", "");
      jsonIn.replace("}", "");
      jsonIn.replace("\"", "");
      int8_t wp = splitString(jsonIn.c_str(), ',',  wPair, 5) ;
      for (int i=0; i<wp; i++)
      {
        //DebugTf("[%d] -> pair[%s]\r\n", i, wPair[i].c_str());
        int8_t wc = splitString(wPair[i].c_str(), ':',  wOut, 5) ;
        //DebugTf("==> [%s] -> field[%s]->val[%s]\r\n", wPair[i].c_str(), wOut[0].c_str(), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("name"))  strCopy(field, sizeof(field), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("value")) strCopy(newValue, sizeof(newValue), wOut[1].c_str());
      }
      //DebugTf("--> field[%s] => newValue[%s]\r\n", field, newValue);
      updateSetting(field, newValue);
      httpServer.send(200, "application/json", httpServer.arg(0));
      writeToSysLog("DSMReditor: Field[%s] changed to [%s]", field, newValue);
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
  int8_t  fileType     = 0;
  char    fileName[20] = "";
  
  //DebugTf("word4[%s], word5[%s], word6[%s]\r\n", word4, word5, word6);
  if (   strcmp(word4, "hours") == 0 )
  {
    fileType = HOURS;
    strCopy(fileName, sizeof(fileName), HOURS_FILE);
  }
  else if (strcmp(word4, "days") == 0 )
  {
    fileType = DAYS;
    strCopy(fileName, sizeof(fileName), DAYS_FILE);
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
      char      record[DATA_RECLEN + 1] = "";
      uint16_t  recSlot;

      String jsonIn  = httpServer.arg(0).c_str();
      DebugTln(jsonIn);
      
      recSlot = buildDataRecordFromJson(record, jsonIn);
      
      //--- update MONTHS
      writeDataToFile(MONTHS_FILE, record, recSlot, MONTHS);
      //--- send OK response --
      httpServer.send(200, "application/json", httpServer.arg(0));
      
      return;
    }
    else 
    {
      strCopy(fileName, sizeof(fileName), MONTHS_FILE);
    }
  }
  else 
  {
    sendApiNotFound(URI);
    return;
  }
  if (strcmp(word5, "desc") == 0)
        sendJsonHist(fileType, fileName, actTimestamp, true);
  else  sendJsonHist(fileType, fileName, actTimestamp, false);

} // handleHistApi()


//====================================================
void handleSmApi(const char *URI, const char *word4, const char *word5, const char *word6)
{
  char    tlgrm[1200] = "";
  uint8_t p=0;  
  bool    stopParsingTelegram = false;

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
    Serial.setTimeout(5000);  // 5 seconds must be enough ..
    memset(tlgrm, 0, sizeof(tlgrm));
    int l = 0;
    // The terminator character is discarded from the serial buffer.
    l = Serial.readBytesUntil('/', tlgrm, sizeof(tlgrm));
    // now read from '/' to '!'
    // The terminator character is discarded from the serial buffer.
    l = Serial.readBytesUntil('!', tlgrm, sizeof(tlgrm));
    Serial.setTimeout(1000);  // seems to be the default ..
    DebugTf("read [%d] bytes\r\n", l);
    if (l == 0) 
    {
      httpServer.send(200, "application/plain", "no telegram received");
      showRaw = false;
      return;
    }

    tlgrm[l++] = '!';
#if !defined( USE_PRE40_PROTOCOL )
    // next 6 bytes are "<CRC>\r\n"
    for (int i=0; ( i<6 && (i<(sizeof(tlgrm)-7)) ); i++)
    {
      tlgrm[l++] = (char)Serial.read();
    }
#else
    tlgrm[l++]    = '\r';
    tlgrm[l++]    = '\n';
#endif
    tlgrm[(l +1)] = '\0';
    // shift telegram 1 char to the right (make room at pos [0] for '/')
    for (int i=strlen(tlgrm); i>=0; i--) { tlgrm[i+1] = tlgrm[i]; yield(); }
    tlgrm[0] = '/'; 
    showRaw = false;
    if (Verbose1) Debugf("Telegram (%d chars):\r\n/%s", strlen(tlgrm), tlgrm);
    httpServer.send(200, "application/plain", tlgrm);

  }
  else sendApiNotFound(URI);
  
} // handleSmApi()


//=======================================================================
void sendDeviceInfo() 
{
  char compileOptions[200] = "";

#ifdef USE_REQUEST_PIN
    strConcat(compileOptions, sizeof(compileOptions), "[USE_REQUEST_PIN]");
#endif
#if defined( USE_PRE40_PROTOCOL )
    strConcat(compileOptions, sizeof(compileOptions), "[USE_PRE40_PROTOCOL]");
#elif defined( USE_BELGIUM_PROTOCOL )
    strConcat(compileOptions, sizeof(compileOptions), "[USE_BELGIUM_PROTOCOL]");
#else
    strConcat(compileOptions, sizeof(compileOptions), "[USE_DUTCH_PROTOCOL]");
#endif
#ifdef USE_UPDATE_SERVER
    strConcat(compileOptions, sizeof(compileOptions), "[USE_UPDATE_SERVER]");
#endif
#ifdef USE_MQTT
    strConcat(compileOptions, sizeof(compileOptions), "[USE_MQTT]");
#endif
#ifdef USE_MINDERGAS
    strConcat(compileOptions, sizeof(compileOptions), "[USE_MINDERGAS]");
#endif
#ifdef USE_SYSLOGGER
    strConcat(compileOptions, sizeof(compileOptions), "[USE_SYSLOGGER]");
#endif
#ifdef USE_NTP_TIME
    strConcat(compileOptions, sizeof(compileOptions), "[USE_NTP_TIME]");
#endif

  sendStartJsonObj("devinfo");

  sendNestedJsonObj("author", "Willem Aandewiel (www.aandewiel.nl)");
  sendNestedJsonObj("fwversion", _FW_VERSION);

  snprintf(cMsg, sizeof(cMsg), "%s %s", __DATE__, __TIME__);
  sendNestedJsonObj("compiled", cMsg);
  sendNestedJsonObj("hostname", settingHostname);
  sendNestedJsonObj("ipaddress", WiFi.localIP().toString().c_str());
  sendNestedJsonObj("macaddress", WiFi.macAddress().c_str());
  sendNestedJsonObj("indexfile", settingIndexPage);
  sendNestedJsonObj("freeheap", ESP.getFreeHeap(), "bytes");
  sendNestedJsonObj("maxfreeblock", ESP.getMaxFreeBlockSize(), "bytes");
  sendNestedJsonObj("chipid", String( ESP.getChipId(), HEX ).c_str());
  sendNestedJsonObj("coreversion", String( ESP.getCoreVersion() ).c_str() );
  sendNestedJsonObj("sdkversion", String( ESP.getSdkVersion() ).c_str());
  sendNestedJsonObj("cpufreq", ESP.getCpuFreqMHz(), "MHz");
  sendNestedJsonObj("sketchsize", formatFloat( (ESP.getSketchSize() / 1024.0), 3), "kB");
  sendNestedJsonObj("freesketchspace", formatFloat( (ESP.getFreeSketchSpace() / 1024.0), 3), "kB");

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        snprintf(cMsg, sizeof(cMsg), "%08X (PUYA)", ESP.getFlashChipId());
  else  snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());
  sendNestedJsonObj("flashchipid", cMsg);  // flashChipId
  sendNestedJsonObj("flashchipsize", formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3), "MB");
  sendNestedJsonObj("flashchiprealsize", formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3), "MB");

  SPIFFS.info(SPIFFSinfo);
  sendNestedJsonObj("spiffssize", formatFloat( (SPIFFSinfo.totalBytes / (1024.0 * 1024.0)), 0), "MB");

  sendNestedJsonObj("flashchipspeed", formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0), "MHz");

  FlashMode_t ideMode = ESP.getFlashChipMode();
  sendNestedJsonObj("flashchipmode ", flashMode[ideMode]);
  sendNestedJsonObj("boardtype",
#ifdef ARDUINO_ESP8266_NODEMCU
     "ESP8266_NODEMCU"
#endif
#ifdef ARDUINO_ESP8266_GENERIC
     "ESP8266_GENERIC"
#endif
#ifdef ESP8266_ESP01
     "ESP8266_ESP01"
#endif
#ifdef ESP8266_ESP12
     "ESP8266_ESP12"
#endif
  );
  sendNestedJsonObj("compileoptions", compileOptions);
  sendNestedJsonObj("ssid", WiFi.SSID().c_str());
#ifdef SHOW_PASSWRDS
  sendNestedJsonObj("pskkey", WiFi.psk());   
#endif
  sendNestedJsonObj("wifirssi", WiFi.RSSI());
  sendNestedJsonObj("uptime", upTime());
  sendNestedJsonObj("oled_type",        (int)settingOledType);
  sendNestedJsonObj("oled_flip_screen", (int)settingOledFlip);
  sendNestedJsonObj("smhasfaseinfo",    (int)settingSmHasFaseInfo);
  sendNestedJsonObj("telegraminterval", (int)settingTelegramInterval);
  sendNestedJsonObj("telegramcount",    (int)telegramCount);
  sendNestedJsonObj("telegramerrors",   (int)telegramErrors);

#ifdef USE_MQTT
  snprintf(cMsg, sizeof(cMsg), "%s:%04d", settingMQTTbroker, settingMQTTbrokerPort);
  sendNestedJsonObj("mqttbroker", cMsg);
  sendNestedJsonObj("mqttinterval", settingMQTTinterval);
  if (mqttIsConnected)
        sendNestedJsonObj("mqttbroker_connected", "yes");
  else  sendNestedJsonObj("mqttbroker_connected", "no");
#endif
#ifdef USE_MINDERGAS
  snprintf(cMsg, sizeof(cMsg), "%s:%d", timeLastResponse, intStatuscodeMindergas);
  sendNestedJsonObj("mindergas_response",     txtResponseMindergas);
  sendNestedJsonObj("mindergas_status",       cMsg);
#endif

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
  DebugTln("sending device settings ...\r");

  sendStartJsonObj("settings");
  
  sendJsonSettingObj("hostname",          settingHostname,        "s", sizeof(settingHostname) -1);
  sendJsonSettingObj("ed_tariff1",        settingEDT1,            "f", 0, 10,  5);
  sendJsonSettingObj("ed_tariff2",        settingEDT2,            "f", 0, 10,  5);
  sendJsonSettingObj("er_tariff1",        settingERT1,            "f", 0, 10,  5);
  sendJsonSettingObj("er_tariff2",        settingERT2,            "f", 0, 10,  5);
  sendJsonSettingObj("gd_tariff",         settingGDT,             "f", 0, 10,  5);
  sendJsonSettingObj("electr_netw_costs", settingENBK,            "f", 0, 100, 2);
  sendJsonSettingObj("gas_netw_costs",    settingGNBK,            "f", 0, 100, 2);
  sendJsonSettingObj("sm_has_fase_info",  settingSmHasFaseInfo,   "i", 0, 1);
  sendJsonSettingObj("tlgrm_interval",    settingTelegramInterval,"i", 2, 60);
  sendJsonSettingObj("oled_type",         settingOledType,        "i", 0, 2);
  sendJsonSettingObj("oled_screen_time",  settingOledSleep,       "i", 1, 300);
  sendJsonSettingObj("oled_flip_screen",  settingOledFlip,        "i", 0, 1);
  sendJsonSettingObj("index_page",        settingIndexPage,       "s", sizeof(settingIndexPage) -1);
  sendJsonSettingObj("mqtt_broker",       settingMQTTbroker,      "s", sizeof(settingMQTTbroker) -1);
  sendJsonSettingObj("mqtt_broker_port",  settingMQTTbrokerPort,  "i", 1, 9999);
  sendJsonSettingObj("mqtt_user",         settingMQTTuser,        "s", sizeof(settingMQTTuser) -1);
  sendJsonSettingObj("mqtt_passwd",       settingMQTTpasswd,      "s", sizeof(settingMQTTpasswd) -1);
  sendJsonSettingObj("mqtt_toptopic",     settingMQTTtopTopic,    "s", sizeof(settingMQTTtopTopic) -1);
  sendJsonSettingObj("mqtt_interval",     settingMQTTinterval,    "i", 0, 600);
#if defined (USE_MINDERGAS )
  sendJsonSettingObj("mindergastoken",  settingMindergasToken,    "s", sizeof(settingMindergasToken) -1);
#endif

  sendEndJsonObj();

} // sendDeviceSettings()


//=======================================================================
void sendDeviceDebug(const char *URI, String tail) 
{
#ifdef USE_SYSLOGGER
  String lLine = "";
  int lineNr = 0;
  int tailLines = tail.toInt();

  DebugTf("list [%d] debug lines\r\n", tailLines);
  sysLog.status();
  sysLog.setDebugLvl(0);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  if (tailLines > 0)
        sysLog.startReading((tailLines * -1));  
  else  sysLog.startReading(0, 0);  
  while( (lLine = sysLog.readNextLine()) && !(lLine == "EOF")) 
  {
    lineNr++;
    snprintf(cMsg, sizeof(cMsg), "%s\r\n", lLine.c_str());
    httpServer.sendContent(cMsg);

  }
  sysLog.setDebugLvl(1);

#else
  sendApiNotFound(URI);
#endif

} // sendDeviceDebug()

//=======================================================================
struct buildJsonApiV0SmActual
{
    bool  skip = false;
    
    template<typename Item>
    void apply(Item &i) {
      skip = false;
      String Name = Item::name;
      //-- for dsmr30 -----------------------------------------------
#if defined( USE_PRE40_PROTOCOL )
      if (Name.indexOf("gas_delivered2") == 0) Name = "gas_delivered";
#endif
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

};  // buildJsonApiV0SmActual()


//=======================================================================
void sendJsonV0Fields() 
{
  objSprtr[0]    = '\0';
  
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send(200, "application/json", "{\r\n");
  DSMRdata.applyEach(buildJsonApiV0SmActual());
  httpServer.sendContent("\r\n}\r\n");

} // sendJsonV0Fields()


//=======================================================================
struct buildJsonApi 
{
    bool  skip = false;
    
    template<typename Item>
    void apply(Item &i) {
      skip = false;
      String Name = Item::name;
      //-- for dsmr30 -----------------------------------------------
#if defined( USE_PRE40_PROTOCOL )
      if (Name.indexOf("gas_delivered2") == 0) Name = "gas_delivered";
#endif
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

};  // buildJsonApi()


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
  uint8_t startSlot, nrSlots, recNr  = 0;
  char    typeApi[10];


  if (DUE(antiWearTimer))
  {
    writeDataToFiles();
    writeLastStatus();
  }
    
  switch(fileType) {
    case HOURS:   startSlot       = timestampToHourSlot(timeStamp, strlen(timeStamp));
                  nrSlots         = _NO_HOUR_SLOTS_;
                  strCopy(typeApi, 9, "hours");
                  break;
    case DAYS:    startSlot       = timestampToDaySlot(timeStamp, strlen(timeStamp));
                  nrSlots         = _NO_DAY_SLOTS_;
                  strCopy(typeApi, 9, "days");
                  break;
    case MONTHS:  startSlot       = timestampToMonthSlot(timeStamp, strlen(timeStamp));
                  nrSlots         = _NO_MONTH_SLOTS_;
                  strCopy(typeApi, 9, "months");
                  break;
  }

  sendStartJsonObj(typeApi);

  if (desc)
        startSlot += nrSlots +1; // <==== voorbij actuele slot!
  else  startSlot += nrSlots;    // <==== start met actuele slot!

  DebugTf("sendJsonHist startSlot[%02d]\r\n", (startSlot % nrSlots));

  for (uint8_t s = 0; s < nrSlots; s++)
  {
    if (desc)
          readOneSlot(fileType, fileName, s, (s +startSlot), true, "hist") ;
    else  readOneSlot(fileType, fileName, s, (startSlot -s), true, "hist") ;
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
    DebugTf("[%2d] => inArray[%s]\r\n", i, inArray[i]); 
  }
  
} // listFieldsArray()


//====================================================
void sendApiNotFound(const char *URI)
{
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send ( 404, "text/html", "<!DOCTYPE HTML><html><head>");

  strCopy(cMsg,   sizeof(cMsg), "<style>body { background-color: lightgray; font-size: 15pt;}");
  strConcat(cMsg, sizeof(cMsg), "</style></head><body>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg,   sizeof(cMsg), "<h1>DSMR-logger</h1><b1>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg,   sizeof(cMsg), "<br>[<b>");
  strConcat(cMsg, sizeof(cMsg), URI);
  strConcat(cMsg, sizeof(cMsg), "</b>] is not a valid ");
  httpServer.sendContent(cMsg);
  
  strCopy(cMsg,   sizeof(cMsg), "<a href=");
  strConcat(cMsg, sizeof(cMsg), "\"https://mrwheel-docs.gitbook.io/dsmrloggerapi/beschrijving-restapis\">");
  strConcat(cMsg, sizeof(cMsg), "restAPI</a> call.");
  httpServer.sendContent(cMsg);
  
  strCopy(cMsg, sizeof(cMsg), "</body></html>\r\n");
  httpServer.sendContent(cMsg);

  writeToSysLog("[%s] is not a valid restAPI call!!", URI);
  
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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\settingsStuff.ino"
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
  DebugT(F("Writing to [")); Debug(SETTINGS_FILE); Debugln(F("] ..."));
  File file = SPIFFS.open(SETTINGS_FILE, "w"); // open for reading and writing
  if (!file) 
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", SETTINGS_FILE);
    return;
  }
  yield();

  if (strlen(settingIndexPage) < 7) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), "DSMRindex.html");
  if (settingTelegramInterval < 2)  settingTelegramInterval = 10;
  if (settingMQTTbrokerPort < 1)    settingMQTTbrokerPort = 1883;
    
  DebugT(F("Start writing setting data "));

  file.print("Hostname = ");          file.println(settingHostname);            Debug(F("."));
  file.print("EnergyDeliveredT1 = "); file.println(String(settingEDT1, 5));     Debug(F("."));
  file.print("EnergyDeliveredT2 = "); file.println(String(settingEDT2, 5));     Debug(F("."));
  file.print("EnergyReturnedT1 = ");  file.println(String(settingERT1, 5));     Debug(F("."));
  file.print("EnergyReturnedT2 = ");  file.println(String(settingERT2, 5));     Debug(F("."));
  file.print("GASDeliveredT = ");     file.println(String(settingGDT,  5));     Debug(F("."));
  file.print("EnergyVasteKosten = "); file.println(String(settingENBK, 2));     Debug(F("."));
  file.print("GasVasteKosten = ");    file.println(String(settingGNBK, 2));     Debug(F("."));
  file.print("OledType = ");          file.println(settingOledType);            Debug(F("."));
  file.print("OledSleep = ");         file.println(settingOledSleep);           Debug(F("."));
  file.print("OledFlip = ");          file.println(settingOledFlip);            Debug(F("."));
  file.print("SmHasFaseInfo = ");     file.println(settingSmHasFaseInfo);       Debug(F("."));

  file.print("TelegramInterval = ");  file.println(settingTelegramInterval);    Debug(F("."));
  file.print("IndexPage = ");         file.println(settingIndexPage);           Debug(F("."));

#ifdef USE_MQTT
  //sprintf(settingMQTTbroker, "%s:%d", MQTTbroker, MQTTbrokerPort);
  file.print("MQTTbroker = ");        file.println(settingMQTTbroker);          Debug(F("."));
  file.print("MQTTbrokerPort = ");    file.println(settingMQTTbrokerPort);      Debug(F("."));
  file.print("MQTTUser = ");          file.println(settingMQTTuser);            Debug(F("."));
  file.print("MQTTpasswd = ");        file.println(settingMQTTpasswd);          Debug(F("."));
  file.print("MQTTinterval = ");      file.println(settingMQTTinterval);        Debug(F("."));
  file.print("MQTTtopTopic = ");      file.println(settingMQTTtopTopic);        Debug(F("."));
#endif
  
#ifdef USE_MINDERGAS
  file.print("MindergasAuthtoken = ");file.println(settingMindergasToken);  Debug(F("."));
#endif

file.close();  
  
  Debugln(F(" done"));
  if (Verbose1) 
  {
    DebugTln(F("Wrote this:"));
    DebugT(F("EnergyDeliveredT1 = ")); Debugln(String(settingEDT1, 5));     
    DebugT(F("EnergyDeliveredT2 = ")); Debugln(String(settingEDT2, 5));     
    DebugT(F("EnergyReturnedT1 = "));  Debugln(String(settingERT1, 5));     
    DebugT(F("EnergyReturnedT2 = "));  Debugln(String(settingERT2, 5));     
    DebugT(F("GASDeliveredT = "));     Debugln(String(settingGDT,  5));     
    DebugT(F("EnergyVasteKosten = ")); Debugln(String(settingENBK, 2));    
    DebugT(F("GasVasteKosten = "));    Debugln(String(settingGNBK, 2));    
    DebugT(F("OledType = "));
    if (settingOledType == 1)          Debugln("SDD1306");
    else if (settingOledType == 2)     Debugln("SH1306");
    else                               Debugln("None");
    DebugT(F("OledSleep = "));         Debugln(settingOledSleep);           
    DebugT(F("OledFlip = "));
    if (settingOledFlip)  Debugln(F("Yes"));
    else                  Debugln(F("No"));

    DebugT(F("SmHasFaseInfo")); 
    if (settingSmHasFaseInfo == 1)     Debugln("Yes");
    else                               Debugln("No");
    DebugT(F("TelegramInterval = "));  Debugln(settingTelegramInterval);            
    DebugT(F("IndexPage = "));         Debugln(settingIndexPage);             

#ifdef USE_MQTT
    DebugT(F("MQTTbroker = "));        Debugln(settingMQTTbroker);          
    DebugT(F("MQTTbrokerPort = "));    Debugln(settingMQTTbrokerPort);          
    DebugT(F("MQTTUser = "));          Debugln(settingMQTTuser);     
  #ifdef SHOW_PASSWRDS       
      DebugT(F("MQTTpasswd = "));        Debugln(settingMQTTpasswd);  
  #else 
      DebugTln(F("MQTTpasswd = ********"));  
  #endif       
    DebugT(F("MQTTinterval = "));      Debugln(settingMQTTinterval);        
    DebugT(F("MQTTtopTopic = "));      Debugln(settingMQTTtopTopic);   
#endif
  
#ifdef USE_MINDERGAS
  #ifdef SHOW_PASSWRDS   
    DebugT(F("MindergasAuthtoken = "));Debugln(settingMindergasToken);
  #else
    DebugTln(F("MindergasAuthtoken = ********")); 
  #endif
#endif
  } // Verbose1
  
} // writeSettings()


//=======================================================================
void readSettings(bool show) 
{
  String sTmp, nColor;
  String words[10];
  
  File file;
  
  DebugTf(" %s ..\r\n", SETTINGS_FILE);

  snprintf(settingHostname, sizeof(settingHostname), "%s", _DEFAULT_HOSTNAME);
  settingEDT1               = 0.1;
  settingEDT2               = 0.2;
  settingERT1               = 0.3;
  settingERT2               = 0.4;
  settingGDT                = 0.5;
  settingENBK               = 15.15;
  settingGNBK               = 11.11;
  settingSmHasFaseInfo      =  1; // default: it does
  settingTelegramInterval   = 10; // seconds
  settingOledType           =  1; // 0=None, 1=SDD1306, 2=SH1106
  settingOledSleep          =  0; // infinite
  settingOledFlip           =  0; // Don't flip
  strCopy(settingIndexPage, sizeof(settingIndexPage), "DSMRindex.html");
  settingMQTTbroker[0]     = '\0';
  settingMQTTbrokerPort    = 1883;
  settingMQTTuser[0]       = '\0';
  settingMQTTpasswd[0]     = '\0';
  settingMQTTinterval      =  0;
  snprintf(settingMQTTtopTopic, sizeof(settingMQTTtopTopic), "%s", settingHostname);

#ifdef USE_MINDERGAS
  settingMindergasToken[0] = '\0';
#endif

  if (!SPIFFS.exists(SETTINGS_FILE)) 
  {
    DebugTln(F(" .. file not found! --> created file!"));
    writeSettings();
  }

  for (int T = 0; T < 2; T++) 
  {
    file = SPIFFS.open(SETTINGS_FILE, "r");
    if (!file) 
    {
      if (T == 0) DebugTf(" .. something went wrong opening [%s]\r\n", SETTINGS_FILE);
      else        DebugT(T);
      delay(100);
    }
  } // try T times ..

  DebugTln(F("Reading settings:\r"));
  while(file.available()) 
  {
    sTmp      = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //DebugTf("[%s] (%d)\r\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();
    nColor    = words[1].substring(0,15);

    if (words[0].equalsIgnoreCase("Hostname"))            strCopy(settingHostname, 29, words[1].c_str());
    if (words[0].equalsIgnoreCase("EnergyDeliveredT1"))   settingEDT1         = strToFloat(words[1].c_str(), 5);  
    if (words[0].equalsIgnoreCase("EnergyDeliveredT2"))   settingEDT2         = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyReturnedT1"))    settingERT1         = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("EnergyReturnedT2"))    settingERT2         = strToFloat(words[1].c_str(), 5);
    if (words[0].equalsIgnoreCase("GasDeliveredT"))       settingGDT          = strToFloat(words[1].c_str(), 5); 
    if (words[0].equalsIgnoreCase("EnergyVasteKosten"))   settingENBK         = strToFloat(words[1].c_str(), 2);
    if (words[0].equalsIgnoreCase("GasVasteKosten"))      settingGNBK         = strToFloat(words[1].c_str(), 2);
    if (words[0].equalsIgnoreCase("SmHasFaseInfo")) 
    {
      settingSmHasFaseInfo = words[1].toInt();
      if (settingSmHasFaseInfo != 0)  settingSmHasFaseInfo = 1;
      else                            settingSmHasFaseInfo = 0;
    }
    
    if (words[0].equalsIgnoreCase("OledType"))           
    {
      settingOledType = words[1].toInt();
      if (settingOledType > 2) settingOledType = 1;
    }
    if (words[0].equalsIgnoreCase("OledSleep"))           
    {
      settingOledSleep    = words[1].toInt();    
      CHANGE_INTERVAL_MIN(oledSleepTimer, settingOledSleep);
    }
    if (words[0].equalsIgnoreCase("OledFlip"))    settingOledFlip = words[1].toInt();
    if (settingOledFlip != 0) settingOledFlip = 1;
    else                      settingOledFlip = 0;
    
    if (words[0].equalsIgnoreCase("TelegramInterval"))   
    {
      settingTelegramInterval     = words[1].toInt(); 
      CHANGE_INTERVAL_SEC(nextTelegram, settingTelegramInterval); 
    }

    if (words[0].equalsIgnoreCase("IndexPage"))           strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), words[1].c_str());  

#ifdef USE_MINDERGAS
    if (words[0].equalsIgnoreCase("MindergasAuthtoken"))  strCopy(settingMindergasToken, 20, words[1].c_str());  
#endif

#ifdef USE_MQTT
    if (words[0].equalsIgnoreCase("MQTTbroker"))  
    {
      memset(settingMQTTbroker, '\0', sizeof(settingMQTTbroker));
      strCopy(settingMQTTbroker, 100, words[1].c_str());
    }
    if (words[0].equalsIgnoreCase("MQTTbrokerPort"))      settingMQTTbrokerPort    = words[1].toInt();  
    if (words[0].equalsIgnoreCase("MQTTuser"))            strCopy(settingMQTTuser    ,35 ,words[1].c_str());  
    if (words[0].equalsIgnoreCase("MQTTpasswd"))          strCopy(settingMQTTpasswd  ,25, words[1].c_str());  
    if (words[0].equalsIgnoreCase("MQTTinterval"))        settingMQTTinterval        = words[1].toInt(); 
    if (words[0].equalsIgnoreCase("MQTTtopTopic"))        strCopy(settingMQTTtopTopic, 20, words[1].c_str());  
    
    CHANGE_INTERVAL_SEC(publishMQTTtimer, settingMQTTinterval);
    CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 1);
#endif
    
  } // while available()
  
  file.close();  

  //--- this will take some time to settle in
  //--- probably need a reboot before that to happen :-(
  MDNS.setHostname(settingHostname);    // start advertising with new(?) settingHostname

  DebugTln(F(" .. done\r"));


  if (strlen(settingIndexPage) < 7) strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), "DSMRindex.html");
  if (settingTelegramInterval  < 2) settingTelegramInterval = 10;
  if (settingMQTTbrokerPort    < 1) settingMQTTbrokerPort   = 1883;

  if (!show) return;
  
  Debugln(F("\r\n==== Settings ===================================================\r"));
  Debugf("                    Hostname : %s\r\n",     settingHostname);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  settingEDT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  settingEDT2);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  settingERT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  settingERT2);
  Debugf("        Gas Delivered Tarief : %9.7f\r\n",  settingGDT);
  Debugf("     Energy Netbeheer Kosten : %9.2f\r\n",  settingENBK);
  Debugf("        Gas Netbeheer Kosten : %9.2f\r\n",  settingGNBK);
  Debugf("  SM Fase Info (0=No, 1=Yes) : %d\r\n",     settingSmHasFaseInfo);
  Debugf("   Telegram Process Interval : %d\r\n",     settingTelegramInterval);
  Debugf("         OLED Type (0, 1, 2) : %d\r\n",     settingOledType);
  Debugf("OLED Sleep Min. (0=oneindig) : %d\r\n",     settingOledSleep);
  Debugf("     Flip Oled (0=No, 1=Yes) : %d\r\n",     settingOledFlip);
  Debugf("                  Index Page : %s\r\n",     settingIndexPage);

#ifdef USE_MQTT
  Debugln(F("\r\n==== MQTT settings ==============================================\r"));
  Debugf("          MQTT broker URL/IP : %s:%d", settingMQTTbroker, settingMQTTbrokerPort);
  if (MQTTclient.connected()) Debugln(F(" (is Connected!)\r"));
  else                 Debugln(F(" (NOT Connected!)\r"));
  Debugf("                   MQTT user : %s\r\n", settingMQTTuser);
#ifdef SHOW_PASSWRDS
  Debugf("               MQTT password : %s\r\n", settingMQTTpasswd);
#else
  Debug( "               MQTT password : *************\r\n");
#endif
  Debugf("          MQTT send Interval : %d\r\n", settingMQTTinterval);
  Debugf("              MQTT top Topic : %s\r\n", settingMQTTtopTopic);
#endif  // USE_MQTT
#ifdef USE_MINDERGAS
  Debugln(F("\r\n==== Mindergas settings ==============================================\r"));
  Debugf("         Mindergas Authtoken : %s\r\n", settingMindergasToken);
#endif  
  
  Debugln(F("-\r"));

} // readSettings()


//=======================================================================
void updateSetting(const char *field, const char *newValue)
{
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (!stricmp(field, "Hostname")) {
    strCopy(settingHostname, 29, newValue); 
    if (strlen(settingHostname) < 1) strCopy(settingHostname, 29, _DEFAULT_HOSTNAME); 
    char *dotPntr = strchr(settingHostname, '.') ;
    if (dotPntr != NULL)
    {
      byte dotPos = (dotPntr-settingHostname);
      if (dotPos > 0)  settingHostname[dotPos] = '\0';
    }
    Debugln();
    DebugTf("Need reboot before new %s.local will be available!\r\n\n", settingHostname);
  }
  if (!stricmp(field, "ed_tariff1"))        settingEDT1         = String(newValue).toFloat();  
  if (!stricmp(field, "ed_tariff2"))        settingEDT2         = String(newValue).toFloat();  
  if (!stricmp(field, "er_tariff1"))        settingERT1         = String(newValue).toFloat();  
  if (!stricmp(field, "er_tariff2"))        settingERT2         = String(newValue).toFloat();  
  if (!stricmp(field, "electr_netw_costs")) settingENBK         = String(newValue).toFloat();

  if (!stricmp(field, "gd_tariff"))         settingGDT          = String(newValue).toFloat();  
  if (!stricmp(field, "gas_netw_costs"))    settingGNBK         = String(newValue).toFloat();

  if (!stricmp(field, "sm_has_fase_info")) 
  {
    settingSmHasFaseInfo = String(newValue).toInt(); 
    if (settingSmHasFaseInfo != 0)  settingSmHasFaseInfo = 1;
    else                            settingSmHasFaseInfo = 0;  
  }

  if (!stricmp(field, "oled_type"))
  {
    settingOledType     = String(newValue).toInt();  
    if (settingOledType > 2)  settingOledType = 1;
    oled_Init();
  }
  if (!stricmp(field, "oled_screen_time")) 
  {
    settingOledSleep    = String(newValue).toInt();  
    CHANGE_INTERVAL_MIN(oledSleepTimer, settingOledSleep)
  }
  if (!stricmp(field, "oled_flip_screen"))
  {
    settingOledFlip     = String(newValue).toInt();  
    if (settingOledFlip != 0) settingOledFlip = 1;
    else                      settingOledFlip = 0;
    oled_Init();
  }
  
  if (!stricmp(field, "tlgrm_interval"))    
  {
    settingTelegramInterval     = String(newValue).toInt();  
    CHANGE_INTERVAL_SEC(nextTelegram, settingTelegramInterval)
  }

  if (!stricmp(field, "index_page"))        strCopy(settingIndexPage, (sizeof(settingIndexPage) -1), newValue);  

#ifdef USE_MINDERGAS
  if (!stricmp(field, "MindergasToken"))    strCopy(settingMindergasToken, 20, newValue);  
#endif //USE_MINDERGAS

#ifdef USE_MQTT
  if (!stricmp(field, "mqtt_broker"))  {
    DebugT("settingMQTTbroker! to : ");
    memset(settingMQTTbroker, '\0', sizeof(settingMQTTbroker));
    strCopy(settingMQTTbroker, 100, newValue);
    Debugf("[%s]\r\n", settingMQTTbroker);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_broker_port")) {
    settingMQTTbrokerPort = String(newValue).toInt();  
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_user")) {
    strCopy(settingMQTTuser    ,35, newValue);  
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_passwd")) {
    strCopy(settingMQTTpasswd  ,25, newValue);  
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!stricmp(field, "mqtt_interval")) {
    settingMQTTinterval   = String(newValue).toInt();  
    CHANGE_INTERVAL_SEC(publishMQTTtimer, settingMQTTinterval);
  }
  if (!stricmp(field, "mqtt_toptopic"))     strCopy(settingMQTTtopTopic, 20, newValue);  
#endif

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

#line 1 "f:\\UsersLeoDocuments\\DIY\\Arduino\\Projects\\DSMRloggerAPI_LittleFS\\timeStuff.ino"
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
  DateTime   = "20" + tmpTS.substring(0, 2);    // YY
  DateTime  += "-"  + tmpTS.substring(2, 4);    // MM
  DateTime  += "-"  + tmpTS.substring(4, 6);    // DD
  DateTime  += " "  + tmpTS.substring(6, 8);    // HH
  DateTime  += ":"  + tmpTS.substring(8, 10);   // MM
  DateTime  += ":"  + tmpTS.substring(10, 12);  // SS
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
  if (Verbose2) DebugTf("epoch(%s) strlen([%d])\r\n", fullTimeStamp, strlen(fullTimeStamp));  
  switch(strlen(fullTimeStamp)) {
    case  4:  //--- timeStamp is YYMM
              strConcat(fullTimeStamp, 15, "01010101X");
              break;
    case  6:  //--- timeStamp is YYMMDD
              strConcat(fullTimeStamp, 15, "010101X");
              break;
    case  8:  //--- timeStamp is YYMMDDHH
              strConcat(fullTimeStamp, 15, "0101X");
              break;
    case  10:  //--- timeStamp is YYMMDDHHMM
              strConcat(fullTimeStamp, 15, "01X");
              break;
    case  12:  //--- timeStamp is YYMMDDHHMMSS
              strConcat(fullTimeStamp, 15, "X");
              break;
    //default:  return now();
  }
  
  if (strlen(fullTimeStamp) < 13) return now();
  
  if (Verbose2) DebugTf("DateTime: [%02d]-[%02d]-[%02d] [%02d]:[%02d]:[%02d]\r\n"
                                                                 ,DayFromTimestamp(timeStamp)
                                                                 ,MonthFromTimestamp(timeStamp)
                                                                 ,YearFromTimestamp(timeStamp)
                                                                 ,HourFromTimestamp(timeStamp)
                                                                 ,MinuteFromTimestamp(timeStamp)
                                                                 ,0
                       );
   
 
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

