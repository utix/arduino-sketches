#ifndef Morse_h
#define Morse_h

#include "Arduino.h"

/* Some basics color
 * Adapt for a common resistor
 *
 * Best result if each color has its resistor
 * 110 Ω for red
 * 100 Ω for green and blue
 */
#define RED    0xf00
#define AQUA   0x0ff
#define ORANGE 0x781
#define GREEN  0x0f0

/* A very simple class to handle rgb led
 * with common anode
 */
class Rgb
{
  public:
    /*
     * \param[in] pin_r   the red   pin
     * \param[in] pin_g   the green pin
     * \param[in] pin_b   the blue  pin
     *
     * on init those colors are displayed 500ms
     * RED, AQUA, GREEN, ORANGE
     */
    Rgb(int pin_r, int pin_g, int pin_b);
    /*
     * \param[in] color   color on 24 bit,
     * for example 0xf00 for red
     *             0x0f0 for green
     *
     */
    void color(unsigned int color);

  private:
    byte pin_r;
    byte pin_g;
    byte pin_b;
};

#endif
