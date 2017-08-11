#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels
Adafruit_WS2801 strip = Adafruit_WS2801(101, dataPin, clockPin);
int p=200 ; // pausenzeit in ms
int rs=0 ;     // startfarbe
int gs=0 ;     // startfarbe
int bs=10 ;     // startfarbe

int rv=0 ;    //vor farbe 
int gv=100 ;    //vor farbe
int bv=0 ;    //vor farbe

int rm=100 ;    //mitte farbe 
int gm=100 ;    //mitte farbe
int bm=100 ;    //mitte farbe

int re=100 ;    //end farbe 
int ge=000 ;    //end farbe
int be=000 ;    //end farbe
void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif
int y=0 ;
for(   y=0 ; y<101 ; y++)
 {
 strip.setPixelColor(y-1, 100,100,100);  // wilkommensfarbe
 strip.show();
 }
 delay (1000);
strip.begin();
 strip.setPixelColor(49, 100,0,0); //rot
 strip.setPixelColor(50, 00,0,100); //rot
 strip.setPixelColor(51, 100,0,0); //rot
 strip.show();
 
}


void loop() {

 ampel(7);  // Led der Ampel eintragen Achtung !! beginnt bei 1
 
int x=0 ;
  strip.begin();
 for(  x=0 ; x<101 ; x++)
 {
 strip.setPixelColor(x-1, rs,gs,bs); //strip.setPixelColor(x-2, rs,gs,bs); 
 strip.setPixelColor(x, re,ge,be); //strip.setPixelColor(x-1, re,ge,be);
 strip.setPixelColor(x+1, rm,gm,bm);
 strip.setPixelColor(x+2, rm,gm,bm);
 strip.setPixelColor(x+3, rm,gm,bm);
 strip.setPixelColor(x+4, rv,gv,bv); 
 strip.show();
 delay (p);
}
 delay (3000);
 for(  int z=0 ; z<101 ; z++)
 {
 strip.setPixelColor(z-1, 100,100,100);  // wilkommensfarbe
 strip.show();
 }
 delay (1000);
  for(  int a=0 ; a<101 ; a++)
 {
 strip.setPixelColor(a-1, 100,100,100);  // schwarz farbe
 strip.show();
 }

}

void ampel(int thepin){
  
 /* strip.begin();
 strip.setPixelColor(thepin, 100,0,0); //rot
 strip.show();
 delay (500);
   strip.begin();
 strip.setPixelColor(thepin, 0,0,0); //schwarz
 strip.show();
 delay (500);
  strip.begin();
 strip.setPixelColor(thepin, 100,100,0); //gelb
 strip.show();
 delay (500);
   strip.begin();
 strip.setPixelColor(thepin, 0,0,0); //schwarz
 strip.show();
 delay (500);
  strip.begin();
 strip.setPixelColor(thepin, 0,100,0);// gruen
 strip.show(); */
strip.begin();
 strip.setPixelColor(9, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(8, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(7, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(6, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(5, 100,0,0); //rot
 strip.show();
 delay (1000); 
   for(  int r=10 ; r<101 ; r++)
 {
 strip.setPixelColor(r-1, 00,00,10);  // blau farbe
 strip.show();
 }
strip.begin();
 strip.setPixelColor(4, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(3, 100,0,0); //rot
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(2, 100,0,0); //rot
 strip.show();
 delay (1000);

 
strip.begin();
 strip.setPixelColor(1, 100,100,0); //gelb
 strip.show();
 delay (1000);
strip.begin();
 strip.setPixelColor(1, 00,100,0); //grün
 strip.show();
}
//
 
