#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int LedPin = LED_BUILTIN;
xQueueHandle FirstQue;
xQueueHandle SecQue;
void LedToggle1(void * par)
{
  int count = 0, blinkRate = 500;
  String msg;
  while (1)
  {
    if (xQueueReceive(FirstQue, &blinkRate, 0))
    {
      msg = "blinking Rate will change";
      Serial.println(msg);
      count = 0;
    }
    count++;
    digitalWrite(LedPin, HIGH);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    digitalWrite(LedPin, LOW);
    if (count % 100 == 0)
    {
      xQueueSend(SecQue, &count, 0);

    }

    if (count == 100000000)
      count = 0;
    vTaskDelay(blinkRate / portTICK_PERIOD_MS);
  }
}
static const uint8_t buf_len = 12;
char buf[buf_len];
bool Done = false;
uint8_t idx = 0;

void ReadSerial(void *parameters) {

  char c;
  int Delay = 500;
  // Clear whole buffer
  memset(buf, 0, buf_len);
  Serial.println("Welcome To The Queue Demo Project");
  // Loop forever
  while (1) {

    // Read characters from serial
    if (Serial.available() > 0 && !Done) {
      c = Serial.read();
      // Update delay variable and reset buffer if we get a newline character
      if (c == '\n') {
        Done = true;
      } else {
        // Only append if index is not over message limit
        if (idx < buf_len - 1) {
          buf[idx] = c;
          idx++;
        }
      }
    }
    const String command = "delay ";

    String DelayValue = "";
    if (Done)
    {
      bool RightCommand = true;
      for (int i = 0; i < idx; i++)
      {
        Serial.print(buf[i]);
        if (i <= 5 && buf[i] != command[i])
          RightCommand = false;
      }
      Serial.println("");
      if (RightCommand)
      {
        for (int i = 6; i < idx; i++)
        {
          if (isDigit(buf[i]))
            DelayValue += buf[i];

        }
        Delay = DelayValue.toInt();
        xQueueSend(FirstQue, &Delay, 0);
      }
      else
      {
        Serial.println("Enter a valid command");
      }
      memset(buf, 0, buf_len);
      idx = 0;
      DelayValue = "";
      Done = false;
    }
    int msg;

    if (xQueueReceive(SecQue, &msg, 0))
    {
      Serial.print("Blinked ");
      Serial.print(msg);
      Serial.print(" times at the rate a blink each ");
      Serial.print(Delay);
      Serial.println("ms");
    }



    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LedPin, OUTPUT);
  FirstQue = xQueueCreate(10, sizeof(int));
  SecQue = xQueueCreate(5, sizeof(int));
  xTaskCreatePinnedToCore(LedToggle1, "Toggle Led 1", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(ReadSerial, "Reading From Serial", 1024, NULL, 1, NULL, app_cpu);

}

void loop() {
  // put your main code here, to run repeatedly:

}
