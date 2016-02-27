#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    5
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    90
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  119

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

uint8_t startIndex = 0;

// Ripple
int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.8;
int diff;
// uint32_t currentBg = random(256);
// uint32_t nextBg = currentBg;
 

void setup()
{
 delay(3000); // 3 second delay for recovery
 FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 FastLED.setBrightness(BRIGHTNESS);
 currentBlending = LINEARBLEND;
}

typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = {rainbowWithGlitter, OnlyGlitter, confetti, sinelon, juggle, bpm, pCloud, pLava, pForest, Ripple};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
 startIndex = startIndex + 1; /* motion speed */
 gPatterns[gCurrentPatternNumber]();
 FastLED.show();  
 FastLED.delay(1000/FRAMES_PER_SECOND); 
 EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
 EVERY_N_SECONDS( 5 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
 uint8_t brightness = 255;
 for( int i = 0; i < NUM_LEDS; i++)
 {
  leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
  colorIndex += 3;
 }
}

void nextPattern()
{
 // add one to the current pattern number, and wrap around at the end
 gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
 fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
 rainbow();
 addGlitter(80);
}

void OnlyGlitter() 
{
 fadeToBlackBy( leds, NUM_LEDS, 20);
 addGlitter(20);
}


void addGlitter( fract8 chanceOfGlitter) 
{
 if (random8() < chanceOfGlitter)
 {
  leds[ random16(NUM_LEDS) ] += CRGB::White;
 }
}

void confetti() 
{
 fadeToBlackBy( leds, NUM_LEDS, 10);
 int pos = random16(NUM_LEDS);
 leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
 fadeToBlackBy( leds, NUM_LEDS, 20);
 int pos = beatsin16(13,0,NUM_LEDS);
 leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
 // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
 uint8_t BeatsPerMinute = 62;
 CRGBPalette16 palette = PartyColors_p;
 uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
 for( int i = 0; i < NUM_LEDS; i++)
 {
  leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
 }
}

void juggle() {
 // eight colored dots, weaving in and out of sync with each other
 fadeToBlackBy( leds, NUM_LEDS, 20);
 byte dothue = 0;
 for( int i = 0; i < 8; i++)
 {
  leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
  dothue += 32;
 }
}

void pCloud()
{
 currentPalette = CloudColors_p;
 FillLEDsFromPaletteColors(startIndex);
}

void pLava()
{
 currentPalette = HeatColors_p;
 FillLEDsFromPaletteColors(startIndex);
}

void pForest()
{
 currentPalette = ForestColors_p;
 FillLEDsFromPaletteColors(startIndex);
}

int wrap(int step)
{
 if(step < 0) return NUM_LEDS + step;
 if(step > NUM_LEDS - 1) return step - NUM_LEDS;
 return step;
}

void Ripple()
{
 /*       
 if (currentBg == nextBg)
 {
  nextBg = random(256);
 }
 else
 if (nextBg > currentBg)
 {
  currentBg++;
 }
 else
 {
  currentBg--;
 }
 */
 for(uint16_t l = 0; l < NUM_LEDS; l++)
 {
  leds[l] = CRGB::Black;
 }
 if (step == -1)
 {
  center = random(NUM_LEDS);
  color = random(256);
  step = 0;
 }
 if (step == 0)
 {
  leds[center] = CHSV(color, 255, 255);
  step ++;
 }
 else
 {
  if (step < maxSteps)
  {
   leds[wrap(center + step)] = CHSV(color, 255, pow(fadeRate, step)*255);
   leds[wrap(center - step)] = CHSV(color, 255, pow(fadeRate, step)*255);
   if (step > 3)
   {
    leds[wrap(center + step - 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);
    leds[wrap(center - step + 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);
   }
   step ++;
  }
  else
  {
   step = -1;
  }
 }
 delay(50);
}
