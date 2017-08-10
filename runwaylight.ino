//////////////////////////////////////////////////////////////////
 //
 // Programmierung eines Lauflichts für eine 400m Rundbahn
 // 
 //
 //////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include "Adafruit_WS2801.h"
// #include "bild1.c"

#define BGTDEBUG 1

//---------------------------------------------------------------------
// WiFi

byte my_WiFi_Mode = 0;  // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint


// Fill in your WiFi router SSID and password
const char* ssid_sta = "";
const char* password_sta = "";

const char * ssid_ap = "TUS_Bothfeld";
const char * password_ap = "";    // alternativ :  = "12345678";

WiFiServer server(80);
WiFiClient client;

#define MAX_PACKAGE_SIZE 2048
char HTML_String[5000];
char HTTP_Header[150];

//---------------------------------------------------------------------
// Allgemeine Variablen

int ok_count = 0;
int notok_count = 0;
int Aufruf_Zaehler = 0;

#define ACTION_OK 1
#define ACTION_NOTOK 2
#define ACTION_SET_DATE_TIME 3
#define ACTION_SET_NAME 4
#define ACTION_LIES_AUSWAHL 5
#define ACTION_LIES_VOLUME 6

int action;

// Vor- Nachname
char Vorname[20] = "B&auml;rbel";
char Nachname[20] = "von der Waterkant";
char Distanz[20] = "von der Waterkant";

// Uhrzeit Datum
byte Uhrzeit_HH = 16;
byte Uhrzeit_MM = 47;
byte Uhrzeit_SS = 0;
byte Datum_TT = 9;
byte Datum_MM = 2;
int Datum_JJJJ = 2016;

// checkboxen
char Wochentage_tab[7][3] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
byte Wochentage = 0;

// Radiobutton
char Jahreszeiten_tab[4][15] = {"Fr&uuml;hling", "Sommer", "Herbst", "Winter"};
byte Jahreszeit = 0;

// Combobox
char Wetter_tab[4][10] = {"Sonne", "Wolken", "Regen", "Schnee"};
byte Wetter;


// Slider
byte Volume = 15;

char tmp_string[20];

// Import der Neopixel-Bibliothek von Adafruit:
#include <Adafruit_NeoPixel.h>

// Anschluss der LEDs an Digitalausgang 2:
#define PIN 2                                   //Pin wS2812 Strip
uint8_t dataPin  = 4;                         // DATA WS2801
uint8_t clockPin = 5;                          // Clock WS2801

#define LEDS 101 //Anzahl LED; im Strang

// Streifen/Ring initialisieren:
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_WS2801 strip = Adafruit_WS2801(LEDS, dataPin, clockPin);
Adafruit_WS2801 strip = Adafruit_WS2801(LEDS, dataPin, clockPin, WS2801_RGB);

//1.Ghost initialisieren
float ghostOnePos;
float ghostOneSplit;
float ghostOneTime;
float ghostOneDist;
float ghostOneZiel;
float ghostOneSec;

//2.Ghost initialisieren
float ghostTwoPos;
float ghostTwoSplit;
float ghostTwoTime;
float ghostTwoDist;
float ghostTwoZiel;
float ghostTwoSec;

//3.Ghost initialisieren
float ghostThreePos;
float ghostThreeSplit;
float ghostThreeTime;
float ghostThreeDist;
float ghostThreeZiel;
float ghostThreeSec;



