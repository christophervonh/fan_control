/*************************************/
/*                                   */
/*    Autor  : Michael Veerkamp      */
/*    Datum  : 20.10.23              */
/*    Projekt: Lüftersteuerung PC    */
/*    Version: 1.0                   */
/*                                   */
/*************************************/

/***************************************************************************/
/*                              Includes                                    */
/***************************************************************************/


#include <FreeRTOS.h>
#include <task.h>
#include "strang.h"
#include "ESPAsyncWebSrv.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <Arduino_JSON.h>

/***************************************************************************/
/*                              Defines                                    */
/***************************************************************************/

// Pins am ESP32
#define S1_V_PIN 3
#define S1_R_PIN 46
#define S1_G_PIN 10
#define S1_B_PIN 11

#define S2_V_PIN 14
#define S2_R_PIN 21
#define S2_G_PIN 47
#define S2_B_PIN 45

// Bessere lesbarkeit
#define ID1 1
#define ID2 2

#define Core1 0
#define Core2 1

#define CPU 12
#define GPU 13

#define CMD_SHOW0 "1:1:1:125:1:0:0:100:100:100"
#define DEFULT_CMD "1:1:1:125:1:1:0:100:100:100"
#define CMD_VAN_FULL "1:1:1:200:1:0:1:80:120:200"  // Strang 1 , Van aktive , User controlles , Full Speed


/***************************************************************************/
/*                         Globale-Variablen                               */
/***************************************************************************/

// command string format in strang.h
String cmd = DEFULT_CMD;

// Task handler für Lüfter / LED und
TaskHandle_t VLtask;

// Strang-Objekte
Strang strang_1("Strang 1", ID1, S1_V_PIN, S1_R_PIN, S1_G_PIN, S1_B_PIN);
Strang strang_2("Strang 2", ID2, S2_V_PIN, S2_R_PIN, S2_G_PIN, S2_B_PIN);


// Variablen für Temperatur
float temp_cpu = 0.4f;
float temp_gpu = 0.5f;

// Network Credentials
const char* ssid = "SKYNET";
const char* password = "COOLC4TBOEF";



/***************************************************************************/
/*                              Funtkionen                                 */
/***************************************************************************/

// // Todo: Umrechnung bestimmten.
// float get_temp(uint8_t Channel) {
//   float temp;

//   for (int i = 0; i < 10; i++) {
//     temp += analogRead(Channel);
//   }

//   temp /= 10.0f;

//   Serial.print(Channel);
//   Serial.print(" ");
//   Serial.print("temp = ");
//   Serial.println(temp);

//   return temp;
// }

float get_temp(uint8_t Channel) {

  uint16_t ADC_value = analogRead(Channel);

  float calc_value;

  if (ADC_value < 400) {
    calc_value = (ADC_value / 14.0f) - 47.0f;
  } 
  else if (ADC_value < 1000) {
    calc_value = ((ADC_value * 3.0f) / 100.0f) - 30.0f;
  }    
  else if (ADC_value < 2950) {
    calc_value = ((ADC_value*5.0f)/200.0f) - 26.0f;
  } 
  else {
    calc_value = ((ADC_value * 5.0f) / 200.0f) - 26.0f;
  }

//  Serial.print(Channel);
//  Serial.print(" ");
//  Serial.print("temperatur = ");
//  Serial.print(calc_value);
//  Serial.print(" , mit ADC Value = ");
//  Serial.println(ADC_value);

  return calc_value;
}

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

String message = "";
String fanRange1 = "0";
String fanRange2 = "0";

int dutyCycle1;
int dutyCycle2;

//Json Variable to Hold Slider Values
JSONVar sliderValues;

//Get Slider Values
String getSliderValues(){
  sliderValues["fanRange1"] = String(fanRange1);
  sliderValues["fanRange2"] = String(fanRange2);

  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

/***************************************************************************/
/*                     VanControll-Task                                    */
/***************************************************************************/

/* to-do
    PID - Funktion alle 100ms aufrufen lassen für swischenwerte berechnen
*/

// Van_Controll Task auf Core 0
void Van_Controll(void* parameter) {
  Serial.println("starte Task Van_Controll.");
  TickType_t last_call = 0;
  TickType_t intervall_10 = 10;

  TickType_t last_call_pid = 0;
  TickType_t intervall_500 = 500;
  analogReadResolution(12);

  while (1) {
    // Ruft Funktion alle 10ms auf stellt Werte jeh nach CMD
    // ist so schnell wegen den animationen.
    if (xTaskGetTickCount() - last_call >= pdMS_TO_TICKS(intervall_10)) {

      strang_1.update_values(cmd);
      strang_2.update_values(cmd);

      cmd = "d";  // Updated

      last_call = xTaskGetTickCount();
    }

    // Ruft Funktion alle 500ms für PID Regler
    if (xTaskGetTickCount() - last_call_pid >= pdMS_TO_TICKS(intervall_500)) {
      temp_cpu = get_temp(CPU);
      //temp_gpu = get_temp(GPU);

      strang_1.calc_pid(temp_cpu);
      //strang_2.calc_pid(temp_gpu);

      //Serial.print(strang_1.get_led_value_red());Serial.print(",");Serial.print(strang_1.get_led_value_green());Serial.print(",");Serial.println(strang_1.get_led_value_blue());

      last_call_pid = xTaskGetTickCount();
    }
  }
}

/***************************************************************************/
/*                               Websocket Functions                       */
/***************************************************************************/

void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0) {
      fanRange1 = message.substring(2);
      dutyCycle1 = map(fanRange1.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      fanRange2 = message.substring(2);
      dutyCycle2 = map(fanRange2.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    } 
    if (strcmp((char*)data, "getValues") == 0) {
      notifyClients(getSliderValues());
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/***************************************************************************/
/*                               Setup                                     */
/***************************************************************************/

void setup() {
  Serial.begin(115200);  // Serielle Verbindung für debugg über "Werkzeuge -> Serial Monitor"
  delay(10);

  // Erstelle Tasks Für Lüfter und LED
  xTaskCreatePinnedToCore(Van_Controll, "Van und LED Controll", 10000, NULL, 1, &VLtask, Core2);
  delay(100);

  cmd = "d";  // Updated
  Serial.println("Setup done.");

   // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    
    strang_1.set_led_aktive(true);
    Serial.print("led_aktuive = ");Serial.println(strang_1.get_led_aktive());
    strang_1.set_led_aktive(false);
    Serial.print("led_aktive = ");Serial.println(strang_1.get_led_aktive());
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.js", "text/javascript");
  });

  // Start server
  server.begin();
}

void loop() {

ws.cleanupClients();

}
