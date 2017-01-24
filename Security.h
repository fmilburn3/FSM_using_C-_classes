/* Security System Finite State Machine
 *
 * Simulates a security system with 3 states:
 *   GREEN: alarm is in the reset state, the green LED is on.
 *   YELLOW: alarm input recently detected, the yellow LED is lit and a
 *           timer (timeout) starts before transition to RED.
 *   RED: alarm not reset before timeout, red LED is lit.
 *   
 * Use the Security constructor to initialize the Finite State Machine:
 *   alarm input is simulated with a button switch on alarmPin.
 *   alarm output is reset with a separate button switch on resetPin.
 *   alarm LEDS have their own pins (greenPin, yellowPin, and redPin).
 *   Yellow timeout before transition to the red state (mSec).
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
           unsigned long yellowTimeout);
  int Update();
  
  private:  
  int greenPin_;                  // Energia defined output pins.
  int yellowPin_;
  int redPin_;
  int alarmPin_;                  // Energia defined input pins.
  int resetPin_;     
         
  unsigned long startTime_;       // Start time for timeout (mSec).
  unsigned long timeout_;         // length of time for timeout (mSec).

  enum Events {                   // Possible input conditions for the FSM.
    TIMEOUT,                      // A timeout has occurred
    ALARM,                        // Alarm is low, reset is high.
    RESET,                        // Reset is low, alarm is high.
    NONE,                         // Alarm and reset buttons are high.
    BOTH                          // Alarm and reset buttons are low - RESET takes precedence.
  };
  Events event_;                  // Captures current input condition.

  SecurityStates lastState_;      // The state of the FSM when last updated.
  SecurityStates state_;          // The state of the FSM currently.

  void doGreen();                 // Perform actions associated with the GREEN state.
  void doYellow();                // Perform actions associated with the YELLOW state.
  void doRed();                   // Perform actions associated with the RED state.
  void getEvents();               // Read input and evaluate timeout status
  void getNextState();            // Determine the next state
};
#endif
