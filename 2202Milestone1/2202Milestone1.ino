#include <Adafruit_NeoPixel.h>
#include <MSE2202_Lib.h>

#define CLAW_SERVO          4


const int ci_Claw_Servo_Open = 690;                                          // Value for open position of claw
const int ci_Claw_Servo_Closed = 1880;
int AA = 35;
int  AB = 36;
int BA = 37;
int  BB = 38;
int distance;

unsigned long ul_Current_Micros;
unsigned long ul_Previous_Micros;

const int limit = 10;

int mode = 0;


void Indicator();              
Motion Bot = Motion();                                                        // Instance of Motion for motor control
Encoders driveEncoders = Encoders();                                          // Instance of Encoders for encoder data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
   //myServo.attach(8);
   pinMode(AA, OUTPUT);
  pinMode(AB, OUTPUT);
  pinMode(BB, OUTPUT);
  pinMode(BA, OUTPUT);
  Bot.servoBegin("S1",CLAW_SERVO);
  pinMode(limit, INPUT_PULLUP);
Bot.ToPosition("S1", 690);
}

void loop() {
  // put your main code here, to run repeatedly:
ul_Current_Micros = micros();                                              // Get current time in microseconds
   if ((ul_Current_Micros - ul_Previous_Micros) >= 1000)                      // Enter when 1 ms has elapsed
   {
      ul_Previous_Micros = ul_Current_Micros;        

 
    digitalWrite(AA,HIGH);
    digitalWrite(AB,LOW);
    digitalWrite(BB,HIGH);
    digitalWrite(BA,LOW);

    if (digitalRead(limit) == HIGH) {

    digitalWrite(AA,LOW);
    digitalWrite(AB,HIGH);
    digitalWrite(BB,LOW);
    digitalWrite(BA,HIGH);
    delay(500);

    digitalWrite(AA,LOW);
    digitalWrite(AB,LOW);
    digitalWrite(BB,LOW);
    digitalWrite(BA,LOW);
    delay(1000);

    Bot.ToPosition("S1", 1880);

    delay(4000);

    digitalWrite(AA,HIGH);
    digitalWrite(AB,LOW);
    digitalWrite(BB,HIGH);
    digitalWrite(BA,LOW);
    delay(3000);

    digitalWrite(AA,LOW);
    digitalWrite(AB,LOW);
    digitalWrite(BB,LOW);
    digitalWrite(BA,LOW);
    delay(10000);
     }
   }
}