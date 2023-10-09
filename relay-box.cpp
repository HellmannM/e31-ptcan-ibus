// Arduino MEGA

//neu keine Secondary_Warnings mit neuer Getriebeanzeigen
//pin analog7 ist Data in vom Klimaschalter Stecker pin9
//pin analog0 ist A/C off relais schaltung auf MASSE Stecker pin8


//pinout STECKER
//1 E   - rt/ge - Dauerpuls VERBR
//2 E   - br    - MASSE
//3 E   - kl15  - IGN
//4 E/A - gr/ge - CAN H
//5 E/A - gr/bl - CAN L
//6 A   - br    - Relais 12 ->            pin 8  OIL LOW Warnung           schaltet auf Masse
//7 A   - gr    - Relais 2 (ehemals L2)   pin A1 GONG PIN                   schaltet auf Masse
//8 A   - ws/rt - Relais 1 (ehemals L1)   Pin A0 A/C Klima OFF Ansteuerung schaltet auf Masse
//9 E   - bl/vi - KLIMA_TASTE_EIN 5V Signal Eingang (Digi -Bus) Pin A7     Eingang mit 1kOhm gesichert
//
//10 A  - br/sw - Relais 5 (ehemals M)    pin A4   CEL_pin gelbe Leuchte "check engine"  schaltet auf MASSE (?)
//11 A  - br/rt - Relais 6 (ehemals S)    pin 3    Brake_pin Schaltet Bremse ein MASSE geschaltet.
//12 A  - gn/bl - Relais 16 (CEL)         pin 7    EML_pin  schaltet rote Leuchte "EML" auf MASSE (?)
//13 A  - bl    - Relais 15 (GEN)         pin 6    BAT_pin  schaltet auf MASSE
//14 A  - br/or - Relais 14 (ABS)         pin 4    ABS_pin  schaltet auf MASSE
//15 A  - sw/ws - Relais 13 (DSC)         pin 5    DSC_pin  schaltet auf MASSE
//16 A  - vi    - EGS DATA pin Ganganzeige separates Modul
//17 A  - rt/sw - Relais 11 (ehemals TEMPO) pin 9 ACC Anschnallzeichen
//18 A  - vi/ws - Ein/AusSChalter für Dauerplus an Arduino Board- Massegeschaltet.

#include <stubs.h>

#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

float accel_pos;
float klima_raw = 0;
unsigned int iii = 1;
//Für Ganganzeige
char gear_digit[2];
unsigned long timer_serial1 = 0;
unsigned int gear_data;
bool IGN = false;
bool _init_ = true;
unsigned long ACC_brake_timer = 0;
bool Brake_lights_on = false;
bool Fussbremse_getreten = false;

int init_time = 500; //initialisierungszeit 10ms brauche ich nicht mehr, kann irgendwann rausgenommen werden, dann bei initialisierung alles auf HIGH setzen.
const int IGN_pin = A5 ; //S_Line muss an pin D3 Digital 3, und IGN an Pin 19 (Analog5) kommt IGN
bool RPM_init=true;

//Motorparams
bool Error_on_off = false;
uint16_t      Error_ID = 0;

const int EML_pin = 7;  // Pin 7
bool    EML = false;     // Angeschlossen

const int CEL_pin = A4;    //ehemals M_Line //Pin A4
bool    CEL = false;     // Angeschlossen
bool CEL_init = false;
unsigned int RPM = 0;
unsigned long timer_CEL_init;

const int BAT_pin = 6;
bool    BAT = false;     // angeschlossen
const int DSC_pin = 5;
bool    DSC = false;     // angeschlossen

const int ABS_pin = 4;
bool    ABS = false;     // angeschlossen

bool    RPA = false;     // RPA

const int OIL_pin = 8;  //Ölsensor //ehemals GET_pin //am stecker L1 pin 6
bool    OIL = false;

const int BRAKEWARN_pin = 9;  //ACC wird als E angezeigt im Gangmenu BRAKEWARN ist nun Anschnaller
bool    BRAKEWARN = false;

const int GONG_pin = A1;  //ACC wird als E angezeigt im Gangmenu BRAKEWARN ist nun Anschnaller
bool    GONG = false;
unsigned long gongtimer = 0;


bool    ACC_on = false;
bool    ACC_leading_veh = false;
//ACC wird als E angezeigt im Gangmenu

bool    GET = false;
//GET wird als " X" gesendet

