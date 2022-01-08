#include <Arduino.h>
#include <WiFiMulti.h>

#if defined(ARDUINO_M5STACK_Core2)
  #include <M5Core2.h>
  #define SERVO_PIN_X 13
  #define SERVO_PIN_Y 14
#elif defined( ARDUINO_M5STACK_FIRE )
  #include <M5Stack.h>
  #define SERVO_PIN_X 21
  #define SERVO_PIN_Y 22
#elif defined( ARDUINO_M5Stack_Core_ESP32 )
  #include <M5Stack.h>
  #define SERVO_PIN_X 21
  #define SERVO_PIN_Y 22
#endif

#include <Avatar.h> // https://github.com/meganetaaan/m5stack-avatar
#include <ServoEasing.hpp> // https://github.com/ArminJo/ServoEasing       
#include "faces/robo8080/AtaruFace.h"
#include "faces/robo8080/RamFace.h"
#include "faces/katsuyoshi/PandaFace.h"
#include "faces/katsuyoshi/TVFace.h"

#if defined(ARDUINO_M5STACK_Core2)
#define USE_VOICE_TEXT //for M5STACK_Core2 Only
#endif

#ifdef USE_VOICE_TEXT
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SLipSync.h"
#include "AudioFileSourceVoiceTextStream.h"

// Copy Env.h.sample to Env.h
// And replace YOUR_WIFI_SSID with SSID name, YOUR_WIFI_PASSWORD with the password.
#include "Env.h"

WiFiMulti wifiMulti;

AudioGeneratorMP3 *mp3;
AudioFileSourceVoiceTextStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SLipSync *out;
const int preallocateBufferSize = 40*1024;
uint8_t *preallocateBuffer;
#endif

#define AVATAR_ATARU        0
#define AVATAR_RAM          1
#define AVATAR_STACK        2
#define AVATAR_SUSU         3
#define AVATAR_BRAUN        4
#define NUMBER_OF_AVATARS   5

using namespace m5avatar;
Avatar avatar;
Face* faces[NUMBER_OF_AVATARS];
ColorPalette* cps[NUMBER_OF_AVATARS];
int avatar_indexes[3] = {0, 1, 2};
int current_avatar_index = 0;

#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 75 //90

ServoEasing servo_x;
ServoEasing servo_y;

// When you uncomment this definition, its mouth slowly opens and closes continuously.
//#define TEST_MOUTH

void behavior(void *args)
{
  float gazeX, gazeY;
  DriveContext *ctx = (DriveContext *)args;
  Avatar *avatar = ctx->getAvatar();
   for (;;)
  {
#ifdef USE_VOICE_TEXT
#ifndef TEST_MOUTH
    int level = out->getLevel();
    level = abs(level);
    if(level > 10000)
    {
      level = 10000;
    }
    float open = (float)level/10000.0;
    avatar->setMouthOpenRatio(open);
#else
  static float open = 0.0;
  static float delta = 0.05;
  avatar->setMouthOpenRatio(open);
  open += delta;
  if ((open > 1.0) || (open < 0.0)) {
    delta = -delta;
  }
#endif
#endif

    avatar->getGaze(&gazeY, &gazeX);
    servo_x.setEaseTo(START_DEGREE_VALUE_X + (int)(20.0 * gazeX));
    if(gazeY < 0) {
      servo_y.setEaseTo(START_DEGREE_VALUE_Y + (int)(20.0 * gazeY));
    } else {
      servo_y.setEaseTo(START_DEGREE_VALUE_Y + (int)(10.0 * gazeY));
    }
    synchronizeAllServosStartAndWaitForAllServosToStop();
  
    delay(33);
//    delay(50);
  }
}

