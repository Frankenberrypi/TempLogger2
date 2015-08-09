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

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup(void)
{
  // Set LCD number of columns and rows.
  lcd.begin(16, 2);
  
  // Set backlight color
  lcd.setBacklight(RED);
  
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

// function to return the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // Read temperature from device, Celcius is default
  float tempC = sensors.getTempC(deviceAddress);
  
  // Convert to fahrenheit
  // float tempF = DallasTemperature::toFahrenheit(tempC);
  
  // Return the temperature in Celcius 
  return tempC;
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  // Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  // Serial.println("DONE");
  
  // Create the array to store temperatures
  float temps[numberOfDevices];
  
  // Todo: This was needed when it printed to serial. Delete, reduce? 
  //delay(500);
  
  // Loop through each device, store temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Output the device ID
		//Serial.print("Temperature for device: ");
		//Serial.println(i,DEC);
		
		// It responds almost immediately. Let's print out the data
		temps[i] = printTemperature(tempDeviceAddress);
	} 
	//else ghost device! Check your power requirements and cabling
	
  }
  // Print to the LCD
  // Todo: change this to work for multiple devices
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temps[0], 1);
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
