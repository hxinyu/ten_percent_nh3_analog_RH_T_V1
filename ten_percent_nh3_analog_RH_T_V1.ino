#include <QueueArray.h>

/***************************************************
reads analog output of high concentration 1-10% ammonia sensor
Reads analog RH and T from defined pins
converts 4-20ma signal to 0-5 V signal
****************************************************/

#define SensorAnalogPin A2  //this pin read the analog voltage from the NH3 sensor
#define Sensor2AnalogPin A3 //2nd NH3 sensor analog voltage
#define RH_AnalogPin A5
#define T_AnalogPin A4
#define VREF  5.125   //default uno reference

QueueArray <double> queueData60;
double deltaPPM = 0;

long interval = 1000;
unsigned long oldtime = 0;
unsigned long prevtime = 0;

int index = 0;
int indexV = 0;
int indexBits = 0;
double rollingsum;
double rollingsumV;
double rollingsumBits;
double analogVoltage;
double analogBits;
double analogTBits;
double analogRHBits;

void setup()
{
    Serial.begin(9600);
}

void loop()
{ 
  double V = analogReadV();
  double tempV = Vavg(V);
  //double ppm = analogReadPPM();
  //double tempppm = ppmavg(ppm);
  //double bits = analogReadBits();
  //double tempBits = bitsavg(bits);
  
  //Serial.println(rollingsum/index);
  unsigned long time = millis()/1000;
  if (time > prevtime)
  {
    Serial.print("Time: ");
    
    //prints time since program started
    Serial.print(time);
    Serial.print(" s | ");

    double ppm = 2.5603 * tempV - 2.4607;
    if(ppm<0){ppm = 0;}

    double ppmcurrent = ppm;
    queueData60.enqueue(ppm);
    if (queueData60.count() >  59){
      double oldppm = queueData60.pop();
      deltaPPM = ppmcurrent - oldppm;
    }
    
    Serial.print(ppm,2);
    Serial.print(" % | ");
  
    Serial.print(tempV,3);
    Serial.print(" V | ");

    Serial.print(deltaPPM, 2);
    Serial.print(" 60s dPPM | ");

    Serial.print(analogReadTemp(), 2);
    Serial.print("F | ");

    Serial.print(analogReadRH(), 2);
    Serial.println("% RH");
    //Serial.println(tempBits, 3);
    /*else
    {
      Serial.println("calibrating");
    }*/
    prevtime = time;
    index = 0;
    rollingsum = 0;
    indexV = 0;
    rollingsumV = 0;
    indexBits = 0;
    rollingsumBits = 0;
  }
    //delay(1);
}

double analogReadBits(){
  analogBits = analogRead(SensorAnalogPin);
  return analogBits;
}

double analogReadTemp(){
  analogTBits = analogRead(T_AnalogPin);
  return analogTBits/1023*236-22;
}

double analogReadRH(){
  analogRHBits = analogRead(RH_AnalogPin);
  return analogRHBits/10.23;
}

double bitsavg(double newBits){
  rollingsumBits += newBits;
  indexBits++;

  double rollingavgBits = rollingsumBits/indexBits;
  return rollingavgBits;
}

double analogReadV()
{
   analogVoltage = analogRead(SensorAnalogPin) / 1023.0 * VREF; //https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/

   return analogVoltage;
}

double Vavg(double newV)
{
  rollingsumV += newV;
  indexV++;

  double rollingavgV = rollingsumV/indexV;
  return rollingavgV;
}

double analogReadPPM()
{
   analogVoltage = analogRead(SensorAnalogPin) / 1023.0 * VREF; //https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
   double ppm = 2.5709 * analogVoltage - 2.494;  //linear relationship(0.848 V for 0 ppm and 4.23V for 10% NH3)
   if(ppm<0)  ppm=0;
   //else if(ppm>5)  ppm = 5;
   return ppm;
}

double ppmavg(double newppm)
{
  rollingsum += newppm;
  index++;

  double rollingavg = rollingsum/index;
  return rollingavg;
}

