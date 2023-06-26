#pragma once


struct menu_element_data
{
    menu_element_data(uint8_t data[6], uint8_t length)
    : m_data(data), m_length(length) {}

    uint8_t m_data[24];
    uint8_t m_length;
}

struct menu_element
{
    menu_element(IbusTrx* ibusTrx, uint64_t interval, menu_element_data data)
    : p_ibusTrx(ibusTrx)
    , m_send_interval(interval)
    , m_data(data)
    , m_enabled(false)
    {}

    virtual void send_if_timer_expired(uint64_t current_time)
    {
        if (!enabled)
            return;
        if (current_time - last_activated < send_interval)
            return;
        //TODO
        //p_ibusTrx->write(m_data.m_data, m_data.m_length);
        m_last_activated = current_time;
    }

    IbusTrx             p_ibusTrx;
    uint64_t            m_send_interval;
    uint64_t            m_last_activated;
    menu_element_data   m_data;
    bool                m_enabled;
};

struct clear_element : public menu_element
{
    clear_element()
    : m_clear_counter(0)
    {}

    virtual void send_if_timer_expried(uint64_t current_time)
    {
        if (!enabled)
            return;

        if (m_clear_counter > 4)
        {
            m_clear_counter = 0;
            p_ibusTrx->write(IBUSDATA_INSPECTIONLIGHTS_OFF);
            p_ibusTrx->available();
            p_ibusTrx->available();
            p_ibusTrx->available();
            p_ibusTrx->write(IBUSDATA_CLEARSCREEN);
            p_ibusTrx->available();
            p_ibusTrx->available();
            clear_counter = 0;
            m_enabled = false;
        }
        else
        {
            ++m_clear_counter;
            p_ibusTrx->available();
            p_ibusTrx->write(IBUSDATA_CHECKCONTROL);
            p_ibusTrx->available();
        }
        m_last_activated = current_time;
    }

    uint64_t m_clear_counter;
};

struct menu
{
    menu(IbusTrx* ibusTrx)
    : m_p_ibusTrx(ibusTrx)
    , m_clear()
    , m_welcome   ({ibusTrx, 200,  {IBUSDATA_WELCOME,    sizeof(IBUSDATA_WELCOME)/sizeof(*IBUSDATA_WELCOME)}})
    , m_verbrauch1({ibusTrx, 503,  {IBUSDATA_VERBRAUCH1, sizeof(IBUSDATA_WELCOME)/sizeof(*IBUSDATA_WELCOME)}})
    , m_verbrauch2({ibusTrx, 530,  {IBUSDATA_VERBRAUCH2, sizeof(IBUSDATA_WELCOME)/sizeof(*IBUSDATA_WELCOME)}})
    , m_range     ({ibusTrx, 2001, {IBUSDATA_RANGE,      sizeof(IBUSDATA_WELCOME)/sizeof(*IBUSDATA_WELCOME)}})
    {
        m_welcome.enabled    = true;
        m_verbrauch1.enabled = true;
        m_verbrauch2.enabled = true;
        m_range.enabled      = true;
    }

    void update()
    {
        const auto current = millis();
        m_clear.send_if_timer_expired(current);
        if (current < 5000)
            m_welcome.send_if_timer_expired(current);
        m_verbrauch1.send_if_timer_expired(current);
        m_verbrauch2.send_if_timer_expired(current);
        m_range.send_if_timer_expired(current);
    }

    //TODO extra menu disables welcome

    IbusTrx         p_ibusTrx;
    clear_element   m_clear;
    menu_element    m_welcome;
    menu_element    m_verbrauch1;
    menu_element    m_verbrauch2;
    menu_element    m_range;
    menu_element    m_extra[6];
};


