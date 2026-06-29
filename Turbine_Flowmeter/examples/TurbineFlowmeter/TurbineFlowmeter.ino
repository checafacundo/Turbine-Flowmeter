#include "TurbineFlowmeter.h"

// [DEFINES]
#define MAX485_DE     32
#define MAX485_RE     33
#define FLOWMETER_ID   1   // Modbus slave address of the turbine flowmeter

// [SERIAL]
HardwareSerial SerialEmulado(1);

// [SETUP]
void setup() {
  Serial.begin(115200);
  SerialEmulado.begin(9600, SERIAL_8N1, 25, 26);   // default baud rate per datasheet
  flowmeterBegin(FLOWMETER_ID, SerialEmulado, MAX485_DE, MAX485_RE);

  delay(100);
}

// [LOOP]
void loop() {

  // ----------------------------------------------------------------
  // OPTION 1: Combined read — Cumulative Flow, Instantaneous Flow,
  // Unit and Battery Voltage in a SINGLE Modbus transaction.
  // This is the fastest way to get everything at once.
  // ----------------------------------------------------------------
  float cumulativeFlow, instantaneousFlow, batteryVoltage;
  uint16_t unit;

  if (readAllFlowData(cumulativeFlow, instantaneousFlow, unit, batteryVoltage)) {
    Serial.println("=== readAllFlowData() ===");
    Serial.print("Cumulative Flow: ");
    Serial.println(cumulativeFlow, 4);
    Serial.print("Instantaneous Flow: ");
    Serial.print(instantaneousFlow, 4);
    Serial.print(" ");
    Serial.println(flowUnitToString(unit));
    Serial.print("Battery Voltage: ");
    Serial.print(batteryVoltage, 2);
    Serial.println(" V");
  } else {
    Serial.println("CAUDALIMETRO DESCONECTADO (readAllFlowData)");
  }

  // ----------------------------------------------------------------
  // OPTION 2: printFlowData() — same as above, but the library does
  // the read + Serial printing for you in one call.
  // ----------------------------------------------------------------
  Serial.println();
  Serial.println("=== printFlowData() ===");
  printFlowData();

  // ----------------------------------------------------------------
  // OPTION 3: Individual reads — one Modbus transaction each.
  // Useful when you only need one value, or want to poll values
  // at different rates (e.g. battery voltage less often).
  // ----------------------------------------------------------------
  Serial.println();
  Serial.println("=== Individual reads ===");

  double cumulativeFlowDouble;
  if (readCumulativeFlowDouble(cumulativeFlowDouble)) {
    Serial.print("Cumulative Flow (double precision): ");
    Serial.println(cumulativeFlowDouble, 6);
  } else {
    Serial.println("Fallo leyendo Cumulative Flow (double)");
  }

  float cumulativeFlowSingle;
  if (readCumulativeFlowFloat(cumulativeFlowSingle)) {
    Serial.print("Cumulative Flow (single precision): ");
    Serial.println(cumulativeFlowSingle, 4);
  } else {
    Serial.println("Fallo leyendo Cumulative Flow (float)");
  }

  float instFlow;
  if (readInstantaneousFlow(instFlow)) {
    Serial.print("Instantaneous Flow: ");
    Serial.println(instFlow, 4);
  } else {
    Serial.println("Fallo leyendo Instantaneous Flow");
  }

  uint16_t flowUnit;
  if (readInstantaneousFlowUnit(flowUnit)) {
    Serial.print("Instantaneous Flow Unit: ");
    Serial.print(flowUnit);
    Serial.print(" (");
    Serial.print(flowUnitToString(flowUnit));
    Serial.println(")");
  } else {
    Serial.println("Fallo leyendo Instantaneous Flow Unit");
  }

  float battery;
  if (readBatteryVoltage(battery)) {
    Serial.print("Battery Voltage: ");
    Serial.print(battery, 2);
    Serial.println(" V");
  } else {
    Serial.println("Fallo leyendo Battery Voltage");
  }

  Serial.println("--------------------------------------------------");
  delay(1000);
}
