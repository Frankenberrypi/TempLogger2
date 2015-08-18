// function to print data to LCD
void printTemp(float temps[], int idNum, int units)
{
    // Print the device number to the LCD
  lcd.setCursor(0,0);
  lcd.print("Sensor ");
  lcd.print(tempDeviceAddress[idNum]);

  // Print the temperature to the LCD
  // Todo: change this to work for multiple devices
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  
  if (units=='c'){
    lcd.print(temps[idNum], 1);
    lcd.print(" deg C");
  }
  else
  {
    lcd.print(DallasTemperature::toFahrenheit(temps[idNum]), 1);
    lcd.print(" deg F");
  }
}
