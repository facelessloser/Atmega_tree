#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8) // Calculate the value needed for the CTC match value in OCR1A

#include <avr/io.h>     
#include <avr/interrupt.h>
#include "light_ws2812.h"
#include <util/atomic.h>

//struct cRGB ledArray[4];
struct cRGB led[6];
struct cRGB rainbowChaseLed[6];
struct cRGB ledPulse[6];
struct cRGB passLed[8];

volatile unsigned long timer1_millis;
 
// Timing for the millus timers
long rainbowSince = 0;
long rainbowWaitTime = 10; 

long rainbowChaseSince = 0;
long rainbowChaseWaitTime = 200; 

long pulseSince1 = 0;
long pulseWaitTime = 10; 

long passSince1 = 0;
long passWaitTime = 500; 

// Button stuff
int Pressed = 0;
int Pressed_Confidence_Level = 0; //Measure button press cofidence
int Released_Confidence_Level = 0; //Measure button release confidence
int buttonPushCounter = 1; // Keeps count of the button pushes

// RGB Led colours
int rgbRed = 0;  
int rgbBlue = 255;  
int rgbGreen = 0;
int move = 0;

// Rainbow chase led
int ledNum = 0;
int change = 0;
int changeRed = 0;
int changeBlue = 0;
int changeGreen = 0;

// White pulse led
int pulseRed = 0;  
int pulseBlue = 0;  
int pulseGreen = 0;
int pulse = 0;
int numberLed;

// Pass led
int passGreen = 0;
int passRed = 1;
int passBlue = 2;
int passYellow = 3;
int passPurple = 4;
int passWhite = 5;

// Set up for the millus timers
ISR (TIMER1_COMPA_vect)
{
    timer1_millis++;
}

unsigned long millis ()
{
  unsigned long millis_return;

  ATOMIC_BLOCK(ATOMIC_FORCEON) // Ensure this cannot be disrupted
  { 
  millis_return = timer1_millis;
  }
 
  return millis_return;
}
// Millus timer code end

void button(void)
{
  if (bit_is_clear(PIND, 2))
  {
    Pressed_Confidence_Level ++; //Increase Pressed Conficence
    Released_Confidence_Level = 0; //Reset released button confidence since there is a button press
    if (Pressed_Confidence_Level >500) //Indicator of good button press
    {
      if (Pressed == 0)
      {
        // Do stuff here
        buttonPushCounter++;
        if (buttonPushCounter == 5) 
        {
          buttonPushCounter = 1;
        }
          Pressed = 1;
        }
        //Zero it so a new pressed condition can be evaluated
        Pressed_Confidence_Level = 0;
    }
  }

  else
  {
    Released_Confidence_Level ++; //This works just like the pressed
    Pressed_Confidence_Level = 0; //Reset pressed button confidence since the button is released
    if (Released_Confidence_Level >500)
    {
      Pressed = 0;
      Released_Confidence_Level = 0;
    }
  }
}

