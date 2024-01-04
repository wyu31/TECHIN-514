#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDRESS 0x3C
#define BME_ADDRESS 0x76
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_BME280 bme;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int motorPin = A1;

const int bufferSize = 10;
float buffer[bufferSize];
int bufferIndex = 0;
float sum = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (!bme.begin(BME_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  if (!veml.begin()) {
    Serial.println("Could not find a valid VEML7700 sensor, check wiring!");
    while (1);
  }
  pinMode(motorPin, OUTPUT);
}

void loop() {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  float lux = veml.readLux();

  sum -= buffer[bufferIndex];
  buffer[bufferIndex] = lux;
  sum += buffer[bufferIndex];
  bufferIndex = (bufferIndex + 1) % bufferSize;
  float averageLux = sum / bufferSize;

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temp: "); display.print(temperature); display.println(" C");
  display.print("Humidity: "); display.print(humidity); display.println("%");
  display.print("Pressure: "); display.print(pressure); display.println(" hPa");
  display.print("Light: "); display.print(averageLux); display.println(" lx");
  display.display();

  if (lux > 10000) {
    digitalWrite(motorPin, HIGH);
    Serial.println("Motor ON");
  } else {
    digitalWrite(motorPin, LOW);
    Serial.println("Motor OFF");
  }

  Serial.print("Raw Lux: "); Serial.print(lux);
  Serial.print(" | Filtered Lux: "); Serial.println(averageLux);

  delay(1000);
}