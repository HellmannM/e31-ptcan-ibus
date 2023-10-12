#pragma once

// gear_0:
// ' ':
// 'S': 0x78 ?? ?? ?? 0xF1
// 'M': 0x78 ?? ?? ?? 0xF2
// 
// gear_1:
// '1': 0x78 0x5C ?? ?? ??
// '2': 0x78 0x6C ?? ?? ??
// '3': 0x78 0x7C ?? ?? ??
// '4': 0x78 0x8C ?? ?? ??
// '5': 0x78 0x9C ?? ?? ??
// 'D': 0x78 0xAC ?? ?? ??
// 'P': 0xE1 ?? ?? ?? ??
// 'N': 0xB4 ?? ?? ?? ??
// 'R': 0xD2 ?? ?? ?? ??
// ' ': 0xF0 ?? ?? ?? ??


#define GEAR_MASK_LENGTH 5

// digit 0:
// gear profile
const static uint8_t GEAR_S[GEAR_MASK_LENGTH]     = {0x78, 0x00, 0x00, 0x00, 0xF1};
const static uint8_t GEAR_M[GEAR_MASK_LENGTH]     = {0x78, 0x00, 0x00, 0x00, 0xF2};
// digit 1:                                  
// gear drive                                
const static uint8_t GEAR_1[GEAR_MASK_LENGTH]     = {0x78, 0x5C, 0x00, 0x00, 0x00};
const static uint8_t GEAR_2[GEAR_MASK_LENGTH]     = {0x78, 0x6C, 0x00, 0x00, 0x00};
const static uint8_t GEAR_3[GEAR_MASK_LENGTH]     = {0x78, 0x7C, 0x00, 0x00, 0x00};
const static uint8_t GEAR_4[GEAR_MASK_LENGTH]     = {0x78, 0x8C, 0x00, 0x00, 0x00};
const static uint8_t GEAR_5[GEAR_MASK_LENGTH]     = {0x78, 0x9C, 0x00, 0x00, 0x00};
const static uint8_t GEAR_D[GEAR_MASK_LENGTH]     = {0x78, 0xAC, 0x00, 0x00, 0x00};
// gear mode                                 
const static uint8_t GEAR_P[GEAR_MASK_LENGTH]     = {0xE1, 0x00, 0x00, 0x00, 0x00};
const static uint8_t GEAR_N[GEAR_MASK_LENGTH]     = {0xB4, 0x00, 0x00, 0x00, 0x00};
const static uint8_t GEAR_R[GEAR_MASK_LENGTH]     = {0xD2, 0x00, 0x00, 0x00, 0x00};
const static uint8_t GEAR_Space[GEAR_MASK_LENGTH] = {0xF0, 0x00, 0x00, 0x00, 0x00};

// S, M
const static bool GEAR_PROFILE_MASK[GEAR_MASK_LENGTH] = {1, 0, 0, 0, 1};
// 1, 2, 3, 4, 5, D
const static bool GEAR_DRIVE_MASK[GEAR_MASK_LENGTH]   = {1, 1, 0, 0, 0};
// P, N, R, ' '
const static bool GEAR_MODE_MASK[GEAR_MASK_LENGTH]    = {1, 0, 0, 0, 0};


struct gear_display
{
    char digits[2];

    void update(const uint8_t* input)
    {
        digits[0] = ' ';
        digits[1] = ' ';
        if (cmp(input, GEAR_S,     GEAR_PROFILE_MASK)) digits[0] = 'S';
        if (cmp(input, GEAR_M,     GEAR_PROFILE_MASK)) digits[0] = 'M';
        if (cmp(input, GEAR_1,     GEAR_DRIVE_MASK))   digits[1] = '1';
        if (cmp(input, GEAR_2,     GEAR_DRIVE_MASK))   digits[1] = '2';
        if (cmp(input, GEAR_3,     GEAR_DRIVE_MASK))   digits[1] = '3';
        if (cmp(input, GEAR_4,     GEAR_DRIVE_MASK))   digits[1] = '4';
        if (cmp(input, GEAR_5,     GEAR_DRIVE_MASK))   digits[1] = '5';
        if (cmp(input, GEAR_D,     GEAR_DRIVE_MASK))   digits[1] = 'D';
        if (cmp(input, GEAR_P,     GEAR_MODE_MASK))    digits[1] = 'P';
        if (cmp(input, GEAR_N,     GEAR_MODE_MASK))    digits[1] = 'N';
        if (cmp(input, GEAR_R,     GEAR_MODE_MASK))    digits[1] = 'R';
        if (cmp(input, GEAR_Space, GEAR_MODE_MASK))    digits[1] = ' ';
    }

    void set_flags(bool acc_on, bool acc_leading_veh, bool get)
    {
        if (acc_on)          digits[1] = ' '; // ACC on
        if (acc_leading_veh) digits[0] = 'E'; // ACC Error
        if (get)             digits[1] = 'X';
    }

    // checks if masked input matches table
    bool cmp(const uint8_t* input, const uint8_t* table, const bool* mask) const
    {
        for (size_t i=0; i<GEAR_MASK_LENGTH; ++i)
        {
            if (mask[i] && (input[i] != table[i]))
                return false;
        }
        return true;
    }
};
