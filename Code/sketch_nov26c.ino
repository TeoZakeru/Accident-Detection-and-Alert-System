#include<SoftwareSerial.h>
#include<AltSoftSerial.h>
#include<TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
SoftwareSerial mySerial(6, 7); // SIM900 Tx & Rx is connected to Arduino #7 & #8
String smsContent = "",sender = "";
int sensor = 0,sensor1 = 1,sensor2 = 2,sensor3 = 3;
double alt = 9;
AltSoftSerial ss(10,8);
TinyGPSPlus gps;
double LAT = 0,LON = 0;
void stop(){
  Serial.println("STOP!");
  while(1<2){

  }
}

  sensors_event_t event; 
  
void setup()
{
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
  mySerial.begin(9600);
  ss.begin(9600);
  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT"); // Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1");           // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");   // Decides how newly arrived SMS messages should be handled
  updateSerial();
  accel.begin();
  
}

void loop(){
  updateSerial();
  int fsr1 = analogRead(sensor),fsr2 = analogRead(sensor1),fsr3 = analogRead(sensor2),fsr4 = analogRead(sensor3);
  accel.getEvent(&event);
  alt = event.acceleration.z;
  Serial.print("Accelerometer reading of the z-component ");
  Serial.println(alt);
  Serial.println("FSR Readings:");
  Serial.print("1 ");
  Serial.println(fsr1);
  Serial.print("2 ");
  Serial.println(fsr2);
  Serial.print("3 ");
  Serial.println(fsr3);
  Serial.print("4 ");
  Serial.println(fsr4);
  while (ss.available() > 0) {
    char gpsChar = ss.read();
    if (gps.encode(gpsChar)) {
      if (gps.location.isUpdated()) {
        LAT = gps.location.lat();
        LON = gps.location.lng();
        Serial.print("Latitude= ");
        Serial.print(LAT, 10);
        Serial.print(" Longitude= ");
        Serial.println(LON, 10); //text content
        updateSerial();
      }
    }
  }
  if(fsr1>500||fsr2>130||fsr3>800||fsr4>200||alt<6.5){
    int i = 0;
    mySerial.println("ATD+ Insert Phone Number of Driver here;");
    updateSerial();
    delay(10000);
    Serial.println("Send Message");
    updateSerial();
    while(i<10){
  if (mySerial.find("+CMT:")){
        // Read the sender's phone number
        sender = mySerial.readStringUntil(',');
        String res = mySerial.readStringUntil('\n');
        smsContent = mySerial.readStringUntil('\n');
        // Print the received SMS information
        Serial.println("Received SMS:");
        Serial.println("Content: " + smsContent);
        Serial.println(1234);
        // senser = sender
        Serial.println(sender);
        break;
      }
      i++;
    }
    if(smsContent.length()==0||sender!=" \"Insert Phone Number of Driver here\""){
      Serial.println(smsContent);
      mySerial.println("AT+CMGS=\"Insert Phone Number of Emergency Contact here\"");
      updateSerial();
      mySerial.print("LAT : ");
      mySerial.print(LAT,10);
      mySerial.print(" LON : ");
      mySerial.println(LON,10);
      mySerial.write(26);
      updateSerial();
      stop();
    }
    smsContent = "";
    sender = "";
  }
}

void updateSerial()
{
  delay(500);
  while (Serial.available()){
    mySerial.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()){
    Serial.write(mySerial.read()); // Forward what Software Serial received to Serial Port
  }
}