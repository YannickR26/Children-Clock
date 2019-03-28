#include "DFPlayer.h"

// Create Software Serial Port Com
SoftwareSerial mySoftwareSerial(12, 14); // RX, TX

DFPlayer::DFPlayer() 
{
}

DFPlayer::~DFPlayer()
{
}

void DFPlayer::setup(void)
{
  mySoftwareSerial.begin(9600);

  //Use softwareSerial to communicate with mp3.
  if (!_player.begin(mySoftwareSerial)) { 
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      //while (true);
      return;
  }
}

void DFPlayer::handle(void) 
{
  if (_player.available()) {
      printDetail(_player.readType(), _player.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void DFPlayer::play(uint8_t song_)
{
  _player.play(song_);
}

void DFPlayer::stop()
{
  _player.stop();
}

void DFPlayer::printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
      Serial.println(F("Time Out!"));
      break;
  case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
  case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
  case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
  case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
  case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
  case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value)
      {
      case Busy:
          Serial.println(F("Card not found"));
          break;
      case Sleeping:
          Serial.println(F("Sleeping"));
          break;
      case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
      case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
      case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
      case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
      case Advertise:
          Serial.println(F("In Advertise"));
          break;
      default:
          break;
      }
      break;
  default:
      break;
  }
}

