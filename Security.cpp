#include "Energia.h"
#include "Security.h"

struct Fsm {                    // basic FSM structure for a single state.
  SecurityStates nextState[3];  // next state for TIMEOUT, ALARM, and RESET
  unsigned long timeout;        // time before moving to next state (mSec)
};
// Initialized FSM table for all states.
static Fsm fsmTable[] = {
  // ------ nextState ------
  // TIMEOUT  ALARM   RESET    timeout             state
  // -------  ------  ------   ---------           -------
  {{ GREEN,   YELLOW, GREEN }, 0        },      // GREEN
  {{ RED,     RED,    GREEN }, 0        },      // YELLOW
  {{ RED,     RED,    GREEN }, 0        },      // RED
};

Security::Security(int alarmPin, int resetPin, int greenPin, int yellowPin,
                   int redPin, unsigned long yellowTimeout)
{
  greenPin_ =  greenPin;
  yellowPin_ = yellowPin;
  redPin_ =    redPin;
  alarmPin_ =  alarmPin;
  resetPin_ =  resetPin;

  // User defines the yellow -- > red transition timeout, all others are left at 0
  fsmTable[YELLOW].timeout = yellowTimeout;         

  // Initialize alarm inputs.
  pinMode(alarmPin_, INPUT_PULLUP); 
  pinMode(resetPin_, INPUT_PULLUP);

  // Initialize alarm outputs.
  pinMode     (greenPin_,  OUTPUT);       
  digitalWrite(greenPin_,  LOW);
  pinMode     (yellowPin_, OUTPUT);
  digitalWrite(yellowPin_, LOW);
  pinMode     (redPin_,    OUTPUT);
  digitalWrite(redPin_,    LOW);
  
  // Put the FSM into the intial state
  event_ =     NONE;         
  lastState_ = GREEN;
  state_ =     GREEN;
  timeout_ = fsmTable[state_].timeout;
  doGreen();     
}

int Security::Update()
{
  // States are updated only when there has been a state change
  if (lastState_ != state_) { 
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
  getEvents();
  getNextState();

  return state_;
}

void Security::doGreen()
{
  digitalWrite(greenPin_,  HIGH);
  digitalWrite(yellowPin_, LOW);
  digitalWrite(redPin_,    LOW);
  timeout_ = fsmTable[state_].timeout;
  startTime_ = millis();
}

void Security::doYellow()
{
  digitalWrite(greenPin_,  LOW); 
  digitalWrite(yellowPin_, HIGH);
  digitalWrite(redPin_,    LOW);
  timeout_ = fsmTable[state_].timeout;
  startTime_ = millis();
}

void Security::doRed()
{
  digitalWrite(greenPin_,  LOW);
  digitalWrite(yellowPin_, LOW);
  digitalWrite(redPin_,    HIGH);
  timeout_ = fsmTable[state_].timeout;
  startTime_ = millis();
}

void Security::getEvents(void)
{ 
  // update the status of inputs and timeout
  int alarmEvent = digitalRead(alarmPin_);
  int resetEvent = digitalRead(resetPin_);
  bool timeoutEvent = true;
  if (timeout_ > 0) {
    unsigned long elapsedTime = millis() - startTime_;
    if (elapsedTime < timeout_){
      timeoutEvent = false;                             
    }
  }
  // determine what event occurred
  event_ = NONE;
  if (resetEvent == 0) {
    event_ = RESET;
  }
  else if ((alarmEvent == 0) && (timeoutEvent == true)){
    event_ = ALARM;         
  } 
  else if (timeoutEvent == true){
    event_ = TIMEOUT;
  }
}

void Security::getNextState(void)
{ 
  if (event_ == NONE) {
    // state remains unchanged
  }
  else {
    state_ = fsmTable[state_].nextState[event_];
  }
}
