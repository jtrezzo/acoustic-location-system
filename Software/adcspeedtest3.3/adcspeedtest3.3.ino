// ADC speed test and points collection
//  with customized ADC software (faster than analogRead() )
//  customized to work on Teensy 3.1: 32-bit MK20DX256 72MHz ARM Cortex-M4
//                      256KB of Flash memory, 64KB of RAM
//                      see http://www.pjrc.com/teensy/ for details
//  should be able to run on other ARM Cortex family processors
//  may need modifications for ADC differences
//  Test results: sps = 285,714, elapsedTime = 7 mSec

// #include <ADC.h>

// ADC *adc = new ADC(); // adc object


// uses interval timer and ISR to read ADC at each timer interrupt

const int readPin = A0;        // ADC0, A0 pin 14
const int maxpoints = 500;    // number of points to collect

int ledPin = LED_BUILTIN;      // On board LED connected to digital pin 13
int triggerPin = 0;            // Start points collection when D0 goes low

int sps = 0;
double startTime, elapsedTime;

// Create an IntervalTimer object 
// note: varialbles shared with ISR need to be volatile and accessed with interrupts disabled
IntervalTimer myTimer;

volatile int iterations = 0;                // initialize iteration counter
volatile uint16_t points[maxpoints];        // store points here (16 bits per sample)
volatile boolean sampleFlag = false;        // sample points while sampleFlag is "true"

void setup() {

  //  ADC_Configuration();   //  set up ADC for single channel on-demand conversions
  analogReadResolution(16);  // set bits of resolution to 16
  analogReadAveraging(1);    // set number of averages - 1 to 32

  pinMode(ledPin, OUTPUT);                  // sets the LED pin as output
  pinMode(triggerPin, INPUT_PULLUP);        // sets the trigger pin as input
  pinMode(readPin, INPUT);                  // A0, pin 14 single ended input
  Serial.begin(9600);                       // open serial over usb
  
  myTimer.begin(adc_isr, 8);          // initialize timer obj:  ISR -> adc_isr, interrupt every 8 usec (125Ksps)
}


void loop()
{
    digitalWrite(ledPin, HIGH);   // Turn ON the LED pin
    iterations = 0;               // Reset iterations counter

    while (digitalRead(triggerPin) == HIGH);   //  Wait for signal (low) to start points collection
    
    startTime = millis();

    noInterrupts();
    sampleFlag = true;            // begin collecting sample points
    interrupts();

    digitalWrite(ledPin, LOW);   // Turn OFF the LED pin while sampling

    while (sampleFlag == true);  // wait until done sampling
    
    elapsedTime = millis() - startTime;

    sps = round ( (float) maxpoints * 1000.0 / (float) elapsedTime );

    printPoints();                              // print points to serial port
    
    delay(2000);                 //  wait 2 sec before looking for new cycle trigger signal
}

void adc_isr(void) {                             //  ADC ISR - collects adc samples
    if (sampleFlag == true) {
      if (iterations < maxpoints) {              // collect samples until maxpoints is reached
      points[iterations] = analogRead(readPin);  // read the analog input channel (16 bits 0-65535  0-3.3V)
                                                 // sample time is the fastest - 1.5 cycles/conversion
                                                 // and store the value in an array of points
      iterations++;
      if (iterations == maxpoints) {
        sampleFlag = false;                      //  stop sampling once maxpoints are collected
        }      
      }      
    }
}

void printPoints(void)          // print points collected
{
    Serial.println();
    Serial.print(maxpoints);
    Serial.print(" ADC samples in ");
    Serial.print(elapsedTime);
    Serial.print(" ms = ");
    Serial.print(sps);
    Serial.println(" samples/sec");
    Serial.println("(16 bits 0-65535  0-3.3V)");
    Serial.println("printing all points 10 per line");
    
    int lines = maxpoints / 10;
    int linenum = 0;

    while (linenum < lines)
    {
        for (int i=0; i <10; i++) {
            Serial.print(points[i+(linenum*10)]);
            Serial.print(",");                      // comma separated
        }
        Serial.println();
        linenum++;
    }
    
    Serial.println("End of points");
}

