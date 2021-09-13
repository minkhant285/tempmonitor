#include <Arduino.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <Ticker.h>
#include <fscheck.h>
#include <db.h>

#define BUILTIN_LED 2
#define IRSENSOR 5

#define SSID "bryyan"
#define PASSWORD "bryyan285"

#define APSSID "esp32"
#define APPASSWORD "esp32temp"

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient *wsClient;

Ticker timer;
bool read_data = false;

bool isAPMode = true;

void readData()
{
  // should only be used to set/unset flags
  read_data = true;
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    wsClient = client;
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    wsClient = nullptr;
  }
}

// void listFilesInDir(File dir, int numTabs = 1);

void waitForWiFiConnectOrReboot(bool printOnSerial = true)
{
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    if (printOnSerial)
    {
      Serial.print(".");
    }
    notConnectedCounter++;
    if (notConnectedCounter > 50)
    { // Reset board if not connected after 5s
      if (printOnSerial)
      {
        Serial.println("Resetting due to Wifi not connecting...");
      }
      ESP.restart();
    }
  }
  if (printOnSerial)
  {
    // Print wifi IP addess
    Serial.print("\n IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Booted");
  // initialize timer function
  if (isAPMode)
  {
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(APSSID, APPASSWORD);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    WiFi.begin(SSID, PASSWORD);
    waitForWiFiConnectOrReboot();
  }

  SPIFFS.begin();

  //   MDNS.begin("demo-server");
  ws.onEvent(onWsEvent);

  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(IRSENSOR, INPUT);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                       "GET, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.addHandler(&ws);

  server.on("/fs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String fileSystem = fsCheck();
              request->send_P(200, "text/html", fileSystem.c_str());
            });

  server.on("/select", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String greet = select();
              request->send_P(200, "text/html", greet.c_str());
            });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              deleteData();
              request->send_P(200, "text/html", "deleted");
            });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              clearFile();
              String fileSystem = fsCheck();
              request->send_P(200, "text/html", fileSystem.c_str());
            });

  server.addHandler(new AsyncCallbackJsonWebHandler(
      "/led", [](AsyncWebServerRequest *request, JsonVariant &json)
      {
        const JsonObject &jsonObj = json.as<JsonObject>();
        if (jsonObj["on"])
        {

          Serial.println("Turn on LED");
          digitalWrite(BUILTIN_LED, HIGH);
        }
        else
        {
          Serial.println("Turn off LED");

          digitalWrite(BUILTIN_LED, LOW);
        }
        request->send(200, "OK");
      }));

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
                    {
                      if (request->method() == HTTP_OPTIONS)
                      {
                        request->send(200);
                      }
                      else
                      {
                        Serial.println("Not found");
                        request->send(404, "Not found");
                      }
                    });

  timer.attach(/*rate in secs*/ 0.1, /*callback*/ readData);
  server.begin();

  // SPIFFS.remove("/test1.db");
  dbInit();
  sqlite3_initialize();
}

boolean irSensorStatus = false;

void loop()
{
  int irSensor = digitalRead(IRSENSOR);
  if (!irSensor)
  {
    writeFile("4,12-8-2021,100.5,150.5");

    insert();

    Serial.println("pushed");
  }
  if (wsClient != nullptr && wsClient->canSend())
  {
    if (read_data)
    {
      String json = "{\"temperature\":";
      json += "20";
      json += ",\"humidity\":";
      json += "30";
      json += "}";
      wsClient->text(json);
    }
  }

  delay(3000);

  // put your main code here, to run repeatedly:
}
