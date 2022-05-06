#include "MQ131.h" // https://github.com/ostaquet/Arduino-MQ131-driver

#define OZO_READ_PIN A0
#define OZO_ACT_PIN 6

#define LOAD_R 1000000
#define R0 (60041.42 + 77894.69 + 94017.15 + 114254.60)/4.0

void warmup_ozo(void)
{
  pinMode(OZO_ACT_PIN, OUTPUT);
  digitalWrite(OZO_ACT_PIN, HIGH);
  delay(20 * 1000);
  digitalWrite(OZO_ACT_PIN, LOW);

  MQ131.begin(OZO_ACT_PIN, OZO_READ_PIN, LOW_CONCENTRATION, LOAD_R);  
//  Serial.print(MQ131.getR0());
}

void calib_ozo(void)
{
  MQ131.begin(OZO_ACT_PIN,OZO_READ_PIN, LOW_CONCENTRATION, LOAD_R);  
  Serial.println("calib starting..");
  MQ131.calibrate();
  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(MQ131.getR0());
  Serial.println(" Ohms");
  Serial.print("Time to heat = ");
  Serial.print(MQ131.getTimeToRead());
  Serial.println(" s");
}

bool toggle_ozo_sensor(void)
{
  static bool sensorStatus = false; // false if currently off

  if(sensorStatus == true)  // if sensor was on, take a reading and turn it off
  {
    MQ131.sampleNow();
    int OzoMeas = MQ131.getO3(PPB);

    Serial.println(OzoMeas);
    postNewMeasurement('o', OzoMeas);
    digitalWrite(OZO_ACT_PIN, LOW);
  }

  else  // else turn the sensor on so next time this function is called it can be read
  {
    digitalWrite(OZO_ACT_PIN, HIGH);
  }

  sensorStatus = !sensorStatus; // toggle sensor status

  return true;  // true so timer repeats
}
