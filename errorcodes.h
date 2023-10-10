#pragma once

//enum ERROR_GROUP {
//    UNDEFINED = 0,
//    ABS,
//    BAT,
//    BRAKE,
//    CEL,
//    DSC,
//    EML,
//    GET,
//    OIL,
//    RPA
//};
//
//enum ERROR_TYPE {
//    UNDEFINED = 0,
//    ABS,
//    DDE,
//    DISTANCE,
//    DSC,
//    EGS,
//    LDM,
//    MISC
//};
//
//enum COLOR {
//    UNDEFINED = 0,
//    YELLOW,
//    ORANGE,
//    MIL,
//    MIL_YELLOW,
//    RED
//};
//
//struct error_message {
//    ERROR_GROUP group;
//    ERROR_TYPE type;
//    size_t id;
//    ssize_t flag;
//    COLOR color;
//};
//
//static error_message error_library[] = {
////EML
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 29 , 1, COLOR::YELLOW     }, // Problem mit Motor, Leistung reduziert
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 30 , 1, COLOR::RED        }, // Sofort anhalten.MOTOR STOP. Motor defekt
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 31 , 1, COLOR::MIL_YELLOW }, // Abgasproblem
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 32 , 0, COLOR::YELLOW     }, // Tankdeckel ist offen
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 33 , 1, COLOR::YELLOW     }, // Problem mit abgastemperatur
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 34 , 1, COLOR::MIL        }, // Allgemeine MIL Lampe
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 39 , 1, COLOR::RED        }, // Übertemperatur
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 49 , 1, COLOR::YELLOW     }, // Partikelfilter defekt
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 148, 1, COLOR::YELLOW     }, // Bremslichtcheck defekt
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 212, 1, COLOR::RED        }, // Öldruck zu niedrig
//
//    {ERROR_GROUP::EML, ERROR_TYPE::DDE, 257, 1, COLOR::YELLOW     }, // Motor zu heiß (steht aber unter Batterie.. sehr komisch..
////OIL
//    {ERROR_GROUP::OIL, ERROR_TYPE::DDE, 27 , 1, COLOR::YELLOW     }, // Öl ist auf minimum
//    {ERROR_GROUP::OIL, ERROR_TYPE::DDE, 28 , 1, COLOR::YELLOW     }, // Öl ist UNTER minimum
//    {ERROR_GROUP::OIL, ERROR_TYPE::DDE, 182, 1, COLOR::YELLOW     }, // Öllevelsensor defekt
////GET
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 105, 1, COLOR::RED        }, // Getriebe überhitzt
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 171, 1, COLOR::YELLOW     }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 172, 1, COLOR::RED        }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 178, 0, COLOR::YELLOW     }, // Getriebe in N. Wagen kann wegrollen
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 248, 1, COLOR::YELLOW     }, // Problem zwischen ABS und EGS
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 254, 1, COLOR::RED        }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 287, 1, COLOR::YELLOW     }, // Getriebe überhitzt
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 288, 1, COLOR::YELLOW     }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 289, 1, COLOR::RED        }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 290, 1, COLOR::RED        }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 291, 1, COLOR::YELLOW     }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 292, 1, COLOR::RED        }, // Getriebe schaltet in N
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 293, 0, COLOR::UNDEFINED  }, // keine LED GELB N oder P einlegen und Bremse treten für Motorstart
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 302, 0, COLOR::YELLOW     }, // Getriebe nicht in P. Wagen kann wegrollen
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 307, 1, COLOR::RED        }, // Getriebe fehler
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 322, 1, COLOR::RED        }, // Getriebe lernt
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 323, 1, COLOR::RED        }, // Getriebe lernt
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 325, 0, COLOR::YELLOW     }, // Getriebe in N
//    {ERROR_GROUP::GET, ERROR_TYPE::EGS, 368, 1, COLOR::YELLOW     }, // Getriebe fehler
////RPA
//    {ERROR_GROUP::RPA, ERROR_TYPE::DSC, 50 , 1, COLOR::UNDEFINED  }, // RPA FEHLER
//    {ERROR_GROUP::RPA, ERROR_TYPE::DSC, 63 , 1, COLOR::RED        }, // RPA Reifen hat Loch
//    {ERROR_GROUP::RPA, ERROR_TYPE::DSC, 336, 1, COLOR::UNDEFINED  }, // RPA fehler
////BRAKE
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::DISTANCE, 282, -1, COLOR::YELLOW }, // FLAG -1 = Auto
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::DISTANCE, 283, -1, COLOR::YELLOW }, // FLAG -1 = Auto
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::DDE, 26 , 1, COLOR::YELLOW   }, // Cruise_Control_Error
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 1  , 1, COLOR::YELLOW   }, // ACC nicht verfügbar, weil glatt
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 2  , 1, COLOR::YELLOW   }, // ACC nicht verfügbar, Sensor blockiert
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 3  , 1, COLOR::YELLOW   }, // ACC fehler
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 59 , 1, COLOR::YELLOW   }, // ACC nicht verfügbar, Handbremse angezogen
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 69 , 1, COLOR::YELLOW   }, // nicht verfügbar unter 30kmh
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 85 , 1, COLOR::YELLOW   }, // ACC zu starkes bremsen
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 176, 1, COLOR::YELLOW   }, // ACC nicht verfügbar, Sensor blockiert
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 276, 0, COLOR::YELLOW   }, // Hochschalten
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 277, 0, COLOR::YELLOW   }, // Hochschalten
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 277, 0, COLOR::YELLOW   }, // Falscher Gang
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 278, 0, COLOR::YELLOW   }, // Runterschalten
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 337, 1, COLOR::YELLOW   }, // Tempomat fehler
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 338, 1, COLOR::YELLOW   }, // Tempomat deaktiv, Handbremse  //hier steht zwei mal 338
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 339, 1, COLOR::YELLOW   }, // DCC zu starkes bremsen
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 341, 1, COLOR::YELLOW   }, // Falscher Gang
//    {ERROR_GROUP::BRAKE, ERROR_TYPE::LDM, 342, 1, COLOR::YELLOW   }, // nicht verfügbar unter 30kmh
////ABS
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 71 , -1, COLOR::UNDEFINED }, // Hilldecent control deaktiv_nicht angeschlossen, aber wenn Fehler, dann funktioniert vieles nicht
//    //{ERROR_GROUP::ABS, ERROR_TYPE::ABS, ?? , -1, COLOR::RED       }, // Bremsbeläge verschlissen
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 74 , -1, COLOR::RED       }, // nicht angeschlossen, aber wenn Fehler, dann funktionier t vieles nicht  ABS ROT Bremsflüssigkeitsstand niedrig
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 236, 1, COLOR::YELLOW     }, // ABS  ABS fehler
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 353, 1, COLOR::YELLOW     }, // ABS  Bremsen überhitzt
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 42 , 1, COLOR::YELLOW     }, // ABS  EBV/CBC Fehler?
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 184, 1, COLOR::YELLOW     }, // DSC  DTC aktiv, DSC reduziert
//    {ERROR_GROUP::ABS, ERROR_TYPE::DSC, 36 , 1, COLOR::YELLOW     }, // DSC  DSC deaktiviert
////DSC
////283 ist ROT Auto Warnung
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 24 , 1, COLOR::YELLOW     }, // ABS  DBC defekt??
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 35 , 1, COLOR::YELLOW     }, // DSC  DSC defekt
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 36 , 1, COLOR::YELLOW     }, // DSC  DSC deaktiviert
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 184, 1, COLOR::YELLOW     }, // DSC  DTC aktiv, DSC reduziert
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 215, 1, COLOR::UNDEFINED  }, // DSC - Allgemeiner DTC Meldung
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 237, 1, COLOR::YELLOW     }, // DSC  DSC fehler
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 330, 0, COLOR::YELLOW     }, // HDC HilldescendControl not available
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 331, 0, COLOR::YELLOW     }, // HDC aktiv
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 332, 0, COLOR::YELLOW     }, // HDC deaktiv
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 333, 0, COLOR::YELLOW     }, // HDC icht  verfügbar
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 334, 1, COLOR::ORANGE     }, // Standarddize RPA ??????????????????????? Fehler nummer73
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 352, 0, COLOR::YELLOW     }, // Bremsen zu heiß
//    {ERROR_GROUP::DSC, ERROR_TYPE::DSC, 354, 1, COLOR::YELLOW     }, // ABS Anfahrhilfe deaktiviert
////VORGLUEHEN
//    //{ERROR_GROUP::VORGLUEHEN, ERROR_TYPE::DDE, 25,  1, COLOR::YELLOW }; // Vorglühen - hier muss eine andere Lampe nehmen!
////BAT
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 213, 1, COLOR::RED        }, // Batterie nicht geladen
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 220, 1, COLOR::YELLOW     }, // Batterie Entladung zu hoch
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 229, 1, COLOR::YELLOW     }, // Batterespannung niedrig
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 247, 1, COLOR::YELLOW     }, // Batteriemonitoring nicht möglich
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 304, 0, COLOR::YELLOW     }, // Batterie check, zu alt
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 305, 1, COLOR::YELLOW     }, // Batterieanschlüsse locker
//    {ERROR_GROUP::BAT, ERROR_TYPE::DDE, 306, 1, COLOR::YELLOW     }, // Batterie entladen
////MISC
////05A9  8 40  EC  0 15  FF  FF  FF  FF  --> 236 ABS ERROR
////05A9  8 40  32  0 21  FF  FF  FF  FF  --> 50 Tire failure warning
////05A9  8 40  18  0 19  FF  FF  FF  FF  --> 24 DSC Error
////05A9  8 40  62  1 21  FF  FF  FF  FF  --> 98
//
//};

