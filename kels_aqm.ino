// air quality monitor code for kelsey's air quality monitor
// Russell Martin May 2022

// includes
#include <arduino-timer.h>
#include "MQ131.h" // https://github.com/ostaquet/Arduino-MQ131-driver


// defines
#define DEBUG 1
#define VOC_FREQ 5000     // ms
#define OZO_FREQ 20000    // ms

// module-level variables
auto timer = timer_create_default();
int VOC_val = 0;
int Ozo_val = 0; //ppb

void setup() {
  // start serial
  if(DEBUG) {
    Serial.begin(115200);
  }
  
  // pin setup
  pinMode(LED_BUILTIN, OUTPUT);

  // indicate start of setup using builtin LED
  digitalWrite(LED_BUILTIN, HIGH);

  // warm up sensors (5 sec)
  warmup_voc();
  warmup_ozo();

  // indicate setup is concluded
  digitalWrite(LED_BUILTIN, LOW);

  // start looping timers to take measurements 
  timer.every(VOC_FREQ, toggle_voc_sensor);
  timer.every(OZO_FREQ, toggle_ozo_sensor);

}

void loop() {
  timer.tick(); 
  
}

///////////////////////////////////////////
// postNewMeasurement posts new measured values from sensor modules into the main module (here)
//
// source options are 'v' (VOC), 'o' (ozone)
void postNewMeasurement(char source, int newMeasVal)
{
  if(source == 'v')
  {
    VOC_val = newMeasVal;
  }
  else if(source == 'o')
  {
    Ozo_val = newMeasVal;
  }

  Serial.print(VOC_val);
  Serial.print(",");
  Serial.println(Ozo_val);
}

///////////////////////////////////////////
// expFilter takes measured value and source and returns a filtered value using past data from the source
// see: https://en.wikipedia.org/wiki/Exponential_smoothing
//
// source options are 'v' (VOC)
//
double expFilt(char source, double measVal)
{
  double returnVal; // filtered value to return

  // filter parameters
  double alpha_voc = 0.5;

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
