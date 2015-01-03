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
const int readPin = A0; // ADC0, A0 pin 14

int ledPin = LED_BUILTIN;      // On board LED connected to digital pin 13
int triggerPin = 0;            // Start points collection when D0 goes high

int iterations = 0;             // iteration counter
const int maxpoints = 500;     // number of points to collect

int sps = 0;
double startTime, elapsedTime;
uint16_t points[maxpoints];        // store points here (16 bits per sample)

void setup() {

//  ADC_Configuration();          //  set up ADC1 for single channel on-demand conversions

//  adc->setAveraging(1); // set number of averages
//  adc->setResolution(16); // set bits of resolution to 16

analogReadResolution(16);

analogReadAveraging(1); // 1 to 32

  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  // see the documentation for more information
//  adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed

  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
//  adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

  // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
  //adc->enableInterrupts(ADC_0); // enable interrupts BEFORE calling a measurement method

//  adc->startContinuous(readPin, ADC_0);


  pinMode(ledPin, OUTPUT);                  // sets the pin as output
  pinMode(triggerPin, INPUT_PULLUP);        // sets the pin as input
  pinMode(readPin, INPUT);                  //A0, pin 14 single ended
  Serial.begin(9600);                       // open serial over usb
}


void loop()
{
    digitalWrite(ledPin, HIGH);   // Turn ON the LED pin
    iterations = 0;

    while (digitalRead(triggerPin) == HIGH);            //  Wait for signal (low) to start points collection
    
    startTime = millis();

    while (iterations < maxpoints) {            // collect samples until maxpoints is reached
    points[iterations] = analogRead(readPin);         // read the analog input channel 0 (12 bits 0-4095  0-3.3V)
                                                // sample time is the fastest - 1.5 cycles/conversion
                                                // and store the value in an array of points

//    points[iterations] = adc->analogRead(readPin, ADC_0); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

//    points[iterations] = (uint16_t)adc->analogReadContinuous(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!

    iterations++;                            
    }

    elapsedTime = millis() - startTime;

    sps = round ( (float) maxpoints * 1000.0 / (float) elapsedTime );

    printPoints();                              // print points to serial port
    
    digitalWrite(ledPin, LOW);   // Turn OFF the LED pin
    delay(1500);
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

/*
void adc0_isr(void) {
    adc->analogReadContinuous(ADC_0);
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN)); // Toggle the led
}
*/
