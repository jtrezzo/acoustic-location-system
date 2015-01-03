/* Blink without Delay and send tone pulse for specified duration
 
 Turns on and off a light emitting diode(LED) connected to a digital  
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.
 
 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.

*/

// constants won't change. Used here to 
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin

const int triggerOutPin =  8;      // the number of the trigger signal pin

const int squareWaveOutPin = 9; // output pin for square wave pulse
const long duration = 1;         // @40KHz is 40 cycles per millisec
const int frequency = 38700;    // 40KHz

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 2000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  pinMode(triggerOutPin, OUTPUT);
  // set trigger to HIGH - waiting to sample analog signal on Teensy 3.1
  digitalWrite(triggerOutPin, HIGH);
  
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);

    // send trigger to start sampling analog signal on Teensy 3.1
    digitalWrite(triggerOutPin, LOW);
        
    tone(squareWaveOutPin, frequency, duration);  // send tone pulse
    
    // set trigger to HIGH - waiting to sample analog signal on Teensy 3.1
    digitalWrite(triggerOutPin, HIGH);

  }
}