static const int CEL_list[] = {25, 31, 34, 282, 283};
static const int EML_list[] = {27, 28, 29, 30, 33, 39, 49, 28, 148, 182, 212, 257}; // OILLEVEL
static const int OIL_list[] = {27, 28, 182};
static const int GET_list[] = {105, 171, 172, 248, 254, 287, 288, 289, 290, 292, 307, 322, 323, 325, 268};
static const int RPA_list[] = {50, 63, 336}; //RPA
static const int BRAKEWARN_list[] = { 1, 2, 3, 26, 59, 69, 85, 176, 337, 338, 339, 341, 342, 282, 283}; //TEMP
static const int ABS_list[] = {71, 74, 236, 42, 36, 353, 236};
static const int DSC_list[] = {24, 35, 36 , 184, 215, 237, 353, 334, 354, 283};
//static const int VORG_list[] = {25};
static const int BAT_list[] = {213, 220, 229, 247, 304, 305, 306};

struct error_state
{
    bool state_array[400] = {0};
    bool CEL, EML, OIL, GET, RPA, BRAKEWARN, ABS, DSC, BAT;

    void reset()
    {
        memset(state_array, 0, sizeof(state_array)*sizeof(*state_array));
    }

    void update(size_t id, bool value)
    {
        if (id < sizeof(state_array))
        {
            state_array[id] = value;
        }
    }
    
