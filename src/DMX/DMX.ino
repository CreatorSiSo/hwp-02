#include "DmxSimple.h"

struct Color {
  uint8_t r = 255;
  uint8_t g = 255;
  uint8_t b = 255;
  uint8_t w = 255;

  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) 
    : r(r), g(g), b(b), w(w) {}

  static Color lerp(Color a, Color b, double t) {
    uint8_t t_int = static_cast<uint8_t>(t * 256.0);
    return Color(
      map(t_int, 0, 255, a.r, b.r),
      map(t_int, 0, 255, a.g, b.g),
      map(t_int, 0, 255, a.b, b.b),
      map(t_int, 0, 255, a.w, b.w)
    );
  }
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

  // void use_blending(bool value) {
  //   if (value)
  //     write(7, 200); // gradient
  //   else
  //     write(7, 150); // jump
  // }

  // void set_blend_speed(uint8_t speed) {
  //   write(8, speed);
  // }

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

  void light_on() {
    write(6, 255);
  }

  void light_off() {
    write(6, 0);
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
    // 0.0 .. 1.0
    double percantage = (double)deg / 540.0;
    write(1, (uint8_t)(percantage * 255.0));
  }


  // Input: 0..180
  // Writes: 0..255
  void tilt(uint16_t deg) {
    // 0.0 .. 1.0
    double percantage = (double)deg / 180.0;
    write(3, (uint8_t)(percantage * 255.0));
  }

  // Input: 0..104
  // Writes: 135..239
  void strobe(uint8_t speed) {
    write(6, 135 + speed);
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

// Putting startup code in setup and calling it multiple times
// seems to not work on the arduino nano
void setup() {
  DmxSimple.usePin(3);
  DmxSimple.maxChannel(44);
}

void start() {
  moving_0.reset();
  moving_1.reset();
  moving_0.set_speed(0);
  moving_1.set_speed(0);
  moving_0.light_off();
  moving_1.light_off();

  simple_0.set_dimming(0);
  simple_1.set_dimming(0);

  Serial.begin(115200, SERIAL_8N1);
  // wait for serial port to connect. Needed for native USB
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
  start();
  sequence();
  Serial.print("End\n");
  Serial.flush();
}
