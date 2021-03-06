#include <Adafruit_PWMServoDriver.h>

#include <PS4BT.h>
#include <usbhub.h>

//In case of redefinition:
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include <Servo.h>
#include <Wire.h>

//Usb bus with bluetooth hosting
USB Usb;
BTD Btd(&Usb);

//PS4-bluetooth contoller boject set to pair 
//PS4BT PS4(&Btd, PAIR);

//One paired, can use this instead
PS4BT PS4(&Btd);

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

//Servo list
//Front Left leg
static unsigned int a = 8; //Upper
static unsigned int b = 9; //Middle
static unsigned int c = 10; //Lower
//Front right leg
static unsigned int d = 12; //Upper
static unsigned int e = 14; //middle
static unsigned int f = 13; //lower
//Back left leg
static unsigned int g = 0; //upper
static unsigned int h = 2; //middle
static unsigned int j = 1; //lower
//Back right leg
static unsigned int k = 4; //upper
static unsigned int l = 5; //middle
static unsigned int m = 6; //lower

//Adafruit servo shield addressing object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(&Wire, 0x40);

//Max servo settings
#define SERVOMIN  147
#define SERVOMAX  589

void setup() {
  //Open bluetooth to the ps4 controller
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1);
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));

  //Start communication with servo shield
  pwm.begin();

  //Set pmw frequency for I2C communication
  pwm.setPWMFreq(60);
}

void zero_position() {
  /** 
   *  For reseting to the inital position, where the robot's center of mass in centered and stabilized
   */
  int alpha = 90;
  int beta = 90;
  int gamma = 90;
   
   //Maps pulse degrees to pulse length
  int apulse = map(alpha, 0, 180, SERVOMIN, SERVOMAX);
  int bpulse = map(beta, 0, 180, SERVOMIN, SERVOMAX);
  int gpulse = map(gamma, 0, 180, SERVOMIN, SERVOMAX);

  //The actuation
  //pwm.setPWM(a, 0, gpulse);
  //pwm.setPWM(b, 0, apulse);
  //pwm.setPWM(c, 0, bpulse);
  //pwm.setPWM(d, 0, gpulse);
  //pwm.setPWM(e, 0, apulse);
  //pwm.setPWM(f, 0, bpulse);
  //pwm.setPWM(g, 0, gpulse);
  //pwm.setPWM(h, 0, apulse);
  //pwm.setPWM(j, 0, bpulse);
  //pwm.setPWM(k, 0, gpulse);
  //pwm.setPWM(l, 0, apulse);
  //pwm.setPWM(m, 0, bpulse);

  inverse_kinematic(d, e, f, 80, 0, 77);
   
}

void set_leg_position(int servo, int x, int y, int z) {
  /**
   * Moving the position of a leg in a cartesian pose
   */
  
}

void inverse_kinematic(int hip, int knee, int ankle, int x, int y, int z) {
  /**
   * Converts cartesian coordinates to servo angles
   */
  //Local vars
  int femur = 55; //mm
  int coxa = 29; //mm
  int tibia = 77; //mm
  int zoffset = 77; //mm
  int l1 = sqrt(x*x + y*y);
  float len = sqrt(zoffset*zoffset + (l1 - coxa)*(l1-coxa));

  //Inverse kinematic functions
  float alpha = acos(zoffset/len) + acos((tibia*tibia - femur*femur - len*len)/(-2*femur*len))*360/(2*PI);
  float beta = acos((len*len - tibia*tibia - femur*femur)/(-2*tibia*femur))*360/(2*PI);
  float gamma = acos(x/y)*360/(2*PI);

  //Maps pulse degrees to pulse length
  int apulse = map(alpha, 0, 180, SERVOMIN, SERVOMAX);
  int bpulse = map(beta, 0, 180, SERVOMIN, SERVOMAX);
  int gpulse = map(gamma, 0, 180, SERVOMIN, SERVOMAX);

  //The actuation
  pwm.setPWM(hip, 0, gpulse);
  pwm.setPWM(knee, 0, apulse);
  pwm.setPWM(ankle, 0, bpulse);
}

