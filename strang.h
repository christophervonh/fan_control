#include <Arduino.h>

#define RESOLUTION     8
#define FREQ       10000
#define STRANG1        1
#define STRANG2        2
/* Format 

Strang_nummer : AN_luefter : luefter_dynamisch : value_lüfter : AN_lichter : static/dynamisch : show_nummer : value_red : value_green : value_blue
             1:           1:                1  :           125:           0:                 0:            5:         80:          120:        200  
String -->  "1:1:1:125:1:0:5:80:120:200"

*/



class Strang{

  private:
    
    String  _name;          // Name 
    int _id;                // ID
    uint8_t _pinv;          // pin für Ventilator
    uint8_t _pinr;          // pin für LED rot
    uint8_t _ping;          // pin für LED grün
    uint8_t _pinb;          // pin für LED blau

    int _ch_van;             // pwm channel van
    int _ch_r;               // pwm channel rot
    int _ch_g;               // pwm channel green
    int _ch_b;               // pwm channel blue

    void show1();           // Anmimation 1
    void show2();           // Anmimation 2
    void show3();           // Anmimation 3
    void show4();           // Anmimation 4
    void show5();           // Anmimation 5
    void show6();           // Anmimation 6    

  public:
  // Defult Value 0%
    int value_v           = 125;
    uint8_t pid_van_value_cpu = 0;
    int pid_van_value_gpu = 0;
    int value_r           = 100;
    int value_g           = 150;
    int value_b           = 200;
    
    int brightness        = 0;        // Helligkeit 0-100
    int value_r_b         = 0;        // Value für Rot mit helligkeit
    int value_g_b         = 0;        // Value für green mit helligkeit
    int value_b_b         = 0;        // Value für blue mit helligkeit

    float cpu_temp;
    float gpu_temp;

    float soll_temp_cpu = 40.0f;
    float soll_temp_gpu = 40.0f;

    float pid_P=1.0f;
    float pid_I=1.0f;
    float pid_D=1.0f;       // Values für PID Regler

    // aktiviert/deaktiviert Strang
    int akitve_van = 1;  // Schaltet Lüfter an bzw aus 
    int state_van  = 1;  // 0 - PID-Controlles ; 1 - User Controll : 2 - PID beide CPU Solllwert ; 3 - PID beide GPU Sollwert 
    int aktive_led = 1;  // Schaltet Lichter an bzw aus.
    int state_led  = 1;  // 0 - Show ; 1 - Usere Controll
    int show_nr = 0;     // merkt sich welche Show

    uint32_t last_call_show = 0;

    uint8_t counter = 0;

    
  Strang(String name, uint8_t id, uint8_t pinv, uint8_t pinr, uint8_t ping, uint8_t pinb){
    
    _name = name;
    _id = id;

    // Pins zuodnen
    _pinv = pinv;
    _pinr = pinr;
    _ping = ping;
    _pinb = pinb;


    // Brechnet PWM Channels Pro Pin
    _ch_van = ((_id-1)*4)+0;               // pwm channel van
    _ch_r   = ((_id-1)*4)+1;               // pwm channel rot
    _ch_g   = ((_id-1)*4)+2;               // pwm channel green
    _ch_b   = ((_id-1)*4)+3;               // pwm channel blue

    // Pins als Ausgänge definieren
    pinMode(_pinv, OUTPUT);
    pinMode(_pinr, OUTPUT);
    pinMode(_ping, OUTPUT);
    pinMode(_pinb, OUTPUT);
    

    // PWM Setup (channel,freq,resolution)
    // Pin -> Chnnale zuordnung.
    ledcSetup(_ch_van, FREQ, RESOLUTION);
    ledcAttachPin(get_pin_van(), _ch_van);

    ledcSetup(_ch_r, FREQ, RESOLUTION);
    ledcAttachPin(get_pin_red(), _ch_r);

    ledcSetup(_ch_g, FREQ, RESOLUTION);
    ledcAttachPin(get_pin_green(), _ch_g);

    ledcSetup(_ch_b, FREQ, RESOLUTION);
    ledcAttachPin(get_pin_blue(), _ch_b);

    
    ESP_LOGI("Stran-Constraktor","Strang %d wird mit Vanpin: %d , rot: %d , grüen: %d , blau %d inizialisiert.", _id,_pinv,_pinr,_ping,_pinb);
   

    ESP_LOGI("Stran-Constraktor","Strang %d wird mit pwmchannel van: %d , rot: %d , grüen: %d , blau %d inizialisiert.", _id,_ch_van,_ch_r,_ch_g,_ch_b);
    
  }
  

