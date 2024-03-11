#define WS2812_PIN 14
#define WS2812_NUMPIXELS   26
#define LED_COUNT 26          // число светодиодов в кольце/ленте
#define LED_DT 14             // пин, куда подключен DIN ленты


enum teapot_Show_LED_Mode  {
  vskipel, heating, hot, warm, cold
};

teapot_Show_LED_Mode tptLedMode = cold;


#define dLM_vskipel 16
#define dLM_heating 22
#define dLM_hot 2
#define dLM_warm 2
#define dLM_cold 2
#define dLM_night_light 890
#define dnight_light_overwrite false

volatile int ledMode = 30;
volatile int ledMode_vskipel = 16;
volatile int ledMode_heating = 22;
volatile int ledMode_hot = 2;
volatile int ledMode_warm = 2;
volatile int ledMode_cold = 2;
volatile int ledMode_night_light = 890;
volatile int led_Mode_preview = 999;
int max_bright = 255;          // максимальная яркость (0 - 255)

bool b_night_light_mode_overwrite = false, b_led_mode_preview = false, b_changed_color_mode = false;
volatile int o_CLR_R = 0, o_CLR_G = 128, o_CLR_B = 0;


// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

volatile byte modeCounter;
volatile boolean changeFlag;
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