void setup() {
  #ifdef BGTDEBUG
  Serial.begin(115200);
  
  for (int i = 10; i > 0; i--) {
    Serial.print("Warte ");
    Serial.print(i);
    Serial.println(" sec");
    delay(1000);
  }
  Serial.println("ESP_HTML_01");
#endif

  //---------------------------------------------------------------------
  // WiFi starten

  WiFi_Start_STA();
  if (my_WiFi_Mode == 0) WiFi_Start_AP();

  strip.begin(); // Streifen initialisieren
  strip.show();  // Alle Pixel erst einmal aus


// Startparameter der Ghosts setzen

  ghostOnePos  =  0;
  ghostOneTime = 20;
  ghostOneDist=400;
  ghostOneSplit=LEDS*ghostOneDist/400/ghostOneTime/4;
  ghostOneZiel = ghostOneDist;

  ghostTwoPos  =  0;
  ghostTwoTime = 30;
  ghostTwoDist=0;
  ghostTwoSplit=LEDS*ghostTwoDist/400/ghostTwoTime/4;
  ghostTwoZiel = ghostTwoDist;

  ghostThreePos  =  0;
  ghostThreeTime = 60;
  ghostThreeDist=0;
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
  //Farben definieren für einfacheren zugriff
    const uint32_t redColor = 0xFF0000;           //  rot
    const uint32_t whiteColor = 0xFFFFFF;         //  weiß
    const uint32_t greenColor = 0x0000FF;         //  grün
    const uint32_t blueColor = 0x00FF00;          //  blau
    const uint32_t yellowColor = 0xFFFF00;        //  gelb
    uint32_t result = 0x050505;                   //Hintergrundfarbe des Strangs
    
    // Markierung aller 50m setzen
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

    //Ghost 1 Zeichnen insgesamt 5 Pixel
  if (ghostOneZiel > 0){
    if (pos ==  ghostOnePos)  result = redColor;  
    if (pos ==  ghostOnePos+1)  result = whiteColor;  
    if (pos ==  ghostOnePos+2)  result = whiteColor;  
    if (pos ==  ghostOnePos+3)  result = blueColor;  
    //if (pos ==  ghostOnePos+4)  result = blueColor;  
    
    }

      //Ghost 2 Zeichnen
  if (ghostTwoZiel > 0){
    if (pos ==  ghostTwoPos)  result = greenColor;  
    if (pos ==  ghostTwoPos+1)  result = whiteColor;  
    if (pos ==  ghostTwoPos+2)  result = whiteColor;  
    if (pos ==  ghostTwoPos+3)  result = whiteColor;  
    if (pos ==  ghostTwoPos+4)  result = redColor;  
    
  }

  //Ghost 3 Zeichnen
    if (ghostThreeZiel > 0){
    if (pos ==  ghostThreePos)  result = greenColor;  
    if (pos ==  ghostThreePos+1)  result = yellowColor;  
    if (pos ==  ghostThreePos+2)  result = yellowColor;  
    if (pos ==  ghostThreePos+3)  result = yellowColor;  
    if (pos ==  ghostThreePos+4)  result = redColor;  
    
    }
    
    return result;
}


//////////////////////////////////////////////////////////////////
//
// Methode:   displayClock()
// Zweck:     Für alle Pixel Farbe errechnen,
//            bei der LED entsprechend setzen,
//            und darstellen
//
//////////////////////////////////////////////////////////////////
void displayClock() {

    for (uint8_t pixno = 1; pixno < LEDS; pixno++) 
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

 //distanz Ghost runterrechnen und nach Runde wieder von vorn beginnen
 ghostOneSec= ghostOneSec+ghostOneSplit;
 
 ghostOnePos = round(ghostOneSec);

 ghostTwoSec= ghostTwoSec+ghostTwoSplit;
 ghostTwoPos = round(ghostTwoSec);
 
 ghostThreeSec= ghostThreeSec+ghostThreeSplit;
 ghostThreePos = round(ghostThreeSec);
 

  if (ghostOnePos > LEDS) { // am Ende des Strangs wieder bei beginnen
    ghostOneSec = 0;       // Ja => Start am letzten Pixel
    
  }
  if (ghostTwoPos > LEDS) { // am Ende des Strangs wieder bei beginnen
    ghostTwoSec = 0;       // Ja => Start am letzten Pixel
    
  }
    if (ghostThreePos > LEDS) { // am Ende des Strangs wieder bei beginnen
    ghostThreeSec = 0;        // Ja => Start am letzten Pixel
    
  }

}
//////////////////////////////////////////////////////////////////
//
// Methode:   loop()
// Zweck:     Sekunden zählen
//
//////////////////////////////////////////////////////////////////
void loop() {
WiFI_Traffic();
   // Uhr anzeigen:
   displayClock();
   // 1 Sekunde Pause:
   delay(250);
   ghostOneZiel=ghostOneZiel-(ghostOneDist/ghostOneTime/4);
   ghostTwoZiel=ghostTwoZiel-(ghostTwoDist/ghostTwoTime/4);
   ghostThreeZiel=ghostThreeZiel-(ghostThreeDist/ghostThreeTime/4);
   Serial.println(ghostOneZiel);
   
   // Neuberechnen der Tageszeit:
   incrementTimeBySecond();
}

//WLAN TEIL
//WLAN TEIL
//WLAN TEIL//WLAN TEIL
//WLAN TEIL
//WLAN TEIL//WLAN TEIL

