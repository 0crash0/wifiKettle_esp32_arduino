///////
//    Vref =1.114V


#include <Preferences.h>

Preferences preferences;

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
//#include <NeoPixelBus.h>
#include "FastLED.h"

//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "RTClib.h"

#include "imags_data.h"
#include "teapot_defs.h"
#include "led_defs.h"


#define DIG_OUT1 12
#define DIG_OUT2 13





RTC_DS1307 RTC;
//String daysOfTheWeek[7] = {"Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"};
String daysOfTheWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// The built in OLED is a 128*64 mono pixel display
// i2c address = 0x3c
// SDA = 5
// SCL = 4

SSD1306  display(0x3c, 5, 4);



const char *ssid = "Naukograd_kettle";
const char *password = "";

WebServer server(80);

//TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  preferences.begin("kettle", false);
  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Or remove the counter key only
  //preferences.remove("counter");

  //unsigned int counter = preferences.getUInt("counter", 0);
  ledMode_vskipel = preferences.getUInt("ledMode_vskipel", dLM_vskipel);
  ledMode_heating = preferences.getUInt("ledMode_heating", dLM_heating);
  ledMode_hot = preferences.getUInt("ledMode_hot", dLM_hot);
  ledMode_warm = preferences.getUInt("ledMode_warm", dLM_warm);
  ledMode_cold = preferences.getUInt("ledMode_cold", dLM_cold);
  ledMode_night_light = preferences.getUInt("ledMode_night_light", dLM_night_light);
  b_night_light_mode_overwrite = preferences.getBool("b_night_light_mode_overwrite", dnight_light_overwrite);
  //preferences.putUInt("counter", counter);

  // Close the Preferences
  preferences.end();


  display.init();
  display.flipScreenVertically();
  delay(500);

  /*adc1_config_width(ADC_WIDTH_12Bit); // 0 .. 4095
    adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_11db); //ADC1 Ch3 = SVN
  */

  RTC.begin(); // Inicia la comunicación con el RTC
  delay(500);
  //RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)


  pinMode(DIG_OUT1, OUTPUT);
  pinMode(DIG_OUT2, OUTPUT);
  pinMode (THERMISTORPIN, INPUT);
  delay(1000);

  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасить все светодиоды
  LEDS.show();                     // отослать команду



  Serial.begin(9600);


  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/ws2812", handle_ws2812);
  server.on("/digital", handle_digital);
  server.on("/settime", handle_settime);
  server.on("/settemp", handle_setTemp);
  server.on("/lightModes", handle_ledModes);
  server.on("/nightLED", handle_nightLED);
  server.on("/getSettings", handle_getSettings);
  server.on("/getStates", handle_getStates);


  server.onNotFound(handleNotFound);
  server.begin();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  /* xTaskCreatePinnedToCore(
     Task1code,   /// Task function./
     "Task1",     // name of task. /
     10000,       /// Stack size of task /
     NULL,        // parameter of the task /
     1,           //priority of the task /
     &Task1,      //Task handle to keep track of created task /
     0);          // pin task to core 0
    delay(500);*/

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   // Task function./
    "Task2",    // name of task. /
    10000,       // Stack size of task /
    NULL,       // parameter of the task /
    1,           // priority of the task/
    &Task2,      // Task handle to keep track of created task /
    1);          // pin task to core 1 /

  startup_SCREEN();
}

unsigned long timing, timing_ws2812, timing_keep_hot, timing_light_color_preview; // Переменная для хранения точки отсчета

DateTime now;
void loop() {
  if (millis() - timing > 1000) { // Вместо 10000 подставьте нужное вам значение паузы
    timing = millis();
    now = RTC.now(); // Obtiene la fecha y hora del RTC
    display_func();
    alarm_ON();
    hotting();
  }
  ws2812_func();
  //  server.handleClient();

}

/*
  void Task1code( void * pvParameters ) {

  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
     if (millis() - timing > 1000) { // Вместо 10000 подставьте нужное вам значение паузы
      timing = millis();
      now = RTC.now(); // Obtiene la fecha y hora del RTC
      display_func();
      alarm_ON();
      hotting();
    }
    ws2812_func();
  }
  }
*/
//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ) {

  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    server.handleClient();
  }
}


void alarm_ON() {
  if (b_alarm) {
    if (alarm_Hour == now.hour() && alarm_Minut == now.minute()) {
      if (b_alarm_hot == true) {
        b_hotting = true;
        keep_hot = true;
        kipitim = false;
        set_temp = set_temp_alarm;
      }
      if (b_alarm_kipitim == true) {
        b_hotting = true;
        keep_hot = false;
        kipitim = true;
        set_temp = KIPITIM_TEMP;
      }
      b_alarm = false;
      b_alarm_hot = false;
      b_alarm_kipitim = false;
      set_temp_alarm = 0.0;
    }
  }
}

bool shwTms = false;

void display_func() {
  display.clear();
  if (b_alarm && shwTms) {
    display.drawXbm(64, 0,  69, 64,   image_data_alarm);
    shwTms = false;
  }
  else {
    if (b_hotting && temp_now < set_temp) {
      display.drawXbm(82, 0,  34, 64,   image_data_hotting);
      shwTms = true;
    }
    else {
      if (temp_now >= HOT_TEMP) {
        display.drawXbm(83, 0,  30, 64,   image_data_hot);
      }
      else {
        if (temp_now >= WARM_TEMP) {
          display.drawXbm(83, 0,  30, 64,   image_data_warm);
        }
        else {
          display.drawXbm(70, 0,  57, 64,   image_data_snow);
        }
      }
      shwTms = true;
    }
  }


  display.setFont(qArialMT_Plain_24);
  get_Temp_now();
  //tstst();

  display.drawString(5, 12,  daysOfTheWeek[now.dayOfTheWeek()]);

  String str = "";
  if (now.hour() < 10) {
    str = "0" + (String)now.hour();
  }
  else {
    str = (String)now.hour();
  }
  str = str + ":" ;
  if (now.minute() < 10) {
    str = str + "0" + (String)now.minute();
  }
  else {
    str = str + (String)now.minute(); // + " " + (String)temp_now;
  }
  display.drawString(0, 38, str);


  display.display();
}

void ws2812_func() {
  if (!b_night_light_mode_overwrite) {
    if (b_hotting) {
      if (temp_now <= set_temp) {
        ledMode = ledMode_heating;
      }
    }
    else {
      if (temp_now >= KIPITIM_TEMP) {
        ledMode = ledMode_vskipel;
      }
      else {

        if (temp_now >= HOT_TEMP) {
          ledMode = ledMode_hot;
        }
        else {
          if (temp_now >= WARM_TEMP) {
            ledMode = ledMode_warm;
          }
          else {
            ledMode = ledMode_cold;
          }
        }
      }

    }
  }

  if (b_led_mode_preview) {
    ws2812_showMode(led_Mode_preview);
    if (millis() - timing_light_color_preview > 7000 ) {
      b_led_mode_preview = false;
    }

  }
  else {
    //if (millis() - timing_ws2812 > 300) {
    //timing_ws2812 = millis();
    ws2812_showMode(ledMode);
    //}
  }

}

void startup_SCREEN() {
  display.setContrast(255);

  display.clear();
  display.drawXbm(23, 1,  81, 61,   image_data_MFUA);


  display.display();

  delay(5000);
  display.clear();

  display.drawXbm(0, 5,  127, 53,   image_data_Naukograd);
  display.display();




  delay(1000);
  display.clear();
  delay(5000);
}