void colourChange(void)
{

  if (buttonPushCounter == 1) 
  {

    if (millis() - rainbowSince >= rainbowWaitTime)
    {
    rainbowSince = millis();

    // Rainbow leds
    led[0].r=rgbRed;led[0].g=rgbGreen;led[0].b=rgbBlue;
    ws2812_setleds(led,1); // Led 1
    led[1].r=rgbRed;led[1].g=rgbGreen;led[1].b=rgbBlue;
    ws2812_setleds(led,2); // Led 2 
    led[2].r=rgbRed;led[2].g=rgbGreen;led[2].b=rgbBlue;
    ws2812_setleds(led,3); // Led 3
    led[3].r=rgbRed;led[3].g=rgbGreen;led[3].b=rgbBlue;
    ws2812_setleds(led,4); // Led 4
    led[4].r=rgbRed;led[4].g=rgbGreen;led[4].b=rgbBlue;
    ws2812_setleds(led,5); // Led 5
    led[5].r=rgbRed;led[5].g=rgbGreen;led[5].b=rgbBlue;
    ws2812_setleds(led,6); // Led 6

    if (move == 0) // Blue to Red
    {
      rgbRed++;
      rgbBlue--;
      if (rgbRed == 255 && rgbBlue == 0) // waits till red and blue reach there respected numbers
      {
        move = 1;
      }
    }

    if (move == 1) // Red to Green
    {
      rgbGreen++;
      rgbRed--;
      if (rgbGreen == 255 && rgbRed == 0) // waits till red and green reach there respected numbers
      {
        move = 2;
      }
    }

    if (move == 2) // Green to Blue
    {
      rgbBlue++;
      rgbGreen--;
      if (rgbBlue == 255 && rgbGreen == 0) // waits till green and blue reach there respected numbers
      {
        move = 0;
      }
    }

    }
  }

  if (buttonPushCounter == 2) 
  {

    if (millis() - rainbowChaseSince >= rainbowChaseWaitTime)
    {
    rainbowChaseSince = millis();

      led[ledNum].r=changeRed;led[ledNum].g=changeGreen;led[ledNum].b=changeBlue; // Rainbow colours
      ws2812_setleds(led,ledNum+1); // Set the led number

      ledNum++; // Enumerate the led number
      if (ledNum == 6) // When reached change int to 0
      {
        ledNum = 0;
        change++; // Enumerate the change colour int
      } 

      if (change == 6) // When reached change int to 0
      {
        change = 0;
      }

      if (change == 0) // When reached change ws2812 to Blue
      {
        changeRed = 00;
        changeBlue = 255;
        changeGreen = 00;
      }

      if (change == 1) // When reached change ws2812 to Pink
      {
        changeRed = 128;
        changeBlue = 128;
        changeGreen = 00;
      }

      if (change == 2) // When reached change ws2812 to Red
      {
        changeRed = 255;
        changeBlue = 00;
        changeGreen = 00;
      }

      if (change == 3) // When reached change ws2812 to Orange
      {
        changeRed = 128;
        changeBlue = 00;
        changeGreen = 128;
      }

      if (change == 4) // When reached change ws2812 to Green
      {
        changeRed = 00;
        changeBlue = 00;
        changeGreen = 255;
      }

      if (change == 5) // When reached change ws2812 to Aqua
      {
        changeRed = 00;
        changeBlue = 128;
        changeGreen = 128;
      }

    }
  }


  if (buttonPushCounter == 3) 
  {

    if (millis() - pulseSince1 >= pulseWaitTime)
    {
    pulseSince1 = millis();

      ledPulse[0].r=pulseRed;ledPulse[0].g=pulseGreen;ledPulse[0].b=pulseBlue; // White
      ws2812_setleds(ledPulse,1);
      ledPulse[1].r=pulseRed;ledPulse[1].g=pulseGreen;ledPulse[1].b=pulseBlue; // White
      ws2812_setleds(ledPulse,2);
      ledPulse[2].r=pulseRed;ledPulse[2].g=pulseGreen;ledPulse[2].b=pulseBlue; // White
      ws2812_setleds(ledPulse,3);
      ledPulse[3].r=pulseRed;ledPulse[3].g=pulseGreen;ledPulse[3].b=pulseBlue; // White
      ws2812_setleds(ledPulse,4);
      ledPulse[4].r=pulseRed;ledPulse[4].g=pulseGreen;ledPulse[4].b=pulseBlue; // White
      ws2812_setleds(ledPulse,5);
      ledPulse[5].r=pulseRed;ledPulse[5].g=pulseGreen;ledPulse[5].b=pulseBlue; // White
      ws2812_setleds(ledPulse,6);

        if (pulse == 0)
        {
          pulseRed++;
          pulseBlue++;
          pulseGreen++;
        }

        if (pulse == 1)
        {
          pulseRed--;
          pulseBlue--;
          pulseGreen--;
        }

        if (pulseRed == 255 && pulseBlue == 255 && pulseGreen == 255)
        {
          pulse = 1;
        }

        if (pulseRed == 0 && pulseBlue == 0 && pulseGreen == 0)
        {
          pulse = 0;
        }

    }
  }

  if (buttonPushCounter == 4) 
  {

    if (millis() - passSince1 >= passWaitTime)
    {
    passSince1 = millis();

      passLed[passGreen].r=00;passLed[passGreen].g=255;passLed[passGreen].b=00; // Green
      ws2812_setleds(passLed,passGreen+1); // Led 2

      passLed[passRed].r=255;passLed[passRed].g=00;passLed[passRed].b=00;			// Red
      ws2812_setleds(passLed,passRed+1);

      passLed[passBlue].r=00;passLed[passBlue].g=00;passLed[passBlue].b=255;	// Blue
      ws2812_setleds(passLed,passBlue+1);

      passLed[passYellow].r=128;passLed[passYellow].g=128;passLed[passYellow].b=00;	// Yellow
      ws2812_setleds(passLed,passYellow+1);

      passLed[passPurple].r=128;passLed[passPurple].g=00;passLed[passPurple].b=128;	// Purple
      ws2812_setleds(passLed,passPurple+1);

      passLed[passWhite].r=255;passLed[passWhite].g=128;passLed[passWhite].b=00;	// White
      ws2812_setleds(passLed,passWhite+1);

      passGreen++;
      if (passGreen == 6)
      {
        passGreen = 0;
      }

      passRed++;
      if (passRed == 6)
      {
        passRed = 0;
      }

      passBlue++;
      if (passBlue == 6)
      {
        passBlue = 0;
      }

      passYellow++;
      if (passYellow == 6)
      {
        passYellow = 0;
      }

      passPurple++;
      if (passPurple == 6)
      {
        passPurple = 0;
      }

      passWhite++;
      if (passWhite == 6)
      {
        passWhite = 0;
      }

    }
  }
} 

int main(void)
{
  DDRB |= 1 << PB1; // Set PB3 as output WS2812
  DDRD &= ~(1 << PD2); // Set PB2 as input
  PORTD |= 1 << PD2; // Set PB2 as input

  TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, Clock/8
 
  OCR1AH = (CTC_MATCH_OVERFLOW >> 8); // Load the high byte, then the low byte into the output compare
  OCR1AL = (unsigned char) CTC_MATCH_OVERFLOW; // Added (unsigned char)
 
  TIMSK1 |= (1 << OCIE1A); // Enable the compare match interrupt
    
  sei(); // Now enable global interrupts

  while (1)
  {
    button();
    colourChange();
  }
}

