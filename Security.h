/* Security System Finite State Machine
 *
 * Simulates a security system with 3 states:
 *   GREEN: alarm is in the cleared state, the green LED is on
 *   YELLOW: alarm input recently detected, the yellow LED lights
 *        and the system pauses to allow the operator to manage
 *        a false alarm
 *   RED: alarm not cleared after prolonged period, red LED lights 
 *   
 * Use the Security constructor to initialize the Finite State Machine
 *   alarm input is simulated with a button switch on alarmPin
 *   alarm output is reset with a separate button switch on clearPin
 *   alarm LEDS have their own pins (greenPin, yellowPin, and redPin)
 *   pause before transition to the red state is user specified (mSec)
 *    
 * Call the Update() method frequently to keep the state current. The
 * state (GREEN = 0, YELLOW = 1, RED = 2) is returned by Update().
 *    
 * Tested with Energia V18 on F5529, FR6989 and TM4C123 LaunchPads
 * Released into the public domain
 * F. Milburn   January 2017
 */
#ifndef Security_h
#define Security_h

#include "Energia.h"

class Security
{
  public:
  Security(int alarmPin, int clearPin, int greenPin, int yellowPin, int redPin,
           unsigned long pauseTime);
  int Update();
  
  private:  
  int greenPin_;              // Energia defined output pins.
  int yellowPin_;
  int redPin_;
  int alarmPin_;              // Energia defined input pins.
  int clearPin_;     
         
  unsigned long pauseTime_;   // Pause between yellow and red states (mSec).
  unsigned long start_;       // Start time for pause from yellow --> red (mSec).
  unsigned long elapsedTime_; // Elapsed time in yellow state (mSec).
  boolean isPaused_;          // Timed condition internal to yellow state.

  enum Inputs {               // All possible input conditions for the FSM.
    NONE,                     // Alarm and clear are high.
    ALARM,                    // Alarm is low, clear is high.
    CLEAR,                    // Clear is low, alarm is high.
    BOTH                      // Alarm and clear buttons are low.
  };
  Inputs input_;              // Captures current input condition.

  enum States {               // All possible states in the FSM.
    GREEN,
    YELLOW,
    RED
  };
  struct Fsm {                // basic structure for the FSM table.
    States nextState[4];
  };
  Fsm fsmTable[3];            // The fully defined FSM table.
  States lastState_;          // The state the FSM was in when last updated.
  States state_;              // The state of the FSM in the current update.

  void doGreen();    
  void doYellow();
  void doRed();
  void readInput();
  void getNextState();
};
  
#endif
