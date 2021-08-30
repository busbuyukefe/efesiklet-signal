#include <FastLED.h>

#define LED_PIN  7

#define SIGNAL_INTERVAL 120;

int interval = SIGNAL_INTERVAL;
const uint8_t kMatrixWidth = 12;
const uint8_t kMatrixHeight = 3;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds[ NUM_LEDS ];
#define LAST_VISIBLE_LED 35
uint8_t XY (uint8_t x, uint8_t y) 
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) 
  {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = 
  {
     8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
     7,   6,   5,   4,  35,  34,  33,  32,  23,  22,  21,  20,
     0,   1,   2,   3,  28,  29,  30,  31,  24,  25,  26,  27
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

long previousMillis = 0;
byte state = 0;

boolean brakeStatus = 0;
boolean signalLeft = 0;
boolean signalRight = 0;

boolean signalLeftLatch = 0;
boolean signalRightLatch = 0;

void setup() 
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CRGB::Black); //turn leds of at boot
  FastLED.show();
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  pinMode(2, INPUT_PULLUP);
}

void loop() 
{
  unsigned long currentMillis = millis();

  //Turn all leds off if all inputs are low
  if(signalLeftLatch==0 && signalRightLatch==0 && brakeStatus==0)fill_solid(leds, NUM_LEDS, CRGB::Black);
  
  brakeStatus = digitalRead(4);
  signalLeft = digitalRead(5);
  signalRight = digitalRead(6);
  
  if(signalLeft == 0)
  {
    //if(signalLeftLatch == 0)state = 0;
    signalLeftLatch = 1;
    signalRightLatch = 0;
  }

  if(signalRight == 0)
  {
    //if(signalRightLatch == 0)state = 0;
    signalLeftLatch = 0;
    signalRightLatch = 1;
  }

  if(digitalRead(2) == 0)
  {
    signalLeftLatch = 0;
    signalRightLatch = 0;
    //state = 0;
  }
  
  if(signalLeftLatch == 1)
  {
    if(currentMillis - previousMillis > interval) 
    {
      if(3-state == 0)interval=interval*4;
      if(3-state < 0)
      {
        for(int x=0; x<=3; x++)
        {
          if(brakeStatus==1)leds[XY(x,0)] = CRGB::Black;
          leds[XY(x,1)] = CRGB::Black;
          leds[XY(x,2)] = CRGB::Black;
        }
        interval = SIGNAL_INTERVAL;
        state = 0;
        //signalLeftLatch = 0;
      }
      else
      {
        previousMillis = currentMillis;
        if(brakeStatus==1)for(int x=0; x<=2; x++)leds[XY(3-state,x)] = CHSV(23,240,255);
        else for(int x=1; x<=2; x++)leds[XY(3-state,x)] = CHSV(23,240,255);
        state++;
      }
    }
  }

  else
  {
    for(int x=0; x<=3; x++)
    {
      if(brakeStatus==1)leds[XY(x,0)] = CRGB::Black;
      leds[XY(x,1)] = CRGB::Black;
      leds[XY(x,2)] = CRGB::Black;
    } 
  }

  if(signalRightLatch == 1)
  {
    if(currentMillis - previousMillis > interval) 
    {
      if(8+state == 11)interval=interval*4;
      if(8+state > 11)
      {
        for(int x=8; x<=11; x++)
        {
          if(brakeStatus==1)leds[XY(x,0)] = CRGB::Black;
          leds[XY(x,1)] = CRGB::Black;
          leds[XY(x,2)] = CRGB::Black;
        }
        interval = SIGNAL_INTERVAL;
        state = 0;
      }
      else
      {
        previousMillis = currentMillis;
        if(brakeStatus==1)for(int x=0; x<=2; x++)leds[XY(8+state,x)] = CHSV(23,240,255);
        else for(int x=1; x<=2; x++)leds[XY(8+state,x)] = CHSV(23,240,255);
        state++;
      }
    }
  }

  else
  {
    for(int x=8; x<=11; x++)
    {
      if(brakeStatus==1)leds[XY(x,0)] = CRGB::Black;
      leds[XY(x,1)] = CRGB::Black;
      leds[XY(x,2)] = CRGB::Black;
    }
  }

  
  if(brakeStatus == 0)for(int x=8; x<=19; x++)leds[x] = CRGB::Red;
  else
  { 
    if(signalLeftLatch == 1)
    {
      for(int x=12; x<=19; x++)leds[x] = CRGB::Black;
    }
    if(signalRightLatch == 1)
    {
      for(int x=8; x<=15; x++)leds[x] = CRGB::Black;
    }
  }
  
  
  //for(int x=0; x<=2; x++)leds[XY(3,x)] = CRGB::White;
  FastLED.show(); 
}
