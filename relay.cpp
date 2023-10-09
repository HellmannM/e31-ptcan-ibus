//Arduino MEGA ! mit LCD shield (kann weg)

// abstandanzeige immer wenn tempomat an
// Welcome message verschwinden
//toDo: Ibus key und light data
//ölstand als % anzeige
//errorlist MRS AIRBAG

//welcome message bei key in? dabei stromversorgung verlegen

#include <stubs.h>

#include <hex_float_converter.h>
#include <LiquidCrystal.h>
#include <IbusMessage.h>
#include <IbusNames.h>
#include <IbusTrx.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 48                              // Set INT to pin 48

//#define button_in  A0    // Eingang des Hebels zum Umschalten des Menu A8 für im Auto und A0 debug
#define button_in  A8

//Zusatzmenu Tasten
unsigned long  check_button_press = 50;
unsigned int check_button_press_interval = 10;
unsigned int adc_key_in = 0;
int32_t page = 1;
int error_page = 0;
uint32_t ind_error_to_show = 1;
const uint8_t Seitenanzahl = 7;

//Ganganzeige

int gear_data;
std::string gear_digit="  ";

//Errorlist variablen
bool error_over_ibus = true;
bool errorlist[399];

//uint16_t list_true_err[99];


//iBus Variablen
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];

long timer_dist_menu = 0;
long timer_get_fuel = 0;
long timer_welcome = 0;
long timer_send_verbr1 = 0;
long timer_send_verbr2 = 0;
long timer_send_range = 0;
long timer_print_lcd = 0;
long timer_acc_set = 0;
long timer_send_speed = 0;
long timer_extra_menu = 0;
long timer_clear_screen = 0;
long timer_error_menu = 0;
long timer_lookforerror_to_ibus = 0;


uint8_t e31LKM_length = 0;
uint8_t e31LKM_data[8];

uint8_t e31ZKE_length = 0;
uint8_t e31ZKE_data[8];



uint8_t e31temp_data2 = 0;
uint8_t e31temp_data1 = 0;
long   count_temp = 0;


uint8_t e31KEY_length = 0;
uint8_t e31KEY_data;


uint8_t clear_counter = 0;
const uint16_t interval_extra_menu = 200;
const uint16_t interval_send_verbr1 = 503;
const uint16_t interval_send_verbr2 = 530;
const uint16_t interval_send_range = 2001;
uint16_t interval_get_Fuel = 1000;
const uint16_t interval_send_acc = 500;
const uint16_t interval_show_speed = 5001;



std::string string_vol1;
std::string string_vol2;

float mpg1 = 0;
float mpg2 = 0;
float speed_ist = 0;
float lw = 0;

float mpg2_ind = 0;
const unsigned int numReadings = 5;
float mpg2_vect[numReadings]={0};
float total=10;
unsigned int readIndex = 0;

int injection_volume = 0;
int injection_volume_raw = 0;
std::string oil2 = "---";
float oil1 = 0;
unsigned int fuel = 0;
unsigned int range_calc = 100;
unsigned int speed_set = 100;
unsigned int old_speed_set;
unsigned int ign_watchdog = 0;
unsigned int water_temp = 0;
unsigned int oil_temp = 0;
float battery_voltage = 0;

bool show_dist = true;
bool ign = false;
bool acc_on = false;
bool acc_set_speed_change = false;
uint8_t dist = 0;
bool brake_status = false;
bool clear_screen = false;
bool extra_menu = false;
bool error_menu = false;
bool veh_in_front = false;










