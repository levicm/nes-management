/**
 * Código principal do Nodo Sensor que contém basicamente as 
 * funções setup() e loop() e invoca os demais códios dos agentes.
 */
#define DEBUG
#define SLEEP_MODE
#include "SystemInfo.h"
#include "Sensors.h"
#include "Debug.h"

// Habilitar cada agente desejado:
#define SNMP_AGENT
#define ZABBIX_AGENT
#define MQTT_AGENT

#if defined(MQTT_AGENT)
  #include <PubSubClient.h>
#endif

long lastMillis = 0;

void setup() {
  Serial.begin(115200); //Iniciando Serial
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN2("Nodo ligado apos: ", ESP.getResetReason());

  systemInfo.begin();
  sensors.begin();

  connectWifi(); // Conectando a alguma rede WiFi

  // Agente SNMP
  #if defined(SNMP_AGENT)
  setupSnmpAgent();
  #endif
  // Agente Zabbix
  #if defined(ZABBIX_AGENT)
  setupZabbixAgent();
  #endif
  // Agente MQTT
  #if defined(MQTT_AGENT)
  setupMqttAgent();
  #endif
}

void loop() {
//  printSensors();

  // Agente SNMP
  #if defined(SNMP_AGENT)
  loopSnmpAgent();
  #endif
  // Agente Zabbix
  #if defined(ZABBIX_AGENT)
  loopZabbixAgent();
  #endif
  // Agente Zabbix
  #if defined(MQTT_AGENT)
  loopMqttAgent();
  #endif
}

void printSensors() {
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    Serial.print("Sensores ==> Temperatura: ");
    Serial.print(sensors.getTemperature());
    Serial.print("*C\tHumidade: ");
    Serial.print(sensors.getHumidity());
    Serial.print("%\tIndice de calor: ");
    Serial.print(sensors.getHeatIndex());
    Serial.println();

    Serial.print("Sistema ==> Chip ID: ");
    Serial.print(systemInfo.getChipId());
    Serial.print("\tVin: ");
    Serial.print(systemInfo.getVin());
    Serial.print("V");
    Serial.print("\tFree Heap: ");
    Serial.print(systemInfo.getFreeHeap());
    Serial.print("\tChip temp *C: ");
    Serial.print(systemInfo.getChipTemp());
    Serial.print("\tUptime: ");
    Serial.print(systemInfo.getUptime());
    Serial.println();
  }
}

