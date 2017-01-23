/* Security System Finite State Machine
 *
 * Simulates a security system with 3 states:
 *   GREEN: alarm is in the reset state, the green LED is on.
 *   YELLOW: alarm input recently detected, the yellow LED is lit and the
 *           system pauses to allow the operator to manage  false alarm.
 *   RED: alarm not reset after prolonged period, red LED is lit.
 *   
 * Use the Security constructor to initialize the Finite State Machine:
 *   alarm input is simulated with a button switch on alarmPin.
 *   alarm output is reset with a separate button switch on resetPin.
 *   alarm LEDS have their own pins (greenPin, yellowPin, and redPin).
 *   pause before transition to the red state is user specified (mSec).
 *    
 * Call the Update() method frequently to keep the state current. The state
 * (GREEN = 0, YELLOW = 1, RED = 2) is returned by Update().
 * 
 * Tested with Energia V18 on F5529, FR6989 and TM4C123 LaunchPads
 * Released into the public domain
 * F. Milburn   January 2017``
 */
#ifndef Security_h
#define Security_h

#include "Energia.h"

enum SecurityStates {              // All possible states in the Security FSM.
  GREEN,
  YELLOW,
  RED
};

class Security
{
  public:
  Security(int alarmPin, int resetPin, int greenPin, int yellowPin, int redPin,
           unsigned long yellowPause);
  int Update();
  
  private:  
  int greenPin_;                  // Energia defined output pins.
  int yellowPin_;
  int redPin_;
  int alarmPin_;                  // Energia defined input pins.
  int resetPin_;     
         
  unsigned long startTime_;       // Start time for pause (mSec).
  unsigned long pause_;           // length of time a pause is in effect (mSec).

  enum Inputs {                   // All possible input conditions for the FSM.
    NONE,                         // Alarm and reset are high.
    ALARM,                        // Alarm is low, reset is high.
    RESET,                        // Reset is low, alarm is high.
    BOTH                          // Alarm and reset buttons are low.
  };
  Inputs input_;                  // Captures current input condition.

  SecurityStates lastState_;      // The state of the FSM when last updated.
  SecurityStates state_;          // The state of the FSM currently.

  void doGreen();                 // Perform actions associated with the GREEN state.
  void doYellow();                // Perform actions associated with the YELLOW state.
  void doRed();                   // Perform actions associated with the RED state.
  void getEvents();               // Read input and evaluate pause status
  void getNextState();            // Determine the next state
};
#endif
