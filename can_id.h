#pragma once

enum can_id
{
    brake_state         = 0xA8,  // Bremse Status
    engine_start        = 0xAA,  // Engine start?
    brakelight_acc_req  = 0xD5,  // Bremslichterleuchten bei ACC Bremsanforderung
    fuehrungsfahrzeug_E = 0x193, // Anzeige Führungsfahrzeug als 'E'
    display_gears       = 0x1D2, // Ganganzeige S, M, ' ', PRND, 123456
    error_min           = 0x580, // Start of errors range
    error_max           = 0x671  // End of errors range
};
