#pragma once

// CAN Data; Message for clear DTC
static int          CLEARDTC_ID       = 0x6F1;
static uint8_t      CLEARDTC_DATA[8]  = {0x12, 0x03, 0x14, 0xFF, 0xFF, 0x00,0x00,0x00};
static const int    CLEARDTC_LENGTH   = 8;

static int          CLEARDSC_ID       = 0x6F1;
static uint8_t      CLEARDSC_DATA[8]  = {0x29, 0x03, 0x14, 0xFF, 0xFF, 0x00,0x00,0x00};
static const int    CLEARDSC_LENGTH   = 8;


// CAN Data; Message for MPG reset
static int          RESET_MPG_ID       = 0x2B8;
static uint8_t      RESET_MPG_DATA[6]  = {0x31, 0x00, 0xF0, 0x00, 0x00, 0xE3};
static const int    RESET_MPG_LENGTH   = 6;