//TODO
/*
  //Debug LCD Terminal
  if (millis() - timer_print_lcd > 200) {
    lcd.setCursor(0, 0);
    lcd.print(e31KEY_data, HEX);
    lcd.print("Z");
    lcd.print(e31ZKE_data[0], HEX);
    lcd.print(" ");
    lcd.print(e31ZKE_data[1], HEX);
    lcd.print(" ");
    lcd.print("L");
    lcd.print(e31LKM_data[3], HEX);
    lcd.print(" ");
    lcd.print(mpg2);
    lcd.setCursor(0, 1);
    lcd.print(fuel);
    lcd.print(" ");
    lcd.print(mpg1);
    lcd.print(" ");
    lcd.print(range_calc);
    lcd.print("k ");
    lcd.print(ign_watchdog, HEX);
    lcd.print(" ");
    timer_print_lcd = millis();
  }

//  diese Funktion löscht die Tempomatanzeige wenn der Timer_ACC_Set abgelaufen ist. Im Automatikmodus (nicht im Extramenu wird der Timer nur einmal gesetzt)
    if (!show_dist&&!acc_on&&(millis() - timer_acc_set > interval_show_speed)) {
      if (acc_set_speed_change) {
        extra_menu = false;
        clear_screen = true;
      }
      acc_set_speed_change = false;
    }

  //send data to ibus from errorlist(CAN input) RDC+OIL (wobei öl muss ich dann rausnehmen
  if (error_over_ibus) {
    if (millis() - timer_lookforerror_to_ibus > 5000) {

      //uint8_t send_oil_low[5]={0x02,0x04,0x01,0xC8,0x61}; //182,27,28
      //uint8_t send_flattyre[5]={0x02,0x04,0x01,0xC8,0x03}; //336,63,142,147,265
      //uint8_t send_rdc_error[5]={0x02,0x04,0x01,0xC8,0x34}; //149,144,145,50,73

      //    if (errorlist[182] || errorlist[27] || errorlist[28]) {
      //      ibusTrx.write(send_oil_low);
      //      ibusTrx.available();
      //    }
      if (errorlist[336] || errorlist[63] || errorlist[142] || errorlist[147] || errorlist[265]) {
        ibusTrx.write(send_flattyre);
      }
      if (errorlist[149] || errorlist[144] || errorlist[145] || errorlist[50] || errorlist[73]) {
        ibusTrx.write(send_rdc_error);
      }
      timer_lookforerror_to_ibus = millis();
    }
  }

  if (extra_menu && (millis() - timer_extra_menu > interval_extra_menu)) {
    //Serial.println(page%Seitenanzahl);
    index = std::string(abs(page % Seitenanzahl));
    switch (abs(page % Seitenanzahl)) {

      case 1:
        // Speed Cruise control
        string_to_ibus(index, 0);
        text_to_ibus(" ACC: ", speed_set, "km/h", 1, 0);
        text_to_ibus(" ", acc_on, "", 14, 0);
        text_to_ibus("B", brake_status, "", 15, 0);
        text_to_ibus("_", dist, "", 17, 0);
        //text_to_ibus("> ", speed_ist, 4, 1);
        break;

      case 2:
        // Battery Voltage Watertemp and Oiltemp
        string_to_ibus(index, 0);
        text_to_ibus(" BAT:", battery_voltage, "V ", 1, 1);
        text_to_ibus("WT:", water_temp, "'C", 12, 0);
        break;

      case 3:
        //Oil level
        string_to_ibus(index, 0);
        string_to_ibus(" OIL", 1);
        string_to_ibus(oil2, 5);
        text_to_ibus(" ", oil1, "% ", 9, 0);
        text_to_ibus("", oil_temp, "'", 15, 0);
        break;
      case 4:
        //tank level
        string_to_ibus(index, 0);
        text_to_ibus(" T:", (float)fuel / 10, "l", 1, 1);
        text_to_ibus("RW:", range_calc, "km", 11, 0);
        break;

      case 5:  // iBus Data
        string_to_ibus(index, 0);
        string_to_ibus(" ", 1);
        string_to_ibus(gear_digit, 2);
        //text_to_ibus("k:", e31KEY_data, " ", 8, 0);
        //text_to_ibus(" ", e31LKM_data[0], "", 11, 0);
        //text_to_ibus(" ", e31LKM_data[1], "", 15, 0);
        text_to_ibus("inj:",  injection_volume , "", 6, 0);
        break;

        case 6:  // iBus Data
        string_to_ibus(index, 0);
        string_to_ibus(" ", 1);
        text_to_ibus("FreeMem",  freeMemory() , "",3, 0);
        break;


      case 0:  //Lenkwinkel

        string_to_ibus(index, 0);
        text_to_ibus(" ", lw, "'", 1, 1);
        text_to_ibus("MPG:", mpg2_ind,"", 7, 1);
        text_to_ibus("", injection_volume,"", 12, 1);
        break;
    }
    ibusTrx.write(text_iBus);

    //Abstandzeige



    timer_extra_menu = millis();
  }



*/
