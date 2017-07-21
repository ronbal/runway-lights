//////////////////////////////////////////////////////////////////
 //
 // Programmierung einer Designeruhr als Wandschmuck
 // Grundlage ist ein LED-Streifen (WS2812B) oder ein Ring mit
 // 60 LEDs
 //
 //////////////////////////////////////////////////////////////////


// Import der Neopixel-Bibliothek von Adafruit:
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


// Anschluss der LEDs an Digitalausgang 6:
#define PIN 2

#define LEDS 104 //Anzahl LED; im Strang

// Streifen/Ring initialisieren:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_GRB + NEO_KHZ800);


// Position des Stundenzeigers:
uint8_t currentHour;
// Position des Minutenzeigers:
uint8_t currentMin;

// Position des Sekundenzeigers:
float ghostOnePos;
float ghostOneSplit;
float ghostOneTime;
float ghostOneDist;
float ghostOneZiel;
float ghostOneSec;

float ghostTwoPos;
float ghostTwoSplit;
float ghostTwoTime;
float ghostTwoDist;
float ghostTwoZiel;
float ghostTwoSec;


float ghostThreePos;
float ghostThreeSplit;
float ghostThreeTime;
float ghostThreeDist;
float ghostThreeZiel;
float ghostThreeSec;

// Initialisierung:
void setup() {
  Serial.begin(9600);
  strip.begin(); // Streifen initialisieren
  strip.show();  // Alle Pixel erst einmal aus

  // Initiale Uhrzeit manuell einstellen
  // Initialer Zustand der Uhrenanzeige bei Programmstart:
  currentHour =  52; // Position wird modulo 12 berechnet
  currentMin  =  52;
  
  ghostOnePos  =  0;
  ghostOneTime = 10;
  ghostOneDist=1200;
  ghostOneSplit=LEDS*ghostOneDist/400/ghostOneTime/4;
  ghostOneZiel = ghostOneDist;

  ghostTwoPos  =  0;
  ghostTwoTime = 100;
  ghostTwoDist=1200;
  ghostTwoSplit=LEDS*ghostTwoDist/400/ghostTwoTime/4;
  ghostTwoZiel = ghostTwoDist;

  ghostThreePos  =  0;
  ghostThreeTime = 60;
  ghostThreeDist=1200;
  ghostThreeSplit=LEDS*ghostThreeDist/400/ghostThreeTime/4;
  ghostThreeZiel = ghostThreeDist;

ghostOneSec=ghostOnePos;
ghostTwoSec=ghostTwoPos;
ghostThreeSec=ghostThreePos;
}
//////////////////////////////////////////////////////////////////
//
// Methode:   colorPicker(Pixelnummer)
// Ergebnis:  zusammengesetzte Farbe
// Zweck:     Farbanteile einer LED berechnen
//
//////////////////////////////////////////////////////////////////
uint32_t colorPicker(uint8_t pos) {
    const uint32_t hourColor = 0xD00000; // Stundenzeiger  mit Rot
    const uint32_t redColor = 0xFF0000; //  Rot
    const uint32_t whiteColor = 0xFFFFFF; //  weiß
    const uint32_t greenColor = 0x00FF00; //  Grün
    const uint32_t blueColor = 0x0000FF; //  blau
    const uint32_t yellowColor = 0xFFFF00; //  gelb
    const uint32_t minColor  = 0x00D000; // Minutenzeiger  mit Grün
    const uint32_t secColor  = 0x0000D0; // Sekundenzeiger mit Blau
    uint32_t result = 0x050505;
    // Bei Position in N, O, S, W Extra-Farbanteil für R, G, B hinzufügen:
    if (pos == LEDS*50/400+2)       result = 0x0000ff; 
    if (pos == LEDS*100/400+2)       result = 0x0000ff; 
    if (pos == LEDS*150/400+2)       result = 0x0000ff; 
    if (pos == LEDS*200/400+2)       result = 0x0000ff; 
    if (pos == LEDS*250/400+2)       result = 0x0000ff; 
    if (pos == LEDS*300/400+2)       result = 0x0000ff; 
    if (pos == LEDS*350/400+2)       result = 0x0000ff; 
    if (pos == 1)       result = 0x0000ff; 
    if (pos == LEDS*50/400+3)       result = 0x0000ff; 
    if (pos == LEDS*100/400+3)       result = 0x0000ff; 
    if (pos == LEDS*150/400+3)       result = 0x0000ff; 
    if (pos == LEDS*200/400+3)       result = 0x0000ff; 
    if (pos == LEDS*250/400+3)       result = 0x0000ff; 
    if (pos == LEDS*300/400+3)       result = 0x0000ff; 
    if (pos == LEDS*350/400+3)       result = 0x0000ff; 
    if (pos == 2)       result = 0x0000ff; 
    // Ist Minutenzeiger  auf dieser Position,
    // Grünanteil hinzufügen:

  if (ghostOneZiel > 0){
    if (pos ==  ghostOnePos)  result = greenColor;  
    if (pos ==  ghostOnePos+1)  result = blueColor;  
    if (pos ==  ghostOnePos+2)  result = blueColor;  
    if (pos ==  ghostOnePos+3)  result = blueColor;  
    if (pos ==  ghostOnePos+4)  result = redColor;  
    // Ist Stundenzeiger auf dieser Position,
    // Rotanteil hinzufügen:
    
    }
      
  if (ghostTwoZiel > 0){
    if (pos ==  ghostTwoPos)  result = greenColor;  
    if (pos ==  ghostTwoPos+1)  result = whiteColor;  
    if (pos ==  ghostTwoPos+2)  result = whiteColor;  
    if (pos ==  ghostTwoPos+3)  result = whiteColor;  
    if (pos ==  ghostTwoPos+4)  result = redColor;  
    // Ist Sekundenzeiger auf dieser Position,
    // Blauanteil hinzufügen:
  }
    if (ghostThreeZiel > 0){
    if (pos ==  ghostThreePos)  result = greenColor;  
    if (pos ==  ghostThreePos+1)  result = yellowColor;  
    if (pos ==  ghostThreePos+2)  result = yellowColor;  
    if (pos ==  ghostThreePos+3)  result = yellowColor;  
    if (pos ==  ghostThreePos+4)  result = redColor;  
    // Ist Stundenzeiger auf dieser Position,
    // Rotanteil hinzufügen:
    
    }
    
    return result;
}


