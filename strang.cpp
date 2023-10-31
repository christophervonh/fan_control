#include <Arduino.h>
#include "strang.h"

/***************************************************************************/
/*                     get - Funktions                                     */
/***************************************************************************/

String Strang::get_name() {
  return _name;
}

int Strang::get_id() {
  return _id;
}

uint8_t Strang::get_pin_van(void){
    return _pinv;
}   

uint8_t Strang::get_pin_red(void){
    return _pinr;
}              

uint8_t Strang::get_pin_green(void){
    return _ping;
}          

uint8_t Strang::get_pin_blue(void){
    return _pinb;
}
// gibt pwm channel van zurük
uint8_t Strang::get_ch_van(void){
  if(_id == 1 ) return 0;
  return 4;
}

// gibt pwm channel red zurük             
uint8_t Strang::get_ch_red(void){
  if(_id == 1 ) return 1;
  return 5;
}     

// gibt pwm channel green zurük          
uint8_t Strang::get_ch_green(void){
  return ((_id-1)*4)+2;
}    

// gibt pwm channel blue zurük         
uint8_t Strang::get_ch_blue(void){
  return ((_id-1))*4+3;  
} 

// Setzt p-value vom PID
int Strang::get_pid_p(void){
  return pid_P;
} 

// Setzt i-value vom PID               
int Strang::get_pid_i(void){
  return pid_I;
}

// Setzt d-value vom PID              
int Strang::get_pid_d(void){
  return pid_D;
}

 // get Soll Temperatur für PID Regler
float Strang::get_soll_temp_cpu(void){
  return soll_temp_cpu;
}    
 
 // get Sell Temperatur für PID Regler
float Strang::get_soll_temp_gpu(void){
  return soll_temp_gpu;
}

uint8_t Strang::get_pid_van_value_cpu(void){
  return pid_van_value_cpu;
}

uint8_t Strang::get_pid_van_value_gpu(void){
  return pid_van_value_gpu;
}

uint8_t Strang::get_led_value_red(void){
  return value_r;
}

uint8_t Strang::get_led_value_green(void){
  return value_g;
}

uint8_t Strang::get_led_value_blue(void){
  return value_b;
}

// schaltet Lichter an / aus
int Strang::get_led_aktive(void){
  return aktive_led;
}               

// Sow oder User Controlled
int Strang::get_led_status(void){              
  return state_led;
}
// Schaltet Lüfter an / aus
int Strang::get_van_state(void){
  return akitve_van;
}       
  // PID oder User         
int Strang::get_van_status(void){
  return state_van;
} 
   // sewt geschwindigkeit 0-255    
int Strang::get_van_value(void){
  return value_v;
}    
     
/***************************************************************************/
/*                         set-Funktions                                   */
/***************************************************************************/

void Strang::set_led_value_rgb(uint8_t r, uint8_t g, uint8_t b){
  value_r = r;
  value_g = g;
  value_b = b; 
}

// Schaltet Lüfter an / aus
void Strang::set_van_state(int state) {
  akitve_van = state;
}

// PID oder User
void Strang::set_van_status(int state) {
  state_van = state;
}

// set geschwindigkeit 0-255
void Strang::set_van_value(int value) {
  value_v = value;
}

// schaltet Lichter an / aus
void Strang::set_led_aktive(bool state) {
  aktive_led = state;
}

// Sow oder User Controlled
void Strang::set_led_status(bool state) {
  state_led = state;
}

void Strang::set_show_nr(uint8_t nr){
  show_nr = nr;
}

// Value red
void Strang::set_led_value_red(uint8_t value) {
  value_r = value;
}

// Value green
void Strang::set_led_value_green(uint8_t value) {
  value_g = value;
}

// Value blue
void Strang::set_led_value_blue(uint8_t value) {
  value_b = value;
}
                

// gibt p-value vom PID
void Strang::set_pid_p(int p){
  pid_P = p;
}

// gibt i-value vom PID            
void Strang::set_pid_i(int i){
  pid_I = i;
}