//---------------------------------------------------------------------
void WiFi_Start_STA() {
  unsigned long timeout;

  WiFi.mode(WIFI_STA);   //  Workstation

  WiFi.begin(ssid_sta, password_sta);
  timeout = millis() + 12000L;
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    delay(10);
  }

  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    my_WiFi_Mode = WIFI_STA;

#ifdef BGTDEBUG
    Serial.print("Connected IP - Address : ");
    for (int i = 0; i < 3; i++) {
      Serial.print( WiFi.localIP()[i]);
      Serial.print(".");
    }
    Serial.println(WiFi.localIP()[3]);
#endif


  } else {
    WiFi.mode(WIFI_OFF);
#ifdef BGTDEBUG
    Serial.println("WLAN-Connection failed");
#endif

  }
}

//---------------------------------------------------------------------
void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  WiFi.softAP(ssid_ap, password_ap);
  server.begin();
  IPAddress myIP = WiFi.softAPIP();
  my_WiFi_Mode = WIFI_AP;

#ifdef BGTDEBUG
  Serial.print("Accesspoint started - Name : ");
  Serial.print(ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(myIP);
#endif
}
//---------------------------------------------------------------------
void WiFI_Traffic() {

  char my_char;
  int htmlPtr = 0;
  int myIdx;
  int myIndex;
  unsigned long my_timeout;

  // Check if a client has connected
  client = server.available();
  if (!client)  {
    return;
  }

  my_timeout = millis() + 250L;
  while (!client.available() && (millis() < my_timeout) ) delay(10);
  delay(10);
  if (millis() > my_timeout)  {
    return;
  }
  //---------------------------------------------------------------------
  htmlPtr = 0;
  my_char = 0;
  while (client.available() && my_char != '\r') {
    my_char = client.read();
    HTML_String[htmlPtr++] = my_char;
  }
  client.flush();
  HTML_String[htmlPtr] = 0;
#ifdef BGTDEBUG
  exhibit ("Request : ", HTML_String);
#endif

  Aufruf_Zaehler++;



  if (Find_Start ("/?", HTML_String) < 0 && Find_Start ("GET / HTTP", HTML_String) < 0 ) {
    send_not_found();
    return;
  }

  //---------------------------------------------------------------------
  // Benutzereingaben einlesen und verarbeiten
  //---------------------------------------------------------------------
  action = Pick_Parameter_Zahl("ACTION=", HTML_String);

  // Vor und Nachname
  if ( action == ACTION_SET_NAME) {

    myIndex = Find_End("VORNAME=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(Vorname, &HTML_String[myIndex], 20);
      
#ifdef BGTDEBUG
      exhibit ("Vorname  : ", Vorname);
#endif
    }

    myIndex = Find_End("NACHNAME=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(Nachname, &HTML_String[myIndex], 20);
#ifdef BGTDEBUG
      exhibit ("Nachname  : ", Nachname);
#endif

  
    }
    
    myIndex = Find_End("DISTANZ=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(Distanz, &HTML_String[myIndex], 20);
#ifdef BGTDEBUG
      exhibit ("Distanz  : ", Distanz);
#endif
  Serial.println(Vorname);
    Serial.println(Nachname);
    Serial.println(Distanz);
    ghostOnePos=atof(Vorname);
      Serial.println(ghostOnePos);
    ghostOnePos = LEDS*ghostOnePos/400;
  Serial.println(ghostOnePos);
  
    
    ghostOneTime=atof(Nachname);

  ghostOneSec = ghostOnePos;
  ghostOneDist=atof(Distanz);
  ghostOneSplit=LEDS*ghostOneDist/400/ghostOneTime/4;
   ghostOneZiel = ghostOneDist;
    }
  }

  
  // Uhrzeit und Datum
  if ( action == ACTION_SET_DATE_TIME) {
    // UHRZEIT=12%3A35%3A25
    myIndex = Find_End("UHRZEIT=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(tmp_string, &HTML_String[myIndex], 8);
      Uhrzeit_HH = Pick_N_Zahl(tmp_string, ':', 1);
      Uhrzeit_MM = Pick_N_Zahl(tmp_string, ':', 2);
      Uhrzeit_SS = Pick_N_Zahl(tmp_string, ':', 3);
#ifdef BGTDEBUG
      Serial.print("Neue Uhrzeit ");
      Serial.print(Uhrzeit_HH);
      Serial.print(":");
      Serial.print(Uhrzeit_MM);
      Serial.print(":");
      Serial.println(Uhrzeit_SS);
#endif
    }
    // DATUM=2015-12-31
    myIndex = Find_End("DATUM=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(tmp_string, &HTML_String[myIndex], 10);
      Datum_JJJJ = Pick_N_Zahl(tmp_string, '-', 1);
      Datum_MM = Pick_N_Zahl(tmp_string, '-', 2);
      Datum_TT = Pick_N_Zahl(tmp_string, '-', 3);
#ifdef BGTDEBUG
      Serial.print("Neues Datum ");
      Serial.print(Datum_TT);
      Serial.print(".");
      Serial.print(Datum_MM);
      Serial.print(".");
      Serial.println(Datum_JJJJ);
#endif
    }
  }


  if ( action == ACTION_LIES_AUSWAHL) {
    Wochentage = 0;
    for (int i = 0; i < 7; i++) {
      strcpy( tmp_string, "WOCHENTAG");
      strcati( tmp_string, i);
      strcat( tmp_string, "=");
      if (Pick_Parameter_Zahl(tmp_string, HTML_String) == 1)Wochentage |= 1 << i;
    }
    Jahreszeit = Pick_Parameter_Zahl("JAHRESZEIT=", HTML_String);
    Wetter = Pick_Parameter_Zahl("WETTER=", HTML_String);

  }

  if ( action == ACTION_LIES_VOLUME) {
    Volume = Pick_Parameter_Zahl("VOLUME=", HTML_String);
  }


  //---------------------------------------------------------------------
  //Antwortseite aufbauen

  make_HTML01();

  //---------------------------------------------------------------------
  // Header aufbauen
  strcpy(HTTP_Header , "HTTP/1.1 200 OK\r\n");
  strcat(HTTP_Header, "Content-Length: ");
  strcati(HTTP_Header, strlen(HTML_String));
  strcat(HTTP_Header, "\r\n");
  strcat(HTTP_Header, "Content-Type: text/html\r\n");
  strcat(HTTP_Header, "Connection: close\r\n");
  strcat(HTTP_Header, "\r\n");

#ifdef BGTDEBUG
  exhibit("Header : ", HTTP_Header);
  exhibit("Laenge Header : ", strlen(HTTP_Header));
  exhibit("Laenge HTML   : ", strlen(HTML_String));
#endif

  client.print(HTTP_Header);
  delay(20);

  send_HTML();

}

