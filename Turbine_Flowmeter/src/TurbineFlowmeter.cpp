#include "TurbineFlowmeter.h"
#include <string.h>

static uint8_t _DE_pin = 255;
static uint8_t _RE_pin = 255;

// Define the ModbusMaster instance
ModbusMaster node;

// Pre-transmission callback: put the RS485 transceiver into transmit mode
static void preTransmission() {
  if (_DE_pin != 255 && _RE_pin != 255) {
    digitalWrite(_DE_pin, HIGH);
    digitalWrite(_RE_pin, HIGH);
  }
}

// Post-transmission callback: back to receive mode
static void postTransmission() {
  if (_DE_pin != 255 && _RE_pin != 255) {
    digitalWrite(_DE_pin, LOW);
    digitalWrite(_RE_pin, LOW);
  }
}

// Initialize the flowmeter (Modbus RTU, slave address, RS485 control pins)
void flowmeterBegin(uint8_t slaveID, HardwareSerial &serialPort, uint8_t DE_pin, uint8_t RE_pin) {
  _DE_pin = DE_pin;
  _RE_pin = RE_pin;

  pinMode(_DE_pin, OUTPUT);
  pinMode(_RE_pin, OUTPUT);
  digitalWrite(_DE_pin, LOW);
  digitalWrite(_RE_pin, LOW);

  node.begin(slaveID, serialPort);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

// ----------------------------------------------------------------
//  Helpers: the protocol stores IEEE-754 floats/doubles across
//  consecutive 16-bit registers, ordered HIGH word to LOW word
//  (verified against the datasheet's own example: regs 0x4284,0x0000 = 66.0f)
// ----------------------------------------------------------------
static float wordsToFloat(uint16_t hiWord, uint16_t loWord) {
  uint32_t raw = ((uint32_t)hiWord << 16) | (uint32_t)loWord;
  float value;
  memcpy(&value, &raw, sizeof(value));
  return value;
}

static double wordsToDouble(uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3) {
  uint64_t raw = ((uint64_t)w0 << 48) | ((uint64_t)w1 << 32) |
                 ((uint64_t)w2 << 16) | (uint64_t)w3;
  double value;
  memcpy(&value, &raw, sizeof(value));
  return value;
}

// ----------------------------------------------------------------
//                          READ FUNCTIONS
// ----------------------------------------------------------------
bool readCumulativeFlowDouble(double &value) {
  if (node.readHoldingRegisters(CUMULATIVE_FLOW_DOUBLE_ADDRESS, 4) != node.ku8MBSuccess)
    return false;

  value = wordsToDouble(node.getResponseBuffer(0), node.getResponseBuffer(1),
                         node.getResponseBuffer(2), node.getResponseBuffer(3));
  return true;
}

bool readCumulativeFlowFloat(float &value) {
  if (node.readHoldingRegisters(CUMULATIVE_FLOW_FLOAT_ADDRESS, 2) != node.ku8MBSuccess)
    return false;

  value = wordsToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
  return true;
}

bool readInstantaneousFlow(float &value) {
  if (node.readHoldingRegisters(INSTANTANEOUS_FLOW_ADDRESS, 2) != node.ku8MBSuccess)
    return false;

  value = wordsToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
  return true;
}

bool readInstantaneousFlowUnit(uint16_t &unit) {
  if (node.readHoldingRegisters(INSTANTANEOUS_FLOW_UNIT_ADDRESS, 1) != node.ku8MBSuccess)
    return false;

  unit = node.getResponseBuffer(0);
  return true;
}

bool readBatteryVoltage(float &value) {
  if (node.readHoldingRegisters(BATTERY_VOLTAGE_ADDRESS, 2) != node.ku8MBSuccess)
    return false;

  value = wordsToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
  return true;
}

// Reads Cumulative Flow (float), Instantaneous Flow, Unit and Battery
// Voltage in a SINGLE Modbus transaction (registers 0x0004-0x000A) — the
// same grouped-register read style shown in the Holykell communication
// example, just extended to cover all four values in one request.
bool readAllFlowData(float &cumulativeFlow, float &instantaneousFlow,
                      uint16_t &unit, float &batteryVoltage) {
  if (node.readHoldingRegisters(CUMULATIVE_FLOW_FLOAT_ADDRESS, 7) != node.ku8MBSuccess)
    return false;

  cumulativeFlow    = wordsToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
  instantaneousFlow = wordsToFloat(node.getResponseBuffer(2), node.getResponseBuffer(3));
  unit              = node.getResponseBuffer(4);
  batteryVoltage    = wordsToFloat(node.getResponseBuffer(5), node.getResponseBuffer(6));

  return true;
}

const char* flowUnitToString(uint16_t unit) {
  switch (unit) {
    case UNIT_M3_H:       return "m3/h";
    case UNIT_L_H:        return "L/h";
    case UNIT_L_MIN:      return "L/min";
    case UNIT_US_GAL_MIN: return "US Gal/min";
    case UNIT_UK_GAL_MIN: return "UK Gal/min";
    case UNIT_US_GAL_H:   return "US Gal/h";
    case UNIT_UK_GAL_H:   return "UK Gal/h";
    case UNIT_KG_H:       return "kg/h";
    case UNIT_T_H:        return "t/h";
    case UNIT_FT3_H:      return "ft3/h";
    default:              return "unknown";
  }
}

void printFlowData() {
  float cumulativeFlow, instantaneousFlow, batteryVoltage;
  uint16_t unit;

  if (readAllFlowData(cumulativeFlow, instantaneousFlow, unit, batteryVoltage)) {
    Serial.println();
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
    Serial.println("FALLO DE LECTURA DEL CAUDALIMETRO");
  }
}