#include <Arduino.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <fscheck.h>
#include <db.h>
#include <EEPROM.h>
#include <credentials.h>
#include <DHT.h>

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
    EEPROM.begin(512);

    esid = getSsid();
    epass = getPassword();

    pinMode(WifiModePin, INPUT);
    if (digitalRead(WifiModePin))
    {
        isSTAMode = true;
    }

    // initialize timer function
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

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("Failed to mount file system");
        return;
    }

    //   MDNS.begin("demo-server");
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
    server.begin();
    dht.begin();

    dbInit();
}

boolean irSensorStatus = false;

void loop()
{
    int irSensor = digitalRead(IRSENSOR);
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);

    if (irSensor)
    {

        if (isnan(tempC) || isnan(tempF))
        {
            Serial.println("Failed to read sensor data");
        }
        else
        {
            String writeData = "4,14-09-2021,";
            writeData += tempC;
            writeData += ",";
            writeData += tempF;
            writeFile(writeData);
            Serial.println("pushed");
            insert(String(tempC), String(tempF));
        }
    }

    if (wsClient != nullptr && wsClient->canSend())
    {
        String json = "{\"temperatureC\":";
        json += tempC;
        json += ",\"temperatureF\":";
        json += tempF;
        json += "}";
        wsClient->text(json);
    }

    delay(2000);
}
