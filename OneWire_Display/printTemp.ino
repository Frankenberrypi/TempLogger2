// function to print data to LCD
void printTemp(float temps[], int idNum, int units, int errorState, int errorSwitch)
{
  // Display errors
  //0 = none, 1 = Sensors, 2 = MemCard
  switch (errorState){
    case 0:
      break;
    case 1:
      lcd.setCursor(0,0);
      lcd.print("NO SENSORS");
      delay(100000);
      break;
    case 2:
      if (errorSwitch = 0){
        errorSwitch = 1;
      }
      else {
        lcd.setCursor(0,0);
        lcd.print("MEM ERROR");
        delay(1000);
        errorSwitch = 0;
      }
      break;
  }
        
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
