#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <cstring>

#define OUTPUT 0
#define INPUT 1
#define LOW 0
#define HIGH 1
#define pinMode(pin, mode)
#define digitalWrite(pin, value)
#define digitalRead(pin) 1
#define analogRead(pin) 1
#define DEC 10
#define HEX 16

#define A0 0
#define A1 0
#define A4 0
#define A5 0
#define A7 0

#define bitRead(byte, index) (((unsigned)(byte) >> (index)) & 1)

static const auto startTime = std::chrono::high_resolution_clock::now();
uint32_t millis() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count(); }


struct serial_stub
{
    void begin(size_t baud){}

    template<typename T, typename... Params>
    void print(const T& t, Params... p) { std::cout << t; }

    template<typename T, typename... Params>
    void println(const T& t, Params... p) { std::cout << t << "\n"; }

    void println() { std::cout << "\n"; }
};
static serial_stub Serial;
static serial_stub Serial3;


