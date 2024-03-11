


void hotting() {
  /* Serial.print("b_hotting_");
    Serial.print(b_hotting);
    Serial.print("   b_teapot_off_");
    Serial.print(b_teapot_off);
    Serial.print("   kipitim_");
    Serial.print(kipitim);
    Serial.print("   heating_times=");
    Serial.print(heating_times);
    Serial.print("   set_temp_");
    Serial.println(set_temp);
  */
  if (temp_now == ERROR_TEMP || b_teapot_off) {
    kipitim = false;
    b_hotting = false;
    b_teapot_off = false;
    set_temp = 0;
    heating_times = 0;
    digitalWrite(DIG_OUT1, false);
  }
  if (b_hotting) {
    Serial.println("********* b_hotting");
    /*if (millis() - timing_keep_hot > TIMING_KEEP_HOT) {

      }*/

    if (temp_now >= set_temp) {

      Serial.println("________________ TEMP NORM________________");
      digitalWrite(DIG_OUT1, false);
      if (kipitim) {
        b_hotting = false;
        kipitim = false;
        if (!b_night_light_mode_overwrite) {
          ledMode = ledMode_vskipel;
        }
        set_temp = 0;
      }
      else {
        if (!b_night_light_mode_overwrite) {
          ledMode = ledMode_hot;

        }
        b_heating_times = true;
      }
    }
    else {
      if (temp_now < set_temp - TEMP_BOUND) {
        digitalWrite(DIG_OUT1, true);
        if (!b_night_light_mode_overwrite) {
          ledMode = ledMode_heating;
        }
        Serial.println(" ____HEATING_______");
        //b_hotting2=true;
        if (!kipitim && b_heating_times) {
          heating_times++;
          b_heating_times = false;
        }

      }
      /*else{
        digitalWrite(DIG_OUT1, false);
         if(kipitim){
              b_hotting=false;
           }
        }*/
    }
    if (heating_times >= MAX_HEATING_TIMES) {
      b_teapot_off = true;

    }
  }

  /*if(!b_hotting && b_hotting2){
    b_hotting2=false;
     Serial.println("-------------!b_hotting && b_hotting2");
     digitalWrite(DIG_OUT1, false);
    }*/

}


void get_Temp_now() {
  uint8_t i;

  float average;


  for (i = 0; i < NUMSAMPLES; i++) {

    samples[i] = analogRead(THERMISTORPIN);

    delay(10);

  }


  average = 0;

  for (i = 0; i < NUMSAMPLES; i++) {

    average += samples[i];

  }

  average = average / NUMSAMPLES;
  float BCOEFFICIENT_temp = BCOEFFICIENT;
  BCOEFFICIENT = BCOEFFICIENT - map(analogRead(THERMISTORPIN), 2300, 3700, 0, 695);

  average = 4095 / average - 1;

  average = SERIESRESISTOR / average;

  float steinhart;

  steinhart = THERMISTORNOMINAL / average ; // (R/Ro)

  steinhart = log(steinhart); // ln(R/Ro)

  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)

  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)

  steinhart = 1.0 / steinhart; // инвертируем

  steinhart -= 273.15; // конвертируем в градусы по Цельсию
  temp_now = steinhart;
  /*
    Serial.print("ADC: ");
    Serial.print(average);
    Serial.print(" TEMP: ");
    Serial.print(temp_now);
     Serial.print("ADC: ");
    Serial.println(analogRead(THERMISTORPIN));*/
  BCOEFFICIENT = BCOEFFICIENT_temp;
}
