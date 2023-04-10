/* Robot bridge crossing code -- no encoders used */

#include <Adafruit_NeoPixel.h>
#include <MSE2202_Lib.h>

#define LEFT_MOTOR_A        35  // GPIO35 pin 28 (J35) Motor 1 A
#define LEFT_MOTOR_B        36  // GPIO36 pin 29 (J36) Motor 1 B
#define RIGHT_MOTOR_A       37  // GPIO37 pin 30 (J37) Motor 2 A
#define RIGHT_MOTOR_B       38  // GPIO38 pin 31 (J38) Motor 2 B

#define MODE_BUTTON         0   // GPIO0  pin 27 for Push Button 1
#define SMART_LED           21  // When DIP Switch S1-11 is on, Smart LED is connected to pin 23 GPIO21 (J21)
#define SMART_LED_COUNT     1   // Number of SMART LEDs in use

#define FRONT_SERVO          41 //  Servo 1 on front of car
#define FRONT_SERVO2         2 // Servo 2 on front of car
#define BACK_SERVO          39 // Servo 1 on back of car 
#define BACK_SERVO2          40 // Servo 2 on back of car


#define trigPin             10 // distance sensor pins
#define echoPin              13

/*                                 All LED Related items                                */
// Declare SK6812 SMART LED object
//   Argument 1 = Number of LEDs (pixels) in use
//   Argument 2 = ESP32 pin number 
//   Argument 3 = Pixel type flags, add together as needed:
//     NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//     NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//     NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//     NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//     NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel SmartLEDs(SMART_LED_COUNT, SMART_LED, NEO_RGB + NEO_KHZ800);

// Smart LED brightness for heartbeat
unsigned char LEDBrightnessIndex = 0; 
unsigned char LEDBrightnessLevels[] = {5,15,30,45,60,75,90,105,120,135,150,165,180,195,210,225,240,255,
                                       240,225,210,195,180,165,150,135,120,105,90,75,60,45,30,15};



unsigned int  ui_Mode_Indicator[7] = {                                        // Colours for different modes
  SmartLEDs.Color(255,0,0),                                                   //   Red - 0
  SmartLEDs.Color(0,255,0),                                                   //   Green - 1
  SmartLEDs.Color(0,0,255),                                                   //   Blue - 2
  SmartLEDs.Color(255,255,0),                                                 //   Yellow - 3
  SmartLEDs.Color(255,0,255),                                                 //   Magenta - 4
  SmartLEDs.Color(0,255,255),                                                 //   Cyan - 5
  SmartLEDs.Color(255,165,0)                                                  //   Orange - 6
};                   
// unsigned long ul_Display_Time;                                                // Heartbeat LED update timer
// const int ci_Display_Update = 100;                                            // Update interval for Smart LED in milliseconds

                        /*                           end of LED related items                 */

int mode = 0; // each mode will determine where our robot is in the process to cross the bridge


unsigned int currentTime = 0; //current time in micro seconds
unsigned int previousTime = 0; //previously recorded time in microseconds
unsigned int ui_Mode_PB_Debounce;                                             // Pushbutton debounce timer count


 

float duration, distance; // distance sensor variables

Motion Bot = Motion();                                                        // Instance of Motion for motor control


void setup() {
  SmartLEDs.begin();                                                         // Initialize smart LEDs object (REQUIRED)
  SmartLEDs.clear();                                                         // Clear pixel
  SmartLEDs.setPixelColor(0,SmartLEDs.Color(0,0,0));                         // Set pixel colors to 'off'
  SmartLEDs.show();     

Serial.begin(9600); //serial monitor initialized

pinMode(LEFT_MOTOR_A, OUTPUT); // iniialize all motors
pinMode(RIGHT_MOTOR_A, OUTPUT);
pinMode(LEFT_MOTOR_B, OUTPUT);
pinMode(RIGHT_MOTOR_B, OUTPUT);

pinMode(MODE_BUTTON, INPUT_PULLUP);                                        //set up mode button with internal pullup

pinMode(echoPin, INPUT);
pinMode(trigPin, OUTPUT);

  Bot.servoBegin("S1",FRONT_SERVO);  
  Bot.servoBegin("S2",FRONT_SERVO2);
  Bot.servoBegin("S3", BACK_SERVO );
  Bot.servoBegin("S4", BACK_SERVO2 );
  
Bot.ToPosition("S1", 690); // start front servos in upwards position
Bot.ToPosition("S2", 690);
Bot.ToPosition("S3", 690);  //start back servos in downwards position (one servo is flipped)
Bot.ToPosition("S4", 1800); 
}

void loop() {

  // the distance sensor needs to react very fast - trigger and echo pins are used outside of the if statement that ensures the loop does not run too fast
    digitalWrite(trigPin, LOW); // every loop, send signal from trigger pin to continuously sense distance
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) * 0.0343;


