#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#define relay 4
#define red 16
#define green 15
Servo myservo;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial sim800l(3, 2); // Gsm Tx-> Arduino Pin 2  GSM Rx -> Arduino Pin 3
int irsensor = A0;
int otp;
String otpstring = "";
int i = 0;
void setup()
{
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object
  pinMode(irsensor, INPUT_PULLUP);
  sim800l.begin(4800);   //Module baude rate, this is on max, it depends on the version
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  myservo.write(0);
  Serial.print("Welcome to SIM800L Project");
  sim800l.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  pinMode(relay, OUTPUT);
  pinMode(red, INPUT);
  pinMode(green, INPUT);
  digitalWrite(relay, LOW);
  delay(500);
  sim800l.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  delay(1000);

}
void updateSerial()
{
  delay(500);
  while (Serial.available()) {
    sim800l.write(Serial.read());
  }//Forward what Serial received to Software Serial Port
  while (sim800l.available()) {
    Serial.write(sim800l.read());
  }//Forward what Software Serial received to Serial Port
}
void loop()
{
  myservo.write(0);
  lcd.setCursor(0, 0);
  lcd.print("   OTP Based");
  lcd.setCursor(0, 1);
  lcd.print("   Door Lock");
  if (digitalRead(irsensor) == LOW)
  {
    otp = random(2000, 9999);
    otpstring = String(otp);
    Serial.println(otpstring);
    while (digitalRead(irsensor) == LOW) {}
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" OTP Sent to");
    lcd.setCursor(0, 1);
    lcd.print(" Your Mobile");
    Serial.print("OTP is ");
    delay(100);
    Serial.println(otpstring);
    delay(100);
    SendSMS();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter OTP :");
    getotp();
  }

}

void getotp() {

  String y = "";
  int a = y.length();
  while (a < 4)
  {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.setCursor(0, 1);
      y = y + customKey;
      lcd.print(y);
      a = y.length();
    }
  }
  Serial.print("Entered OTP is ");
  Serial.println(y);
  if (otpstring == y)
  {
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door Opening");
    //myservo.write(45);
    //delay(5000);
    //myservo.write(0);
    digitalWrite(relay, HIGH);
     digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    delay(5000);
    digitalWrite(relay, LOW);
     digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Access Failed");
    lcd.setCursor(0, 1);
    lcd.print("Try Again !!!");
    delay(3000);
  }


}
void SendSMS()
{
  Serial.println("Sending SMS...");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(100);
  sim800l.print ("AT+CSMP=17,167,0,0\r");
  delay(500);
  sim800l.print("AT+CMGS=\"+919975617490\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  delay(500);
  sim800l.print("Your OTP is " + otpstring + " Just Type OTP And Unlock The Door");   //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  delay(500);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);

}
