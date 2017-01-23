#include "Energia.h"
#include "Security.h"

struct Fsm {                   // basic FSM structure for a single state.
  SecurityStates nextState[4]; // next state for NONE, ALARM, BOTH, and RESET
  unsigned long pause;         // pause before moving to next state (mSec)
};
// Initialized FSM table for all states.
static Fsm fsmTable[] = {
  // ---------- nextState ----------
  // NONE     ALARM   BOTH    RESET    pauseTime           state
  // -------  ------  ------  ------   ---------           -------
  {{ GREEN,   YELLOW, GREEN,  GREEN }, 0        },      // GREEN
  {{ RED,     RED,    GREEN,  GREEN }, 0        },      // YELLOW
  {{ RED,     RED,    GREEN,  GREEN }, 0        },      // RED
};

Security::Security(int alarmPin, int resetPin, int greenPin, int yellowPin,
                   int redPin, unsigned long yellowPause)
{
  greenPin_ =  greenPin;
  yellowPin_ = yellowPin;
  redPin_ =    redPin;
  alarmPin_ =  alarmPin;
  resetPin_ =  resetPin;

  // User defines the yellow -- > red transition pause, all others are left at 0
  fsmTable[YELLOW].pause = yellowPause;         

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
  input_ =     NONE;         
  lastState_ = GREEN;
  state_ =     GREEN;
  pause_ = fsmTable[state_].pause;
  doGreen();     
}

int Security::Update()
{
  // States are updated only when there has been a state change and the
  // state is not paused
  if ((lastState_ != state_) && (pause_ == 0)){ 
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
  pause_ = fsmTable[state_].pause;
  startTime_ = millis();
}

void Security::doYellow()
{
  digitalWrite(greenPin_,  LOW); 
  digitalWrite(yellowPin_, HIGH);
  digitalWrite(redPin_,    LOW);
  pause_ = fsmTable[state_].pause;
  startTime_ = millis();
}

void Security::doRed()
{
  digitalWrite(greenPin_,  LOW);
  digitalWrite(yellowPin_, LOW);
  digitalWrite(redPin_,    HIGH);
  pause_ = fsmTable[state_].pause;
  startTime_ = millis();
}

void Security::getEvents(void)
{ 
  int alarmEvent = digitalRead(alarmPin_);
  int resetEvent = digitalRead(resetPin_);
  
  if ((alarmEvent == 1) && (resetEvent == 1)){
    input_ = NONE;
  }
  else if ((alarmEvent == 0) && (resetEvent == 1)){
    input_ = ALARM;
  }
  else if ((alarmEvent == 1) && (resetEvent == 0)){
    input_ = RESET;
  }
  else{
    input_ = BOTH;
  }
  
  // Check to see if a pause is in place for the state
  if (pause_ > 0) {
    // A pause condition is in place.  Compare it to the elapsed time.
    unsigned long elapsedTime = millis() - startTime_;
    if (elapsedTime >= pause_){
      // Pause is over.  Set it to 0.
      pause_ = 0;                               
    }
  }
}

void Security::getNextState(void)
{ 
  // If the reset button is pressed (either alone or in combination with 
  // ALARM), the state is updated and the pause set to 0.  
  if ((input_ == RESET) || (input_ == BOTH)) {
    pause_ = 0;
    state_ = fsmTable[state_].nextState[input_]; 
  }
  // else update the state only when pause is set to 0.
  else if (pause_ == 0) {
    state_ = fsmTable[state_].nextState[input_];     
  }
}
