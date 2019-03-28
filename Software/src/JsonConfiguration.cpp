#include <FS.h>

#include "JsonConfiguration.h"

JsonConfiguration::JsonConfiguration()
{
  /* Initialize SPIFFS */
  if (!SPIFFS.begin()) {
    Serial.println("failed to initialize SPIFFS");
  }
}

JsonConfiguration::~JsonConfiguration()
{
}

void JsonConfiguration::setup(void)
{
  readConfig();

	if ((_hostname.length() == 0) || (_ftpLogin.length() == 0) || (_ftpPasswd.length() == 0)) {
		Serial.println("Invalid configuration values, restoring default values");
		restoreDefault();
    return;
	}

	// Serial.printf("hostname : %s\n", _hostname.c_str());
	// Serial.printf("passwd : %s\n", _passwd.c_str());
	// Serial.printf("ftpLogin : %s\n", _ftpLogin.c_str());
	// Serial.printf("ftpPasswd : %s\n", _ftpPasswd.c_str());
	// Serial.printf("intensityMax : %d\n", _maxIntensity);
  // for (std::vector<tRules>::iterator it = _tabRules.begin() ; it != _tabRules.end() ; ++it) {
  //   Serial.printf("rule : %s\n",(*it).toString().c_str());
  // }
}

bool JsonConfiguration::readConfig()
{
  Serial.println("Read Configuration file from SPIFFS...");
  
  // Open file
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  uint16_t size = configFile.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	configFile.readBytes(buf.get(), size);
  
  decodeJsonFromFile(buf.get());

  configFile.close();
  
  return true;
}

bool JsonConfiguration::saveConfig()
{ 
  DynamicJsonDocument _json(5000);
	encodeToJson(_json);
  
	File configFile = SPIFFS.open("/config.json", "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		return false;
	}

  serializeJson(_json, configFile);

  configFile.close();
	
	return true;
}

uint8_t JsonConfiguration::encodeToJson(JsonDocument &_json)
{
  // DynamicJsonDocument _json(5000);
  _json.clear();
  JsonObject global = _json.createNestedObject("global");
  global["hostname"]        = _hostname;
  global["passwd"]          = _passwd;
	global["ftp_login"]       = _ftpLogin;
	global["ftp_passwd"]      = _ftpPasswd;
	global["intensityMax"]    = _maxIntensity;

  JsonArray rules = _json.createNestedArray("tabRules");

  for (std::vector<tRules>::iterator it = _tabRules.begin() ; it != _tabRules.end() ; ++it) {
    (*it).toJson(rules);
  }

  return 0;
}

uint8_t JsonConfiguration::decodeJsonFromFile(const char* input)
{
  DynamicJsonDocument _json(5000);
  _json.clear();
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(_json, input);
  if (error) {
    Serial.print("Failed to deserialize JSON, error: ");
    Serial.println(error.c_str());
    // restoreDefault();
    return -1;
  }

  JsonObject jsonObject = _json["global"].as<JsonObject>();
  if (!jsonObject.isNull()) {
    _hostname = jsonObject["hostname"].as<String>();
    _passwd = jsonObject["passwd"].as<String>();
    _ftpLogin = jsonObject["ftp_login"].as<String>();
    _ftpPasswd = jsonObject["ftp_passwd"].as<String>();
    _maxIntensity = jsonObject["intensityMax"].as<uint8_t>();
  }

  JsonArray jsonArray = _json["tabRules"].as<JsonArray>();
  if (!jsonArray.isNull()) {
    for (JsonArray::iterator it=jsonArray.begin() ; it!=jsonArray.end() ; ++it) {
      JsonObject jsonArrayRule = (*it).as<JsonObject>();
      _tabRules.push_back(tRules(jsonArrayRule));
    }
  }

  return 0;
}

void JsonConfiguration::restoreDefault()
{
  uint8_t day = 0;
  
	_hostname = "reveil_enfant";
  _passwd = "reveil";
	_ftpLogin = "reveil";
	_ftpPasswd = "reveil";
  _maxIntensity = 50;

  /********* Add default rules *********/
  _tabRules.clear();
  // Sunday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 30, 0));   // yelow at 7h30
  _tabRules.push_back(tRules(0x00FF00, day, 8, 0, 0));    // green at 8h00
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 0, 0));   // yelow at 20h00
  _tabRules.push_back(tRules(0xFF0000, day, 20, 20, 0));  // red at 20h30
  day++;
  // Monday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 10, 0));   // yelow at 7h10
  _tabRules.push_back(tRules(0x00FF00, day, 7, 20, 0));   // green at 7h20
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 0, 0));   // yelow at 20h00
  _tabRules.push_back(tRules(0xFF0000, day, 20, 20, 0));  // red at 20h20
  day++;
  // Tuesday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 10, 0));   // yelow at 7h10
  _tabRules.push_back(tRules(0x00FF00, day, 7, 20, 0));   // green at 7h20
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 0, 0));   // yelow at 20h00
  _tabRules.push_back(tRules(0xFF0000, day, 20, 20, 0));  // red at 20h20
  day++;
  // Wednesday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 30, 0));   // yelow at 7h30
  _tabRules.push_back(tRules(0x00FF00, day, 8, 0, 0));    // green at 8h00
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 0, 0));   // yelow at 20h00
  _tabRules.push_back(tRules(0xFF0000, day, 20, 20, 0));  // red at 20h20
  day++;
  // Thursday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 10, 0));   // yelow at 7h10
  _tabRules.push_back(tRules(0x00FF00, day, 7, 20, 0));   // green at 7h20
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 0, 0));   // yelow at 20h00
  _tabRules.push_back(tRules(0xFF0000, day, 20, 20, 0));  // red at 20h20
  day++;
  // Wednesday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 10, 0));   // yelow at 7h10
  _tabRules.push_back(tRules(0x00FF00, day, 7, 20, 0));   // green at 7h20
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 15, 0));  // yelow at 20h15
  _tabRules.push_back(tRules(0xFF0000, day, 20, 30, 0));  // red at 20h30
  day++;
  // Saturday
  _tabRules.push_back(tRules(0xFFFF00, day, 7, 30, 0));   // yelow at 7h30
  _tabRules.push_back(tRules(0x00FF00, day, 8, 0, 0));    // green at 8h00
  _tabRules.push_back(tRules(0xFFFF00, day, 13, 0, 0));   // yelow at 13h00
  _tabRules.push_back(tRules(0x00FF00, day, 16, 0, 0));   // green at 16h00
  _tabRules.push_back(tRules(0xFFFF00, day, 20, 15, 0));  // yelow at 20h15
  _tabRules.push_back(tRules(0xFF0000, day, 20, 30, 0));  // red at 20h30
  day++;

	saveConfig();
	Serial.println("configuration restored.");
}

#if !defined(NO_GLOBAL_INSTANCES) 
JsonConfiguration Configuration;
#endif
