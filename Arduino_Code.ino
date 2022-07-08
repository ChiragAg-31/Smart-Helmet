


#include<Servo.h>
Servo myservo;
int pos = 0;
int val;
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10);
byte tx = 1;

#include <TinyGPS.h>
TinyGPS gps;  //Creates a new instance of the TinyGPS object

const int Buzzer = 6;
const int DC_Motor = 7;

int gasC_1 = 0; //set initial tempC 0 for all MQ 3
int smkC_1 = 0; //set initial tempC 0 for all MQ 2

const int SensorPin1 = A0; //danger input sensor pin
const int SensorPin2 = A1;
const int SensorPin3 = 8;

String textForSMS;
void setup()
{
  lcd.begin(16, 2);
  delay(100);
  pinMode(tx, OUTPUT);
  myservo.attach(13);
  pinMode(Buzzer, OUTPUT);
  pinMode(SensorPin1, INPUT);
  pinMode(SensorPin2, INPUT);
  pinMode(SensorPin3, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(DC_Motor, OUTPUT);

  mySerial.begin(9600);
  Serial.begin(9600); //Start the serial connection with the computer
}
void loop()
{
  int gasC_1 = analogRead(SensorPin1);
  int SmkC_1 = analogRead(SensorPin2);
  int vib_1  = digitalRead(SensorPin3);
  gasC_1 = analogRead(SensorPin1); //read the value from the LM35 sensor
  gasC_1 = (5.0 * gasC_1 * 100.0) / 1024.0; //convert the analog data to temperature
  smkC_1 = analogRead(SensorPin2); //read the value from the MQ 2 sensor
  smkC_1 = (5.0 * smkC_1 * 100.0) / 1024.0; //convert the analog data to temperature
  vib_1  = digitalRead(SensorPin3);
  delay(50);

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }

  

  if (gasC_1 >= 100 || smkC_1 >= 100 || vib_1==1)
  {
    val = analogRead(pos);
    val = map(val, 0, 1023, 0, 180);
    myservo.write(val);
    delay(50);
    digitalWrite(DC_Motor, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  THERE IS DANGER ");
    lcd.setCursor(0, 1);
    lcd.print(" NOT SAFE HERE ");
    delay(100);
    lcd.clear();
    lcd.print("Sending SMS...");
    delay(100);

    digitalWrite(Buzzer, HIGH);
    delay(200);
    digitalWrite(Buzzer, LOW);
    delay(200);
    digitalWrite(Buzzer, HIGH);
    delay(200);
    digitalWrite(Buzzer, LOW);
    delay(5);

    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    
    Serial.print("AT+CMGF=1\r");
    delay(100);
    Serial.print("AT+CMGS=\"+918269626889\"\r");
    Serial.print("DANGER ALERT! Please Be Informed that ACCIDENT has Occured!\r");
    Serial.print("Location Co-ordinates :-\r");
    
    Serial.print("Latitude = ");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" Longitude = ");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);

    delay(200);
    Serial.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    Serial.println();
  }
  else
  {
    digitalWrite(DC_Motor, LOW);
    myservo.write(95);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  NO DANGER    ");
    lcd.setCursor(0, 1);
    lcd.print("   ALL SAFE   ");
  }
}
