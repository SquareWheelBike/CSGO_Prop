
/* TODO:
    - convert TON to class
    - convert ONS to class
	- create countdown class for bomb timer
//	- figure out how to use the lcd (demo)
//	- figure out the keypad
//	- create a demo where stars are replaced by the number you press
	- create an asynchronous tone system
//	- import lcd code from demo and make updates asynchronous
//	- add next number to top left corner
	- add a countdown display when armed in bottom left corner
	- create a settings page accessible when disarmed to set custom defuse time
	- run the buzzer on an interrupt
*/

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = 
{
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
} ;
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



// function call memory bits available
boolean oneShotBits[8];			// oneshot bits available for use for oneshot or toggle calls
// BITS 32-63 ARE FOR FAULTS ONLY!!

uint8_t ONSTracker;
uint8_t timerTracker;

boolean timerInSession[8];		// for speed, so we only update timer timers when needed
boolean timerMemory[sizeof(timerInSession)];			// make function calls smaller by remembering previous output state
unsigned long timerTimers[sizeof(timerInSession)];		// debounce timers available for use


// STATUS BITS
boolean armed = 0;
int nextInput = rand() % 10;


void setup()
{

	Serial.begin(9600);
	
	Serial.print("Initializing LCD...");
	lcd.init(); //initialize the lcd
	lcd.backlight(); //open the backlight
	Serial.print("Done.\n");
	
	resetDisplay(); // initial reset of display
	updateNextInput(); // queue next keypad input

}

void loop()
{ 

	ONSTracker = 0;
	timerTracker = 0;
	
	if (oneShot(FlasherBit(20), ONSTracker++)) updateDisplay();	// check for keypress and update machine status every 20ms
	
}


// SUBROUTINES


void updateDisplay()
{
	char key = keypad.getKey();
	static int i = 0;
	if (key != NO_KEY)
	{
		if (key-48 == nextInput)	// 48 decimal is ascii 0, so necessary for char to int comparison
		{
			Serial.println(key);
			lcd.setCursor(i+4+!armed, 0);	// 4 if armed, 5 if disarmed
			lcd.print(key);
			i++;
		}
	else 
	{	
		Serial.println(String(key) + " was inputted, looking for " + String(nextInput) + "... Resetting.");
		resetDisplay();
		i = 0;
	}
		if ((!armed && i == 6) || (armed && i == 8))
		{
			armed = !armed;
			i = 0;
			resetDisplay();
			// ZZZZZ ADD REQUEST FOR ARMED BEEP HERE IN FINAL
		}
		updateNextInput();	// queue next keypad input
	}
}

void resetDisplay()
{
	if (!armed)
	{
		Serial.println("DISARMED");
		
		// reset number inputs
		lcd.setCursor(4,0);
		lcd.print(" ****** ");
		// set armed/disarmed
		lcd.setCursor(8,1);
		lcd.print("DISARMED");
		// clear timer space
		lcd.setCursor(0,1);
		lcd.print("     ");
	}
	else 
	{
		Serial.println("ARMED");
		
		// reset number input
		lcd.setCursor(4,0);
		lcd.print("********");
		// set armed display
		lcd.setCursor(8,1);
		lcd.print("   ARMED");
	}
}

void updateNextInput()
{
	nextInput = rand() % 10;
	lcd.setCursor(0,0);
	lcd.print(nextInput);
}



// FUNCTION CALLS
boolean FlasherBit(float hz)
{
	int T = round(1000.0 / hz);
	if ( millis() % T >= T/2 ) return 1;
	else return 0;
}

boolean oneShot(boolean precond, uint8_t OSR)
{
	// use global memory to keep track of oneshot bits
	if (precond == 1 && oneShotBits[OSR] == 0)
	{
		oneShotBits[OSR] = 1;
		return 1;
	}
	else if (precond == 0 && oneShotBits[OSR] == 1)
	{
		oneShotBits[OSR] = 0;
		return 0;
	}
	else return 0;
}

boolean toggleState(boolean precond, boolean toggled, uint8_t OSR)
{
	if (oneShot(precond, OSR)) toggled = !toggled;
	return toggled;
}

float voltsFromAnalogIn (int input)
{
	float output = ((float)input * 5.0) / 1024.0;
	return output;
}

int voltsToAnalogIn (float input)
{
	int output = (int)((input * 1024.0) / 5.0);
	return output;
}

int voltsToAnalogOut(float input)
{
	int output = (input * 255.0 / 5.0);
	return output;
}

boolean TON(boolean input, int preset, int timerNumber)
{
	if (input && !timerInSession[timerNumber]) timerTimers[timerNumber] = millis();
	else if (input && timerMemory[timerNumber]) return 1;
	else if (input && millis() - timerTimers[timerNumber] >= preset)
	{
		timerMemory[timerNumber] = 1;
		return 1;
	}
	else;
	timerMemory[timerNumber] = 0;
	timerInSession[timerNumber] = input;
	return 0;
}

boolean limit(float input, float lower, float upper)
{
	if (input < lower) return 0;
	else if (input > upper) return 0;
	else return 1;
}

String secondsToClock(int input)
{
	int seconds, minutes, hours;
	
	seconds = input % 60;
	minutes = (input / 60) % 60;
	hours = input / 3600;
	
	char outputBuffer[14];
	sprintf(outputBuffer,"%02u:%02u:%02u",hours,minutes,seconds);
	
	return String(outputBuffer);
}