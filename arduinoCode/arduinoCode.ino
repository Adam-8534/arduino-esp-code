// SoftwareSerial for esp8266 - Version: Latest 
#include <SoftwareSerial.h> //Allows serial connection to ESP8266 to have pins changed
#include <Temperature_LM75_Derived.h> //temperature
SoftwareSerial ESPserial(2, 3); // RX | TX
//GLOBAL VARIABLES

// plant stuff
char plantType[100] = "Tropical";
//Variable setup for moisture sensors
const int AirValue = 3550;
const int WaterValue = 310;
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
 double temp;
 int soilMoisture;
 
 soilMoisture = GetSM();
 temp = GetTemp();
 
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
    delay(3000); //time on
    // Now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(2500);
  }
///////////////////////////////////////////////////////////////////////////
//PROTOTYPE TEMP SENSOR CODE TO INTEGRATE LATER
/*
  void loop() {
  Serial.print("Temperature = ");
  Serial.print(temperature.readTemperatureC());
  Serial.println(" C");
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if(soilmoisturepercent >= 100)
  {
  Serial.println("100 %");
  }
  else if(soilmoisturepercent <=0)
  {
  Serial.println("0 %");
  }
  else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
  {
  Serial.print(soilmoisturepercent);
  Serial.println("%");

  }
  delay(2500);
  } */
///////////////////////////////////////////////////////////////////////////
void SendData(int SMS, double temp)
{
  String data = String(SMS) + "/" + String(temp);
  // String stringTemp = "T" + (int)temp;
  ESPserial.print(data);
  Serial.println(data);
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
  if (plantType == "Tropical" && soilmoisturepercent < 100)
  {
    // WaterPlant();  
  }
  else if (plantType == "Temperate" && soilmoisturepercent < 50)
  {
    // WaterPlant();  
  }
  else if (plantType == "Arid" && soilmoisturepercent < 0)
  {
    // WaterPlant();  
  }
}

double GetTemp()
{
  return temperature.readTemperatureC(); //Return temperature in Celcius
}