//---------------------------------------------------------------------
// HTML Seite 01 aufbauen
//---------------------------------------------------------------------
void make_HTML01() {

  strcpy( HTML_String, "<!DOCTYPE html>");
  strcat( HTML_String, "<html>");
  strcat( HTML_String, "<head>");
  strcat( HTML_String, "<title>TUS Bothfeld</title>");
  strcat( HTML_String, "</head>");
  strcat( HTML_String, "<body bgcolor=\"#adcede\">");
  strcat( HTML_String, "<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">");
  strcat( HTML_String, "<h1>TUS Bothfeld runaway light</h1>");


  //-----------------------------------------------------------------------------------------
  // Textfelder Vor- und Nachname
  strcat( HTML_String, "<h2>Ghost 1</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Startposition:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"VORNAME\" maxlength=\"20\" Value =\"");
  strcat( HTML_String, Vorname);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_NAME);
  strcat( HTML_String, "\">&Uuml;bernehmen</button></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Zeit:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"NACHNAME\" maxlength=\"20\" Value =\"");
  strcat( HTML_String, Nachname);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Distanz:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"DISTANZ\" maxlength=\"20\" Value =\"");
  strcat( HTML_String, Distanz);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br>");


/*
  //-----------------------------------------------------------------------------------------
  // Uhrzeit + Datum
  strcat( HTML_String, "<h2>Uhrzeit und Datum</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Uhrzeit</b></td>");
  strcat( HTML_String, "<td><input type=\"time\"   style= \"width:100px\" name=\"UHRZEIT\" value=\"");
  strcati2( HTML_String, Uhrzeit_HH);
  strcat( HTML_String, ":");
  strcati2( HTML_String, Uhrzeit_MM);
  strcat( HTML_String, ":");
  strcati2( HTML_String, Uhrzeit_SS);

  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_DATE_TIME);
  strcat( HTML_String, "\">&Uuml;bernehmen</button></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Datum</b></td>");
  strcat( HTML_String, "<td><input type=\"date\"  style= \"width:100px\" name=\"DATUM\" value=\"");
  strcati( HTML_String, Datum_JJJJ);
  strcat( HTML_String, "-");
  strcati2( HTML_String, Datum_MM);
  strcat( HTML_String, "-");
  strcati2( HTML_String, Datum_TT);
  strcat( HTML_String, "\"></td></tr>");

  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br>");

  //-----------------------------------------------------------------------------------------
  // Checkboxen
  strcat( HTML_String, "<h2>Checkbox, Radiobutton und Combobox</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Wochentage</b></td>");

  strcat( HTML_String, "<td>");
  for (int i = 0; i < 7; i++) {
    if (i == 5)strcat( HTML_String, "<br>");
    strcat( HTML_String, "<input type=\"checkbox\" name=\"WOCHENTAG");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" id = \"WT");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value = \"1\" ");
    if (Wochentage & 1 << i) strcat( HTML_String, "checked ");
    strcat( HTML_String, "> ");
    strcat( HTML_String, "<label for =\"WT");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, Wochentage_tab[i]);
    strcat( HTML_String, "</label>");
  }
  strcat( HTML_String, "</td>");

  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_LIES_AUSWAHL);
  strcat( HTML_String, "\">&Uuml;bernehmen</button></td>");
  strcat( HTML_String, "</tr>");

  //-----------------------------------------------------------------------------------------
  // Radiobuttons

  for (int i = 0; i < 4; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0)  strcat( HTML_String, "<td><b>Jahreszeit</b></td>");
    else strcat( HTML_String, "<td> </td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"JAHRESZEIT\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if (Jahreszeit == i)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, Jahreszeiten_tab[i]);
    strcat( HTML_String, "</label></td></tr>");
  }

  //-----------------------------------------------------------------------------------------
  // Combobox
  strcat( HTML_String, "<tr><td><b>Wetter</b></td>");

  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<select name = \"WETTER\" style= \"width:160px\">");
  for (int i = 0; i < 4; i++) {
    strcat( HTML_String, "<option ");
    if (Wetter == i)strcat( HTML_String, "selected ");
    strcat( HTML_String, "value = \"");
    strcati( HTML_String, i);
    strcat(HTML_String, "\">");
    strcat(HTML_String, Wetter_tab[i]);
    strcat(HTML_String, "</option>");
  }
  strcat( HTML_String, "</select>");
  strcat( HTML_String, "</td></tr>");

  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br>");

  //-----------------------------------------------------------------------------------------
  // Slider
  strcat( HTML_String, "<h2>Slider</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr><td><b>Lautst&auml;rke</b></td>");

  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"range\" name=\"VOLUME\" min=\"0\" max=\"30\" value = \"");
  strcati(HTML_String, Volume);
  strcat( HTML_String, "\">");
  strcat( HTML_String, "</td>");

  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_LIES_VOLUME);
  strcat( HTML_String, "\">&Uuml;bernehmen</button></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<BR>");

  strcat( HTML_String, "<FONT SIZE=-1>");
  strcat( HTML_String, "Aufrufz&auml;hler : ");
  strcati(HTML_String, Aufruf_Zaehler);
  strcat( HTML_String, "</font>");
  strcat( HTML_String, "</font>");
  strcat( HTML_String, "</body>");
  strcat( HTML_String, "</html>");
*/

}


