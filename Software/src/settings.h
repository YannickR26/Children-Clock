#ifndef _Settings_H_
#define _Settings_H_

// Setup Wifi
String WIFI_SSID = "yourSSID";
String WIFI_PASS = "yourPASS";

int NTP_UPDATE_INTERVAL_SEC = 1 * 3600; // Update time from NTP server every 1 hours
int SCREEN_UPDATE_INTERVAL_SEC = 2; // Update time from screen
int SAVER_INTERVAL_SECS = 20;   // Going to screen saver after idle times, set 0 for dont screen saver.
int SLEEP_INTERVAL_SECS = 30;   // Going to Sleep after idle times, set 0 for dont sleep.

// TFT
#define TFT_DC 0
#define TFT_CS 2
#define TFT_LED 16

// TouchPad
#define TOUCH_CS 5
#define TOUCH_IRQ 4

// Strip LED WS2811
#define NB_LED  24
#define PIN_LED 3

// General Board
#define ADC_vref                2.5f
#define ADC_quantum             (ADC_vref/4096.f)              
#define NTC_nominalResistance   10000.f   // NTC 10K
#define NTC_bCoefficient        3950.f    // B 3950
#define NTC_tempReference       298.15f   // T0 25°C or 298K
#define NTC_serialResistance    10000.f   // R 10K

// Timezone
#define UTC_OFFSET + 1
struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour

// Change for 12 Hour/ 24 hour style clock
bool IS_STYLE_12HR = false;
bool IS_SHOW_SECONDS = false;

// change for different ntp (time servers)
//#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"
#define NTP_SERVERS "0.fr.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

#endif
