/*
 * LCD_Keypad_Demo.ino
 *
 * Created: 12/16/2020 4:44:54 PM
 * Author: colef
 */ 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const char keys[ROWS][COLS] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};
const byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
const byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// STATES
boolean armed = 0;



void setup()
{
	Serial.begin(9600);
	
	Serial.print("Initializing LCD...");
	lcd.init(); //initialize the lcd
	lcd.backlight(); //open the backlight
	Serial.print("Done.\n");
	
	resetDisplay(); // initial reset of display
	
}

void loop()
{
	if (armed) armedMode();
	else notArmedMode();
	delay(20);
}

void armedMode()
{
	char key = keypad.getKey();
	static int i = 0;
	if (key != NO_KEY)
	{
		Serial.println(key);
		lcd.setCursor(i+4, 0);
		lcd.print(key);
		i++;
		if (i == 8)
		{
			armed = 0;
			i = 0;
			resetDisplay();
			// ZZZZZ ADD REQUEST FOR ARMED BEEP HERE IN FINAL
		}
	}
}

void notArmedMode()
{
	char key = keypad.getKey();
	static int i = 0;
	if (key != NO_KEY)
	{
		Serial.println(key);
		lcd.setCursor(i+5, 0);
		lcd.print(key);
		i++;
		if (i == 6)
		{
			armed = 1;
			i = 0;
			resetDisplay();
			// ZZZZZ ADD REQUEST FOR ARMED BEEP HERE IN FINAL
		}
	}
}

void resetDisplay()
{
	if (!armed)
	{
		Serial.println("DISARMED");
		// fill the 6 middle spots with '*' chars, to be overwritten by keypad inputs
		/*for (int i=0; i<6; i++)
		{
			lcd.setCursor(i+5, 0);
			lcd.print('*');
			delay(10);
		}
		lcd.setCursor(4,0);
		lcd.print(' ');
		lcd.setCursor(11,0);
		lcd.print(' ');*/
		
		lcd.setCursor(4,0);
		lcd.print(" ****** ");
		lcd.setCursor(8,1);
		lcd.print("DISARMED");
		lcd.setCursor(0,1);
		lcd.print("     ")
	}
	else 
	{
		Serial.println("ARMED");
		// fill the 8 middle spots with '*' chars, to be overwritten by keypad inputs
		/*for (int i=0; i<8; i++)
		{
			lcd.setCursor(i+4, 0);
			lcd.print('*');
			delay(10);
		}*/
		lcd.setCursor(4,0);
		lcd.print("********");
		lcd.setCursor(8,1);
		lcd.print("   ARMED");
	}
}