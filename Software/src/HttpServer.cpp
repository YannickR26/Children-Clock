#include <FS.h>
#include <ESP8266mDNS.h>

#include "HttpServer.h"

/********************************************************/
/******************** Public Method *********************/
/********************************************************/

HttpServer::HttpServer()
{
}

HttpServer::~HttpServer()
{
}

void HttpServer::setup(void)
{
  MDNS.begin(Configuration._hostname.c_str()); 
  MDNS.addService("http", "tcp", 80);

  _ftpServer.begin(Configuration._ftpLogin, Configuration._ftpPasswd);
  MDNS.addService("ftp", "tcp", 21);

  _webServer.on("/config/reset", HTTP_GET, []() { 
    Configuration._tabRules.clear(); 
    Serial.println("Clear All Rules !");
    HTTPServer.webServer().sendHeader("Access-Control-Allow-Origin", "*");
	  HTTPServer.webServer().send(200, "application/json", "{\"result\":true}");
  });
  _webServer.on("/config", HTTP_GET, HttpServer::get_config);
  _webServer.on("/config", HTTP_POST, HttpServer::set_config);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  _webServer.onNotFound([]() {
    if (!HTTPServer.handleFileRead(HTTPServer.webServer().uri())) {
      HTTPServer.webServer().send(404, "text/plain", "File Not Found !");
    }
  });

  _webServer.begin();
}

void HttpServer::handle(void)
{
  _webServer.handleClient();
  MDNS.update();
  _ftpServer.handleFTP();
}

String HttpServer::getContentType(String filename)
{
	if (_webServer.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

void HttpServer::get_config() 
{
  Serial.println("Send Configuration");
  
  DynamicJsonDocument _json(5000);
  Configuration.encodeToJson(_json);
  HTTPServer.sendJson(200, _json);
}

void HttpServer::set_config() 
{
  if (HTTPServer.webServer().hasArg("plain") == false) {
    Serial.println("Error, no body received !");
    HTTPServer.webServer().sendHeader("Access-Control-Allow-Origin", "*");
    HTTPServer.webServer().send(404, "text/plain", "Body not received");
  }
  else {
    if (!Configuration.decodeJsonFromFile(HTTPServer.webServer().arg("plain").c_str())) {
      Serial.println("Received new configuration !");
      HTTPServer.webServer().sendHeader("Access-Control-Allow-Origin", "*");
      // HTTPServer.webServer().send(200, "application/json", Configuration.encodeToJson());
      HTTPServer.webServer().send(200, "application/json", "{\"result\":true}");
    }
    else {
      Serial.println("Error, parsing JSON !");
      HTTPServer.webServer().sendHeader("Access-Control-Allow-Origin", "*");
      HTTPServer.webServer().send(404, "text/plain", "Error with parsing JSON");
    }
  }
}

// send the right file to the client (if it exists)
bool HttpServer::handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.html";                                     // If a folder is requested, send the index file
  }
  String contentType = HTTPServer.getContentType(path);       // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {     // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                            // If there's a compressed version available
      path += ".gz";                                          // Use the compressed verion
    File file = SPIFFS.open(path, "r");                       // Open the file
    HTTPServer.webServer().streamFile(file, contentType);     // Send it to the client
    file.close();                                             // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);        // If the file doesn't exist, return false
  return false;
}

ESP8266WebServer& HttpServer::webServer() 
{
  return _webServer;
}

/********************************************************/
/******************** Private Method ********************/
/********************************************************/

void HttpServer::sendJson(const uint16 code, JsonDocument& doc)
{
  WiFiClient client = HTTPServer.webServer().client();

  // Write Header
  client.print(F("HTTP/1.0 "));
  client.print(code);
  client.println(F(" OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Access-Control-Allow-Origin: *"));
  client.print(F("Content-Length: "));
  client.println(measureJson(doc));
  client.println(F("Connection: close"));
  client.println();

  // Write JSON document
  serializeJson(doc, client);
}

#if !defined(NO_GLOBAL_INSTANCES) 
HttpServer HTTPServer;
#endif
