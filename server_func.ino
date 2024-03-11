bool err_exec = false;


void handleRoot() {

  server.send(200, "text/plain", "hello from wifiKettle!");

}
void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);

}

void handle_digital() {
  err_exec = false;
  int pin = 0, val = 0;
  if (server.hasArg("port")) {
    if (server.arg("port") == "1") {
      pin = DIG_OUT1;
    }
    if (server.arg("port") == "2") {
      pin = DIG_OUT2;
    }
  }
  else {
    err_exec = true;
  }
  if (server.hasArg("value")) {
    if (server.arg("value") == "H") {
      val = HIGH;
    }
    if (server.arg("value") == "L") {
      val = LOW;
    }
  }
  else {
    err_exec = true;
  }
  if (!err_exec) {
    digitalWrite(pin, val);
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }


}

void handle_setTemp() {
  err_exec = false;
  if (server.hasArg("hotting")) {
    if (server.arg("hotting") == "H") {
      if (server.hasArg("settemp")) {
        if (server.arg("settemp") == "80") {
          set_temp = HOT_TEMP;
          b_hotting = true;
          keep_hot = true;

          Serial.println("+++hotting       WEB SET 80");
        }
        if (server.arg("settemp") == "50") {
          set_temp = WARM_TEMP;
          b_hotting = true;
          keep_hot = true;
          Serial.println("+++hotting       WEB SET 50");
        }
      }
      else  {
        Serial.println("+++hotting     ERROR");
        Serial.println(server.arg("hotting"));
        err_exec = true;
      }
    }
    else {
      b_teapot_off = true;
      b_hotting = false;
      set_temp = 0.0;
      Serial.println("+++hotting       WEB SET 0");
    }
  }
  if (server.hasArg("kipitim")) {
    if (server.arg("kipitim") == "H") {
      kipitim = true;
      b_hotting = true;
      set_temp = KIPITIM_TEMP;
      Serial.println("+++kipitim       WEB SET 120");
    }
    if (server.arg("kipitim") == "L") {
      b_teapot_off = true;
      b_hotting = false;
      set_temp = 0.0;
      Serial.println("+++kipitim       WEB SET 0");
    }

  }
  else {
    Serial.println("+++kipitim     ERROR");
    err_exec = true;
  }
  if (!err_exec) {
    //digitalWrite(pin, val);
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }


}

void handle_ws2812() {
  err_exec = false;
  String r, g, b;
  if (server.hasArg("R") && server.hasArg("G") && server.hasArg("B")) {
    r = server.arg("R");
    g = server.arg("G");
    b = server.arg("B");
    Serial.print("R=");
    Serial.print(r.toInt());
    Serial.print(" G=");
    Serial.print(g.toInt());
    Serial.print(" B=");
    Serial.println(b.toInt());
  } else {
    err_exec = true;
  }
  if (!err_exec) {
    o_CLR_R = r.toInt();
    o_CLR_G = g.toInt();
    o_CLR_B = b.toInt();
    Serial.print("ORGB ____ R=");
    Serial.print(o_CLR_R);
    Serial.print(" G=");
    Serial.print(o_CLR_G);
    Serial.print(" B=");
    Serial.println(o_CLR_B);
    timing_light_color_preview = millis();
    b_led_mode_preview = true;
    led_Mode_preview = 890;
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }

}

void handle_settime() {
  err_exec = false;
  int g_year = 0, g_month = 0, g_day = 0, g_hour = 0, g_min = 0, g_sec = 0;

  if (server.hasArg("setalarm") || server.hasArg("setdate")) {
    if (server.arg("setalarm") == "H") {
      if (server.hasArg("hour") && server.hasArg("minut") && server.hasArg("alarm_hot_kip")) {
        b_alarm = true;
        if (server.arg("alarm_hot_kip") == "kipitim") {
          b_alarm_hot = false;
          b_alarm_kipitim = true;
          set_temp_alarm = KIPITIM_TEMP;
        }
        if (server.arg("alarm_hot_kip") == "keep_hot") {
          b_alarm_hot = true;
          set_temp_alarm = HOT_TEMP;
          b_alarm_kipitim = false;
        }
        if (server.arg("alarm_hot_kip") == "keep_warm") {
          b_alarm_hot = true;
          set_temp_alarm = WARM_TEMP;
          b_alarm_kipitim = false;
        }
        alarm_Hour = server.arg("hour").toInt();
        alarm_Minut = server.arg("minut").toInt();

      }
      else {
        err_exec = true;
      }
    }
    if (server.arg("setalarm") == "L") {
      b_alarm = false;
      b_alarm_hot = false;
      b_alarm_kipitim = false;
      set_temp_alarm = 0.0;
    }
    if (server.arg("setdate") == "H") {
      if (server.hasArg("year") && server.hasArg("month") && server.hasArg("day") && server.hasArg("hour") && server.hasArg("minut") && server.hasArg("second")) {
        g_year = server.arg("year").toInt();
        g_month = server.arg("month").toInt();
        g_day = server.arg("day").toInt();
        g_hour = server.arg("hour").toInt();
        g_min = server.arg("minut").toInt();
        g_sec = server.arg("second").toInt();
        RTC.adjust(DateTime((uint16_t)g_year, (uint8_t)g_month, (uint8_t)g_day, (uint8_t)g_hour, (uint8_t)g_min, (uint8_t)g_sec));
        //        String sds=DateTime((uint16_t)g_year, (uint8_t)g_month, (uint8_t)g_day, (uint8_t)g_hour, (uint8_t)g_min, (uint8_t)g_sec).toString();
        String snd = "YEAR:" + server.arg("year") + " MONTH:" + server.arg("month") + " DAY:" + server.arg("day") + " HOUR:" + server.arg("hour") + " MINUT:" + server.arg("minut") + " SEC:" + server.arg("second");
        Serial.println(snd);
        //  Serial.println(sds);
      }
      else {
        err_exec = true;
      }

    }
  }
  Serial.print("----- set date:  ");
  Serial.println(server.arg("setdate"));
  if (!err_exec) {
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }


}


