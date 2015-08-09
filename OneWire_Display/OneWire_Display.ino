#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// I2C SLCL and SDA pins on analog 4 and 5. 
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Backlight color setup
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

// Start with sensor number 0
int idNum=0;
// Start with temp units in Celsius
char units='c';
  
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup(void)
{
  // Set LCD number of columns and rows.
  lcd.begin(16, 2);
  
  // Set backlight color
  lcd.setBacklight(TEAL);
    
  // start serial port
  Serial.begin(9600);
  Serial.println("Temp controller debugging");

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // Make an array for the temperature values
  // Todo: moved this into the main loop
  //float temps[numberOfDevices];
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		 Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }

}



void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures();
  
  // Create the array to store temperatures
  float temps[numberOfDevices];
  
  // Loop through each device, store temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{temps[i] = printTemperature(tempDeviceAddress);} 
	//else ghost device! Check your power requirements and cabling
  }
  
  
  // Read the values from the buttons
  uint8_t buttons = lcd.readButtons();
  
  if (buttons) {
//    if (buttons & BUTTON_UP) {
//      lcd.print("UP ");
//      lcd.setBacklight(RED);
//    }
//    if (buttons & BUTTON_DOWN) {
//      lcd.print("DOWN ");
//      lcd.setBacklight(YELLOW);
//    }
//    if (buttons & BUTTON_LEFT) {
//      lcd.print("LEFT ");
//      lcd.setBacklight(GREEN);
//    }
    if (buttons & BUTTON_RIGHT | buttons & BUTTON_UP) {
      if (idNum==numberOfDevices-1){idNum=0;}  
      else {idNum++;}
    }
    if (buttons & BUTTON_LEFT | buttons & BUTTON_DOWN) {
      if (idNum==0){idNum=numberOfDevices-1;}  
      else {idNum--;}
    }
    if (buttons & BUTTON_SELECT) {
      if (units=='c') {
        units='f';
        lcd.setBacklight(BLUE);
      }
      else {
        units='c';
        lcd.setBacklight(TEAL);
      }
    }
  }
  
  
  // Call function to print to LCD
  printTemp(temps, idNum, units);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to return the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // Read temperature from device, Celcius is default
  float tempC = sensors.getTempC(deviceAddress);
  
  // Return the temperature in Celcius 
  return tempC;
}

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
