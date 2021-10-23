
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Switch.h>



//HARDWARE PINOUT

//gates

#define PIN_OVERRIDE_blast_gate_1_button 2
#define PIN_OVERRIDE_blast_gate_2_button 3
#define PIN_OVERRIDE_blast_gate_3_button 4

#define PIN_BLAST_blast_gate_1_button 6
#define PIN_BLAST_blast_gate_2_button 7
#define PIN_BLAST_blast_gate_3_button 8



//#define

#define PIN_MODE_SWITCH 12
#define PIN_ALERT_LED 13
#define PIN_PCA9865_ENABLE 10

#define BUTTERFLY_GATE_1 1
#define BUTTERFLY_GATE_2 2
#define BUTTERFLY_GATE_3 3

#define NUM_GATES 3


//SERVO CALIBRATION - See adafruit website
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//OBJECT INSTANTIATION
Adafruit_PWMServoDriver servo_driver = Adafruit_PWMServoDriver();

Switch override_gate_1_button(PIN_OVERRIDE_blast_gate_1_button);
Switch override_gate_2_button(PIN_OVERRIDE_blast_gate_2_button);
Switch override_gate_3_button(PIN_OVERRIDE_blast_gate_3_button);

Switch blast_gate_1_button(PIN_BLAST_blast_gate_1_button);
Switch blast_gate_2_button(PIN_BLAST_blast_gate_2_button);
Switch blast_gate_3_button(PIN_BLAST_blast_gate_3_button);

//FUNCTION declarations
void setDustExtractor(bool state);
void setGateConfig(int gate);
void controlGate(int gateNumber, bool state);
void printSystemState();
bool areAnyGatesOpen();
void closeAllGates();
void checkOverideButtons();
void controlGatesByArray();
void checkGateSwitchState();

//State variables
bool extractorState = false;
int gate_config_state = 0;
bool gate_states_array[NUM_GATES];
bool previous_gate_states_array[NUM_GATES];
bool button_states_array[NUM_GATES];
bool previous_button_states_array[NUM_GATES];

void setup() {

    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_PCA9865_ENABLE, OUTPUT);
    digitalWrite(PIN_PCA9865_ENABLE, LOW);

    Serial.println("Servo ButterFly Diagnostic");

    servo_driver.begin();
    servo_driver.setOscillatorFrequency(27000000);
    servo_driver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    delay(10);

}

void loop() {

  printSystemState();
  checkGateSwitchState();
  controlGatesByArray();
}

void printSystemState(){

  Serial.print("gates gate_states_array: " );
  Serial.print(gate_states_array[0]);
  Serial.print(gate_states_array[1]);
  Serial.println(gate_states_array[2]);

  Serial.print("buttons previous_button_states_array: " );
  Serial.print(previous_button_states_array[0]);
  Serial.print(previous_button_states_array[1]);
  Serial.println(previous_button_states_array[2]);

  Serial.print("buttons button_states_array: " );
  Serial.print(button_states_array[0]);
  Serial.print(button_states_array[1]);
  Serial.println(button_states_array[2]);

  Serial.print("Extractor state: " );
  Serial.println(extractorState);

}

void controlGate(int gateNumber, bool state){

  int servo_number = gateNumber-1;

  if(previous_gate_states_array[servo_number]!=gate_states_array[servo_number]){


    digitalWrite(PIN_PCA9865_ENABLE, HIGH);
    delay(100);

    if(state){
      servo_driver.setPWM(servo_number, 0, SERVOMIN);
    }else{
      servo_driver.setPWM(servo_number, 0, SERVOMAX);
    }
    delay(100);

    //servo_driver.setPWM(gateNumber-1, 0, 4096);
    //servo_driver.setPWM(gateNumber, 0, 0);
    digitalWrite(PIN_PCA9865_ENABLE, LOW);

    previous_gate_states_array[servo_number]=state;

  }
  digitalWrite(PIN_PCA9865_ENABLE, LOW); // attempt to disable the servo driver to stop humming.

}

void closeAllGatesArray(){
  gate_states_array[0] = false;
  gate_states_array[1] = false;
  gate_states_array[2] = false;
}

void closeAllGates(){
  controlGate(BUTTERFLY_GATE_1,false);
  controlGate(BUTTERFLY_GATE_2,false);
  controlGate(BUTTERFLY_GATE_3,false);

}

void controlGatesByArray(){
  controlGate(BUTTERFLY_GATE_1, gate_states_array[0]);
  controlGate(BUTTERFLY_GATE_2, gate_states_array[1]);
  controlGate(BUTTERFLY_GATE_3, gate_states_array[2]);
}

void checkGateSwitchState(){

  override_gate_1_button.poll();
  override_gate_2_button.poll();
  override_gate_3_button.poll();

  blast_gate_1_button.poll();
  blast_gate_2_button.poll();
  blast_gate_3_button.poll();

  button_states_array[0] =blast_gate_1_button.on() || override_gate_1_button.on();
  button_states_array[1] =blast_gate_2_button.on() || override_gate_2_button.on();
  button_states_array[2] =blast_gate_3_button.on() || override_gate_3_button.on();

  if(!previous_button_states_array[0] && button_states_array[0]){
      gate_states_array[0] = true;
      gate_states_array[1] = false;
      gate_states_array[2] = false;
  }
  if(!previous_button_states_array[1] && button_states_array[1]){
    gate_states_array[0] = false;
    gate_states_array[1] = true;
    gate_states_array[2] = false;
  }
  if(!previous_button_states_array[2] && button_states_array[2]){
    gate_states_array[0] = false;
    gate_states_array[1] = false;
    gate_states_array[2] = true;
  }

  previous_button_states_array[0] = button_states_array[0];
  previous_button_states_array[1] = button_states_array[1];
  previous_button_states_array[2] = button_states_array[2];
}

void setDustExtractor(bool state){


  if(state!= extractorState){

    Serial.print("Extractor toggled: " );
    Serial.println(state);
  }


  extractorState = state;
  digitalWrite(LED_BUILTIN, state);

}

bool areAnyGatesOpen(){
  for(int i=0; i<NUM_GATES;i++){
    if(gate_states_array[i]){
      return true;
    }else{

      return false;
    }
  }
  return false;
}
