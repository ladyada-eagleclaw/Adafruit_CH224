#include <Adafruit_CH224.h>

Adafruit_CH224 ch224;
Adafruit_CH224A ch224a;
Adafruit_CH224Q ch224q;
bool ch224Ready = false;

const uint16_t voutSensePin = A0;
const float dividerTopOhms = 10000.0;
const float dividerBottomOhms = 560.0;
const float voltageTolerance = 1.0;

float readVoutVoltage() {
  uint32_t adcTotal = 0;
  const uint8_t sampleCount = 16;

  for (uint8_t sample = 0; sample < sampleCount; sample++) {
    adcTotal += analogRead(voutSensePin);
    delay(5);
  }

  uint16_t adcValue = adcTotal / sampleCount;
  float senseVolts = map(adcValue, 0, 1023, 0, 5000) / 1000.0;
  float dividerRatio =
      (dividerTopOhms + dividerBottomOhms) / dividerBottomOhms;
  return senseVolts * dividerRatio;
}

void clearOutputsAndHalt(const char *message) {
  Serial.println(message);

  if (ch224Ready) {
    if (ch224.setVoltage(CH224_VOLTAGE_5V)) {
      Serial.println("Returned CH224 request to 5 V");
    } else {
      Serial.println("Could not return CH224 request to 5 V");
    }
  }

  while (1) {
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("CH224 hardware test 03_variant_guard");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A/Q begin failed");
  }
  ch224Ready = true;
  pinMode(voutSensePin, INPUT);

  Serial.print("Begin succeeded at I2C address 0x");
  Serial.println(ch224.getI2CAddress(), HEX);
  Serial.print("Detected variant: CH224");
  Serial.println(ch224.getVariant() == CH224_VARIANT_A ? "A" : "Q");

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("Initial 5 V request failed");
  }
  delay(1000);

  if (ch224.getVariant() == CH224_VARIANT_A) {
    if (ch224.supportsPPS() || ch224.supportsAVS()) {
      clearOutputsAndHalt("CH224A incorrectly reports Q-only support");
    }
    Serial.println("CH224A reports no PPS or AVS control support");

    if (!ch224a.begin()) {
      clearOutputsAndHalt("CH224A subclass did not accept CH224A hardware");
    }
    Serial.println("CH224A subclass accepted CH224A hardware");

    if (ch224q.begin()) {
      clearOutputsAndHalt("CH224Q subclass accepted CH224A hardware");
    }
    Serial.println("CH224Q subclass rejected CH224A hardware");

    if (ch224q.setAVSVoltage(15.0)) {
      clearOutputsAndHalt("CH224A accepted an AVS request");
    }
    Serial.println("CH224A AVS request was safely rejected");

    float measuredVolts = readVoutVoltage();
    Serial.print("Measured VOUT after rejected request: ");
    Serial.print(measuredVolts, 2);
    Serial.println(" V");
    if (abs(measuredVolts - 5.0) > voltageTolerance) {
      clearOutputsAndHalt("Rejected AVS request changed VOUT");
    }
    Serial.println("VOUT remained at 5 V");
  } else {
    if (!ch224.supportsPPS() || !ch224.supportsAVS()) {
      clearOutputsAndHalt("CH224Q does not report Q-only support");
    }
    Serial.println("CH224Q reports PPS and AVS control support");

    if (ch224a.begin()) {
      clearOutputsAndHalt("CH224A subclass accepted CH224Q hardware");
    }
    Serial.println("CH224A subclass rejected CH224Q hardware");

    if (!ch224q.begin()) {
      clearOutputsAndHalt("CH224Q subclass did not accept CH224Q hardware");
    }
    Serial.println("CH224Q subclass accepted CH224Q hardware");
    Serial.println("No programmable voltage was requested");
  }

  Serial.println("03_variant_guard passed");
}

void loop() { delay(1000); }
