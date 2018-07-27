// function to return the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // Read temperature from device, Celcius is default
  float tempC = sensors.getTempC(deviceAddress);
  
  // Return the temperature in Celsius
  return tempC;
}
