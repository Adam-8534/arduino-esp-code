// SoftwareSerial for esp8266 - Version: Latest 
#include <SoftwareSerial.h> //Allows serial connection to ESP8266 to have pins changed
#include <Temperature_LM75_Derived.h> //temperature
SoftwareSerial ESPserial(2, 3); // RX | TX
//GLOBAL VARIABLES

// plant stuff
String plantType = "Arid";
//Variable setup for moisture sensors
const int AirValue = 567;
const int WaterValue = 295;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;
Generic_LM75 temperature;

// Motor Control Pin Numbers
int enA = 9;
int in1 = 8;
int in2 = 7;

/////////////////////////////////////////////////////////////////////////////////////////////////
//SETUP I/O PINS
void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  //Begin Serial data transmission
  Serial.begin(115200);
  ESPserial.begin(115200);
  Wire.begin(); //I2C
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  delay(5000);
 double temp;
 int soilMoisture;
 
 soilMoisture = GetSM();
 temp = GetTemp();
 CheckPlantSM();
 
 //Test stuff
  SendData(soilMoisture, temp);

  delay(2000);
 
} //end of void loop

 ////////////////////////////////////////////////////////////////////////////////////////////////////
  //PLANT FUNCTION
  void WaterPlant() {
    // Turn on Motor
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enA, 255); //Set speed: possible range 0 to 255
    delay(1000); //time on
    // change motor dir
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(2000);
    // Now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(5000);
  }
///////////////////////////////////////////////////////////////////////////
void SendData(int SMS, double temp)
{
  String data = String(SMS) + "/" + String(temp);
  // String stringTemp = "T" + (int)temp;
  ESPserial.print(data);
  // Serial.println(data);
  Serial.println("Moisture Percentage: " + String(SMS) + "%");
  Serial.println("Temperature: " + String(temp) + " C");
  delay(1000);
  // ESPserial.write(temp);
  // Serial.println(temp);
  //Serial.print(temp);
  //delay(1000);
}
///////////////////////////////////////////////////////////////////////////
int GetSM()
{
    //FIND MOISTURE SENSOR && CALCULATE VALUE
  soilMoistureValue = analogRead(A0); //put Sensor insert into soil
  //Serial.println(soilMoistureValue); //raw sensor value
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
if (soilmoisturepercent >= 100)
  {
  return 100;
  }
  else if(soilmoisturepercent <=0)
  {
  return 0;
  }
  return soilmoisturepercent;
}
////////////////////////////////////////////////////////////
// function that checks the plant type and the soil mositure to see if we need to water the plant.
void CheckPlantSM()
{
  GetSM();
  // Serial.println("Soil moisture %");
  // Serial.println(soilmoisturepercent);
  // Serial.println(plantType);
  if (plantType == "Tropical" && soilmoisturepercent < 80)
  {
    WaterPlant();
  }
  else if (plantType.equals("Temperate") && soilmoisturepercent < 50)
  {
    WaterPlant();
  }
  else if (plantType.equals("Arid") && soilmoisturepercent < 30)
  {
    // Serial.println("We are watering the Arid plant!!!!!!");
    WaterPlant();
  }
}

double GetTemp()
{
  return temperature.readTemperatureC(); //Return temperature in Celcius
}
