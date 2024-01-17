#include "DmxSimple.h"

struct Color {
  uint8_t r = 255;
  uint8_t g = 255;
  uint8_t b = 255;
  uint8_t w = 255;

  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) 
    : r(r), g(g), b(b), w(w) {}
};

struct Direction {
  // 0..540
  uint16_t pan;
  // 0..180
  uint16_t tilt;

  Direction(uint16_t pan, uint16_t tilt)
    : pan(pan), tilt(tilt) {}
};

struct SimpleLight {
  uint16_t offset;

  void set_dimming(uint8_t value) {
    write(1, value);
  }

  void set_color(Color color) {
    write(2, color.r);
    write(3, color.g);
    write(4, color.b);
    write(5, color.w);
  }

  void write(uint8_t channel, uint8_t value) {
    DmxSimple.write(offset + channel, value);
  }
};

struct MovingHead {
  uint16_t offset;

  void reset() {
    write(14, 255);
  }

  void set_dimming(uint8_t value) {
    write(6, map(value, 0, 255, 8, 134));
  }

  // Lower is faster
  // Input: 0..255
  // Writes: 0..255
  void set_speed(uint8_t speed) {
    write(5, speed);
  }

  // Input: 0..540
  // Writes: 0..255
  void pan(uint16_t deg) {
    write(1, map(deg, 0, 540, 0, 255));
  }


  // Input: 0..180
  // Writes: 0..255
  void tilt(uint16_t deg) {
    write(3, map(deg, 0, 180, 0, 255));
  }

  // Input: 0..255
  // Writes: 135..239
  void set_strobe(uint8_t speed) {
    write(6, map(speed, 0, 255, 135, 239));
  }

  void set_color(Color color) {
    write(7, color.r);
    write(8, color.g);
    write(9, color.b);
    write(10, color.w);
  }

  void write(uint8_t channel, uint8_t value) {
    DmxSimple.write(offset + channel, value);
  }
};

SimpleLight simple_0 = { .offset = 0 };
SimpleLight simple_1 = { .offset = 36 };

MovingHead moving_0 = { .offset = 8 };
MovingHead moving_1 = { .offset = 22 };

void setup() {
  DmxSimple.usePin(3);
  DmxSimple.maxChannel(44);
}

void reset() {
  moving_0.reset();
  moving_1.reset();
  moving_0.set_speed(0);
  moving_1.set_speed(0);

  moving_0.set_dimming(0);
  moving_1.set_dimming(0);
  simple_0.set_dimming(0);
  simple_1.set_dimming(0);
}

void wait_for_start() {
  Serial.begin(115200, SERIAL_8N1);
  // Wait for serial port to connect
  while (!Serial);

  while (true) {
    auto received = Serial.readString();
    if (received.startsWith("Start")) break;
    Serial.print("Waiting for start signal, got '");
    Serial.print(received);
    Serial.print("'\n");
  }
  Serial.print("Starting\n");
}

void loop() {
  reset();
  wait_for_start();
  sequence();
  Serial.print("End\n");
  Serial.flush();
}
