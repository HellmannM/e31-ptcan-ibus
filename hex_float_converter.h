#pragma once

struct float_hex_converter
{
    unsigned char chars[2];

    void float_to_fake_hex(float finput)
    {
        int input = (int)(finput * 1.f);
        chars[1] =     (input       ) % 10
                   + (((input / 10  ) % 10) * 16);
        chars[0] =     (input / 100 ) % 10
                   + (((input / 1000) % 10) * 16);
    }

    float fake_hex_to_float(unsigned char* c)
    {
        return ((float)((((c[0] / 16) % 10) * 10) + (c[0] % 16)) * 10.f)
             + ((float)((((c[1] / 16) % 10) * 10) + (c[1] % 16)) / 10.f);
    }
};
