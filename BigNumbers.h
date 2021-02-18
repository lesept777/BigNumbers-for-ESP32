/*
    Big Numbers library for ESP32

    A big number is defined as follows:
    mantissa 10^( expo_0 10^(expo_1 10^...))
    where mantissa, expo_0, expo_1 ... expo_n are floating
    point numbers. n is the number of floors of the big number.

    This library enables to apply some of the standard operations
    to the big numbers: addition, multiplication, exponentianation,
    factorial, comparison (< == > !=)

    The maximum number of floors is defined below as MAXFLOORS and
    can be changed

    (c) 2021 Lesept
    contact: lesept777@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef Big_h
#define Big_h

#include <Arduino.h>
// #include "utils.h"
#define MAXPREC 15 // Maximum number of significant digits (<= 18)

/*
    For maximum precision, all the values are stored as uint64
    All values, except the last floor's exponent are normalized
    between 1 and 10. To keep them readable, the stored value is
    the real value multiplied by 1e16
    Example:
    1000000000000000 means 1
    1234560000000000 means 1.23456
    1234560000000010 means 1.23456000000001 (accuracy kept)
*/
#define MAXFLOORS 10

class bigNumber
{
  public:
    bigNumber();
    ~bigNumber();

// Methods to display the Big Numbers
    void displayBigNumber(uint8_t = 8);
    void displayStruct();

// Create a big number from it mantissa and set of exponents
    void set(double, double*, int);
    void set (double, std::vector<double> const &, int);
    void parseNumber(char*);

// Overloaded operators
    bool operator== (bigNumber const&) const;
    bool operator!= (bigNumber const&) const;
    bool operator<  (bigNumber const&) const;
    bool operator<= (bigNumber const&) const;
    bool operator>  (bigNumber const&) const;
    bool operator>= (bigNumber const&) const;
    void operator+= (bigNumber &);
    void operator*= (bigNumber &);

    void bigPower    (bigNumber &);
    void bigStirling (bigNumber &);

    bigNumber extract(uint8_t, uint8_t = 0);
    void push (bigNumber &, uint8_t);


  private:
    int64_t  mant = 0;
    int64_t  expo[MAXFLOORS] = {0};
    uint8_t  floors = 0;
    uint8_t  l_mant = 0;
    uint8_t  l_exp[MAXFLOORS] = {0};

    void XXdisp (int64_t, uint8_t, bool);
};


inline bigNumber operator+(bigNumber & N1, bigNumber & N2) {
    bigNumber N = N1.extract(0);
    N += N2;
    return N;
}

inline bigNumber operator*(bigNumber & N1, bigNumber & N2) {
    bigNumber N = N1.extract(0);
    N *= N2;
    return N;
}

inline bigNumber power (bigNumber & N1, bigNumber & N2) {
    bigNumber N = N1.extract(0);
    N.bigPower(N2);
    return N;
}

inline bigNumber stirling (bigNumber & N1) {
    bigNumber N;
    N.bigStirling(N1);
    return N;
}

#endif