#include "Strip.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

Strip::Strip() : Adafruit_NeoPixel()
{
}

Strip::~Strip() 
{
}

void Strip::setup(uint16_t n, uint8_t p, neoPixelType t)
{
  Adafruit_NeoPixel::updateLength(n);
  Adafruit_NeoPixel::setPin(p);
  Adafruit_NeoPixel::updateType(t);
  
  brightness = DEFAULT_BRIGHTNESS;
  speed = DEFAULT_SPEED;
  running = false;
  mode = MODE_NO_MODE;
  tabNewColor = (tColor *) calloc(n, sizeof(tColor));
  tabCurrentColor = (tColor *) calloc(n, sizeof(tColor));
  
  // this resets all the neopixels to an off state
  Adafruit_NeoPixel::begin();

  Adafruit_NeoPixel::show();
  delay(1);
}

void Strip::handle(void)
{
  static int previousTick;
  
  if (running) {
    if (abs(millis() - previousTick) >= speed) {
      previousTick = millis();
      
      switch (mode) {
        case MODE_NO_MODE:
          if (brightness <= 5) {
            setQuarterColor(tabNewColor[0]);
          }
          else if (brightness <= 10) {
            setThirdColor(tabNewColor[0]);
          }
          else if (brightness <= 20) {
            setHalfColor(tabNewColor[0]);
          }
          else { 
            setAllColor(tabNewColor[0]);
          }
          for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
            setPixelColor(i, tabNewColor[i]);
            tabCurrentColor[i] = tabNewColor[i];
          }
        break;
  
        case MODE_FADE_IN:
          fade_in();
        break;
        
        case MODE_FADE_OUT:
          fade_out();
        break;
  
        case MODE_FADE_IN_OUT:
          fade_in_out();
        break;

        case MODE_FADING:
          fading();
        break;
  
        default : break;
      }
      
      // Apply configuration
      Adafruit_NeoPixel::setBrightness(brightness);
      Adafruit_NeoPixel::show();
    }
  }
}

void Strip::start() {
  running = true;
}

void Strip::stop() {
  running = false;
  Strip_off();
}

void Strip::setBrightness(uint8_t brightness_) {
  if (brightness_ == brightness) return;
  
  brightness = constrain(brightness_, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
}

void Strip::setMode(uint8_t mode_) {
  mode = mode_;  
  
  switch (mode) {
      case MODE_FADE_IN:
        // Shut Off all LED
        setAllCurrentColor(tColor(0,0,0));
      break;

      default : break;
  }  
}

void Strip::setSpeed(uint8_t speed_) {
  speed = speed_;
}

void Strip::setAllColor(tColor color_)
{
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
    tabNewColor[i] = color_;
  }
}

void Strip::setHalfColor(tColor color_)
{
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; i+=2) {
    tabNewColor[i] = color_;
    tabNewColor[i+1] = tColor(); // Half Pixel are black;
  }
}

void Strip::setThirdColor(tColor color_)
{
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; i+=3) {
    tabNewColor[i] = color_;
    tabNewColor[i+1] = tColor(); // Third Pixel are black
    tabNewColor[i+2] = tColor(); // Third Pixel are black
  }
}

void Strip::setQuarterColor(tColor color_)
{
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; i+=6) {
    tabNewColor[i] = color_;
    for (int j=i+1 ; j<i+6 ; ++j) {
      tabNewColor[j] = tColor(); // All Pixel are black
    }
  }
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

void Strip::setAllCurrentColor(tColor color_)
{
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
    tabCurrentColor[i] = color_;
  }
}

void Strip::setPixelColor(uint16_t n, tColor color_)
{
  Adafruit_NeoPixel::setPixelColor(n, color_.R, color_.G, color_.B);
}

tColor Strip::getPixelColor(uint16_t n)
{
  uint32_t col = Adafruit_NeoPixel::getPixelColor(n);
  tColor color(col);
  return color;
}

/*
 * Turns everything off. Doh.
 */
void Strip::Strip_off() {
  Adafruit_NeoPixel::clear();
  Adafruit_NeoPixel::show();
}

void Strip::fade_in() {
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
    if (tabCurrentColor[i] != tabNewColor[i]) {
      if (tabCurrentColor[i].R < tabNewColor[i].R) tabCurrentColor[i].R++;
      if (tabCurrentColor[i].G < tabNewColor[i].G) tabCurrentColor[i].G++;
      if (tabCurrentColor[i].B < tabNewColor[i].B) tabCurrentColor[i].B++;
      setPixelColor(i, tabCurrentColor[i]);
    }
  }
}

void Strip::fade_out() {
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
    if (tabCurrentColor[i].R > 0) tabCurrentColor[i].R--;
    if (tabCurrentColor[i].G > 0) tabCurrentColor[i].G--;
    if (tabCurrentColor[i].B > 0) tabCurrentColor[i].B--;
    setPixelColor(i, tabCurrentColor[i]);
  }
}

void Strip::fade_in_out() {
  static boolean mode = false;
  
  for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
    if (mode) { 
      if (tabCurrentColor[i] != tabNewColor[i]) {
        if (tabCurrentColor[i].R < tabNewColor[i].R) tabCurrentColor[i].R++;
        if (tabCurrentColor[i].G < tabNewColor[i].G) tabCurrentColor[i].G++;
        if (tabCurrentColor[i].B < tabNewColor[i].B) tabCurrentColor[i].B++;
      }
      else {
        mode = !mode;
      }
    }
    else {
      if (tabCurrentColor[i] != tColor(0,0,0)) {
        if (tabCurrentColor[i].R > 0) tabCurrentColor[i].R--;
        if (tabCurrentColor[i].G > 0) tabCurrentColor[i].G--;
        if (tabCurrentColor[i].B > 0) tabCurrentColor[i].B--;
      }
      else {
        mode = !mode;
      }
    }
    setPixelColor(i, tabCurrentColor[i]);
  }
  
}

void Strip::fading() {
  static uint8_t state = 0;

  if (state == 0) {
    for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
      if (tabCurrentColor[i] != tabNewColor[i]) {
        state = 1;    
      }
    }
  }
  else if (state == 1) {
    for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
      if (tabCurrentColor[i] != tColor(0,0,0)) {
        if (tabCurrentColor[i].R > 0) tabCurrentColor[i].R--;
        if (tabCurrentColor[i].G > 0) tabCurrentColor[i].G--;
        if (tabCurrentColor[i].B > 0) tabCurrentColor[i].B--;
      }
      else {
        state = 2;
      }
      setPixelColor(i, tabCurrentColor[i]);
    }
  }
  else if (state == 2) {
    for (int i=0 ; i<Adafruit_NeoPixel::numPixels() ; ++i) {
      if (tabCurrentColor[i] != tabNewColor[i]) {
        if (tabCurrentColor[i].R < tabNewColor[i].R) tabCurrentColor[i].R++;
        if (tabCurrentColor[i].G < tabNewColor[i].G) tabCurrentColor[i].G++;
        if (tabCurrentColor[i].B < tabNewColor[i].B) tabCurrentColor[i].B++;
      }
      else {
        state = 0;
      }
      setPixelColor(i, tabCurrentColor[i]);
    }
  }
  else {
    state = 0;
  }
}

#if !defined(NO_GLOBAL_INSTANCES) 
Strip strip;
#endif