void text_to_ibus(std::string text_to_display, float data_to_display, std::string unit_to_display, int pos, bool floatsigns) {
  std::string input = std::string(data_to_display, floatsigns);
  if (text_to_display.length() + pos + input.length() + unit_to_display.length() < 21) { //20 Ziffern können beschrieben werden

    for (int i = 0; i < text_to_display.length(); i++) {
      text_iBus[i + 4 + pos] = text_to_display[i];
    }
    if (!floatsigns && (data_to_display < 10) && (data_to_display >= 0)) {
      text_iBus[text_to_display.length() + 4 + pos ] = input[1];
      text_iBus[text_to_display.length() + 4 + pos + 1] = 0x20;
    } else {
      for (int i = 0; i < input.length(); i++) {
        text_iBus[text_to_display.length() + 4 + pos + i] = input[i];
      }
    }
    for (int i = 0; i < unit_to_display.length(); i++) {
      text_iBus[text_to_display.length() + input.length() + 4 + pos + i] = unit_to_display[i];
    }

    for (int i = text_to_display.length() + 4 + pos + input.length() + unit_to_display.length(); i < 23; i++) { //mögicherweise <24
      text_iBus[i] = 0x20;
    }
  } else {
    //Serial.print("Message too long");
    text_iBus[4 + pos] = 'X';
  }
}

void string_to_ibus(std::string text_to_display, int pos) {
  int a = text_to_display.length() + pos;
  if (a < 21) {
    for (int i = 4; i < text_to_display.length() + 4; i++) {
      text_iBus[i + pos] = text_to_display[i - 4];
    }

    for (int i = text_to_display.length() + 4 + pos; i < 23; i++) {
      text_iBus[i + pos] = 0x20;
    }



  } else {
    for (int i = 4; i < 24 - pos; i++) {
      text_iBus[i + pos] = text_to_display[i - 4];
    }
  }
}



/// SETUP
static LiquidCrystal    lcd(8, 2, 9, 4, 5, 6, 7);
static MCP_CAN          CAN0(53);
static IbusTrx          ibusTrx;
static menu             main_menu(ibusTrx);
static float_to_hex_converter conv;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  Serial.begin(115200);
  Serial.println("iBus Modul");
  Serial2.begin(9600);
  ibusTrx.begin(Serial2);
  while (CAN0.begin(MCP_ANY, CAN_100KBPS, MCP_8MHZ) == !CAN_OK) {
    Serial.println("MCP2515 error....");
    lcd.print("CAN error");
    delay(200);
  }
  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
  lcd.print("CAN ok. iBus Modul");

  //Stromversorgung für die Tempomattaste aka Menu in!
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
}



/// LOOP