//////////////////////////////////////////////////////////////////
//
// Methode:   displayClock()
// Zweck:     Für alle 60 Pixel Farbe errechnen,
//            bei der LED entsprechend setzen,
//            und darstellen
//
//////////////////////////////////////////////////////////////////
void displayClock() {

    for (uint8_t pixno = LEDS+1; pixno > 0; pixno--) 
       strip.setPixelColor(pixno-1, colorPicker(pixno));
    strip.show();
}


//////////////////////////////////////////////////////////////////
//
// Methode:   incrementTimeBySecond
// Zweck:     Berechnet neue Zeit nach einer Sekunde
//
//////////////////////////////////////////////////////////////////
void incrementTimeBySecond() {
  
 ghostOneSec= ghostOneSec-ghostOneSplit;
 ghostOnePos = round(ghostOneSec);

 ghostTwoSec= ghostTwoSec-ghostTwoSplit;
 ghostTwoPos = round(ghostTwoSec);
 
 ghostThreeSec= ghostThreeSec-ghostThreeSplit;
 ghostThreePos = round(ghostThreeSec);
 

  if (ghostOnePos < 1) { // Minutengrenze erreicht?
    ghostOneSec = LEDS+1;       // Ja => Sekundenzeiger auf 0
    //currentMin--;         //       und Minuten erhöhen
  }
  if (ghostTwoPos < 1) { // Stundengrenze erreicht?
    ghostTwoSec = LEDS+1;       // Ja => Minutenzeiger auf 0
    //currentHour--;        //       und Stunden erhöhen
  }
    if (ghostThreePos < 1) { // Stundengrenze erreicht?
    ghostThreeSec = LEDS+1;       // Ja => Minutenzeiger auf 0
    //currentHour--;        //       und Stunden erhöhen
  }

}
//////////////////////////////////////////////////////////////////
//
// Methode:   loop()
// Zweck:     Uhr implementieren
//
//////////////////////////////////////////////////////////////////
void loop() {
   // Uhr anzeigen:
   displayClock();
   // 1 Sekunde Pause:
   delay(250);
   ghostOneZiel=ghostOneZiel-(ghostOneDist/ghostOneTime/4);
   ghostTwoZiel=ghostTwoZiel-(ghostTwoDist/ghostTwoTime/4);
   ghostThreeZiel=ghostThreeZiel-(ghostThreeDist/ghostThreeTime/4);
   // Neuberechnen der Tageszeit:
   incrementTimeBySecond();
}