    void check_groups()
    {
        CEL = false;
        EML = false;
        OIL = false;
        GET = false;
        RPA = false;
        BRAKEWARN = false;
        ABS = false;
        DSC = false;
        BAT = false;
        for (size_t i=0; i<sizeof(CEL_list); ++i)
        {
            if (state_array[CEL_list[i]])
                CEL = true;
        }
        for (size_t i=0; i<sizeof(EML_list); ++i)
        {
            if (state_array[EML_list[i]])
                EML = true;
        }
        for (size_t i=0; i<sizeof(OIL_list); ++i)
        {
            if (state_array[OIL_list[i]])
                OIL = true;
        }
        for (size_t i=0; i<sizeof(GET_list); ++i)
        {
            if (state_array[GET_list[i]])
                GET = true;
        }
        for (size_t i=0; i<sizeof(RPA_list); ++i)
        {
            if (state_array[RPA_list[i]])
                RPA = true;
        }
        for (size_t i=0; i<sizeof(BRAKEWARN_list); ++i)
        {
            if (state_array[BRAKEWARN_list[i]])
                BRAKEWARN = true;
        }
        for (size_t i=0; i<sizeof(ABS_list); ++i)
        {
            if (state_array[ABS_list[i]])
                ABS = true;
        }
        for (size_t i=0; i<sizeof(DSC_list); ++i)
        {
            if (state_array[DSC_list[i]])
                DSC = true;
        }
        for (size_t i=0; i<sizeof(BAT_list); ++i)
        {
            if (state_array[BAT_list[i]])
                BAT = true;
        }
    }
};


