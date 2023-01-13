#include <Arduino.h>
#include <DFRobot_BME280.h>
#include <Wire.h>
#include <Wifi.h>
#include <HTTPClient.h>
#include <string.h>
// #include <Arduino_JSON.h>

#define SEA_LEVEL_PRESSURE 1015.0f
#define I2C_SDA 27
#define I2C_SCL 26

typedef DFRobot_BME280_IIC BME; // ******** use abbreviations instead of full names ********

BME *evnSensor;
TwoWire I2C = TwoWire(0);

const char *ssid = "Owls Nest";
const char *pw = "gp8gp8gp8";
const char *server = "http://192.168.50.119:5000/api/v1/weathers/create";

WiFiClient wific;
HTTPClient http;

// show last sensor operate status
void printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch (eStatus)
  {
  case BME::eStatusOK:
    Serial.println("everything ok");
    break;
  case BME::eStatusErr:
    Serial.println("unknow error");
    break;
  case BME::eStatusErrDeviceNotDetected:
    Serial.println("device not detected");
    break;
  case BME::eStatusErrParameter:
    Serial.println("parameter error");
    break;
  default:
    Serial.println("unknow status");
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  I2C.begin(I2C_SDA, I2C_SCL);
  evnSensor = new BME(&I2C, 0x77);
  evnSensor->reset();
  Serial.println("bme read data test");
  while (evnSensor->begin() != BME::eStatusOK)
  {
    Serial.println("bme begin failed");
    delay(2000);
  }
  Serial.println("bme begin success");
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("not connected!");
  }
  Serial.println("wifi connected!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  double temp = evnSensor->getTemperature();
  double press = evnSensor->getPressure();
  double alti = evnSensor->calAltitude(SEA_LEVEL_PRESSURE, press);
  double humi = evnSensor->getHumidity();
  Serial.println();
  Serial.println("======== start print ========");
  Serial.print("temperature (unit Celsius): ");
  Serial.println(temp);
  Serial.print("pressure (unit pa):         ");
  Serial.println(press);
  Serial.print("altitude (unit meter):      ");
  Serial.println(alti);
  Serial.print("humidity (unit percent):    ");
  Serial.println(humi);
  Serial.println("========  end print  ========");
  delay(1000);

  if (WiFi.status() == WL_CONNECTED)
  {
    http.begin(wific, server);
    String data = "temp=";
    String st_temp = String(temp, 5);
    String st_press = String(press, 5);
    String st_alti = String(alti, 5);
    String st_humi = String(humi, 5);
    data += st_temp;
    data += "&press=";
    data += st_press;
    data += "&alt=";
    data += st_alti;
    data += "&humid=";
    data += st_humi;
    http.POST(data);
    http.end();
  }
}