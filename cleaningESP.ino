
#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>



// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

const char* ssid = "TurkTelekom_TPFD80_2.4GHz";
const char* password = "Berk18Halil22";

const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

int EN_A = 2;
int EN_B = 16;
int IN1 = 12;
int IN2 = 13;
int IN3 = 14;
int IN4 = 15;

bool stopEngine = true;


WiFiClient espClient;
PubSubClient client(espClient);


void startCameraServer();

void setup() {

  pinMode(EN_A,OUTPUT);
  pinMode(EN_B,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_init();
  connectWifi();
  connectMQQT();

}

void loop() {

  client.loop();
  delay(100);

}

void camera_init(){
  // Kamera yayın bağlantısı
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  }

void connectWifi(){

   // WiFi Bağlantısı
   WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(Serial.readString());
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void connectMQQT()
{
    /* MQTT BAĞLANTISI */
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect(mqttServer, mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }


  client.subscribe("POSITION");

}

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
Serial.println(payloadStr);
  if(payloadStr.equals("STOP")){
    stopEngine = true;
    stopMotors();
    return;
  }else   if(payloadStr.equals("START")){
    stopEngine = false;
    return;
  }else if(payloadStr.equals("RIGHT")){
     frontRight();
    }else if(payloadStr.equals("LEFT")){
      frontLeft();
    }else if(payloadStr.equals("FRONT")){
      front();
    }

}

void front(){
  digitalWrite(EN_A,HIGH);
  digitalWrite(EN_B,HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3,  LOW);
  digitalWrite(IN4, HIGH);


}

void stopMotors(){
 //Para o motor A
 digitalWrite(EN_A,LOW);
 digitalWrite(EN_B,LOW);
 digitalWrite(IN1,  LOW);
 digitalWrite(IN2,  LOW);
  //Para o motor B
 digitalWrite(IN3,  LOW);
 digitalWrite(IN4,  LOW);

}
void frontLeft(){
  digitalWrite(EN_A,HIGH);
  digitalWrite(EN_B,HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2,  LOW);
  digitalWrite(IN3,  LOW);
  digitalWrite(IN4,  LOW);
}
void frontRight(){
 digitalWrite(EN_A,HIGH);
 digitalWrite(EN_B,HIGH);
 digitalWrite(IN1,  LOW);
 digitalWrite(IN2,  LOW);
 digitalWrite(IN3,  LOW);
 digitalWrite(IN4, HIGH);
}