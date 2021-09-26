#include <Arduino.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <fscheck.h>
#include <db.h>
#include <EEPROM.h>
#include <credentials.h>
#include <DHT.h>
#include <SPIFFS.h>

#define DHT11PIN 4
#define FORMAT_SPIFFS_IF_FAILED true

String esid = "";
String epass = "";

#define IRSENSOR 5
#define WifiModePin 16

#define APSSID "archer"
#define APPASSWORD "archer1234"

DHT dht(DHT11PIN, DHT11);

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient *wsClient;

bool isSTAMode = false;

TaskHandle_t Task1;
TaskHandle_t Task2;

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
    EEPROM.begin(512);

    esid = getSsid();
    epass = getPassword();

    pinMode(WifiModePin, INPUT);
    if (!digitalRead(WifiModePin))
    {
        isSTAMode = true;
    }

    if (isSTAMode && esid != "" && epass != "")
    {
        WiFi.begin(esid.c_str(), epass.c_str());
        waitForWiFiConnectOrReboot();
    }
    else
    {

        WiFi.softAPConfig(local_ip, gateway, subnet);
        WiFi.softAP(APSSID, APPASSWORD);
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }

    Serial.println(F("Inizializing FS..."));
    if (SPIFFS.begin())
    {
        Serial.println(F("done."));
    }
    else
    {
        Serial.println(F("fail."));
    }

    ws.onEvent(onWsEvent);

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

    server.on("/csv", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", readCSV().c_str()); });

    server.on("/eclear", HTTP_DELETE, [](AsyncWebServerRequest *request)
              {
                  clearEEprom();
                  delay(500);
                  ESP.restart();
                  request->send(200, "OK");
              });

    server.on("/select", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  String greet = "[";
                  greet += select();
                  greet += "]";
                  request->send_P(200, "text/html", greet.c_str());
              });

    server.on("/selectdrange", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  int paramsNr = request->params();
                  String arguments[2];

                  for (int i = 0; i < paramsNr; i++)
                  {

                      AsyncWebParameter *p = request->getParam(i);
                      arguments[i] = p->value();
                  }

                  String greet = selectDateRange(arguments[0].c_str(), arguments[1].c_str());

                  request->send(200, "text/html", greet.c_str());
              });

    server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  deleteData();
                  request->send_P(200, "text/html", "deleted");
              });

    server.addHandler(new AsyncCallbackJsonWebHandler(
        "/wconfig", [](AsyncWebServerRequest *request, JsonVariant &json)
        {
            const JsonObject &jsonObj = json.as<JsonObject>();

            if (jsonObj["ssid"] && jsonObj["pass"])
            {

                String qsid = jsonObj["ssid"];
                String qpass = jsonObj["pass"];

                setCredentials(qsid, qpass);

                request->send(200, "OK");
                delay(1000);
                ESP.restart();
            }
            else
            {
                Serial.println("No Data provided");
            }
            request->send(200, "OK");
        }));
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html"); });
    server.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/js/jquery.min.js", "text/javascript"); });
    server.on("/js/data.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/js/data.js", "text/javascript"); });
    server.on("/css/milligram.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/css/milligram.min.css", "text/css"); });
    server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/css/style.css", "text/css"); });
    server.on("/assets/person.svg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/assets/person.svg", "image/svg+xml"); });

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
    server.begin();
    // MDNS.begin("esp32");
    // MDNS.addService("_http", "_tcp", 80);
    // MDNS.addServiceTxt("_http", "_tcp", "board", "ESP32");
    dht.begin();

    dbInit();
}

void loop()
{
    int irSensor = digitalRead(IRSENSOR);
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);
    String insertDate = "1632027049";

    if (!irSensor)
    {

        if (isnan(tempC) || isnan(tempF))
        {
            Serial.println("Failed to read sensor data");
        }
        else
        {
            // writeData += tempC;
            // writeData += ",";
            // writeData += tempF;
            // writeFile(writeData);
            // if (tempF >= 92)
            // {
            insert(insertDate.c_str(), String(tempC), String(tempF));
            // Serial.println("pushed");
            // }
        }
    }

    if (wsClient != nullptr && wsClient->canSend())
    {

        String json = "{\"temperatureC\":";
        json += !irSensor ? tempC : 0.0;
        json += ",\"temperatureF\":";
        json += !irSensor ? tempF : 0.0;
        if (!irSensor)
        {
            json += ",";
            json += "\"sensorData\":";
            json += "{\"tempC\":";
            json += tempC;
            json += ",\"tempF\":";
            json += tempF;
            json += ",\"date\":";
            json += "\"";
            json += insertDate;
            json += "\"}";
        }
        json += "}";
        wsClient->text(json);
    }

    ws.cleanupClients();

    delay(1000);
}
