// Abyss black
const Color c_black(0, 0, 0, 0);
// Hair orange
const Color c_hair(255, 20, 0, 30);
// Sky light blue
const Color c_sky(0, 0, 100, 255);
// Beautiful violet
const Color c_violet(255, 0, 200, 255);
// Natural green
const Color c_green(100, 255, 50, 255);
// Special violet
const Color c_special(255, 0, 255, 50);

// One beat
// 113 beat/min = 1.883333333 beat/sec = 0.530973451 sec/beat
const uint32_t step_time = 531;

bool odd(uint32_t time, uint32_t period) {
  return (time / period) % 2;
}

template<typename T>
void swap(T& l, T& r, T a, T b, uint32_t time, uint32_t period) {
    if (odd(time, period)) {
      l = a;
      r = b;
    } else {
      l = b;
      r = a;
    }
}

template<typename T>
void alternate(T& l, T& r, T a, T b, uint32_t time, uint32_t period) {
    if (odd(time, period)) {
      l = a;
      r = a;
    } else {
      l = b;
      r = b;
    }
}

// 0.0 - 5.3 Normal
// Special
// 6.3 - 9.2 Normal
// Special
// 10.2 - 14 Normal
// Special
// 15.0 - 17.7 Normal
// Voice starts
// 18.7 - 20.5 Normal
// LoooOOOOooove
// 23.0 - 30.0 Normal

void sequence() {
  const uint32_t start_time = millis();
  uint32_t time = 0;

  Color color_simple[2] = { Color(0, 0, 0, 0), Color(0, 0, 0, 0) };
  Color color_moving[2] = { Color(0, 0, 0, 0), Color(0, 0, 0, 0) };
  Direction directions[2] = { Direction(0, 0), Direction(0, 0) };

  while(time < 30000) {
    bool is_special = (5300 < time) && (time < 6300)
      || (9200 < time) && (time < 10200)
      || (14000 < time) && (time < 15000)
      || (17700 < time) && (time < 18700);
    bool is_love = (20500 < time) && (time < 23000);

    if (is_special) {
      swap(color_simple[0], color_simple[1], c_special, c_hair, time, step_time);
      swap(color_moving[0], color_moving[1], c_hair, c_special, time, step_time);
    } else if (is_love) {
      // LoooooOOOOooove
      auto color = Color(255, 10, 0, 20);
      swap(color_simple[0], color_simple[1], c_hair, color, time, step_time);
      swap(color_moving[0], color_moving[1], color, c_hair, time, step_time);
    } else {
      swap(color_simple[0], color_simple[1], c_green, c_black, time, step_time);
      swap(color_moving[0], color_moving[1], c_black, c_violet, time, step_time);
    }

    alternate(directions[0].pan, directions[0].pan, 20U, 230U, time, step_time * 4);
    alternate(directions[1].pan, directions[1].pan, 130U, 270U, time, step_time * 4);
    alternate(directions[0].tilt, directions[1].tilt, 0U, 180U, time, step_time * 2);

    simple_0.set_color(color_simple[0]);
    simple_1.set_color(color_simple[1]);

    moving_0.set_color(color_moving[0]);
    moving_1.set_color(color_moving[1]);
    moving_0.pan(directions[0].pan);
    moving_0.tilt(directions[0].tilt);
    moving_1.pan(directions[1].pan);
    moving_1.tilt(directions[1].tilt);

    // Use triangle function for dimming (period is step_time)
    double time_f = time;
    double step_time_f = step_time;
    uint8_t dimming = static_cast<uint8_t>(2 * abs(time_f / step_time_f - floor(time_f / step_time_f + 0.5)) * 256.0);
    simple_0.set_dimming(dimming);
    simple_1.set_dimming(dimming);
    moving_0.set_dimming(dimming);
    moving_1.set_dimming(dimming);

    time = millis() - start_time;
  }
}