//----------------------------------------------------------------
#include <Arduino.h>

static int PIN_A PB0; // Encoder PIN A
static int PIN_B PB1; // Encoder PIN B
static int BUTTON PA7; // Button PIN

volatile bool aFlag = 0;
volatile bool bFlag = 0;
volatile uint16_t reading = 0;
volatile int16_t count = 0; // +1 from CW and -1 from CCW (for sample)
volatile bool button = 0; // Button pressed flag (for event out of interrupt)
volatile bool cw_flag = 0; // Clockwise flag (for event out of interrupt)
volatile bool ccw_flag = 0; // Counterclockwise flag (for event out of interrupt)

void CW(){
  noInterrupts();
  reading = GPIOB->IDR & 0x3;
  if(reading == 0b0000000000000011 && aFlag)
  {
    bFlag = 0;
    aFlag = 0;
    cw_flag = 1; // Set the clockwise rotation flag
    count ++; // Counter +1
  }
  else if (reading == 0b0000000000000001) bFlag = 1;
  interrupts();
}

void CCW(){
  noInterrupts();
  reading = GPIOB->IDR & 0x3;
  if (reading == 0b0000000000000011 && bFlag)
  {
    bFlag = 0;
    aFlag = 0;
    ccw_flag = 1; // Set the Counterclockwise rotation flag
    count --; // Counter -1
  }
  else if (reading == 0b0000000000000010) aFlag = 1;
  interrupts();
}

void BUT(){
  noInterrupts();
  button = 1; // Set button pressed flag
  interrupts();
}

void setup() {
  Serial1.begin(115200);
  Serial.println("");
  Serial.println("Please turn the encoder!");
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_A), CW, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_B), CCW, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON), BUT, RISING);
}

void loop() {
  if (cw_flag==1) // out of interrupt handling CW
  {
    Serial.println("-->");
    Serial.println(count);
    cw_flag = 0;
  }
  if (ccw_flag == 1) // out of interrupt handling CCW
  {
    Serial.println("<--");
    Serial.println(count);
    ccw_flag = 0;
  }
  if (button == 1) // out of interrupt handling button
  {
    Serial.println("PUSHED");
    button = 0;
  }
}
