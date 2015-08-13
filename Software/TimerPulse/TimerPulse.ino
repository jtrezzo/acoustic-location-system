/* Blink using timer1 and send squarewave pulse for specified duration
 
 Turns on and off the on-board LED attached to pin 13,
 using timer1 as specified by "interval"
 Squarewave pulse sent every two intervals for "numCycles" on pin "squareWaveOutPin".
 Trigger sent (high to low for 1 usec) on pin "triggerOutPin" to start external points collection via DAQ
 
*/

#include <TimerOne.h>

// constants won't change. Used here to 
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin

const int triggerOutPin =  8;      // the number of the trigger signal pin
const int squareWaveOutPin = 9; // output pin for square wave pulse
const int frequency = 38700;    // 40KHz - not used at the moment, instead Timer1.initialize() sets 1/2 cycle duration

// Variables will change:
int ledState = LOW;             // ledState used to set the LED


// the follow variable is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)
                                // new cycle starts after two intervals


// Uses the timer interrupt to send a square wave pulse
// shares variables (declared as "volatile") between
// the interrupt service routine and the main program.

void setup() {
  // set the digital pins as output:
  pinMode(ledPin, OUTPUT);
  pinMode(squareWaveOutPin, OUTPUT);
  pinMode(triggerOutPin, OUTPUT);
  // init trigger to HIGH - waiting to sample analog signal on Teensy 3.1
  // High to Low signals start of new sampling cycle
  digitalWrite(triggerOutPin, HIGH);
  
  Timer1.initialize(13);
  Timer1.attachInterrupt(sendPulse); // sendPulse to run every 13 useconds (half of a cycle, approx 38.5KHz)

}

// The interrupt will send a pulse (alternating square wave state), and keep
// track of how many pulses were sent
volatile int pulseState = LOW;
volatile unsigned int cycleCount = 0;    // use volatile for shared variables
volatile unsigned int numCycles = 6;     // pulse duration is numCycles of square wave
volatile boolean  pulse = false;         // send pulse only when true

void sendPulse(void)  //  interrupt service routine
{
  if (pulse == true) {
    if (pulseState == LOW) {
      pulseState = HIGH;
      digitalWrite(squareWaveOutPin, pulseState);
      }
    else {
      pulseState = LOW;
      digitalWrite(squareWaveOutPin, pulseState);
      cycleCount++;                   // increase after each complete square wave sent
      if (cycleCount == numCycles) {  // check to see if we have sent required number of cycles
        pulse = false;                // set false when pulse cycles completed
        cycleCount = 0;        
        }
      }
    }
}


void loop()
{
  // here is where you'd put code that needs to be running all the time.

  
  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {  // blink led
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {        // start a new cycle
      ledState = HIGH;

      // toggle trigger to start sampling analog signal on Teensy 3.1
      digitalWrite(triggerOutPin, LOW);

  // to read a variable which the interrupt code writes, we
  // must temporarily disable interrupts, to be sure it will
  // not change while we are reading.  To minimize the time
  // with interrupts off, just quickly make a copy, and then
  // use the copy while allowing the interrupt to keep working.

      noInterrupts();
      pulse = true;    // start sending new pulse cycles
      interrupts();

      // reset trigger to HIGH - waiting to sample analog signal on Teensy 3.1
      digitalWrite(triggerOutPin, HIGH);

      }
    
      else {
        ledState = LOW;
      }
      
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
  }
}

