#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
//#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <SD.h>

// I2C SLCL and SDA pins on analog 4 and 5. 
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Data wire is plugged into digital port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12

// Backlight color setup
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// SD card on SPI bus as follows:
// MOSI - Digital 11, MISO - Digital 12, CLK - Digital 13, CS - Digital 4
const int chipSelect = 4;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
int errorState = 0; //0 = none, 1 = Sensors, 2 = MemCard
int errorSwitch = 0;

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
    
  // start serial port for debugging
  Serial.begin(9600);
  Serial.println("Temp controller debugging");

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  if(numberOfDevices == 0){errorState = 1;}
  
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
  
  // SD Card stuff
  Serial.print("Initializing SD card///");
  pinMode(11, OUTPUT);
  
  // See if the card is there and can be initialized 
  if (!SD.begin(chipSelect)){
    Serial.println("Card failed");
    errorState = 2;
    return;
  }
  Serial.println("Card initialized");
  
  // Delete the old file
  if (SD.exists("DATALOG.CSV"));{
    SD.remove("DATALOG.CSV");
  }
  File dataFile = SD.open("DATALOG.CSV", FILE_WRITE);
  dataFile.print("Time_(ms),");
  for (int i=0; i < numberOfDevices; i++){
    dataFile.print("Temp ");
    dataFile.print(tempDeviceAddress[i]);
    dataFile.print(" ,");
  }
  dataFile.println("");
  dataFile.close();
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
  printTemp(temps, idNum, units, errorState, errorSwitch);
  
  // Log data to SD card
  // Todo: check if there is an old file and back it up
    
  // Open the file
  File dataFile = SD.open("DATALOG.CSV", FILE_WRITE);
//  int m = millis();
//  Serial.println(m);
  
  // Write to the file.  First time, then loop through temps
  if (dataFile){
    dataFile.print(millis());
    for (int i=0; i < numberOfDevices; i++){
      dataFile.print(" ,");
      dataFile.print(temps[i]);
    }
    // newline in file, then close it
    dataFile.println(",");
    dataFile.close();  
  }
}






