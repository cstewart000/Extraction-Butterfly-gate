
#include <Arduino.h>
#include <Wire.h>
#include <Button.h>
#include <Adafruit_PWMServoDriver.h>
#include <ezButton.h>




//HARDWARE PINOUT

//override switches
#define PIN_TOGGLE_1 2
#define PIN_TOGGLE_2 3
#define PIN_TOGGLE_3 4
#define PIN_TOGGLE_MODE 6 //NOT YET USED.

//gates
#define PIN_GATE_1 =6
#define PIN_GATE_2 =7
#define PIN_GATE_3 =8


//#define PIN_GATE_1

#define PIN_MODE_SWITCH 12

#define PIN_ALERT_LED 13


#define GATE_1 1
#define GATE_2 2
#define GATE_3 3

#define NUM_GATES 3


//SERVO CALIBRATION
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//OBJECT INSTANTIATION
Adafruit_PWMServoDriver servo_driver = Adafruit_PWMServoDriver();

ezButton override1button(2);
ezButton override2button(3);
ezButton override3button(4);

Button gate_1(6);
Button gate_2(7);
Button gate_3(8);
Button mode(12);


//FUNCTION declarations
void setDustExtractor(bool state);
void setGateConfig(int gate);
void controlGate(int gateNumber, bool state);
void printSystemState();
bool areAnyGatesOpen();
bool closeAllGates();
void checkOverideButtons();
void controlGatesByArray();
void checkGateSwitchState();


//State variables
bool extractorState = false;
int gate_config_state = 0;
bool gate_states_array[NUM_GATES];


void setup() {

    Serial.begin(9600);

    override1button.setDebounceTime(100); // set debounce time to 100 milliseconds
    override2button.setDebounceTime(100); // set debounce time to 100 milliseconds
    override3button.setDebounceTime(100); // set debounce time to 100 milliseconds

    override1button.setCountMode(COUNT_FALLING);
    override2button.setCountMode(COUNT_FALLING);
    override3button.setCountMode(COUNT_FALLING);

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("Servo ButterFly Diagnostic");

    servo_driver.begin();
    servo_driver.setOscillatorFrequency(27000000);
    servo_driver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    gate_1.begin();
    gate_2.begin();
    gate_3.begin();

    delay(10);

}

void loop() {

  printSystemState();
  override1button.loop();
  override2button.loop();
  override3button.loop();

  checkGateSwitchState();

  checkOverideButtons();

  controlGatesByArray();



  setDustExtractor(areAnyGatesOpen());
}

void printSystemState(){

  Serial.print("gate config: " );
  Serial.println(gate_config_state);

  Serial.print("gate 1 state: " );
  Serial.println(gate_1.read());

  Serial.print("gates gate_states_array: " );
  Serial.print(gate_states_array[0]);
  Serial.print(gate_states_array[1]);
  Serial.print(gate_states_array[2]);

  Serial.print("switch mode state: " );
  Serial.println(mode.read());

  Serial.print("Extractor state: " );
  Serial.println(extractorState);




  return;

}



void controlGate(int gateNumber, bool state){

  Serial.print("control Gate number: ");
  Serial.println(gateNumber);
  Serial.print("control Gate state: ");
  Serial.println(state);

  if(state){
    servo_driver.setPWM(gateNumber, 0, SERVOMIN);
  }else{
    servo_driver.setPWM(gateNumber, 0, SERVOMAX);
  }
  delay(500);
  //servo_driver.setPWM(gateNumber, 0, 0);

  gate_states_array[gateNumber-1]=state;
}

void closeAllGatesArray(){
  gate_states_array[0] = false;
  gate_states_array[1] = false;
  gate_states_array[2] = false;
}

void closeAllGates(){
  controlGate(GATE_1,false);
  controlGate(GATE_2,false);
  controlGate(GATE_3,false);

}

void controlGatesByArray(){
  controlGate(GATE_1, gate_states_array[0]);
  controlGate(GATE_2, gate_states_array[1]);
  controlGate(GATE_3, gate_states_array[2]);
}


void checkGateSwitchState(){

  int lastGateOpened =0;

  if(gate_1.isReleased() && lastGateOpened!= 1){
    gate_states_array[0] = true;
    lastGateOpened =1;

  }else{
    gate_states_array[0] = false;
  }


  if(gate_2.isReleased() && lastGateOpened!= 2){
    gate_states_array[1] = true;
    lastGateOpened =2;

  }else{
    gate_states_array[1] = false;
  }

  if(gate_3.isReleased() && lastGateOpened!= 3){
    gate_states_array[2] = true;
    lastGateOpened =3;

  }else{
    gate_states_array[2] = false;
  }

}

void checkOverideButtons(){
  if (override1button.getCount()){
    bool swithTo = !gate_states_array[0]
    closeAllGatesArray();
    gate_states_array[0] = swithTo;

    override1button.resetCount();
  }
  if( override2button.getCount()){
    bool swithTo = !gate_states_array[1]
    closeAllGatesArray();
    gate_states_array[1] = swithTo;

    override2button.resetCount();

  }
  if( override3button.getCount()){
    bool swithTo = !gate_states_array[2]
    closeAllGatesArray();
    gate_states_array[2] = swithTo;
    override3button.resetCount();
  }
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
}