void loop() {

  while (ibusTrx.transmitWaiting()) {
    ibusTrx.available();
  };

  main_menu.update();

  if ((millis() > 20000) && (millis() - timer_dist_menu > 251)) {
    if (extra_menu || show_dist) {
      switch (dist) {
        case 0:
          ibusTrx.write(send_inspectionlights_off);
          break;
        case 1:
          send_inspectionlights_show[4] = 0x08;
          if (veh_in_front) {
            send_inspectionlights_show[4] = send_inspectionlights_show[4] + 0x20;
          }
          ibusTrx.write(send_inspectionlights_show);
          break;
        case 2:
          send_inspectionlights_show[4] = 0x0C;
          if (veh_in_front) {
            send_inspectionlights_show[4] = send_inspectionlights_show[4] + 0x20;
          }
          ibusTrx.write(send_inspectionlights_show);
          break;
        case 3:
          send_inspectionlights_show[4] = 0x0E;
          if (veh_in_front) {
            send_inspectionlights_show[4] = send_inspectionlights_show[4] + 0x20;
          }
          ibusTrx.write(send_inspectionlights_show);
          break;
        case 4:
          send_inspectionlights_show[4] = 0x0F;
          if (veh_in_front) {
            send_inspectionlights_show[4] = send_inspectionlights_show[4] + 0x20;
          }
          ibusTrx.write(send_inspectionlights_show);
          break;
        default:
          ibusTrx.write(send_inspectionlights_off);
          break;
      }
    }
    else {
      ibusTrx.write(send_inspectionlights_off);
    }

    timer_dist_menu = millis();
  }



//  if (error_menu && (millis() - timer_error_menu > interval_extra_menu)) {
//    //Serial.print("error menu:");
//    uint16_t number_of_err = 0;
//
//    for (int i = 0; i < sizeof(errorlist) / sizeof(errorlist[0]); i++) {
//      if (errorlist[i]) {
//        list_true_err[number_of_err] = i;
//        number_of_err++;
//        //Serial.print(i, HEX);
//        //Serial.print(" ");
//      }
//    }
//
//    if (number_of_err > 0) {
//      ind_error_to_show = error_page % number_of_err;
//      text_to_ibus("", ind_error_to_show + 1, "", 0, 0);
//      text_to_ibus("/", number_of_err, "", 1, 0);
//      text_to_ibus(" ", list_true_err[ind_error_to_show], "", 3, 0);
//      string_to_ibus(error_translate[list_true_err[ind_error_to_show]], 7);
//
//      //Serial.print(error_page );
//      //Serial.print(" ");
//      //Serial.print(number_of_err);
//      //Serial.print(" ");
//      //Serial.print(list_true_err[ind_error_to_show]);
//      //Serial.println();
//
//    } else {
//      string_to_ibus("No ERRORS", 0);
//      //Serial.print("no error");
//    }
//    ibusTrx.write(text_iBus);
//    ibusTrx.available();
//    timer_error_menu = millis();
//  }


  if (millis() - timer_get_fuel > interval_get_Fuel) {
    if (fuel > 0) {
      interval_get_Fuel = 10000; //nur noch alle 5 sek nach Fuelstand schauen
    }
    ibusTrx.write(request_diagnose);
    ibusTrx.write(request_fuellevel);
    timer_get_fuel = millis();
  }



  // READ IBUS

  while (ibusTrx.available()) {

    IbusMessage m = ibusTrx.readMessage(); // grab incoming messages

    // get fuel level from diagDataMessage
    if ((m.source() == 0x02) && (m.destination() == 0x01) && (m.b(1) == 0x85)) {
      string_vol1 = std::string(m.b(3), HEX);
      string_vol2 = std::string(m.b(2), HEX);
      fuel = (string_vol1.toInt()) * 100 + string_vol2.toInt();
    }



    //01 04 02 92 00 95 button pressed reset_average_fuel_1  -> Send to can reset average fuel
    if ((m.source() == 0x01) && (m.destination() == 0x02) && (m.b(0) == 0x92) && (m.b(1) == 0x00)) {
      CAN0.sendMsgBuf(reset_mpg_id, 0, reset_mpg_length, reset_mpg_data);
    }

    //01 04 02 93 00 95 button pressed reset_average_fuel_2  -> Reset Errorlist
    if ((m.source() == 0x01) && (m.destination() == 0x02) && (m.b(0) == 0x93) && (m.b(1) == 0x00)) {
      //hier ist Verbrauch2 reset gedrückt
      //Errorliste löschen
      memset(errorlist, 0, sizeof(errorlist));
      error_over_ibus = !error_over_ibus;
      show_dist = !show_dist;

      //clear DTC
      CAN0.sendMsgBuf(clearDTC_id, 0, clearDTC_length, clearDTC_data);
      CAN0.sendMsgBuf(clearDSC_id, 0, clearDSC_length, clearDSC_data);
    }

    //get LKM Info
    if ((m.source() == 0x0A) && (m.destination() == 0x02)) {
      if (m.b(0) == 0xFA) { //FA is only ACKN for EKM request
      } else {
        e31LKM_length = m.length();
        for (int i = 0; i < m.length(); i++) {
          e31LKM_data[i] = m.b(i);
        }
      }
    }



    //get ZKE Info
    if ((m.source() == 0x09) && (m.destination() == 0x02)) {
      if (m.b(0) == 0xFA) { //FA is only ACKN for EKM request
      } else {
        e31ZKE_length = m.length();
        for (int i = 0; i < m.length(); i++) {
          e31ZKE_data[i] = m.b(i);
        }
      }
    }

    //get KEY Info
    if ((m.source() == 0x02) && (m.destination() == 0xFF)) {  //capture boardcast
      //get key pos data
      if (m.b(0) == 0xF8) { //FA is only ACKN for EKM request
        e31KEY_data = m.b(1);
      }
    }


    //get outside temp
    //02 05 FF A0 20 C2 BA
    if ((m.source() == 0x02) && (m.destination() == 0xFF)) {  //capture boardcast
      //get temp
      if (m.b(0) == 0xA0) { //FA is only ACKN for EKM request
        e31temp_data2 = m.b(1);
        e31temp_data1 = m.b(2);
      }
      count_temp = millis() - count_temp;
    }

  }  // ENDE iBUS read




  //READ CAN
  while (!digitalRead(CAN0_INT)) {                      // If CAN0_INT pin is low, read receive buffer
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)



    if (rxId == 0x362) { //mpg1
      mpg1 = ((float)(((rxBuf[2] << 4) + (rxBuf[1] >> 4)))) / 10;
      conv.float_to_fake_hex(mpg1 * 10);
      send_verbr1[5] = conv.chars[0];
      send_verbr1[4] = conv.chars[1];

      if (mpg1 == 0) {
        range_calc = 0;
        send_range[5] = 0xFF;
        send_range[4] = 0xFF;
      } else {
        range_calc = int(10 * fuel / mpg1); //eigentlich *100 da pro 100 km/l aber fuel ist bereits mal 10
        conv.float_to_fake_hex((range_calc));
        send_range[5] = conv.chars[0];
        send_range[4] = conv.chars[1];
      }
    }

    if (rxId == 0x338) {  //alternative: ((rxId >= 0x580 )&&(rxId <= 0x671))
        int error_id = rxBuf[1] * 256 + rxBuf[0];
        if (error_id = 25) {} else {
          errorlist[error_id] = 1;
          //      Serial.print(error_id,HEX);
          //      Serial.print(" ");
          //      Serial.print(rxBuf[2],HEX);
          //      Serial.print(" ");
          //      Serial.println(rxBuf[3],HEX);
        }
    }

    if (rxId == 0x130) {
      if (rxBuf[0] == 00) {
        ign = 0;
      } else {
        ign = 1;
        ign_watchdog = rxBuf[4];
      }
    }


    if (rxId == 0x1D2) { //Gearbox 0x1D2
      gear_digit[0] = ' ';
      if (rxBuf[0] == 0x78) { // D ist eingelegt
        if (rxBuf[4] == 241) {    //S
          gear_digit[0] = 'S';
        };
        if (rxBuf[4] == 242) { //M
          gear_digit[0] = 'M';
        };
      }
      gear_data = rxBuf[1]; // Ganzuordnung nach Tabelle unten
      gear_digit[1] = ' ';
      if (gear_data == 0x5C)gear_digit[1] = '1';
      if (gear_data == 0x6C)gear_digit[1] = '2';
      if (gear_data == 0x7C)gear_digit[1] = '3';
      if (gear_data == 0x8C)gear_digit[1] = '4';
      if (gear_data == 0x9C)gear_digit[1] = '5';
      if (gear_data == 0xAC)gear_digit[1] = 'D';
    }


    if (rxId == 897) { //Motoröl 0x381=897

      //19= Bar at min, 35 is bar at half , 55=around full,7A=Full,
      //oil3=100*(rxBuf[0]-0x19)/(0x7A-0x19); //Prozent
      //F0 = "OK", F1 = ">MAX", F2 = "+1qt"
      oil1 = rxBuf[0];

      if (rxBuf[1] == 0xF0) {
        oil2 = "OK ";
      }
      else if (rxBuf[1] == 0xF1) {
        oil2 = "MAX";
      }
      else if (rxBuf[1] == 0xF2) {
        oil2 = "LOW"; //Hier Warnung
      } else {
        oil2 = "---";
      }
    }


    if (rxId == 0x1D0) {                      // Engine params

      water_temp = rxBuf[0] - 48;
      oil_temp = rxBuf[1] - 48;
      injection_volume = (rxBuf[4] + rxBuf[5] * 256) - injection_volume_raw;
      injection_volume_raw = (int)(rxBuf[4] + rxBuf[5] * 256);
      if (injection_volume < 0) {
        injection_volume = injection_volume + 0x10000;
      }
      if (speed_ist == 0) {
        mpg2_ind = 0;
      } else {
        mpg2_ind = (float)injection_volume / (float)speed_ist ;
        if(mpg2_ind>=99){mpg2_ind=99;}
      }
      //alterCode

      // subtract the last reading:
      total = total - mpg2_vect[readIndex];
      // read from the sensor:
      mpg2_vect[readIndex] = mpg2_ind;
      // add the reading to the total:
      total = total + mpg2_vect[readIndex];
      // advance to the next position in the array:
      readIndex = readIndex + 1;

      // if we're at the end of the array...
      if (readIndex >= numReadings) {
        // ...wrap around to the beginning:
        readIndex = 0;
      }

      // calculate the average:
      mpg2 = (float)total / (float)numReadings;




      //alterCode


      //Serial.println(mpg2);
      conv.float_to_fake_hex(mpg2 * 10);
      send_verbr2[5] = conv.chars[0];
      send_verbr2[4] = conv.chars[1];
    }

    if (rxId == 0x3B4) { //Battery Voltage
      //DB F3 00 F
      //Example F3DB - F000 = 3DB = 987 (dec) / 68 = 14.51V
      battery_voltage = ((float)(rxBuf[1] * 256 + rxBuf[0] - 0xF000) / 68);
    }


    if (rxId == 0x193) { //ACC Speed

      if (rxBuf[1] == 254) {
        speed_set = 0;
      }  else {
        speed_set = rxBuf[1];
      }
      if (rxBuf[3] > 1) {
        veh_in_front = true;
      } else {
        veh_in_front = false;
      }
      if ((rxBuf[5] == 0x50) || (rxBuf[5] == 0x08)) {
        dist = 0;
        acc_on = false;
      } else {
        acc_on = true;
        dist = rxBuf[5] - 0x59;
      }
      if (rxBuf[6] == 0) {} else {
        if (error_menu) {} else {
          timer_acc_set = millis();
          acc_set_speed_change = true;
          page = 1;
          extra_menu = true;
        }
      }

    }


    if (rxId == 0xA8) { //Status Bremse
      if (rxBuf[7] < 20) {
        brake_status = false;
      } else {
        brake_status = true;
      }

    }


    if (rxId == 0x1B4) { //current speed
      speed_ist = (float)(((rxBuf[1] - 0xD0 ) * 256) + rxBuf[0] ) / 10;
    }

    if (rxId == 0x0C8) { //lenkwinkel
      lw = (float)((rxBuf[1] * 256) + rxBuf[0] ) / 23;
    }
  }





  if (millis() - check_button_press > check_button_press_interval) {
    adc_key_in = analogRead(button_in);      // read the value from the sensor
    Serial.println(adc_key_in);

    if (adc_key_in < 200) { //Abruf
      page=page-1;
      error_menu = false;
      extra_menu = true;
      check_button_press_interval = 300;

    } else if (adc_key_in < 310) { //drücken
      //Serial.println("drücken");
      error_menu = true;
      extra_menu = false;
      check_button_press_interval = 300;
      error_page++;



    } else  if (adc_key_in < 500) { //ziehen
      //Serial.println("ziehen");
      page=page+1;
      error_menu = false;
      extra_menu = true;
      check_button_press_interval = 300;

    } else if (adc_key_in < 660) { //passiv
      check_button_press_interval = 50;

    } else if (adc_key_in < 900) { //o+u
      //Serial.println("oben+unten");
      extra_menu = false;
      error_menu = false;
      clear_screen = true;
      page = 1;
      error_page = 0;
      check_button_press_interval = 300;
    } else {
      check_button_press_interval = 50;
      //Serial.println("nichts");
    }

Serial.println(page);
    check_button_press = millis();
  }

}





void SerialprintiBus(uint8_t vector[], int laenge) {
  for (int i = 0; i < laenge; i++) {
    Serial.print("0x");
    if (vector[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(send_verbr1[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

}





#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

