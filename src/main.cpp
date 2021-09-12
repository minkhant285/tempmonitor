#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_Sensor.h>
#include <Ticker.h>
#include <DHT.h>

#define DHT11PIN 4

//Sensor initialization
DHT dht(DHT11PIN, DHT11);

//initialize network parameters
const char *ssid="bryyan";
const char *password="bryyan285";

const char *ap_ssid="TempMonitor";
const char *ap_password="temp1234";

//webpage html code
char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        // receive the color data from the server
        var data = JSON.parse(event.data);
        console.log(data);
       document.getElementById("temp").innerHTML = data.temperature;
       document.getElementById("humi").innerHTML = data.humidity;
      }
    }  
  </script>
</head>
<body onload="javascript:init()">
  <h4>Websocket client served from the Sensor Board!</h4>
  <p>
  	Temp:<span id="temp"> null </span>
	Humi:<span id="humi"> null </span>  	
  </p>
</body>
</html>
)=====";

// Instantiate server objects
WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

// Declare/initialize timer variables
Ticker timer;
bool read_data = false;

void readData() {
  // should only be used to set/unset flags
  read_data = true;
}

//enabling websocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_TEXT)
  {
    // processs any returned data
    Serial.printf("payload [%u]: %s\n", num, payload);
  }
}

//wifi events
void OnWiFiEvent(WiFiEvent_t event)
{
  switch (event) {
 
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to WiFi Network");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("ESP32 soft AP started");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Station connected to ESP32 soft AP");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Station disconnected from ESP32 soft AP");
      break;
    default: break;
  }
}

void setup() {
  dht.begin();
  Serial.begin(9600);

  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(ap_ssid,ap_password);
    WiFi.begin(ssid,password);
    delay(100);    

   

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

   Serial.println();
  Serial.print("IP Address (STA): "); 
  Serial.println(WiFi.localIP());
  Serial.print("Hostpot IP(AP): ");
  Serial.println(WiFi.softAPIP());

  // define the routes in which the server is accessible
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });

  // initialize server and websockets
  server.begin();
  webSocket.begin();
  
  // initialize timer function
  timer.attach(/*rate in secs*/ 0.1, /*callback*/ readData);

   webSocket.onEvent(webSocketEvent);
}



void loop() {

  webSocket.loop();
  server.handleClient();

  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

    if(read_data){
        if( isnan(temp)|| isnan(humi)){
            Serial.println("Failed to read sensor data");
        }

        // Serial.print("Temp: ");
        // Serial.println(temp);

        // Serial.print("Humi: ");
        // Serial.println(humi);

        String json = "{\"temperature\":";
        json += temp;
        json += ",\"humidity\":";
        json += humi;
        json += "}";

         webSocket.broadcastTXT(json.c_str(), json.length());
  }
}