void setup() {
  
#ifdef USE_VOICE_TEXT
  preallocateBuffer = (uint8_t*)ps_malloc(preallocateBufferSize);
#endif

#if defined(ARDUINO_M5STACK_Core2)
  M5.begin(true, true, true, false, kMBusModeOutput);
#elif defined( ARDUINO_M5STACK_FIRE ) || defined( ARDUINO_M5Stack_Core_ESP32 )
  M5.begin(true, true, true, false); // Grove.Aを使う場合は第四引数(I2C)はfalse
#endif
  if (servo_x.attach(SERVO_PIN_X, START_DEGREE_VALUE_X, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo x");
  }
  if (servo_y.attach(SERVO_PIN_Y, START_DEGREE_VALUE_Y, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y");
  }
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  setSpeedForAllServos(60);

#ifdef USE_VOICE_TEXT
  M5.Axp.SetSpkEnable(true);
  M5.Lcd.setBrightness(30);
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  delay(1000);

  Serial.println("Connecting to WiFi");
  M5.Lcd.print("Connecting to WiFi");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
#ifdef WIFI_SSID2
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
#endif
  // If you have more points, repeat the above three lines like WIFI_SSID3, 4, 5 ...

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    M5.Lcd.print(".");
  }
  Serial.println("\nConnected");
  M5.Lcd.println("\nConnected");
  
  syncTime();

  audioLogger = &Serial;
  out = new AudioOutputI2SLipSync();
  out->SetPinout(12, 0, 2);           // ピン配列を指定（BCK, LRCK, DATA)BashCopy
  mp3 = new AudioGeneratorMP3();
#endif

  faces[AVATAR_ATARU] = new AtaruFace();
  faces[AVATAR_RAM] = new RamFace();
  faces[AVATAR_STACK] = avatar.getFace();
  faces[AVATAR_SUSU] = new PandaFace();
  faces[AVATAR_BRAUN] = new TVFace();

  for (int i = 0; i < NUMBER_OF_AVATARS; i++) {
    cps[i] = new ColorPalette();
  }
  cps[AVATAR_ATARU]->set(COLOR_PRIMARY, TFT_BLACK);
  cps[AVATAR_ATARU]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[AVATAR_ATARU]->set(COLOR_SECONDARY, TFT_WHITE);

  cps[AVATAR_RAM]->set(COLOR_PRIMARY, TFT_BLACK);
  cps[AVATAR_RAM]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[AVATAR_RAM]->set(COLOR_SECONDARY, TFT_WHITE);

  cps[AVATAR_SUSU]->set(COLOR_PRIMARY, TFT_BLACK);
  cps[AVATAR_SUSU]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[AVATAR_SUSU]->set(COLOR_SECONDARY, TFT_WHITE);

  cps[AVATAR_BRAUN]->set(COLOR_PRIMARY, TFT_WHITE);
  cps[AVATAR_BRAUN]->set(COLOR_BACKGROUND, TFT_BLUE);
  cps[AVATAR_BRAUN]->set(COLOR_SECONDARY, TFT_YELLOW);

  avatar.init();
  avatar.setFace(faces[0]);
  avatar.setColorPalette(*cps[0]);
  avatar.addTask(behavior, "behavior");
}

#ifdef USE_VOICE_TEXT
char *texts[NUMBER_OF_AVATARS] = {
  "こんにちは、僕の名前はあたるです。よろしくね！",
  "こんにちは、私の名前はラムちゃんです。よろしくね！",
  "こんにちは、私の名前はスタックちゃんです。よろしくね！",
  "こんにちは、私の名前はスースーです。よろしくね！",
  "こんにちは、僕の名前はブラウン君です。よろしくね！"
};

char *tts_params[] = {
  "&emotion_level=2&emotion=happiness&format=mp3&speaker=takeru&volume="TSS_VOLUME"&speed=100&pitch=130",
  "&emotion_level=2&emotion=happiness&format=mp3&speaker=hikari&volume="TSS_VOLUME"&speed=120&pitch=130",
  "&emotion_level=4&emotion=anger&format=mp3&speaker=bear&volume="TSS_VOLUME"&speed=120&pitch=100",
  "&emotion_level=2&emotion=sadness&format=mp3&speaker=hikari&volume="TSS_VOLUME"&speed=90&pitch=60",
  "&format=mp3&speaker=show&volume="TSS_VOLUME"&speed=150&pitch=200"
};

void VoiceText_tts(char *text,char *tts_parms) {
    if (mp3->isRunning()) return;
    file = new AudioFileSourceVoiceTextStream( text, tts_parms);
    buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
    delay(100);
    mp3->begin(buff, out);
}

time_t time_synced_at;
uint32_t millis_since_time_synced;

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void syncTime() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
  Serial.print(timeinfo.tm_year);

  time_synced_at = mktime(&timeinfo);
  millis_since_time_synced = millis();
}

time_t now() {
  time_t n = (time_t)((millis() + 9 * 3600 * 1000 - (time_t)millis_since_time_synced) / 1000) + time_synced_at;
  return n;
}

void announce_time_if_needed() {
  static uint32_t st = millis();
  uint32_t n = millis();
  if (n - st >= 60000) {
    time_t t = now();
    struct tm *t_tm = gmtime(&t);
    if (TIME_ANNOUNCE_INTERVAL == 0 ||
        t_tm->tm_min % TIME_ANNOUNCE_INTERVAL == 0) {
      say_time();
    }
    st = n;
  }
}

void say_time() {
  time_t n = now();
  struct tm *t = gmtime(&n);
  char msg[64];
  sprintf(msg, "%d時%d分になりました。", t->tm_hour,t->tm_min);
  
  VoiceText_tts(msg, tts_params[current_avatar_index]);
  avatar.setExpression(Expression::Neutral);
  Serial.println("mp3 begin");
}

#endif


void loop() {
  int index;
  M5.update();
#ifdef USE_VOICE_TEXT
  static int lastms = 0;
  if (M5.BtnA.wasPressed())
  {
    index = avatar_indexes[0];
    avatar_indexes[0] = 3 - index;
    current_avatar_index = index;
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
    delay(1000);
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(texts[index], tts_params[index]);
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
  }
  if (M5.BtnB.wasPressed())
  {
    index = avatar_indexes[1];
    avatar_indexes[1] = 5 - index;
    current_avatar_index = index;
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
    delay(1000);
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(texts[index], tts_params[index]);
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
  }
  if (M5.BtnC.wasPressed())
  {
    index = avatar_indexes[2];
    current_avatar_index = index;
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
    delay(1000);
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(texts[index], tts_params[index]);
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
  }
  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()) {
      mp3->stop();
      out->setLevel(0);
      delete file;
      delete buff;
      Serial.println("mp3 stop");
    }

  }

#ifdef USE_VOICE_TEXT
  announce_time_if_needed();
#endif

#else
  if (M5.BtnA.wasPressed())
  {
    index = avatar_indexes[0];
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
  }
  if (M5.BtnB.wasPressed())
  {
    index = avatar_indexes[1];
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
  }
  if (M5.BtnC.wasPressed())
  {
    index = avatar_indexes[2];
    avatar.setFace(faces[index]);
    avatar.setColorPalette(*cps[index]);
  }
#endif
}
