
#ifndef TURBINE_FLOWMETER_H
#define TURBINE_FLOWMETER_H

#include <Arduino.h>
#include <ModbusMaster.h>

// ---- Modbus instance (shared across library) ----
extern ModbusMaster node;

// ---- Turbine Flowmeter register map (Holykell HLY-E Type-01 protocol) ----
// NOTE: every register below is READ-ONLY, accessed with function code 0x03.
// This device has no configuration/write registers like the level radar did.
#define CUMULATIVE_FLOW_DOUBLE_ADDRESS    0x0000  // 4 words, double (high precision)
#define CUMULATIVE_FLOW_FLOAT_ADDRESS     0x0004  // 2 words, float
#define INSTANTANEOUS_FLOW_ADDRESS        0x0006  // 2 words, float
#define INSTANTANEOUS_FLOW_UNIT_ADDRESS   0x0008  // 1 word,  unsigned int
#define BATTERY_VOLTAGE_ADDRESS           0x0009  // 2 words, float

// ---- Instantaneous flow unit codes (register 0x0008) ----
#define UNIT_M3_H        0
#define UNIT_L_H         1
#define UNIT_L_MIN       2
#define UNIT_US_GAL_MIN  3
#define UNIT_UK_GAL_MIN  4
#define UNIT_US_GAL_H    5
#define UNIT_UK_GAL_H    6
#define UNIT_KG_H        7
#define UNIT_T_H         8
#define UNIT_FT3_H       9

// ---- Initialization ----
void flowmeterBegin(uint8_t slaveID, HardwareSerial &serialPort, uint8_t DE_pin, uint8_t RE_pin);

// ---- Individual reads (one Modbus transaction each) ----
bool readCumulativeFlowDouble(double &value);
bool readCumulativeFlowFloat(float &value);
bool readInstantaneousFlow(float &value);
bool readInstantaneousFlowUnit(uint16_t &unit);
bool readBatteryVoltage(float &value);

// ---- Combined read: Cumulative Flow, Instantaneous Flow, Unit and Battery
//      Voltage in a SINGLE Modbus transaction (registers 0x0004-0x000A) ----
bool readAllFlowData(float &cumulativeFlow, float &instantaneousFlow,
                      uint16_t &unit, float &batteryVoltage);

// ---- Helpers ----
const char* flowUnitToString(uint16_t unit);
void printFlowData();

#endif