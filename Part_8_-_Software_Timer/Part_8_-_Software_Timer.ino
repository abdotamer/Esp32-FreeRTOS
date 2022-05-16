#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 20;

// Pins
static const int led_pin = 2;
static TimerHandle_t timer = NULL;


void CBTimer (TimerHandle_t xTimer) {
  digitalWrite(led_pin, LOW);
}
void readSerial(void *parameters) {

  char c;

  // Loop forever
  while (1) {

    // Read characters from serial
    if (Serial.available() > 0) {
      c = Serial.read();
      digitalWrite(led_pin, HIGH);
      Serial.print(c);
      xTimerStart(timer,portMAX_DELAY);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  while (timer == NULL)
  {
    timer = xTimerCreate("Timer", 5000 / portTICK_PERIOD_MS, pdTRUE, (void *) 0, CBTimer);
  }
  xTaskCreatePinnedToCore(readSerial, "Read Serial", 1024, NULL, 1, NULL, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
