#include <math.h>
#include <Stepper.h>

/* general config */
const int serialBaud = 9600;
const int dt = 100;
const int photoDelayT = 500;
byte joyEnabled = false;
volatile byte toggledJoy = false;
volatile unsigned long joyLastSet = 0;
const unsigned long debounceDelay = 2500;

/* config for stepper motors */
const int stepsPerRevolution = 2048;
const int rpm = 10; // rpm
int stepsPerInterval; // (steps per rev * rpm * read interval) / (60s * 1000 ms/s)
float mmPerStep;
float moveLengthMm = 10.0;

Stepper myXStepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper myYStepper(stepsPerRevolution, 7, 5, 6, 4);

/* config for joystick */
const int joyXPin = A0;
const int joyYPin = A1;
const int joySwitchPin = 2;
int joyXVal, joyYVal;
int joyNeutral = 512, joyMax = 1023, joyMin = 0, joyTolerence = 10;

/**
 * Code for the IR signal adapted from: https://github.com/MajicDesigns/MD_multiCameraIrControl/tree/main
 * (it was further adapted from other sources)
 * Another good document: https://adapting-camera.blogspot.com/2018/08/how-to-make-your-own-olympus-rm-1.html
 *
 * I simplified it for my specific use case.
 */

/* config for IR remote for camera */
#define ARRAY_SIZE(a)  (sizeof(a)/sizeof((a)[0]))
const uint8_t BITFIELD_SIZE = 32;
const int LED_PIN = 12;
const int FREQ = 40; // khz - the IR signal

/* Used to create the IR pulse */
void wait(uint32_t time) {
  uint32_t timeStart = micros();
  do { /* nothing */ } while (micros() - timeStart < time);
}

void high(uint32_t time) {
  uint32_t pause = (1000 / FREQ / 2) - 4;
  uint32_t start = micros();

  while (micros() - start <= time)
  {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(pause);
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(pause);
  }
}

inline void signal(uint32_t timeH, uint32_t timeW) {
  high(timeH);
  delayMicroseconds(timeW);
}

void fire() {
  uint32_t seq[] = { 0b01100001110111001000000001111111 };
  uint8_t seqValid[] = { 32 };    // number of valid bits from MSB for each seq[i]

  signal(8972, 4384);
  high(600);

  for (uint8_t j = 0; j < ARRAY_SIZE(seq); j++)   // for each 32 bit field
  {
    for (uint8_t i = 0; i < seqValid[j]; i++)     // for each bit in the field
    {
      wait(seq[j] & (1L << (BITFIELD_SIZE - i - 1)) ? 1600 : 488);
      high(580);
    }
  }
}

/**
 * My application. The goal is to use a stepper motor to drive a platform in a linear fashion,
 * and with two along a plane, to snap photos in a grid.
 */

void setupGeneral() {
 Serial.begin(serialBaud);
}

void setupJoystick() {
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  pinMode(joySwitchPin, INPUT_PULLUP);
  digitalWrite(joySwitchPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(joySwitchPin), joyStick_ISR, FALLING);
}

void setupStepperMotors() {
  myXStepper.setSpeed(rpm);
  myYStepper.setSpeed(rpm);
  float spic = (float) stepsPerRevolution * rpm * dt / 60000.0;
  stepsPerInterval = round(spic);
  mmPerStep = 25.4 / 2048;
  Serial.println(spic);
  Serial.println(stepsPerInterval);
  Serial.println(mmPerStep);
}

void setupIR() {
    pinMode(LED_PIN, OUTPUT);
}

void setup() {
  setupGeneral();
  setupJoystick();
  setupIR();
  setupStepperMotors();
}

void captureJoystickState() {
  joyXVal = analogRead(joyXPin);
  joyYVal = analogRead(joyYPin);
}

void captureFrame(int frame) {
  Serial.print("Shot ");
  Serial.println(frame);
  fire();
  delay(photoDelayT);
}

void scanNegative() {
  Serial.println("scanning");
  int stepsToMove = (int) round(moveLengthMm / mmPerStep);

  captureFrame(1);
  if (joyEnabled) return;
  myXStepper.step(stepsToMove);

  captureFrame(2);
  if (joyEnabled) return;
  myXStepper.step(stepsToMove);

  captureFrame(3);
  if (joyEnabled) return;
  myXStepper.step(stepsToMove);

  captureFrame(4);
  if (joyEnabled) return;
  myYStepper.step(stepsToMove);

  captureFrame(5);
  if (joyEnabled) return;
  myXStepper.step(-stepsToMove);

  captureFrame(6);
  if (joyEnabled) return;
  myXStepper.step(-stepsToMove);

  captureFrame(7);
  if (joyEnabled) return;
  myXStepper.step(-stepsToMove);

  captureFrame(8);
  if (joyEnabled) return;
  myYStepper.step(-stepsToMove);

  Serial.println("done scanning");
}

void moveXStepper() {
  int displaceX = joyMax - joyXVal - joyNeutral;
  if (abs(displaceX) > joyTolerence) {
    int toMove = (displaceX > 0 ? stepsPerInterval : -stepsPerInterval);
    Serial.print("Moving X ");
    Serial.println(displaceX);
    myXStepper.step(toMove);
  }
}

void moveYStepper() {
  int displaceY = joyMax - joyYVal - joyNeutral;
  if (abs(displaceY) > joyTolerence) {
    int toMove = (displaceY > 0 ? stepsPerInterval : -stepsPerInterval);
    Serial.print("Moving Y ");
    Serial.println(displaceY);
    myYStepper.step(toMove);
  }
}

void joyStick_ISR() {
  unsigned long ct = micros();
  if (ct - joyLastSet > debounceDelay) {
    if (digitalRead(joySwitchPin) == HIGH) {
      toggledJoy = true;
      joyLastSet = ct;
    }
  }
}

inline void handleJoystickToggled() {
  if (toggledJoy) {
    joyEnabled = !joyEnabled;
    toggledJoy = false;
    Serial.print("toggled joystick - ");
    Serial.println((joyEnabled == true) ? "enabled" : "disabled");
  }
}

void loop() {
  while (!(Serial.available() || joyEnabled || toggledJoy)) { }
  handleJoystickToggled();
  if (joyEnabled) {
    captureJoystickState();
    moveXStepper();
    moveYStepper();
  } else {
    if (Serial.readString().equals("go")) {
      scanNegative();
    }
  }
}
