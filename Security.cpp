#include "Energia.h"
#include "Security.h"

Security::Security(int alarmPin, int clearPin, int greenPin, int yellowPin,
                   int redPin, unsigned long pauseTime)
{
  greenPin_ =  greenPin;
  yellowPin_ = yellowPin;
  redPin_ =    redPin;
  alarmPin_ =  alarmPin;
  clearPin_ =  clearPin;
  pauseTime_ = pauseTime;

  // Initialize alarm inputs.
  pinMode(alarmPin_, INPUT_PULLUP); 
  pinMode(clearPin_, INPUT_PULLUP);

  // Initialize alarm outputs.
  pinMode     (greenPin_,  OUTPUT);       
  digitalWrite(greenPin_,  LOW);
  pinMode     (yellowPin_, OUTPUT);
  digitalWrite(yellowPin_, LOW);
  pinMode     (redPin_,    OUTPUT);
  digitalWrite(redPin_,    LOW);

  // Define the Finite State Machine table.
  //       state            input      next State   
  fsmTable[GREEN].nextState[NONE]    = GREEN;       
  fsmTable[GREEN].nextState[ALARM]   = YELLOW;
  fsmTable[GREEN].nextState[CLEAR]   = GREEN;
  fsmTable[GREEN].nextState[BOTH]    = GREEN;
  fsmTable[YELLOW].nextState[NONE]   = RED;     
  fsmTable[YELLOW].nextState[ALARM]  = RED;
  fsmTable[YELLOW].nextState[CLEAR]  = GREEN;
  fsmTable[YELLOW].nextState[BOTH]   = GREEN;
  fsmTable[RED].nextState[NONE]      = RED;         
  fsmTable[RED].nextState[ALARM]     = RED;
  fsmTable[RED].nextState[CLEAR]     = GREEN;
  fsmTable[RED].nextState[BOTH]      = GREEN;
  
  // Put the FSM into the intial state
  input_ =     NONE;         
  isPaused_ =  false;
  lastState_ = GREEN;
  state_ =     GREEN;
  doGreen();
         
}

int Security::Update()
{
  // States are updated only when there has been a state change
  // or the state is being paused (to check if pause expired)
  if ((lastState_ != state_) || (isPaused_ == true)){ 
    lastState_ = state_;
    switch (state_){
      case GREEN:
        doGreen();
        break;
      case YELLOW:
        doYellow();
        break;
      case RED:
        doRed();
        break;
    }
  }
  readInput();
  getNextState();

  return state_;
}

void Security::doGreen()
{
  digitalWrite(greenPin_, HIGH);
  digitalWrite(yellowPin_, LOW);
  digitalWrite(redPin_, LOW);
  isPaused_ = false;
}

void Security::doYellow()
{
  if (isPaused_ == false) {           // first time through
    start_ = millis();
    digitalWrite(greenPin_, LOW); 
    digitalWrite(yellowPin_, HIGH);
    digitalWrite(redPin_, LOW);
    isPaused_ = true;
  }
  elapsedTime_ = millis() - start_;
  if (elapsedTime_ >= pauseTime_){    // is the pause over?
    isPaused_ = false;
  }
}

void Security::doRed()
{
  digitalWrite(greenPin_, LOW);
  digitalWrite(yellowPin_, LOW);
  digitalWrite(redPin_, HIGH);
  isPaused_ = false;
}

void Security::readInput(void)
{ 
  int alarm_event = digitalRead(alarmPin_);
  int clear_event = digitalRead(clearPin_);
  if ((alarm_event == 1) && (clear_event == 1)){
    input_ = NONE;
  }
  else if ((alarm_event == 0) && (clear_event == 1)){
    input_ = ALARM;
  }
  else if ((alarm_event == 1) && (clear_event == 0)){
    input_ = CLEAR;
  }
  else{
    input_ = BOTH;
  }
}

void Security::getNextState(void)
{ 
  // If the clear button is pressed (either alone or in
  // combination with ALARM), the state is always updated.  
  if ((input_ == CLEAR) || (input_ == BOTH)) {
    isPaused_ = false;
    state_ = fsmTable[state_].nextState[input_]; 
  }
  // Other state are also updated except when there is a
  // pause that has not expired.
  else if (isPaused_ == false) {
    state_ = fsmTable[state_].nextState[input_];     
  }
}
