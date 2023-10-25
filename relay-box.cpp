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
#include <states.h>
#include <gears.h>

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

char            msgString[128];                 // Array to store serial string
error_state     errors;
init_state      initialized;
timer_state     timer;
car_state       car;
digital_pins    pins;

/*
* state == low_value -> writes LOW to pin
* state != low_value -> writes HIGH to pin
*/
void conditionalDigitalWrite(int pin, bool state, bool low_value = false)
{
    if (state == low_value)
        digitalWrite(pin, LOW);
    else
        digitalWrite(pin, HIGH);
}

void setup()
{
    while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {}
    CAN0.setMode(MCP_LISTENONLY);   // Change to normal mode to allow messages to be transmitted

    Serial.begin(115200);
    Serial3.begin(9600);
    Serial.println("Reboot - CAN OK");

    pinMode(pins.CEL, OUTPUT);
    digitalWrite(pins.CEL, LOW);
    pinMode(pins.EML, OUTPUT);
    digitalWrite(pins.EML, LOW);
    pinMode(pins.BAT, OUTPUT);
    digitalWrite(pins.BAT, LOW);
    pinMode(pins.DSC, OUTPUT);
    digitalWrite(pins.DSC, LOW);
    pinMode(pins.ABS, OUTPUT);
    digitalWrite(pins.ABS, LOW);
    pinMode(pins.OIL, OUTPUT);
    digitalWrite(pins.OIL, LOW);
    pinMode(pins.BRAKEWARN, OUTPUT);
    digitalWrite(pins.BRAKEWARN, HIGH);  //Invertiert
    pinMode(pins.ACC_brake, OUTPUT);
    digitalWrite(pins.ACC_brake, LOW);
    pinMode(pins.GONG, OUTPUT);
    digitalWrite(pins.GONG, LOW);
    pinMode(pins.KLIMA_off, OUTPUT);
    digitalWrite(pins.KLIMA_off, LOW);

    initialized.CEL = true;
}

void loop() {

    if (analogRead(pins.IGN) > 400) {  //512 ??
        car.IGN = true;
        if (initialized.CEL && ((millis() - timer.CEL_init) > 10000)) {
            car.CEL = false;
            errors.update(34, false);
            initialized.CEL = false;
            Serial.print("EML errorlist[34] RESET");
        }
    } else {
        car.IGN = false;
        initialized.CEL = true;
        initialized.RPM = true;
        errors.reset();
    }


    car.klima_raw = (car.klima_raw * (car.iii - 1) + analogRead(pins.KLIMA_TASTE_EIN)) / car.iii;
    if (car.iii > 100) {
        car.iii = 1;
    };
    car.iii = car.iii + 1;

    if (car.klima_raw > 50) {
        if (car.accel_pos > 70) {
            car.KLIMA_off = true;
        } else {
            car.KLIMA_off = false;
        }
    } else {
        car.KLIMA_off = true;
    }
#if 0
    Serial.print(KLIMA_off);
    Serial.print("  ");
    Serial.print(klima_raw);
    Serial.print(" ");
    Serial.println(analogRead(pins.KLIMA_TASTE_EIN));
#endif


    if (!car.Fussbremse_getreten && car.ACC_brake && ((millis() - timer.ACC_brake) > 750)) {
        //Wenn CAN message Bremse AN und diese länger als 750ms an ist, dann setzte Bremslichter ein
        car.Brake_lights_on = true;
    } else {
        car.Brake_lights_on = false;
    }


    // RECEIVE DATA /////////////////////////////////////////////////////////////////////******************************
    gear_display gears;

    //read new can message
    if (!digitalRead(CAN0_INT)) {                       // If CAN0_INT pin is low, read receive buffer

        unsigned long rxId;
        unsigned char len = 0;
        byte rxBuf[8];

        CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)


        if (rxId == can_id::engine_start) {
            car.accel_pos = float((uint16_t(rxBuf[3]) << 8) + rxBuf[2]) / 655.35f;
            car.RPM = (uint16_t(rxBuf[5] << 8) + rxBuf[4]) / 4;
            if (initialized.RPM && (car.RPM > 400)) {
                Serial.println();
                Serial.println("engine-Start");
                Serial.println();
                Serial.println(initialized.CEL);
                Serial.println();
                timer.CEL_init = millis();
                initialized.RPM=false;
            }
        }

        if (rxId == can_id::brake_state) { //Status Bremse
            if (rxBuf[7] < 20) {
                car.Fussbremse_getreten = false;
            } else {
                car.Fussbremse_getreten = true;
            }

        }

        // Anzeige Führungsfahrzeug als 'E'
        if (rxId == can_id::fuehrungsfahrzeug_E) {

            if ((rxBuf[5] == 0x50) || (rxBuf[5] == 0x08)) {
                car.ACC_on = false;
            } else {
                car.ACC_on = true;
            }


            if (rxBuf[3] == 0x11) {
                car.ACC_leading_veh = true;
            } else {
                car.ACC_leading_veh = false;
            };
        }

        // Bremslichterleuchten bei ACC Bremsanforderung
        if (rxId == can_id::brakelight_acc_req) {
            if (rxBuf[5] == 0xF1) {
                car.ACC_brake = true;
            }
            else {
                car.ACC_brake = false;
                timer.ACC_brake = millis();
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
            gears.set_flags(car.ACC_on, car.ACC_leading_veh, car.GET);
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
                    errors.update(Error_ID, Error_on_off);
                }

                errors.check_groups();

#if 1
                Serial.print(" Error Warning ID: ");
                Serial.print(Error_ID, DEC);
                Serial.print(" DEC, rxBuf[3] ");
                Serial.print(rxBuf[3], HEX);
                Serial.print(" HEX  bool: ");
                Serial.print(Error_on_off);
                Serial.println();
#endif
#if 0
                Serial.print("Errorlist: ");
                for (size_t i=0; i<sizeof(errors.state_array); ++i)
                {
                    if (errors.state_array[i])
                    {
                        Serial.print(i);
                        Serial.print(", ");
                    }
                }
                Serial.println();
#endif
            }
        } //ende der CAN Botschaften, die Errormessages enthalten.

        // ################## CAN Nachrichten abfragen ENDE  ##################