const int ACC_brake_pin = 3; //ehemals S_Line
bool ACC_brake = false;

bool ID_Error = false;

bool    KLIMA_off = false;
const int KLIMA_off_pin = A0; //A0


bool    KLIMA_TASTE_EIN = false;
const int KLIMA_TASTE_EIN_pin = A7; //A7




unsigned long rxId;
unsigned char len = 0;
byte rxBuf[8];
char msgString[128];                        // Array to store serial string
bool errorlist[400] = {0};


const int CEL_liste[] = {25, 31, 34, 282, 283};

const int EML_liste[] = {27, 28, 29, 30, 33, 39, 49, 28, 148, 182, 212, 257}; // OILLEVEL
//DDE 29  1   GELB  Problem mit Motor, Leistung reduziert
//DDE 30  1   ROT Sofort anhalten.MOTOR STOP. Motor defekt
//DDE 31  1 MIL GELB  Abgasproblem
//DDE 32  0   GELB  Tankdeckel ist offen
//DDE 33  1   GELB  Problem mit abgastemperatur
//DDE 34  1 MIL - Allgemeine MIL Lampe
//DDE 39  1   ROT Übertemperatur
//DDE 49  1   GELB  Partikelfilter defekt
//DDE 148 1   GELB  Bremslichtcheck defekt
//DDE 212 1   ROT Öldruck zu niedrig

//DDE 257 1   GELB  Motor zu heiß (steht aber unter Batterie.. sehr komisch..


const int OIL_liste[] = {27, 28, 182};
//DDE 27  1   GELB  Öl ist auf minimum
//DDE 28  1   GELB  Öl ist UNTER minimum
//DDE 182 1   GELB  Öllevelsensor defekt

const int GET_liste[] = {105, 171, 172, 248, 254, 287, 288, 289, 290, 292, 307, 322, 323, 325, 268};
//EGS  105 1   ROT Getriebe überhitzt
//EGS 171 1   GELB  Getriebe fehler
//EGS 172 1   ROT Getriebe fehler
//EGS 178 0   GELB  Getriebe in N. Wagen kann wegrollen
//EGS 248 1   GELB  Problem zwischen ABS und EGS
//EGS 254 1   ROT Getriebe fehler
//EGS 287 1   GELB  Getriebe überhitzt
//EGS 288 1   GELB  Getriebe fehler
//EGS 289 1   ROT Getriebe fehler
//EGS 290 1   ROT Getriebe fehler
//EGS 291 1   GELB  Getriebe fehler
//EGS 292 1   ROT Getriebe schaltet in N
//EGS 293 0 keine LED GELB  N oder P einlegen und Bremse treten für Motorstart
//EGS 302 0   GELB  Getriebe nicht in P. Wagen kann wegrollen
//EGS 307 1   ROT Getriebe fehler
//EGS 322 1   ROT Getriebe lernt
//EGS 323 1   ROT Getriebe lernt
//EGS 325 0   GELB  Getriebe in N
//EGS 368 1   GELB  Getriebe fehler


const int RPA_liste[] = {50, 63, 336}; //RPA
//DSC 50  1   RPA FEHLER
//DSC 63  1   ROT RPA Reifen hat Loch
//DSC 336 1   RPA   RPA fehler


const int BRAKEWARN_liste[] = { 1, 2, 3, 26, 59, 69, 85, 176, 337, 338, 339, 341, 342, 282, 283}; //TEMP
//abstand 282 Auto GELB
//abstand 283 Auto ROT


//DDE 26  1   GELB  Cruise_Control_Error

//LDM 1 1   GELB  ACC nicht verfügbar, weil glatt
//LDM 2 1   GELB  ACC nicht verfügbar, Sensor blockiert
//LDM 3 1   GELB  ACC fehler
//LDM 59  1   GELB  ACC nicht verfügbar, Handbremse angezogen
//LDM 69  1   GELB  nicht verfügbar unter 30kmh
//LDM 85  1   GELB  ACC zu starkes bremsen
//LDM 176 1   GELB  ACC nicht verfügbar, Sensor blockiert
//LDM 276 0   GELB  Hochschalten
//LDM 277 0   GELB  Hochschalten
//LDM 277 0   GELB  Falscher Gang
//LDM 278 0   GELB  Runterschalten
//LDM 337 1   GELB  Tempomat fehler
//LDM 338 1   GELB  Tempomat deaktiv, Handbremse  //hier steht zwei mal 338
//LDM 339 1   GELB  DCC zu starkes bremsen
//LDM 341 1   GELB  Falscher Gang
//LDM 342 1   GELB  nicht verfügbar unter 30kmh