// gibt d-value vom PID              
void Strang::set_pid_d(int d){
  pid_D = d;
}

// Setzt Soll Temperatur für PID Regler
void Strang::set_soll_temp_cpu(float soll_temperatur){
  soll_temp_cpu = soll_temperatur;
} 
// Setzt Sell Temperatur für PID Regler         
void Strang::set_soll_temp_gpu(float soll_temperatur){
  soll_temp_gpu = soll_temperatur;
}
       

/***************************************************************************/
/*                           PID Regler                                    */
/***************************************************************************/
void Strang::calc_pid(float temp_now){

    // Empfindlichkeit des PID-Reglers
  static float Tn = 5.0f;
  static float Tv = 2.0f;
  static float Kp = 1.0f;
    // Variablen für P,I,D
  //float P,I,D;
    // Zeitkostente 
  static float delta_tt=0.5f;
  static float En=0.0,en1=0.0,en2=0.0;
  static float Yn=0.0,yn1=0.0;
  static float delta_y;

  static float soll_temp = 40.0f;

  // if(get_id() == STRANG1) soll_temp = soll_temp_cpu;
  // if(get_id() == STRANG2) soll_temp = soll_temp_gpu;

  En =  40.0f - temp_now ;     // T1 = Temperatur_1 = Sollwert hier
                                      // delat = erlaubte Regelabweichung   
                                      // T2 = Temperatur_2 = Istwert
                                      // En = Regelabweichung
                      
  // Berechnugn P I D
  pid_P = En - en1;
  pid_D = (delta_tt / Tn) * En;
  pid_I = Tv * (En - 2 * en1 + en2);

  // Berechnung neuer Stellgröße
  delta_y = Kp * ( pid_P + pid_D + pid_I);
  Yn = yn1 + delta_y;

  // Speichert alte Werte
  en2 = en1;
  en1 = En;
  yn1 = Yn;

  // Begrenzt Yn auf 100
  if (Yn > 100.0) Yn=99.99;
  if (Yn <   0.0) Yn=  0.0;

  pid_van_value_cpu = map(Yn,0,99,0,255); 
//if(get_id() == STRANG2)pid_van_value_gpu = map(Yn,0,99,0,255);


  // char bufferr[100];
  // sprintf(bufferr,"P: %f , I: %f D: %f resultiert in gesch: %d",P,I,D,pid_van_value_cpu);
  // Serial.print(bufferr);

         // Gibt Stellgröße zurück und springt ins Hautprogramm zurück
}
 
/***************************************************************************/
/*                     Update Procces                                      */
/***************************************************************************/

void Strang::update_values(String cmd){
  
  // int cmd_buffer[10] = {0};

  // //ESP_LOGI("update_values","OLD VALUES \n Strang %d: \n akitve_van = %d \n state_van  = %d \n value_v    = %d \n aktive_led = %d \n state_led  = %d  \n show_nr    = %d \n value_r = %d \n value_g = %d \n value_b = %d. \n",get_id(),cmd_buffer[1],cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8],cmd_buffer[9]);
  
  // if(check_cmd(cmd)){

  //   pars_cmd( cmd, cmd_buffer );

  //   //ESP_LOGI("update_values","NEW VALUES \n Strang %d: \n akitve_van = %d \n state_van  = %d \n value_v    = %d \n aktive_led = %d \n state_led  = %d  \n show_nr    = %d \n value_r = %d \n value_g = %d \n value_b = %d. \n",get_id(),cmd_buffer[1],cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8],cmd_buffer[9]);

  //   check_values(cmd_buffer);

  //   if( get_id() == cmd_buffer[0] ){   

  //     akitve_van = cmd_buffer[1];  
  //     state_van  = cmd_buffer[2];    
  //     value_v    = cmd_buffer[3];

  //     aktive_led = cmd_buffer[4]; 
  //     state_led  = cmd_buffer[5];  
  //     show_nr    = cmd_buffer[6];

  //     value_r = cmd_buffer[7];
  //     value_g = cmd_buffer[8];
  //     value_b = cmd_buffer[9];      

  //     ESP_LOGI("update_values","nach IF VALUES \n Strang %d: \n akitve_van = %d \n state_van  = %d \n value_v    = %d \n aktive_led = %d \n state_led  = %d  \n show_nr    = %d \n value_r = %d \n value_g = %d \n value_b = %d. \n",get_id(), akitve_van, state_van, value_v, aktive_led, state_led, show_nr, value_r, value_g, value_b);

  //   }
    
  // }


  run();

}