#if 0
        //Serial.println(rxBuf[1], HEX);
        Serial.print("--> Gang123456:");
        Serial.print(gears.digits[0]);
        Serial.println(gears.digits[1]);
#endif

#if 0
        sprintf(msgString, "0x%.3lX, %1d,  Data:", rxId, len);
        Serial.print(msgString);
        for (byte i = 0; i < len; i++) {
            sprintf(msgString, " 0x%.2X", rxBuf[i]);
            Serial.print(msgString);
        }
        Serial.println();
#endif
    }

    if (millis() - timer.serial1 > 110) {
        Serial3.print(gears.digits[0]);
        //Serial.print("Gear:");
        //Serial.print(gears.digits[0]);

        Serial3.println(gears.digits[1]);
        //Serial3.println("P");
        //Serial.println(gears.digits[1]);
        timer.serial1 = millis();
    }



    // ################## CAN          ENDE  ##################




    // ################# Hier sind wir noch in der LOOP aber nach dem CAN Input ##################
#if 0
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
#endif

    if ( (!initialized.pins) && (millis() > timer.init_time)) {
        digitalWrite(pins.CEL, HIGH);
        digitalWrite(pins.EML, HIGH);
        digitalWrite(pins.BAT, HIGH);
        digitalWrite(pins.DSC, HIGH);
        digitalWrite(pins.ABS, HIGH);
        digitalWrite(pins.OIL, HIGH);
        digitalWrite(pins.BRAKEWARN, LOW); //Invertiert
        digitalWrite(pins.ACC_brake, HIGH);
        digitalWrite(pins.GONG, HIGH);
        digitalWrite(pins.KLIMA_off, HIGH);
        initialized.pins = true;
    }


    if (initialized.pins) {

        conditionalDigitalWrite(pins.EML, (car.EML || car.Brake_lights_on || car.RPA), true);
        conditionalDigitalWrite(pins.CEL, car.CEL, true);
        conditionalDigitalWrite(pins.BAT, car.BAT, true);
        conditionalDigitalWrite(pins.DSC, car.DSC, true);
        conditionalDigitalWrite(pins.ABS, car.ABS, true);
        conditionalDigitalWrite(pins.ACC_brake, car.Brake_lights_on, true);
        conditionalDigitalWrite(pins.OIL, car.OIL, true);
        conditionalDigitalWrite(pins.BRAKEWARN, car.BRAKEWARN, false);
        conditionalDigitalWrite(pins.KLIMA_off, car.KLIMA_off, true);

        if (car.GONG) {
            if ((millis() - timer.gong) > (1000 * 15)) {
                digitalWrite(pins.GONG, HIGH);
            } else { //wenn GONG länger als 15s an, dann deaktivieren sonst gong auf low
                digitalWrite(pins.GONG, LOW);
            }
        } else {
            digitalWrite(pins.GONG, HIGH);
            timer.gong = millis();
        };
    }

}


