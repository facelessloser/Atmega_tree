#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8) // Calculate the value needed for the CTC match value in OCR1A

#include <avr/io.h>     
#include <avr/interrupt.h>
#include "light_ws2812.h"
#include <util/atomic.h>

//#define MAXPIX 253
struct cRGB ledArray[4];
struct cRGB led[6];

volatile unsigned long timer1_millis;
 
// Timing
long flashSince1 = 0;
long flashWaitTime1 = 500; 
long flashSince2 = 0;
long flashWaitTime2 = 10; 

int Pressed = 0;
int Pressed_Confidence_Level = 0; //Measure button press cofidence
int Released_Confidence_Level = 0; //Measure button release confidence
int buttonPushCounter = 1; // Keeps count of the button pushes

int pos=0;
int direction=1;	
int i;

// Led colours
int red = 0;  
int blue = 255;  
int green = 0;
int move = 0;

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
        if (buttonPushCounter == 3) 
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
    if (millis() >= flashSince1)
    {
    for (i=0; i<pos; i++) 
      ws2812_sendarray((uint8_t *)&ledArray[0],3);			// Repeatedly send "Green" to the led string. 

    for (i=0; i<(7-pos); i++) 
      ws2812_sendarray((uint8_t *)&ledArray[1],3);			// Red
    
    pos+=direction;		
    if ((pos==7)||(pos==0)) direction=-direction;

    flashSince1 += flashWaitTime1;
    }
  }

  if (buttonPushCounter == 2) 
  {
    if (millis() >= flashSince2)
    {
    // Rainbow leds
    led[0].r=red;led[0].g=green;led[0].b=blue;
    ws2812_setleds(led,1); // Led 1
    led[1].r=red;led[1].g=green;led[1].b=blue;
    ws2812_setleds(led,2); // Led 2 
    led[2].r=red;led[2].g=green;led[2].b=blue;
    ws2812_setleds(led,3); // Led 3
    led[3].r=red;led[3].g=green;led[3].b=blue;
    ws2812_setleds(led,4); // Led 4
    led[4].r=red;led[4].g=green;led[4].b=blue;
    ws2812_setleds(led,5); // Led 5
    led[5].r=red;led[5].g=green;led[5].b=blue;
    ws2812_setleds(led,6); // Led 6

    if (move == 0) // Blue to Red
    {
      red++;
      blue--;
      if (red == 255 && blue == 0) // waits till red and blue reach there respected numbers
      {
        move = 1;
      }
    }

    if (move == 1) // Red to Green
    {
      green++;
      red--;
      if (green == 255 && red == 0) // waits till red and green reach there respected numbers
      {
        move = 2;
      }
    }

    if (move == 2) // Green to Blue
    {
      blue++;
      green--;
      if (blue == 255 && green == 0) // waits till green and blue reach there respected numbers
      {
        move = 0;
      }
    }

    flashSince2 += flashWaitTime2;
    }
  }
} 

int main(void)
{
  DDRB |= 1 << PB1; // Set PB3 as output
  DDRD &= ~(1 << PD2); // Set PB2 as input
  PORTD |= 1 << PD2; // Set PB2 as input

  TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, Clock/8
 
  OCR1AH = (CTC_MATCH_OVERFLOW >> 8); // Load the high byte, then the low byte into the output compare
  OCR1AL = (unsigned char) CTC_MATCH_OVERFLOW; // Added (unsigned char)
 
  TIMSK1 |= (1 << OCIE1A); // Enable the compare match interrupt
    
  sei(); // Now enable global interrupts

// led array
  ledArray[0].r=00;ledArray[0].g=255;ledArray[0].b=00;	// LED 0 is Green
  ledArray[1].r=255;ledArray[1].g=00;ledArray[1].b=00;	// LED 1 is Red
  ledArray[2].r=00;ledArray[2].g=00;ledArray[2].b=255;	// LED 2 is Blue
  ledArray[3].r=00;ledArray[3].g=00;ledArray[3].b=00;		// LED 3 is Off



  while (1)
  {
  button();
  colourChange();
  }
}


