#include "Arduino.h"
#include "rgb.h"


Rgb::Rgb(int pin_r, int pin_g, int pin_b) {
  this->pin_r = pin_r;
  this->pin_g = pin_g;
  this->pin_b  = pin_b;

  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);

  this->color(RED);
  delay(500);
  this->color(AQUA);
  delay(500);
  this->color(GREEN);
  delay(500);
  this->color(ORANGE);
  delay(500);
  this->color(0);
}

void Rgb::color(unsigned int color) {
  color = ~color;
  analogWrite(this->pin_r, 17 * ((color >> 8) & 0xf));
  analogWrite(this->pin_g, 17 * ((color >> 4) & 0xf));
  analogWrite(this->pin_b, 17 * ((color >> 0) & 0xf));
}
