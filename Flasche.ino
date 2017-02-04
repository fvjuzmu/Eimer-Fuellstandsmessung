#include <WS2812.h>

#define ledDataPin A5  
#define LEDCount 2 
#define powerOutPin 5  
#define readCurrentPin A7

WS2812 LED(LEDCount); 
cRGB rgbValue;

cRGB offRGBValue;
cRGB errorRGBValue;

boolean ledOnOff = false;
int defcon = 0;
unsigned long blinkInterval[3] = { 99999, 3000, 280 };

unsigned long blinkTimer = 0;
unsigned long currentReadTimer = 0;
unsigned long defconTimer = 0;
long changeDefcon = 1800000; //30min.

float lastVoltage = 0;
float voltageAlarm = 0.4;

void setup() 
{
  pinMode(powerOutPin, OUTPUT);
  digitalWrite(powerOutPin, HIGH);

  Serial.begin(9600);

  //set off color
  offRGBValue.r = 0;
  offRGBValue.g = 0;
  offRGBValue.b = 0;

  //set error rgb color
  errorRGBValue.r = 0;
  errorRGBValue.g = 0;
  errorRGBValue.b = 255;

  //setup led
	LED.setOutput(ledDataPin); 
	LED.setColorOrderRGB();  // Uncomment for RGB color order

  //disable led
  rgbValue = offRGBValue ;

  //turn led of on start
  LED.set_crgb_at(0, offRGBValue);
  LED.set_crgb_at(1, offRGBValue);
  LED.sync();
}

void setDefcon()
{
  if(lastVoltage > voltageAlarm)
  {
    unsigned long currentMillis = millis();
    if(defconTimer == 0)
    {
      defconTimer = millis();
    }

    defcon = 1;
    
    if ((currentMillis - defconTimer) >= changeDefcon)
    {
      defcon = 2;
    }
  }
  else
  {
    defconTimer = 0;
    defcon = 0;
  }
}

void readCurrent()
{
  int sensorValue = analogRead(readCurrentPin);
  lastVoltage = sensorValue * (5.0 / 1023.0);
  
  Serial.println(lastVoltage);
}

void setLEDColor()
{
  switch(defcon)
  {
    case 0:      
      rgbValue = offRGBValue; 
      break;
    case 1:
      rgbValue.r = 255;
      rgbValue.g = 192; 
      rgbValue.b = 0; 
      break;
    case 2:
      rgbValue.r = 255;
      rgbValue.g = 0; 
      rgbValue.b = 0; 
      break;
    default:
      rgbValue.r = 0;
      rgbValue.g = 0; 
      rgbValue.b = 255; 
      break;
  }
}

void ledBlink()
{
  setLEDColor();
  
  unsigned long currentMillis = millis();

  if (defcon != 0 && (currentMillis - blinkTimer) >= blinkInterval[defcon])
  {
    blinkTimer = currentMillis;
    
    if(ledOnOff)
    {
      LED.set_crgb_at(0, rgbValue);
      LED.set_crgb_at(1, rgbValue);      
    }
    else
    {
      LED.set_crgb_at(0, offRGBValue);
      LED.set_crgb_at(1, offRGBValue);
    }
    LED.sync();
    ledOnOff = !ledOnOff;
  }
}

void loop() 
{
  ledBlink();
  readCurrent();
  setDefcon();
}