#include <SPI.h>
#include <SD.h>
/*
  SD card data logger
  This example shows how to log data from three analog sensors
  to an SD card using the SD library. Pin numbers reflect the default
  SPI pins for Uno and Nano models
  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** SDO - pin 11
 ** SDI - pin 12
 ** CLK - pin 13
 ** CS - depends on your SD card shield or module.
        Pin 10 used here for consistency with other Arduino examples
    (for MKRZero SD: SDCARD_SS_PIN)
  created  24 Nov 2010
  modified  24 July 2020
  by Tom Igor
  This example code is in the public domain.
*/

const int chipSelect = 10; // reopen this serial monitor after fixing your is

void setup() {  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed.");
    while (true) delay(20);
  }
  Serial.println("initialization done.");
  
  
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
	
	Serial.println("To sdcard:");
    
    for (int i=1; i<=10; i++)
	{
		// if the file is available, write to it:
		if (dataFile) {
			dataFile.println( (String) i );
			// print to the serial port too:
			Serial.println( (String) i );
		}
	}
	dataFile.close();
	
	Serial.println("Reading:");
	File readFile = SD.open("datalog.txt", FILE_READ);
	
	
	if (readFile) {

		while (readFile.available()) {

			Serial.write(readFile.read());

		}

		readFile.close();

	}
  
  
}

void loop() {
	/*
  // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    dataString += String(analogPin);
    if (analogPin < 2) {
      dataString += ",";
    }
  }
*/
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.


  // if the file isn't open, pop up an error:
  delay(1000);
}