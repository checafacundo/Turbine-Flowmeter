# Turbine Flowmeter Arduino Library

Librería Arduino para la lectura de caudalímetros turbina Modbus RTU, basada en el protocolo Holykell HLY-E Type-01.

## Características

- Lectura de flujo acumulado (float y double)
- Lectura de flujo instantáneo
- Lectura de unidad de flujo
- Lectura de voltaje de batería
- Lectura combinada de todos los valores en una sola transacción Modbus
- Compatibilidad con cualquier arquitectura Arduino

## Dependencias

- [ModbusMaster](https://github.com/4-20ma/ModbusMaster)

## Instalación

### Opción 1: Arduino Library Manager

1. Abrir Arduino IDE
2. Ir a **Sketch > Include Library > Manage Libraries...**
3. Buscar **TurbineFlowmeter**
4. Hacer clic en **Install**

### Opción 2: Instalación manual

1. Descargar o clonar este repositorio
2. Copiar la carpeta `Turbine_Flowmeter` dentro de `~/Arduino/libraries/`
3. Reiniciar el Arduino IDE

## Hardware

Esta librería requiere un conversor RS485 (por ejemplo, MAX485) conectado entre el Arduino y el caudalímetro.

### Conexiones típicas (ESP32)

| Caudalímetro | MAX485 | Arduino |
|--------------|--------|---------|
| RS485 A      | A      | -       |
| RS485 B      | B      | -       |
| -            | RO     | GPIO 26 (RX) |
| -            | DI     | GPIO 25 (TX) |
| -            | DE     | GPIO 32 |
| -            | RE     | GPIO 33 |
| VCC          | VCC    | 5V/3.3V |
| GND          | GND    | GND     |

## Uso

```cpp
#include "TurbineFlowmeter.h"

#define MAX485_DE     32
#define MAX485_RE     33
#define FLOWMETER_ID   1

HardwareSerial SerialEmulado(1);

void setup() {
  Serial.begin(115200);
  SerialEmulado.begin(9600, SERIAL_8N1, 25, 26);
  flowmeterBegin(FLOWMETER_ID, SerialEmulado, MAX485_DE, MAX485_RE);
}

void loop() {
  // Lectura combinada
  float cumulativeFlow, instantaneousFlow, batteryVoltage;
  uint16_t unit;

  if (readAllFlowData(cumulativeFlow, instantaneousFlow, unit, batteryVoltage)) {
    Serial.print("Flujo acumulado: ");
    Serial.println(cumulativeFlow, 4);
    Serial.print("Flujo instantáneo: ");
    Serial.print(instantaneousFlow, 4);
    Serial.print(" ");
    Serial.println(flowUnitToString(unit));
    Serial.print("Batería: ");
    Serial.print(batteryVoltage, 2);
    Serial.println(" V");
  }

  delay(1000);
}
```

## API

### Funciones principales

| Función | Descripción |
|---------|-------------|
| `flowmeterBegin(id, serial, DE, RE)` | Inicializa el caudalímetro |
| `readAllFlowData(cf, isf, unit, bv)` | Lee todos los valores en una transacción |
| `readCumulativeFlowDouble(value)` | Lee flujo acumulado (double) |
| `readCumulativeFlowFloat(value)` | Lee flujo acumulado (float) |
| `readInstantaneousFlow(value)` | Lee flujo instantáneo |
| `readInstantaneousFlowUnit(unit)` | Lee la unidad de flujo |
| `readBatteryVoltage(value)` | Lee voltaje de batería |
| `flowUnitToString(unit)` | Convierte código de unidad a string |
| `printFlowData()` | Imprime todos los valores por Serial |

### Unidades de flujo

| Código | Unidad |
|--------|--------|
| 0 | m³/h |
| 1 | L/h |
| 2 | L/min |
| 3 | US Gal/min |
| 4 | UK Gal/min |
| 5 | US Gal/h |
| 6 | UK Gal/h |
| 7 | kg/h |
| 8 | t/h |
| 9 | ft³/h |

## Licencia

MIT License - Ver [LICENSE](LICENSE) para más detalles.
