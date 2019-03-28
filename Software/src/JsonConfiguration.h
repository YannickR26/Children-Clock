#ifndef _JSONCONFIGURATION_h
#define _JSONCONFIGURATION_h

#include "tRules.h"
#include <ArduinoJson.h>

class JsonConfiguration 
{
  public:
  	JsonConfiguration();
  	virtual ~JsonConfiguration();
  
  	void setup();

    bool readConfig();
  	bool saveConfig();
  
		uint8_t encodeToJson(JsonDocument &_json);
		uint8_t decodeJsonFromFile(const char* input);

  	void restoreDefault();
  
  	String _hostname;
    String _passwd;
  	String _ftpLogin;
  	String _ftpPasswd;
    
    std::vector<tRules>   _tabRules;
		uint8_t	_maxIntensity;
  	
  private:
		// StaticJsonDocument<6800> _json;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern JsonConfiguration Configuration;
#endif

#endif