const int ABS_liste[] = {71, 74, 236, 42, 36, 353, 236};

//DSC 71  Hilldecent control deaktiv_nicht angeschlossen, aber wenn Fehler, dann funktionier t vieles nicht
//ABS ROT Bremsbeläge verschlissen
//DSC 74  nicht angeschlossen, aber wenn Fehler, dann funktionier t vieles nicht  ABS ROT Bremsflüssigkeitsstand niedrig
//DSC 236 1 ABS GELB  ABS fehler
//DSC 353 1 ABS GELB  Bremsen überhitzt
//DSC 42  1 ABS GELB  EBV/CBC Fehler?
//DSC 184 1 DSC GELB  DTC aktiv, DSC reduziert
//DSC 36  1 DSC GELB  DSC deaktiviert

const int DSC_liste[] = {24, 35, 36 , 184, 215, 237, 353, 334, 354, 283};
//283 ist ROT Auto Warnung
//DSC  24  1 ABS GELB  DBC defekt??
//DSC 35  1 DSC GELB  DSC defekt
//DSC 36  1 DSC GELB  DSC deaktiviert
//DSC 184 1 DSC GELB  DTC aktiv, DSC reduziert
//DSC 215 1 DSC - Allgemeiner DTC Meldung
//DSC 237 1 DSC GELB  DSC fehler
//DSC 330 0   GELB  HDC HilldescendControl not available
//DSC 331 0   GELB  HDC aktiv
//DSC 332 0   GELB  HDC deaktiv
//DSC 333 0   GELB  HDC icht  verfügbar
//DSC 334 1   Orange  Standarddize RPA ??????????????????????? Fehler nummer73
//DSC 352 0   GELB  Bremsen zu heiß
//DSC 354 1 ABS GELB  Anfahrhilfe deaktiviert

//const int VORG_liste[] = {25};
//25 vorglühen
//DDE 25  1   GELB  Vorglühen   // hier muss eine andere Lampe nehmen!


const int BAT_liste[] = {213, 220, 229, 247, 304, 305, 306};
//DDE 213 1   ROT Batterie nicht geladen
//DDE 220 1   GELB  Batterie Entladung zu hoch
//DDE 229 1   GELB  Batterespannung niedrig
//DDE 247 1   GELB  Batteriemonitoring nicht möglich
//DDE 304 0   GELB  Batterie check, zu alt
//DDE 305 1   GELB  Batterieanschlüsse locker
//DDE 306 1   GELB  Batterie entladen

// ZUSÄTZLICH


//05A9  8 40  EC  0 15  FF  FF  FF  FF  --> 236 ABS ERROR
//05A9  8 40  32  0 21  FF  FF  FF  FF  --> 50 Tire failure warning
//05A9  8 40  18  0 19  FF  FF  FF  FF  --> 24 DSC Error
//05A9  8 40  62  1 21  FF  FF  FF  FF  --> 98



bool is_true(const int input_array[], int laenge) {
  bool error_found = false;

  //Errorliste hat maximal 399 einträge...
  if (laenge > 398) {
    laenge = 398;
  }


  for (int ii = 0; ii < laenge; ii++ ) {


    if (errorlist[input_array[ii]] == true) {
      error_found = true;
      break;
    }

  }

  return error_found;
}