void handle_ledModes() {
  err_exec = false;
  //String get_param="";
  preferences.begin("kettle", false);


  if (server.hasArg("lm_vskipel") || server.hasArg("lm_heating") || server.hasArg("lm_hot") || server.hasArg("lm_night_light") || server.hasArg("night_light_over") || server.hasArg("lm_warm") || server.hasArg("lm_cold")) {

    if (server.arg("lm_vskipel") != "") {
      //get_param=
      ledMode_vskipel = server.arg("lm_vskipel").toInt();
      preferences.putUInt("ledMode_vskipel", ledMode_vskipel);
      led_Mode_preview = ledMode_vskipel;
      Serial.print(" ledmode vskipel=");
      Serial.println(server.arg("lm_vskipel"));
    }
    if (server.arg("lm_heating") != "") {
      ledMode_heating = server.arg("lm_heating").toInt();
      preferences.putUInt("ledMode_heating", ledMode_heating);

      led_Mode_preview = ledMode_heating;
      Serial.print(" ledmode heating=");
      Serial.println(server.arg("lm_heating"));
    }
    if (server.arg("lm_hot") != "") {
      ledMode_hot = server.arg("lm_hot").toInt();
      preferences.putUInt("ledMode_hot", ledMode_hot);

      led_Mode_preview = ledMode_hot;
      Serial.print(" ledmode hot=");
      Serial.println(server.arg("lm_hot"));
    }
    if (server.arg("lm_warm") != "") {
      ledMode_warm = server.arg("lm_warm").toInt();
      preferences.putUInt("ledMode_warm", ledMode_warm);
      led_Mode_preview = ledMode_warm;
      Serial.print(" ledmode warm=");
      Serial.println(server.arg("lm_warm"));
    }
    if (server.arg("lm_cold") != "") {
      ledMode_cold = server.arg("lm_cold").toInt();
      preferences.putUInt("ledMode_cold", ledMode_cold);

      led_Mode_preview = ledMode_cold;
      Serial.print(" ledmode cold=");
      Serial.println(server.arg("lm_cold"));
    }
    if (server.arg("lm_night_light") != "") {
      ledMode_night_light = server.arg("lm_night_light").toInt();

      preferences.putUInt("ledMode_night_light", ledMode_night_light);


      led_Mode_preview = ledMode_night_light;

      Serial.print(" ledmode nightlight=");
      Serial.println(server.arg("lm_night_light"));


    }
    timing_light_color_preview = millis();
    b_led_mode_preview = true;

    if (server.hasArg("night_light_over")) {
      if (server.arg("night_light_over") == "H") {
        b_night_light_mode_overwrite = true;
      }
      else {
        b_night_light_mode_overwrite = false;
      }
      preferences.putBool("b_night_light_mode_overwrite", b_night_light_mode_overwrite);
    }
  }
  else {
    err_exec = true;
  }
  if (!err_exec) {
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }
  preferences.end();
}

void handle_nightLED() {
  err_exec = false;
  if ( server.hasArg("ledMode_night_light")) {
    if (server.arg("ledMode_night_light") == "H") {
      ledMode = ledMode_night_light;
    }
  }
  else {
    err_exec = true;
  }
  if (!err_exec) {
    server.send(202, "text/plain", "OK");
  }
  else {
    server.send(503, "text/plain", "eror while executing");
  }
}


void handle_getSettings() {
  String str = "{\"ledMode_vskipel\":" + (String)ledMode_vskipel + ", \"ledMode_heating\":" + (String)ledMode_heating + ", \"ledMode_hot\":" + (String)ledMode_hot + ", \"ledMode_warm\":" + (String)ledMode_warm + ", \"ledMode_cold\":" + (String)ledMode_cold + ", \"ledMode_night_light\":" + (String)ledMode_night_light + ", \"b_night_light_mode_overwrite\":"  +boolTOstr(b_night_light_mode_overwrite)  + "}";
/*
 * bool_peremennaja ? "true" : "false"
 */

/*
 * bool_peremennaja ? "true" : "false"
 */
  server.send(202, "text/plain", str);
}

void handle_getStates() {
   String str = "{\"b_hotting\":" + boolTOstr(b_hotting)+"\"kipitim\":" + boolTOstr(kipitim)+ ", \"keep_hot\":" + boolTOstr(keep_hot) + ", \"set_temp\":" + (String)set_temp + 
   + ", \"b_alarm\":" + boolTOstr(b_alarm) + ", \"b_alarm_hot\":" + boolTOstr(b_alarm_hot)+ ", \"b_alarm_kipitim\":" +  boolTOstr(b_alarm_kipitim) + ", \"alarm_Hour\":" + (String)alarm_Hour + ", \"alarm_Minut\":" + (String)alarm_Minut  + ", \"set_temp_alarm\":" + (String)set_temp_alarm +"}";
  
 // bool  = false, b_teapot_off = false, 
 

}

String boolTOstr(bool b){
  if(b){
    return "true" ;
  }
  else{
    return "false";
  }
}
