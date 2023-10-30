
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

/***************************************************************************/
/*                              Defines                                    */
/***************************************************************************/

// Pins am ESP32
#define S1_V_PIN 48 
#define S1_R_PIN 38
#define S1_G_PIN 39
#define S1_B_PIN 40

#define S2_V_PIN 15 
#define S2_R_PIN 16
#define S2_G_PIN 17 // anderen Pin finden
#define S2_B_PIN 18 // anderen PiN finden

// Bessere lesbarkeit
#define ID1     1
#define ID2     2

#define Core1   0
#define Core2   1

#define CPU     19
#define GPU     20

#define CMD_SHOW0 "1:1:1:125:1:0:0:100:100:100"
#define DEFULT_CMD "1:1:1:125:1:1:0:100:100:100"
#define CMD_VAN_FULL "1:1:1:200:1:0:1:80:120:200"    // Strang 1 , Van aktive , User controlles , Full Speed 


/***************************************************************************/
/*                         Globale-Variablen                               */
/***************************************************************************/

// command string format in strang.h
String cmd = DEFULT_CMD;

// Task handler für Lüfter / LED und 
TaskHandle_t VLtask;

// Strang-Objekte
Strang strang_1("Strang 1", ID1, S1_V_PIN, S1_R_PIN, S1_G_PIN, S1_B_PIN);
Strang strang_2("Strang 2" ,ID2, S2_V_PIN, S2_R_PIN, S2_G_PIN, S2_B_PIN);

// Variablen für Temperatur
float temp_cpu = 0.4f;
float temp_gpu = 0.5f;


/***************************************************************************/
/*                              Funtkionen                                 */
/***************************************************************************/

// Todo: Umrechnung bestimmten.
float get_temp(uint8_t Channel){
  float temp;

  for(int i=0; i<10 ; i++){ 
    temp += analogRead(Channel);
  }

  temp/=10.0f;

  // umrechnung 

  return temp;
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


  while (1) {
    // Ruft Funktion alle 10ms auf stellt Werte jeh nach CMD
    // ist so schnell wegen den animationen.
    if(xTaskGetTickCount()-last_call>= pdMS_TO_TICKS(intervall_10)){
   
      strang_1.update_values(cmd);
      strang_2.update_values(cmd);
      
      cmd = "d"; // Updated

      last_call = xTaskGetTickCount();
    }

    // Ruft Funktion alle 500ms für PID Regler
    if(xTaskGetTickCount()-last_call_pid>= pdMS_TO_TICKS(intervall_500)){
      temp_cpu = get_temp(CPU);
      temp_gpu = get_temp(GPU);
   
      strang_1.calc_pid(temp_cpu);
      strang_2.calc_pid(temp_gpu);
    
      //Serial.print(strang_1.get_led_value_red());Serial.print(",");Serial.print(strang_1.get_led_value_green());Serial.print(",");Serial.println(strang_1.get_led_value_blue());

      last_call_pid = xTaskGetTickCount();
    }
  }
}


/***************************************************************************/
/*                               Setup                                     */
/***************************************************************************/

void setup() {
  Serial.begin(115200);      // Serielle Verbindung für debugg über "Werkzeuge -> Serial Monitor"
  delay(10);

  // Erstelle Tasks Für Lüfter und LED
  xTaskCreatePinnedToCore(Van_Controll, "Van und LED Controll", 10000, NULL, 1, &VLtask, Core2);
  delay(100);

 
  cmd = DEFULT_CMD;
  Serial.println("Setup done.");
}

void loop() {
  
  // Hier ist dein Task 
  // cmd = update_from_web_Server();
      if(Serial.available()){
      delay(1);
      cmd = Serial.readStringUntil('\n');
      delay(1);
      Serial.print("hab empfangen: ");Serial.println(cmd);
    }
}


