// air quality monitor code for kelsey's air quality monitor
// Russell Martin May 2022

// includes
#include <arduino-timer.h>
//#include "voc_helpers.h"

// defines
#define DEBUG 1

// module-level variables
auto timer = timer_create_default();
int VOC_val;

void setup() {
  // pin setup
  pinMode(LED_BUILTIN, OUTPUT);

  // indicate start of setup using builtin LED
  digitalWrite(LED_BUILTIN, HIGH);

  // warm up sensors (5 sec)
  warmup_voc();

  // indicate setup is concluded
  digitalWrite(LED_BUILTIN, LOW);

  // start looping timers to take measurements every 5 secs
  timer.every(1000, toggle_voc_sensor);

  // start serial
  if(DEBUG) {
    Serial.begin(115200);
  }
}

void loop() {
  timer.tick(); 
  
}

///////////////////////////////////////////
// postNewMeasurement posts new measured values from sensor modules into the main module (here)
//
void postNewMeasurement(char source, int newMeasVal)
{
  if(source == 'v')
  {
    VOC_val = newMeasVal;
    Serial.println(VOC_val);
  }
}

///////////////////////////////////////////
// expFilter takes measured value and source and returns a filtered value using past data from the source
// see: https://en.wikipedia.org/wiki/Exponential_smoothing
//
// source options are 'v' (VOC), 
//
double expFilt(char source, double measVal)
{
  double returnVal; // filtered value to return

  // filter parameters
  double alpha_voc = 0.001;

  // static values that persist every function call
  static bool firstVOC = true;
  static double prevVOC;

  // VOC sensor filtering
  if(source == 'v')
  {
    // first filt value is just meas value
    if(firstVOC)
    {
      returnVal = measVal;

      // store filtered value and note that first measurement has been taken
      prevVOC = returnVal;
      firstVOC = false;
    }
    // all other values filter according to equation: s_t = alpha * x_t + (1 - alpha) * s_t-1
    else
    {
      returnVal = alpha_voc * measVal + (1 - alpha_voc) * prevVOC;
      prevVOC = returnVal;
    }
  }

  return returnVal;
}
