// Putting startup code in setup and calling it multiple times
// seems to not work on the arduino nano
void setup() {}

void wait_for_start() {
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
  wait_for_start();
  delay(30000);
  Serial.print("End\n");
  Serial.flush();
}