  void update_values(String);             // update Values
  void pars_cmd(String,int*);             //parsed cmd in buffer
  void run(void);                         // Hauptsteuerung
  void check_values(int*);                // checkt werte
  int check_cmd(String);                 // checkt cmd
  
  void set_rgb_values(int,uint8_t,uint8_t,uint8_t);  // Helligkeit, rot , grün , blau

  String  get_name(void);                 // gibt Name als String zurück
  
  int     get_id(void);                   // gibt ID zurück
  uint8_t get_pin_van(void);              // gibt van pin
  uint8_t get_pin_red(void);              // gibt red pin
  uint8_t get_pin_green(void);            // gibt green pin
  uint8_t get_pin_blue(void);             // gibt blue pin

  uint8_t get_ch_van(void);               // gibt pwm channel van zurük
  uint8_t get_ch_red(void);               // gibt pwm channel red zurük
  uint8_t get_ch_green(void);             // gibt pwm channel green zurük
  uint8_t get_ch_blue(void);              // gibt pwm channel blue zurük
   
  void set_van_state(int);                // Schaltet Lüfter an / aus
  void set_van_status(int);               // PID oder User
  void set_van_value(int);                // sewt geschwindigkeit 0-255

  int get_van_state(void);                // Schaltet Lüfter an / aus
  int get_van_status(void);               // PID oder User
  int get_van_value(void);                // sewt geschwindigkeit 0-255    

  void set_led_aktive(bool);               // schaltet Lichter an / aus
  void set_led_status(bool);              // Sow oder User Controlled
  void set_led_value_red(uint8_t);        // Value red
  void set_led_value_green(uint8_t);      // Value green
  void set_led_value_blue(uint8_t);       // Value blue
  void set_led_value_rgb(uint8_t,uint8_t,uint8_t);
  void set_show_nr(uint8_t);              // animation nummer

  int get_led_aktive(void);               // schaltet Lichter an / aus
  int get_led_status(void);              // Sow oder User Controlled
  uint8_t get_led_value_red(void);        // Value red
  uint8_t get_led_value_green(void);      // Value green
  uint8_t get_led_value_blue(void);       // Value blue
  uint8_t get_show_nr(void);              // animation nummer

  void set_soll_temp_cpu(float);          // Setzt Soll Temperatur für PID Regler
  void set_soll_temp_gpu(float);          // Setzt Sell Temperatur für PID Regler
  float get_soll_temp_cpu(void);          // get Soll Temperatur für PID Regler
  float get_soll_temp_gpu(void);          // get Sell Temperatur für PID Regler

  void calc_pid(float);                   // rechnet Stellgöße aus

  int get_pid_p(void);                    // Setzt p-value vom PID
  int get_pid_i(void);                    // Setzt i-value vom PID
  int get_pid_d(void);                    // Setzt d-value vom PID

  void set_pid_p(int);                    // gibt p-value vom PID
  void set_pid_i(int);                    // gibt i-value vom PID
  void set_pid_d(int);                    // gibt d-value vom PID

  uint8_t get_pid_van_value_cpu(void);    // gibt pid_van_value_cpu 0,255
  uint8_t get_pid_van_value_gpu(void);    // gibt pid_van_value_gpu 0,255

  void wheel(uint8_t);                    // Gibt regenbogen farben raus 0-255
  void set_brightness(uint8_t);               // Setzt brighntess auf 0-100
};
