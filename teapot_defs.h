#define ERROR_TEMP 120.0
#define KIPITIM_TEMP 105.0
#define WARM_TEMP 50.0
#define HOT_TEMP 80.0
#define TIMING_KIPITIM 50000
#define TIMING_KEEP_HOT 100000
#define TEMP_BOUND 5.0
#define MAX_HEATING_TIMES 10


float temp_now = 0.0;

int heating_times = 0;
bool b_heating_times = true;

bool b_hotting = false, b_teapot_off = false, keep_hot = false, kipitim = false;
float set_temp, set_temp_alarm;
bool b_alarm = false, b_alarm_hot = false, b_alarm_kipitim = false;
int alarm_Hour = 0, alarm_Minut = 0;


////////////////////
//                     THERMISTOR


#define THERMISTORPIN 39
//+++   term options
#define THERMISTORNOMINAL 8250
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 100
float BCOEFFICIENT = 3095.0;  //2700
#define SERIESRESISTOR 10000
int samples[NUMSAMPLES];
