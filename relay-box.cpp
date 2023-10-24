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

#include <can_id.h>
#include <errorcodes.h>
#include <gears.h>

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

float accel_pos;
float klima_raw = 0;
unsigned int iii = 1;
//Für Ganganzeige
unsigned long timer_serial1 = 0;
bool IGN = false;
bool _init_ = true;
unsigned long ACC_brake_timer = 0;
bool Brake_lights_on = false;
bool Fussbremse_getreten = false;

unsigned int init_time = 500; //initialisierungszeit 10ms brauche ich nicht mehr, kann irgendwann rausgenommen werden, dann bei initialisierung alles auf HIGH setzen.
const int IGN_pin = A5 ; //S_Line muss an pin D3 Digital 3, und IGN an Pin 19 (Analog5) kommt IGN
bool RPM_init=true;

//Motorparams
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




char msgString[128];                        // Array to store serial string

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

error_state state;

void loop() {

    if (analogRead(IGN_pin) > 400) {  //512 ??
        IGN = true;
        if (CEL_init && ((millis() - timer_CEL_init) > 10000)) {
            CEL = false;
            state.update(34, false);
            CEL_init = false;
            Serial.print("EML errorlist[34] RESET");
        }
    } else {
        IGN = false;
        CEL_init = true;
        RPM_init = true;
        state.reset();
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
    gear_display gears;

    //read new can message
    if (!digitalRead(CAN0_INT)) {                       // If CAN0_INT pin is low, read receive buffer

        unsigned long rxId;
        unsigned char len = 0;
        byte rxBuf[8];

        //TODO does it really only read max 8 bytes?
        CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)


        if (rxId == can_id::engine_start) {
            accel_pos = float((uint16_t(rxBuf[3]) << 8) + rxBuf[2]) / 655.35f;
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

        if (rxId == can_id::brake_state) { //Status Bremse
            if (rxBuf[7] < 20) {
                Fussbremse_getreten = false;
            } else {
                Fussbremse_getreten = true;
            }

        }




        // Anzeige Führungsfahrzeug als 'E'
        if (rxId == can_id::fuehrungsfahrzeug_E) {

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
        if (rxId == can_id::brakelight_acc_req) {
            if (rxBuf[5] == 0xF1) {
                ACC_brake = true;
            }
            else {
                ACC_brake = false;
                ACC_brake_timer = millis();
            };
        }


        // ###############  GANG ANZEIGE S M ' ' PRND und 123456  #################
        if (rxId == can_id::display_gears) { //Gearbox 0xBA=186  //R=B4 C FF ;  N=D2 C FF; P=E1 C FF; D=78 7C FF; F0 F FF (zwischen zwei Gängen)
#if 0
            Serial.print(rxBuf[0], HEX);
            Serial.print(" ");
            Serial.print(rxBuf[1], HEX);
            Serial.print(" ");
            Serial.print(rxBuf[2], HEX);
            Serial.print(" ");
            Serial.print(rxBuf[3], HEX);
            Serial.print(" ");
            Serial.println(rxBuf[4], HEX);
#endif
            gears.update(rxBuf);
            gears.set_flags(ACC_on, ACC_leading_veh, GET);
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

        if ((can_id::error_min <= rxId) && (rxId <= can_id::error_max)) {
            //0x40 sind Fehlermeldungen die im KMB gezeigt werden
            if (rxBuf[0] == 0x40) {
                //Error ist byte1 +erstes bit von byte2
                const uint16_t Error_ID = rxBuf[1] + uint16_t(bitRead(rxBuf[2], 0) << 8);
                //Fehler an oder aus steht in Byte3
                const bool Error_on_off = (bitRead(rxBuf[3], 0));

                if (Error_ID > 0 && Error_ID < 399) {
                    state.update(Error_ID, Error_on_off);
                }

                state.check_groups();

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
                    for (size_t i=0; i<sizeof(state.state_array); ++i)
                    {
                        if (state.state_array[i])
                        {
                            Serial.print(i);
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
            Serial.print(gears.digits[0]);
            Serial.println(gears.digits[1]);
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
        Serial3.print(gears.digits[0]);
        //Serial.print("Gear:");
        //Serial.print(gears.digits[0]);

        Serial3.println(gears.digits[1]);
        //Serial3.println("P");
        //Serial.println(gears.digits[1]);
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