bool istGueltigerString(String input) {
  for (size_t i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    if (!isdigit(c) && c != ':') {
      return false; // Wenn ein Zeichen keine Ziffer und kein ':' ist, ist es ungültig
    }
  }
  return true; // Alle Zeichen sind Ziffern oder ':', also ist es gültig
}

int Strang::check_cmd(String cmd){

  if(cmd == "d" ) return 0;  // Keine neuen daten
  if(cmd.length() < 5) return 0;
  if( !istGueltigerString(cmd) ){ 
    Serial.println("ungueltiger String"); 
    return 0;
  }

  return 1;                  // neue Daten

}



//              0123456789
// String -->  "1:1:1:125:1:0:5:80:120:200"
void Strang::pars_cmd(String cmd, int* cmd_buff){
  
  int semi = cmd.indexOf(":");
    cmd_buff[0] = cmd.substring(0, semi).toInt();

  int next_semi = cmd.indexOf(":",semi+1);
    cmd_buff[1] = cmd.substring(semi+1,next_semi).toInt();

  semi = cmd.indexOf(":",next_semi+1);
    cmd_buff[2] = cmd.substring(next_semi+1,semi).toInt();

  next_semi = cmd.indexOf(":",semi+1);
    cmd_buff[3] = cmd.substring(semi+1,next_semi).toInt();

  semi = cmd.indexOf(":",next_semi+1);
    cmd_buff[4] = cmd.substring(next_semi+1,semi).toInt();

  next_semi = cmd.indexOf(":",semi+1);
    cmd_buff[5] = cmd.substring(semi+1,next_semi).toInt();

  semi = cmd.indexOf(":",next_semi+1);
    cmd_buff[6] = cmd.substring(next_semi+1,semi).toInt();

  next_semi = cmd.indexOf(":",semi+1);
    cmd_buff[7] = cmd.substring(semi+1,next_semi).toInt();

  semi = cmd.indexOf(":",next_semi+1);
    cmd_buff[8] = cmd.substring(next_semi+1,semi).toInt();

    cmd_buff[9] = cmd.substring(semi+1).toInt();

 

  //ESP_LOGI("pars_cmd","NEW VALUES \n Strang %d: \n akitve_van = %d \n state_van  = %d \n value_v    = %d \n aktive_led = %d \n state_led  = %d  \n show_nr    = %d \n value_r = %d \n value_g = %d \n value_b = %d. \n",cmd_buff[0],cmd_buff[1],cmd_buff[2],cmd_buff[3],cmd_buff[4],cmd_buff[5],cmd_buff[6],cmd_buff[7],cmd_buff[8],cmd_buff[9]);
}

void Strang::check_values(int* buffer){

  // begrenzt auf Strang 1 und 2
  if(buffer[0] < 1 ||  buffer[0] > 2 ) buffer[0] = 1;

  // akitve_van 0 oder 1
  if(buffer[1] < 0 ||  buffer[1] > 1 ) buffer[1] = 1;

  // state_van 0 oder 1
  if(buffer[2] < 0 ||  buffer[2] > 1 ) buffer[2] = 1;

  // value_v 0 oder 255
  if(buffer[3] < 0 ||  buffer[3] > 255 ) buffer[3] = 255;

  // aktive_led 0 oder 1
  if(buffer[4] < 0 ||  buffer[4] > 1 ) buffer[4] = 1;

  // state_led 0 oder 1
  if(buffer[5] < 0 ||  buffer[5] > 1 ) buffer[5] = 1;  

  // show_nr 0 oder 10
  if(buffer[6] < 0 ||  buffer[6] > 10 ) buffer[6] = 5; 

  // value_r 0 oder 255
  if(buffer[7] < 0 ||  buffer[7] > 255 ) buffer[7] = 255; 

  // value_r 0 oder 255
  if(buffer[8] < 0 ||  buffer[8] > 255 ) buffer[8] = 255; 

  // value_r 0 oder 255
  if(buffer[9] < 0 ||  buffer[9] > 255 ) buffer[9] = 255; 
}


