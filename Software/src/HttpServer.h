#ifndef _HTTPSERVER_h
#define _HTTPSERVER_h

#include <ESP8266WebServer.h>
#include <ESP8266FtpServer.h>
#include "JsonConfiguration.h"

class HttpServer
{
public:
	HttpServer() ;
	virtual ~HttpServer();

	void setup(void);
	void handle(void);

	String getContentType(String filename);
  bool handleFileRead(String path);

  ESP8266WebServer& webServer();

  void sendJson(const uint16 code, JsonDocument& doc);

protected:
  static void get_config();
  static void set_config();

private:
  ESP8266WebServer  _webServer;
  FtpServer         _ftpServer;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern HttpServer HTTPServer;
#endif

#endif

