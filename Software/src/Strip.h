#pragma once

#include <Adafruit_NeoPixel.h>

#define DEFAULT_SPEED 10
#define SPEED_MIN 0
#define SPEED_MAX 255

#define DEFAULT_BRIGHTNESS 50
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 255

enum {
  MODE_NO_MODE = 0,
  MODE_FADE_IN,
  MODE_FADE_OUT,
  MODE_FADE_IN_OUT,
  MODE_FADING,
};

class tColor {
  public:
    tColor(): R(0), G(0), B(0) {}
    tColor(uint8_t R_, uint8_t G_, uint8_t B_) : R(R_), G(G_), B(B_) {}
    tColor(uint32_t RGB_) : R((RGB_ >> 16) & 0xff), G((RGB_ >> 8) & 0xff), B((RGB_ >> 0) & 0xff) {}

    bool operator==(const tColor& other) const {
      return (R == other.R && G == other.G && B == other.B);
    }

    bool operator!=(const tColor& other) const {
      return !(*this == other);
    }

    uint8_t R, G, B;
};

class Strip : public Adafruit_NeoPixel {
  public:
    Strip();
    virtual ~Strip();

    void setup(uint16_t n, uint8_t p, neoPixelType t = NEO_GRB + NEO_KHZ800);
    void handle(void);
    
    void start();
    void stop();
    void setBrightness(uint8_t brightness_);
    void setMode(uint8_t mode_);
    void setSpeed(uint8_t speed_);
    void setAllColor(tColor color_);
    void setHalfColor(tColor color_);
    void setThirdColor(tColor color_);
    void setQuarterColor(tColor color_);

  private:
    void setAllCurrentColor(tColor color_);
    void setPixelColor(uint16_t n, tColor color_);
    tColor getPixelColor(uint16_t n);  
    void Strip_off();
    // All mode here
    void fade_in();
    void fade_out();
    void fade_in_out();
    void fading();

  private:
    tColor *tabNewColor;
    tColor *tabCurrentColor;
    uint8_t brightness;
    uint8_t speed;
    bool running;
    uint8_t mode;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern Strip strip;
#endif