void lookfornewerrors() {


  if (is_true(EML_liste, sizeof(EML_liste) / sizeof(EML_liste[0]))) { //34 MIL; 212 OILPRESSURE LOW
    EML = true;
  } else {
    EML = false;
  }

  if (is_true(CEL_liste, sizeof(CEL_liste) / sizeof(CEL_liste[0]))) { //CEL
    CEL = true;
  } else {
    CEL = false;
  }


  if (is_true(OIL_liste, sizeof(OIL_liste) / sizeof(OIL_liste[0]))) {
    OIL = true;
  } else {
    OIL = false;
  }


  if (is_true(BAT_liste, sizeof(BAT_liste) / sizeof(BAT_liste[0]))) {
    BAT = true;
  } else {
    BAT = false;
  }

  if (is_true(DSC_liste, sizeof(DSC_liste) / sizeof(DSC_liste[0]))) {
    DSC = true;
  } else {
    DSC = false;
  }

  if (is_true(ABS_liste, sizeof(ABS_liste) / sizeof(ABS_liste[0]))) {
    ABS = true;
  } else {
    ABS = false;
  }

  if (is_true(BRAKEWARN_liste, sizeof(BRAKEWARN_liste) / sizeof(BRAKEWARN_liste[0]))) {
    BRAKEWARN = true;
    GONG = true;
  } else {
    BRAKEWARN = false;
    GONG = false;
  }

  if (is_true(RPA_liste, sizeof(RPA_liste) / sizeof(RPA_liste[0]))) {
    RPA = true;
  } else {
    RPA = false;
  }
  if (is_true(GET_liste, sizeof(GET_liste) / sizeof(GET_liste[0]))) {
    GET = true;
  } else {
    GET = false;
  }

}


void setup()
{
  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {}
  CAN0.setMode(MCP_LISTENONLY);   // Change to normal mode to allow messages to be transmitted

  Serial.begin(115200);
  Serial3.begin(9600);
  Serial.println("Reboot - CAN OK");

  pinMode(CEL_pin, OUTPUT);
  digitalWrite(CEL_pin, LOW);
  pinMode(EML_pin, OUTPUT);
  digitalWrite(EML_pin, LOW);
  pinMode(BAT_pin, OUTPUT);
  digitalWrite(BAT_pin, LOW);
  pinMode(DSC_pin, OUTPUT);
  digitalWrite(DSC_pin, LOW);
  pinMode(ABS_pin, OUTPUT);
  digitalWrite(ABS_pin, LOW);
  pinMode(OIL_pin, OUTPUT);
  digitalWrite(OIL_pin, LOW);
  pinMode(BRAKEWARN_pin, OUTPUT);
  digitalWrite(BRAKEWARN_pin, HIGH);  //Invertiert
  pinMode(ACC_brake_pin, OUTPUT);
  digitalWrite(ACC_brake_pin, LOW);
  pinMode(GONG_pin, OUTPUT);
  digitalWrite(GONG_pin, LOW);
  pinMode(KLIMA_off_pin, OUTPUT);
  digitalWrite(KLIMA_off_pin, LOW);


  CEL_init = true;
}

