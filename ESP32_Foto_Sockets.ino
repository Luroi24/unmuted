#include "esp_camera.h"
#include "Arduino.h"
#include "WiFi.h"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Variables
camera_fb_t * fb = NULL;
WiFiServer wifiServer(5005);
const char* ssid = "Totalplay-00A8";
const char* password = "00A84E32mc36n3su";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectándose a WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("¡Conectado!");
  Serial.println(WiFi.localIP());

  wifiServer.begin();

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

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Inicialización de cámara erronea: 0x%x", err);
    return;
  }
}

void loop() {
  WiFiClient client;
  const char* host = "192.168.100.7";
  const int port = 1234;
  if (!client.connect(host, port)) {

      Serial.println("Conexión al host fallida");

      delay(1000);
      return;
  }

  Serial.println("¡Conexión al host exitosa!");

  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb) {
     Serial.println("Captura de cámara fallida.");
      return;
  } else {
      Serial.println("¡Captura de cámara exitosa!");
  }
  const char *data = (const char *)fb->buf;
  Serial.print("Size of image:");
  Serial.println(fb->len);
  Serial.print("Shape->width:");
  Serial.print(fb->width);
  Serial.print("height:");
  Serial.println(fb->height);
  client.print("Shape->width:");
  client.print(fb->width);
  client.print("height:");
  client.println(fb->height);
  //delay(1000);
  client.write(data, fb->len);
  esp_camera_fb_return(fb);
  client.stop();

  Serial.println("Desconectando...");

  //delay(2000);
}
