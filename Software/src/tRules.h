#ifndef _TRULES_H_
#define _TRULES_H_

#include <ArduinoJson.h>

class tRules {
  public:
    tRules() : enable(false), color(0), dayOfWeek(0), hour(0), min(0), smiley(0) {}
    tRules(uint8_t dayOfWeek_, uint8_t hour_, uint8_t min_) : enable(false), color(0), dayOfWeek(dayOfWeek_), hour(hour_), min(min_), smiley(0) {}
    tRules(uint32_t color_, uint8_t dayOfWeek_, uint8_t hour_, uint8_t min_, uint8_t smiley_) : enable(true), color(color_), dayOfWeek(dayOfWeek_), hour(hour_), min(min_), smiley(smiley_) {}
    tRules(JsonObject &json_) { fromJson(json_); }
  
    bool operator==(const tRules& other) const {
      return (dayOfWeek == other.dayOfWeek && hour == other.hour && min == other.min);
    }
    
    bool operator!=(const tRules& other) const {
      return !(*this == other);
    }

    tRules& operator=(const tRules& other) {
      enable = other.enable;
      color = other.color;
      dayOfWeek = other.dayOfWeek;
      hour = other.hour;
      min = other.min;
      smiley = other.smiley;
      return *this;
    }

    int operator-(const tRules& other) const {
      tRules tmp(*this);
      int val = 0;
      if (other.min > tmp.min) {
        tmp.hour--;
        tmp.min += 60;
      }
      val += (tmp.min - other.min);
      
      if (other.hour > tmp.hour) {
        tmp.dayOfWeek--;
        tmp.hour += 24;
      }
      val += (tmp.hour - other.hour) * 60;

      if (other.dayOfWeek > tmp.dayOfWeek) {
        tmp.dayOfWeek += 7;
      }
      val += (tmp.dayOfWeek - other.dayOfWeek) * 60 * 24;
      
      return val;
    }

    void fromJson(JsonObject &json_) {
      enable = json_["enable"].as<bool>();
      String colorStr = json_["color"];
      color = strtol(colorStr.c_str(), NULL, 16);
      dayOfWeek = json_["day"].as<unsigned int>();
      String time = json_["time"];
      hour = strtol(time.c_str(), NULL, 10);
      min = strtol(time.c_str()+3, NULL, 10);
      smiley = json_["smiley"].as<unsigned int>();
    }

    void toJson(JsonArray &jsonArray_) {
      JsonObject json = jsonArray_.createNestedObject();
      json["enable"] = (bool)enable;
      char colorStr[10];
      sprintf_P(colorStr, "%06X", color);
      json["color"] = colorStr;
      json["day"] = (uint8_t)dayOfWeek;
      char time[10];
      sprintf(time, "%02d:%02d", hour, min);
      json["time"] = time;
      json["smiley"] = (uint8_t)smiley;
    }

    String toString(void) {
      char txt[150];
      sprintf(txt, "enable: %d, color: 0x%06x, dayOfWeek: %d, hour: %d, min: %d, smiley: %d", enable, color, dayOfWeek, hour, min, smiley);
      return String(txt);
    }
    
    boolean   enable;     // 1 for enable rules
    uint32_t  color;      // in html format: #RRGGBB
    uint8_t   dayOfWeek;  // 0 => Sunday, 6 => Saturday
    uint8_t   hour;
    uint8_t   min;
    uint8_t   smiley;
};

#endif