currentTime = micros();
if ((currentTime - previousTime) >= 1000){ // make sure the loop does not run too fast

    previousTime = currentTime;

    
    SmartLEDs.setPixelColor(0, ui_Mode_Indicator[mode]);         // Set pixel colors to = mode 
    SmartLEDs.show(); // update the LED on microcontroller

    

    switch (mode) 
    {
      case 0: // first mode = don't move - RED
      {
        digitalWrite(LEFT_MOTOR_A, LOW);
        digitalWrite(LEFT_MOTOR_B, LOW);
        digitalWrite(RIGHT_MOTOR_A, LOW);
        digitalWrite(RIGHT_MOTOR_B, LOW);

        delay(5000);
        mode = 1;
        break;
      }

      case 1: // second mode = drive until edge of table and stop - GREEN
      {
        Serial.println(distance);

        if (distance < 30) 
        {
            digitalWrite(LEFT_MOTOR_A, HIGH);
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, HIGH);
            digitalWrite(RIGHT_MOTOR_B, LOW);
        }

        if (distance > 30) 
        {
             digitalWrite(LEFT_MOTOR_A, LOW);
             digitalWrite(LEFT_MOTOR_B, LOW);
             digitalWrite(RIGHT_MOTOR_A, LOW);
             digitalWrite(RIGHT_MOTOR_B, LOW);
             delay(1000);
            mode = 2;
        }
        break;
      }

      case 2: // third mode = reverse, drop ladder, and start going across bridge - BLUE
      {
            digitalWrite(LEFT_MOTOR_A, LOW);
            digitalWrite(LEFT_MOTOR_B, HIGH);
            digitalWrite(RIGHT_MOTOR_A, LOW);
            digitalWrite(RIGHT_MOTOR_B, HIGH);

            delay(300); // reverse for three seconds 
             
        digitalWrite(LEFT_MOTOR_A, LOW);
        digitalWrite(LEFT_MOTOR_B, LOW);
        digitalWrite(RIGHT_MOTOR_A, LOW);
        digitalWrite(RIGHT_MOTOR_B, LOW);
        delay(1000); // stop for one second before dropping the ladder
         
            Bot.ToPosition("S1", 1800);  //drop ladder by turning servos to the down position
            Bot.ToPosition("S2", 1800);

            delay(1000); // wait one second before continuing onto the bridge
            // drive forward onto bridge so the distance sensor is off the side of the bridge
            digitalWrite(LEFT_MOTOR_A, HIGH); 
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, HIGH);
            digitalWrite(RIGHT_MOTOR_B, LOW);
            delay(2000);

            mode = 3;

        break;
      }

      case 3: // fourth mode = keep driving across bridge and stop after getting across  - YELLOW
      {
        if (distance > 30) //if we are still on the bridge
        {
          digitalWrite(LEFT_MOTOR_A, HIGH);
          digitalWrite(LEFT_MOTOR_B, LOW);
          digitalWrite(RIGHT_MOTOR_A, HIGH);
          digitalWrite(RIGHT_MOTOR_B, LOW);

        } 
        // if we are not on the bridge, go forward a little bit to make sure all of the car is off the bridge and stop
        else 
        {
          digitalWrite(LEFT_MOTOR_A, HIGH);
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, HIGH);
            digitalWrite(RIGHT_MOTOR_B, LOW);
          delay(1000);                           // drive forward for one second after detecting the edge to ensure all of car is off bridge 
            digitalWrite(LEFT_MOTOR_A, LOW);
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, LOW);
            digitalWrite(RIGHT_MOTOR_B, LOW);
            mode = 4; // go to pick up the ladder

        }
          break;
      }
     
     case 4: //fifth mode = pick up the ladder by turning the servo motor -- ladder picked up with magnets - MAGENTA
     {
            delay(1000); // make sure front servos are in the upward position
            Bot.ToPosition("S1", 690); 
            Bot.ToPosition("S2", 690);

            delay(1000); // wait one second before putting the back servos in the upward position

            Bot.ToPosition("S3", 1800);  
            Bot.ToPosition("S4", 690); 
            delay(1000);
       mode = 5;
        break;
     }


    case 5: //sixth mode = go to the end of the table, detect edge, drive backwards for 1.5 seconds  - LIGHT BLUE
    {
       if(distance < 30) // drive until end of table
       {
            digitalWrite(LEFT_MOTOR_A, HIGH);
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, HIGH);
            digitalWrite(RIGHT_MOTOR_B, LOW);
       }


       if (distance > 30)  // reverse if at the end of the table
        {
            digitalWrite(LEFT_MOTOR_A, LOW);
            digitalWrite(LEFT_MOTOR_B, HIGH);
            digitalWrite(RIGHT_MOTOR_A, LOW);
            digitalWrite(RIGHT_MOTOR_B, HIGH);
            delay(1500);
            mode = 6;
       }
    break;
    }

    case 6: // seventh mode = end by stopping - YELLOW
    {
            digitalWrite(LEFT_MOTOR_A, LOW);
            digitalWrite(LEFT_MOTOR_B, LOW);
            digitalWrite(RIGHT_MOTOR_A, LOW);
            digitalWrite(RIGHT_MOTOR_B, LOW);

    }
         
      }
    }
  }

