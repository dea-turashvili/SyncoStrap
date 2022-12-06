/******************************************************************************
SyncoStrap.ino
This code is compatible with Arduino Uno, MPU6050 (Accelerometer), 
AD8232 (ECG Sensor), and DHT11(Temp and Humidity Sensor). It contains an
algorithm that can determine whether or not someone is at risk of fainting.
Contributers include Esha Vaishnav, Yousuf Shehadi, Ruhaan Baghaat, and Dea Turashvili

******************************************************************************/
#include <Wire.h>
#include <MPU6050.h>
#include <DHT.h>;

#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

MPU6050 mpu;

int standFactor = 0;
float prevNormAccel = 0;

int count,time2,bpm;
int time1=0;
int threshold=380;
int bpm1 = 0;

int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
const int buzzer = 9;

void setup() {

  pinMode(buzzer, OUTPUT);
  // initialize the serial communication:
  Serial.begin(115200);
  //pinMode(10, INPUT); // Setup for leads off detection LO +
  //pinMode(11, INPUT); // Setup for leads off detection LO -

    Serial.println("Initialize MPU6050");
  Serial.println("User has entered emergency contact: Dea Turashvili (phone number)");

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  dht.begin();

  checkSettings();

}

void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}

void loop() {

  time2=time1;
  //Serial.println(analogRead(A1));
  if (analogRead(A1)>threshold){
  time1=millis(); 
  bpm=time1-time2;
  bpm=60000/bpm;
  //if( bpm > 75 && bpm < 85){
  Serial.print("Heart Rate : ");
  Serial.println(bpm);

  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();

  if(normAccel.ZAxis > 3){
    Serial.println("Lying Down");
  }
  else if(normAccel.ZAxis < 3 && normAccel.ZAxis > -3){
    if (standFactor % 2 == 0){
      Serial.println("Sitting");
    }
    else {
      Serial.println("Standing");
    }

  }
  else if(normAccel.ZAxis < -3 && normAccel.ZAxis > -9){
    if (prevNormAccel < 3 && prevNormAccel > -3 ) { //check if sitting previously so it only looks at initial standing moment
      standFactor=standFactor + 1;
    }
    Serial.println("Standing");
  }

  else if(normAccel.ZAxis < -9){
    Serial.println("Fainted");
    Serial.println("Call Emergency Contact");
  }

  prevNormAccel = normAccel.ZAxis;

  hum = dht.readHumidity();
  temp= dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print(" Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  delay(200);
  }

  if (bpm > 120){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000); 

  if (temp > 30){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(500);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(500); 
  }

  if (hum > 65){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(250);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(250); 
  }
    
  }
  
 
  //Wait for a bit to keep serial data from saturating
  
}
