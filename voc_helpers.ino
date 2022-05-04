#define VOC_READ_PIN A1
#define VOC_ACT_PIN 7

void warmup_voc(void)
{
  digitalWrite(VOC_ACT_PIN, HIGH);
  delay(1 * 1000);
  digitalWrite(VOC_ACT_PIN, LOW);
}

bool toggle_voc_sensor(void)
{
  static bool sensorStatus = false; // false if currently off

  if(sensorStatus == true)  // if sensor was on, take a reading and turn it off
  {
    int VOCmeas = analogRead(VOC_READ_PIN);       // reading
    int filtVOCmeas = expFilt('v', VOCmeas);      // filtered value
    postNewMeasurement('v', filtVOCmeas);
    digitalWrite(VOC_ACT_PIN, LOW);
  }

  else  // else turn the sensor on so next time this function is called it can be read
  {
    digitalWrite(VOC_ACT_PIN, HIGH);
  }

  sensorStatus = !sensorStatus; // toggle sensor status

  return true;  // true so timer repeats
}