//--------------------------------------------------------------------------
void send_not_found() {
#ifdef BGTDEBUG
  Serial.println("Sende Not Found");
#endif
  client.print("HTTP/1.1 404 Not Found\r\n\r\n");
  delay(20);
  client.stop();
}

//--------------------------------------------------------------------------
void send_HTML() {
  char my_char;
  int  my_len = strlen(HTML_String);
  int  my_ptr = 0;
  int  my_send = 0;

  //--------------------------------------------------------------------------
  // in Portionen senden
  while ((my_len - my_send) > 0) {
    my_send = my_ptr + MAX_PACKAGE_SIZE;
    if (my_send > my_len) {
      client.print(&HTML_String[my_ptr]);
      delay(20);
#ifdef BGTDEBUG
      Serial.println(&HTML_String[my_ptr]);
#endif
      my_send = my_len;
    } else {
      my_char = HTML_String[my_send];
      // Auf Anfang eines Tags positionieren
      while ( my_char != '<') my_char = HTML_String[--my_send];
      HTML_String[my_send] = 0;
      client.print(&HTML_String[my_ptr]);
      delay(20);
#ifdef BGTDEBUG
      Serial.println(&HTML_String[my_ptr]);
#endif
      HTML_String[my_send] =  my_char;
      my_ptr = my_send;
    }
  }
  client.stop();
}