void loop() {
  zero_position();
  
  Usb.Task();

  if (PS4.connected()) {
    if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117 || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117 || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117) {
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS4.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS4.getAnalogHat(LeftHatY));
      Serial.print(F("\tRightHatX: "));
      Serial.print(PS4.getAnalogHat(RightHatX));
      Serial.print(F("\tRightHatY: "));
      Serial.print(PS4.getAnalogHat(RightHatY));
    }

    if (PS4.getAnalogButton(L2) || PS4.getAnalogButton(R2)) { // These are the only analog buttons on the PS4 controller
      Serial.print(F("\r\nL2: "));
      Serial.print(PS4.getAnalogButton(L2));
      Serial.print(F("\tR2: "));
      Serial.print(PS4.getAnalogButton(R2));
    }
    if (PS4.getAnalogButton(L2) != oldL2Value || PS4.getAnalogButton(R2) != oldR2Value) // Only write value if it's different
      PS4.setRumbleOn(PS4.getAnalogButton(L2), PS4.getAnalogButton(R2));
    oldL2Value = PS4.getAnalogButton(L2);
    oldR2Value = PS4.getAnalogButton(R2);

    if (PS4.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS4.disconnect();
    }
    else {
      if (PS4.getButtonClick(TRIANGLE)) {
        Serial.print(F("\r\nTraingle"));
        PS4.setRumbleOn(RumbleLow);
      }
      if (PS4.getButtonClick(CIRCLE)) {
        Serial.print(F("\r\nCircle"));
        PS4.setRumbleOn(RumbleHigh);
      }
      if (PS4.getButtonClick(CROSS)) {
        Serial.print(F("\r\nCross"));
        PS4.setLedFlash(10, 10); // Set it to blink rapidly
      }
      if (PS4.getButtonClick(SQUARE)) {
        Serial.print(F("\r\nSquare"));
        PS4.setLedFlash(0, 0); // Turn off blinking
      }

      if (PS4.getButtonClick(UP)) {
        Serial.print(F("\r\nUp"));
        PS4.setLed(Red);
      } if (PS4.getButtonClick(RIGHT)) {
        Serial.print(F("\r\nRight"));
        PS4.setLed(Blue);
      } if (PS4.getButtonClick(DOWN)) {
        Serial.print(F("\r\nDown"));
        PS4.setLed(Yellow);
      } if (PS4.getButtonClick(LEFT)) {
        Serial.print(F("\r\nLeft"));
        PS4.setLed(Green);
      }

      if (PS4.getButtonClick(L1))
        Serial.print(F("\r\nL1"));
      if (PS4.getButtonClick(L3))
        Serial.print(F("\r\nL3"));
      if (PS4.getButtonClick(R1))
        Serial.print(F("\r\nR1"));
      if (PS4.getButtonClick(R3))
        Serial.print(F("\r\nR3"));

      if (PS4.getButtonClick(SHARE))
        Serial.print(F("\r\nShare"));
      if (PS4.getButtonClick(OPTIONS)) {
        Serial.print(F("\r\nOptions"));
        printAngle = !printAngle;
      }
      if (PS4.getButtonClick(TOUCHPAD)) {
        Serial.print(F("\r\nTouchpad"));
        printTouch = !printTouch;
      }

      if (printAngle) { // Print angle calculated using the accelerometer only
        Serial.print(F("\r\nPitch: "));
        Serial.print(PS4.getAngle(Pitch));
        Serial.print(F("\tRoll: "));
        Serial.print(PS4.getAngle(Roll));
      }

      if (printTouch) { // Print the x, y coordinates of the touchpad
        if (PS4.isTouching(0) || PS4.isTouching(1)) // Print newline and carriage return if any of the fingers are touching the touchpad
          Serial.print(F("\r\n"));
        for (uint8_t i = 0; i < 2; i++) { // The touchpad track two fingers
          if (PS4.isTouching(i)) { // Print the position of the finger if it is touching the touchpad
            Serial.print(F("X")); Serial.print(i + 1); Serial.print(F(": "));
            Serial.print(PS4.getX(i));
            Serial.print(F("\tY")); Serial.print(i + 1); Serial.print(F(": "));
            Serial.print(PS4.getY(i));
            Serial.print(F("\t"));
          }
        }
      }
    }
  }
}
