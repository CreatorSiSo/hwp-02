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

// One beat
// 113 beat/min = 1.883333333 beat/sec = 0.530973451 sec/beat
const uint32_t step_time = 531;

bool odd(uint32_t time, uint32_t part_time) {
  return (time / part_time) % 2;
}

template<typename T>
void swap(T& l, T& r, T a, T b, uint32_t time, uint32_t part_time) {
    if (odd(time, part_time)) {
      l = a;
      r = b;
    } else {
      l = b;
      r = a;
    }
}

template<typename T>
void alternate(T& l, T& r, T a, T b, uint32_t time, uint32_t part_time) {
    if (odd(time, part_time)) {
      l = a;
      r = a;
    } else {
      l = b;
      r = b;
    }
}

// 0 - 1.5 Intro
// 1.5 - 4 Normal
// 8.5 Special
// 9 - 13.5 Normal
// 13.5 Special
// 14 - 17.5 Normal
// 17.5 Special
// 18 - 20 Voice
// 20 - 22 LoooOOOOooove

void sequence() {
  uint32_t start_time = millis();
  uint32_t time = 0;

  moving_0.light_on();
  moving_1.light_on();
  simple_0.set_dimming(255);
  simple_1.set_dimming(255);

  Color color_simple[2] = { Color(0, 0, 0, 0), Color(0, 0, 0, 0) };
  Color color_moving[2] = { Color(0, 0, 0, 0), Color(0, 0, 0, 0) };
  Direction directions[2] = { Direction(0, 0), Direction(0, 0) };

  while(time < 33000) {
    bool is_special = (5300 < time) && (time < 6300)
      || (9200 < time) && (time < 10200)
      || (14000 < time) && (time < 15000)
      || (17700 < time) && (time < 18700);

    if (is_special) {
      swap(color_simple[0], color_simple[1], Color(255, 0, 255, 50), c_hair, time, step_time);
      swap(color_moving[0], color_moving[1], c_hair, Color(255, 0, 255, 50), time, step_time);
    } else if ((20500 < time) && (time < 23000)) {
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
    alternate(directions[0].tilt, directions[1].tilt, 0U, 30U, time, step_time * 2);

    simple_0.set_color(color_simple[0]);
    simple_1.set_color(color_simple[1]);

    moving_0.set_color(color_moving[0]);
    moving_1.set_color(color_moving[1]);
    moving_0.pan(directions[0].pan);
    moving_0.tilt(directions[0].tilt);
    moving_1.pan(directions[1].pan);
    moving_1.tilt(directions[1].tilt);

    double time_f = time;
    double step_time_f = step_time;
    uint8_t dimming = time > 30000 
      ? map(time, 30000, 33000, 255, 0)
      : static_cast<uint8_t>(2 * abs(time_f / step_time_f - floor(time_f / step_time_f + 0.5)) * 256.0);

    simple_0.set_dimming(dimming);
    simple_1.set_dimming(dimming);
    moving_0.set_dimming(dimming);
    moving_1.set_dimming(dimming);

    time = millis() - start_time;
  }
}