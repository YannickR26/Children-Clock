#include <SPI.h>
#include <ESP8266WiFi.h>
#include <StreamString.h>

#define CFG_POWER  0b10100111
#define CFG_TEMP0  0b10000111
#define CFG_TEMP1  0b11110111
#define CFG_AUX    0b11100111
#define CFG_IRQ    0b11010010
#define CFG_LIRQ   0b11010000

void drawProgress(uint8_t percentage, String text);

void XPT2046_EnableIrq(uint8_t q) {
    SPI.beginTransaction(SPISettings(2500000, MSBFIRST, SPI_MODE0));
    digitalWrite(TOUCH_CS, 0);
    const uint8_t buf[4] = { q, 0x00, 0x00, 0x00 };
    SPI.writeBytes((uint8_t *) &buf[0], 3);
    digitalWrite(TOUCH_CS, 1);
    SPI.endTransaction();
}

uint32_t XPT2046_ReadRaw(uint8_t c) {
    uint32_t p = 0;
    uint8_t i = 0;  
    digitalWrite(TFT_CS, HIGH);    
    SPI.beginTransaction(SPISettings(2500000, MSBFIRST, SPI_MODE0));
    digitalWrite(TOUCH_CS, 0);
    SPI.transfer16(c) >> 3;
    for(; i < 10; i++) {
      delay(2);
      p += SPI.transfer16(c) >> 3;
    }
    p /= i;

    XPT2046_EnableIrq(CFG_IRQ);
    digitalWrite(TOUCH_CS, 1);
    SPI.endTransaction();
    return p;
}

int board_readNTC() {
  uint32_t valADC = XPT2046_ReadRaw(CFG_AUX);
  float U_NTC = valADC * ADC_quantum;
  float value = 1/((log(((U_NTC*NTC_serialResistance/ADC_vref)/(1-(U_NTC/ADC_vref)))/NTC_nominalResistance)/NTC_bCoefficient)+(1/NTC_tempReference)) - 273.15f - 10.f;
  static float average = value;
  average = (average + value) / 2;
  return (int)(average * 10); 
}

float board_getPower() {
  static int i;
  static float p = 0;
  float power;
  //XPT2046_setCFG(CFG_POWER);
  if (!power) 
    power = XPT2046_ReadRaw(CFG_POWER) * ADC_vref * 4 / 4096;        
  if (!i) {
    power = p/10;
    p = 0;
    i = 10;
  } else {
    i--;
    int v = XPT2046_ReadRaw(CFG_POWER);   
    p += v * ADC_vref * 4 / 4096;
  }
  return power;
}

void board_systemRestart() {
    Serial.flush();
    delay(500);
    Serial.swap();
    delay(100);
    ESP.restart();
    while (1) {
        delay(1);
    }; 
}

/**
 * write Update to flash
 * @param in Stream&
 * @param size uint32_t
 * @param md5 String
 * @return true if Update ok
 */
bool board_runUpdate(Stream& in, uint32_t size, int command)
{

    StreamString error;

    if(!Update.begin(size, command)) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.begin failed! (%s)\n", error.c_str());
        return false;
    }

    if(Update.writeStream(in) != size) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.writeStream failed! (%s)\n", error.c_str());
        return false;
    }

    if(!Update.end()) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.end failed! (%s)\n", error.c_str());
        return false;
    }

    return true;
}

void board_checkUpdate(void)
{
  //we look if a file named lighter.ino.bin is on the spiffs
  //if yes we update the rom and delete the file
  String updateFilename = "/Reveil_enfants.ino.bin";
  Serial.println("Checking update file " + updateFilename);
  if (SPIFFS.exists(updateFilename))
  {
    Serial.println("update file found");
    drawProgress(10, "Update file found");
    File updateFile = SPIFFS.open(updateFilename, "r");
    Serial.println("running update");
    drawProgress(30, "running update");
    bool ret = board_runUpdate(updateFile, updateFile.size(), U_FLASH);  
    updateFile.close();
    if (ret) {
      Serial.println("update finished");
      drawProgress(80, "update finished");
      Serial.println("removing update file");
      drawProgress(90, "removing update file");
      SPIFFS.remove(updateFilename);
      Serial.println("restarting");
      drawProgress(100, "restarting");
      board_systemRestart();
    }
    else {
      Serial.println("restarting");
      drawProgress(100, "restarting");
      board_systemRestart();
    }
  }
} 