//----------------------------------------------------------------------------------------------
void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
  strcat( HTML_String, "<colgroup>");
  set_colgroup1(w1);
  set_colgroup1(w2);
  set_colgroup1(w3);
  set_colgroup1(w4);
  set_colgroup1(w5);
  strcat( HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
  if (ww == 0) return;
  strcat( HTML_String, "<col width=\"");
  strcati( HTML_String, ww);
  strcat( HTML_String, "\">");
}


//---------------------------------------------------------------------
void strcati(char* tx, int i) {
  char tmp[8];

  itoa(i, tmp, 10);
  strcat (tx, tmp);
}

//---------------------------------------------------------------------
void strcati2(char* tx, int i) {
  char tmp[8];

  itoa(i, tmp, 10);
  if (strlen(tmp) < 2) strcat (tx, "0");
  strcat (tx, tmp);
}

//---------------------------------------------------------------------
int Pick_Parameter_Zahl(const char * par, char * str) {
  int myIdx = Find_End(par, str);

  if (myIdx >= 0) return  Pick_Dec(str, myIdx);
  else return -1;
}
//---------------------------------------------------------------------
int Find_End(const char * such, const char * str) {
  int tmp = Find_Start(such, str);
  if (tmp >= 0)tmp += strlen(such);
  return tmp;
}

//---------------------------------------------------------------------
int Find_Start(const char * such, const char * str) {
  int tmp = -1;
  int ww = strlen(str) - strlen(such);
  int ll = strlen(such);

  for (int i = 0; i <= ww && tmp == -1; i++) {
    if (strncmp(such, &str[i], ll) == 0) tmp = i;
  }
  return tmp;
}
//---------------------------------------------------------------------
int Pick_Dec(const char * tx, int idx ) {
  int tmp = 0;

  for (int p = idx; p < idx + 5 && (tx[p] >= '0' && tx[p] <= '9') ; p++) {
    tmp = 10 * tmp + tx[p] - '0';
  }
  return tmp;
}
//----------------------------------------------------------------------------
int Pick_N_Zahl(const char * tx, char separator, byte n) {

  int ll = strlen(tx);
  int tmp = -1;
  byte anz = 1;
  byte i = 0;
  while (i < ll && anz < n) {
    if (tx[i] == separator)anz++;
    i++;
  }
  if (i < ll) return Pick_Dec(tx, i);
  else return -1;
}

//---------------------------------------------------------------------
int Pick_Hex(const char * tx, int idx ) {
  int tmp = 0;

  for (int p = idx; p < idx + 5 && ( (tx[p] >= '0' && tx[p] <= '9') || (tx[p] >= 'A' && tx[p] <= 'F')) ; p++) {
    if (tx[p] <= '9')tmp = 16 * tmp + tx[p] - '0';
    else tmp = 16 * tmp + tx[p] - 55;
  }

  return tmp;
}

//---------------------------------------------------------------------
void Pick_Text(char * tx_ziel, char  * tx_quelle, int max_ziel) {

  int p_ziel = 0;
  int p_quelle = 0;
  int len_quelle = strlen(tx_quelle);

  while (p_ziel < max_ziel && p_quelle < len_quelle && tx_quelle[p_quelle] && tx_quelle[p_quelle] != ' ' && tx_quelle[p_quelle] !=  '&') {
    if (tx_quelle[p_quelle] == '%') {
      tx_ziel[p_ziel] = (HexChar_to_NumChar( tx_quelle[p_quelle + 1]) << 4) + HexChar_to_NumChar(tx_quelle[p_quelle + 2]);
      p_quelle += 2;
    } else if (tx_quelle[p_quelle] == '+') {
      tx_ziel[p_ziel] = ' ';
    }
    else {
      tx_ziel[p_ziel] = tx_quelle[p_quelle];
    }
    p_ziel++;
    p_quelle++;
  }

  tx_ziel[p_ziel] = 0;
}
//---------------------------------------------------------------------
char HexChar_to_NumChar( char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 55;
  return 0;
}

#ifdef BGTDEBUG
//---------------------------------------------------------------------
void exhibit(const char * tx, int v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned int v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned long v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, const char * v) {
  Serial.print(tx);
  Serial.println(v);
}
#endif