/***************************************************************************/
/*                             Statemaschine                               */
/***************************************************************************/

void Strang::run(void){
  
  // Kümmert sich um Lüfter auswahl
  switch (akitve_van){

    // Lüfter aus
    case 0: 
      ledcWrite(get_ch_van(),0);
    break;

    // Lüfter an
    case 1:  
      switch (state_van){
        case 0: //  PID Cantrolled
          if(get_id() == STRANG1)ledcWrite(get_ch_van(), pid_van_value_cpu);
          if(get_id() == STRANG2)ledcWrite(get_ch_van(), pid_van_value_gpu);          
        break;

        case 1: // User Controllied
          ledcWrite(get_ch_van(), value_v);
        break;

        case 2:
          ledcWrite(get_ch_van(), pid_van_value_cpu);
        break;

        case 3:
          ledcWrite(get_ch_van(), pid_van_value_gpu);
        break;

      }
    break;


  }

  switch (aktive_led){

    // Lampen aus
    case 0:  
      ledcWrite(get_ch_red()  , 0);
      ledcWrite(get_ch_green(), 0);
      ledcWrite(get_ch_blue() , 0);
    break;

    // Lampen an 
    case 1: 
      switch (state_led){
        
        // SHOW 
        case 0:
          switch (show_nr){
            case 0:
               show1();
            break;
            
            case 1:
            // show_2();
            break;
            
            case 2:
            // show_3();
            break;
            
            case 3:
            // show_4();
            break;
            
            case 4:
            // show_5();
            break;
          }

        // User Controlled
        case 1:
          ledcWrite(get_ch_red()  , value_r);
          ledcWrite(get_ch_green(), value_g);
          ledcWrite(get_ch_blue() , value_b);
        break;
      }
  }

}

/***************************************************************************/
/*                           Farben und shows                              */
/***************************************************************************/

void Strang::wheel(byte WheelPos) {
  if (WheelPos < 85) {
    value_r = WheelPos * 3;
    value_g = 255 - WheelPos * 3;
    value_b = 0;
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    value_r = 255 - WheelPos * 3;
    value_g = 0;
    value_b = WheelPos * 3;
  } else {
    WheelPos -= 170;
    value_r = 0;
    value_g = WheelPos * 3;
    value_b = 255 - WheelPos * 3;
  }
}

void Strang::set_brightness(uint8_t brightness){

    if(brightness < 0) brightness = 0;
    if(brightness > 100)brightness = 100;
    
    // Berechnung des Gesamtwerts (Summe der RGB-Komponenten)
    int total = value_r + value_g + value_b;

    // Neuen Gesamtwert basierend auf Helligkeit berechnen
    int newTotal = (total * brightness) / 100;

    // Verhältnisse der RGB-Komponenten beibehalten und neu skalieren
    if (total > 0) {
        value_r_b = (value_r * newTotal) / total;
        value_g_b = (value_g * newTotal) / total;
        value_b_b = (value_b * newTotal) / total;
    }

}

// Setzt Physisch die Helligkeit
void Strang::set_rgb_values(int h,uint8_t r, uint8_t g, uint8_t b){

  set_brightness(h);

  ledcWrite(get_ch_red()    , value_r_b );
  ledcWrite(get_ch_green()  , value_g_b );
  ledcWrite(get_ch_blue()   , value_b_b );
}

void Strang::show1(){

  if(last_call_show++ % 3 == 0){
    
    wheel(counter++);
    
    set_rgb_values(brightness,value_r,value_g,value_b);

  }
  
}