void loop() {

  if (analogRead(IGN_pin) > 400) {  //512 ??
    IGN = true;
    if (CEL_init && ((millis() - timer_CEL_init) > 10000)) {
      CEL = false;
      errorlist[34] = 0;
      CEL_init = false;
      Serial.print("EML errorlist[34] RESET");
    }
  } else {
    IGN = false;
    CEL_init = true;
    RPM_init = true;
    memset(errorlist, 0, sizeof(errorlist));
  }





  klima_raw = (klima_raw * (iii - 1) + analogRead(KLIMA_TASTE_EIN_pin)) / iii;
  if (iii > 100) {
    iii = 1;
  };
  iii = iii + 1;

  if (klima_raw > 50) {
    if (accel_pos > 70) {
      KLIMA_off = true;
    } else {
      KLIMA_off = false;
    }
  } else {
    KLIMA_off = true;
  }
  if (0) {
    Serial.print(KLIMA_off);
    Serial.print("  ");
    Serial.print(klima_raw);
    Serial.print(" ");
    Serial.println(analogRead(KLIMA_TASTE_EIN_pin));
    ;
  }
  if (!Fussbremse_getreten && ACC_brake && ((millis() - ACC_brake_timer) > 750)) {  //Wenn CAN message Bremse AN und diese länger als 750ms an ist, dann setzte Bremslichter ein
    Brake_lights_on = true;
  } else {
    Brake_lights_on = false;
  }


  // RECEIVE DATA /////////////////////////////////////////////////////////////////////******************************
  //read new can message
  if (!digitalRead(CAN0_INT)) {                       // If CAN0_INT pin is low, read receive buffer

    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)


    if (rxId == 0xAA) {
      accel_pos = float((uint16_t(rxBuf[3]) << 8) + rxBuf[2]) / 655.35;
      RPM = (uint16_t(rxBuf[5] << 8) + rxBuf[4]) / 4;
      if (RPM_init && (RPM > 400)) {
        Serial.println();
        Serial.println("engine-Start");
        Serial.println();
        Serial.println(CEL_init);
        Serial.println();
        timer_CEL_init = millis();
        RPM_init=false;
      }
    }

    if (rxId == 0xA8) { //Status Bremse
      if (rxBuf[7] < 20) {
        Fussbremse_getreten = false;
      } else {
        Fussbremse_getreten = true;
      }

    }




    // Anzeige Führungsfahrzeug als 'E'
    if (rxId == 0x193) {

      if ((rxBuf[5] == 0x50) || (rxBuf[5] == 0x08)) {
        ACC_on = false;
      } else {
        ACC_on = true;
      }


      if (rxBuf[3] == 0x11) {
        ACC_leading_veh = true;
      } else {
        ACC_leading_veh = false;
      };
    }


    // Bremslichterleuchten bei ACC Bremsanforderung
    if (rxId == 0xD5) {
      if (rxBuf[5] == 0xF1) {
        ACC_brake = true;
      }
      else {
        ACC_brake = false;
        ACC_brake_timer = millis();
      };
    }


    // ###############  GANG ANZEIGE S M ' ' PRND und 123456  #################
    if (rxId == 0x1D2) { //Gearbox 0xBA=186  //R=B4 C FF ;  N=D2 C FF; P=E1 C FF; D=78 7C FF; F0 F FF (zwischen zwei Gängen)
      if (0) {
        Serial.print(rxBuf[0], HEX);
        Serial.print(" ");
        Serial.print(rxBuf[1], HEX);
        Serial.print(" ");
        Serial.print(rxBuf[2], HEX);
        Serial.print(" ");
        Serial.print(rxBuf[3], HEX);
        Serial.print(" ");
        Serial.println(rxBuf[4], HEX);
      }

      gear_digit[0] = ' ';
      if (rxBuf[0] == 0x78) { // D ist eingelegt
        if (rxBuf[4] == 241) {    //S
          gear_digit[0] = 'S';
        };
        if (rxBuf[4] == 242) { //M
          gear_digit[0] = 'M';
        };


        gear_data = rxBuf[1]; // Gangzuordnung nach Tabelle unten
        gear_digit[1] = ' ';
        if (gear_data == 0x5C)gear_digit[1] = '1';
        if (gear_data == 0x6C)gear_digit[1] = '2';
        if (gear_data == 0x7C)gear_digit[1] = '3';
        if (gear_data == 0x8C)gear_digit[1] = '4';
        if (gear_data == 0x9C)gear_digit[1] = '5';
        if (gear_data == 0xAC)gear_digit[1] = 'D';

      } else if (rxBuf[0] == 0xE1) { //P
        gear_digit[1] = 'P';
      } else if (rxBuf[0] == 0xB4) { //R
        gear_digit[1] = 'N';
      } else if (rxBuf[0] == 0xD2) { //N
        gear_digit[1] = 'R';
      } else if (rxBuf[0] == 0xF0) { //Zwischen den Gängen
        gear_digit[1] = ' ';
      }




      if (ACC_on) {                      // Hier ACC-an
        gear_digit[1] = ' ';
      }
      if (ACC_leading_veh) {                      // Hier ACC-Fehler als E
        gear_digit[0] = 'E';
      }
      if (GET) {
        gear_digit[1] = 'X';
      };
    }


    // ###############  Errorlights  #################
    //0x592  DME ERRORLIGHTS =1426
    //0x5A9  DSC ERRORLIGHT
    //0x598  EGS
    //0x59C  LDM
    //neu
    //0x5A1  ACC
    //0x597 EKP
    //0x5A0 RDC
    //0x5E0 KOMBI

    if ((rxId >= 0x580 ) && (rxId <= 0x671)) {


      //0x40 sind Fehlermeldungen die im KMB gezeigt werden
      if (rxBuf[0] == 0x40) {
        //Error ist byte1 +erstes bit von byte2
        Error_ID = rxBuf[1] + uint16_t(bitRead(rxBuf[2], 0) << 8);
        //Fehler an oder aus steht in Byte3
        Error_on_off = (bitRead(rxBuf[3], 0));

        if (Error_ID > 0 && Error_ID < 399) {
          errorlist[Error_ID] = Error_on_off;
        }

        lookfornewerrors();

        if (1) {
          Serial.print(" Error Warning ID: ");
          Serial.print(Error_ID, DEC);
          Serial.print(" DEC, rxBuf[3] ");
          Serial.print(rxBuf[3], HEX);
          Serial.print(" HEX  bool: ");
          Serial.print(Error_on_off);
          Serial.println();
        }
        if (0) {
          Serial.print("Errorlist: ");
          for (int kk = 0; kk < sizeof(errorlist) / sizeof(errorlist[0]); kk++) {
            if (errorlist[kk] == 1) {
              Serial.print(kk);
              Serial.print(", ");
            }
          }
          Serial.println();
        }


      }
    } //ende der CAN Botschaften, die Errormessages enthalten.

    // ################## CAN Nachrichten abfragen ENDE  ##################



    if (0) {
      //Serial.println(rxBuf[1], HEX);
      Serial.print("--> Gang123456:");
      Serial.print(gear_digit[0]);
      Serial.println(gear_digit[1]);
    }




    if (0) {
      sprintf(msgString, "0x%.3lX, %1d,  Data:", rxId, len);
      Serial.print(msgString);
      for (byte i = 0; i < len; i++) {
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
      Serial.println();
    }
  }

  if (millis() - timer_serial1 > 110) {
    Serial3.print(gear_digit[0]);
    //Serial.print("Gear:");
    //Serial.print(gear_digit[0]);

    Serial3.println(gear_digit[1]);
    //Serial3.println("P");
    //Serial.println(gear_digit[1]);
    timer_serial1 = millis();
  }



  // ################## CAN          ENDE  ##################




  // ################# Hier sind wir noch in der LOOP aber nach dem CAN Input ##################
  if (0) {
    Serial.print(IGN);
    Serial.print(CEL);
    Serial.print(EML);
    Serial.print(BAT);
    Serial.print(DSC);
    Serial.print(OIL);
    Serial.print(BRAKEWARN);
    Serial.print(Brake_lights_on);
    Serial.print(RPA);
    Serial.print(GONG);
    Serial.print(KLIMA_TASTE_EIN);
    Serial.println(KLIMA_off);
  }

  if (_init_ && (millis() > init_time)) {
    digitalWrite(CEL_pin, HIGH);
    digitalWrite(EML_pin, HIGH);
    digitalWrite(BAT_pin, HIGH);
    digitalWrite(DSC_pin, HIGH);
    digitalWrite(ABS_pin, HIGH);
    digitalWrite(OIL_pin, HIGH);
    digitalWrite(BRAKEWARN_pin, LOW); //Invertiert
    digitalWrite(ACC_brake_pin, HIGH);
    digitalWrite(GONG_pin, HIGH);
    digitalWrite(KLIMA_off_pin, HIGH);
    _init_ = false;
  }

  if (!_init_) {

    if (EML || Brake_lights_on || RPA) {
      digitalWrite(EML_pin, LOW);
    } else {
      digitalWrite(EML_pin, HIGH);
    }

    if (CEL) {
      digitalWrite(CEL_pin, LOW);
    } else {
      digitalWrite(CEL_pin, HIGH);
    }


    if (BAT) {
      digitalWrite(BAT_pin, LOW);
    } else {
      digitalWrite(BAT_pin, HIGH);
    };

    if (DSC) {
      digitalWrite(DSC_pin, LOW);
    } else {
      digitalWrite(DSC_pin, HIGH);
    };


    if (ABS) {
      digitalWrite(ABS_pin, LOW);
    } else {
      digitalWrite(ABS_pin, HIGH);
    }


    if (Brake_lights_on) {
      digitalWrite(ACC_brake_pin, LOW);
    } else {
      digitalWrite(ACC_brake_pin, HIGH);
    };

    if (OIL) {
      digitalWrite(OIL_pin, LOW);
    } else {
      digitalWrite(OIL_pin, HIGH);
    };

    if (BRAKEWARN) {
      digitalWrite(BRAKEWARN_pin, HIGH);
    } else {
      digitalWrite(BRAKEWARN_pin, LOW);
    };

    if (GONG) {
      if ((millis() - gongtimer) > (1000 * 15)) {
        digitalWrite(GONG_pin, HIGH);
      } else { //wenn GONG länger als 15s an, dann deaktivieren sonst gong auf low
        digitalWrite(GONG_pin, LOW);
      }
    } else {
      digitalWrite(GONG_pin, HIGH);
      gongtimer = millis();
    };

    if (KLIMA_off) {
      digitalWrite(KLIMA_off_pin, LOW);
    } else {
      digitalWrite(KLIMA_off_pin, HIGH);
    };
  }

}
;

