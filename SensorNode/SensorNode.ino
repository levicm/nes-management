/**
 * Código principal do Nodo Sensor que contém basicamente as 
 * funções setup() e loop() e invoca os demais códios dos agentes.
 */
//#define DEBUG

// Habilitar cada agente desejado:
//#define SNMP_AGENT
//#define ZABBIX_AGENT_V2
//#define ZABBIX_AGENT_V3
//#define MQTT_AGENT
#if not defined(SNMP_AGENT) and not defined(ZABBIX_AGENT) // SleepMode só é compativel com MQTT, pois os demais precisam escutar
//#define SLEEP_MODE
#endif

#if defined(MQTT_AGENT)
  #include <PubSubClient.h>
#endif
#include "SystemInfo.h"
#include "Sensors.h"
#include "Debug.h"


long lastMillis = 0;
long samples = 0;

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
  #if defined(ZABBIX_AGENT) or defined(ZABBIX_AGENT_V2) or defined(ZABBIX_AGENT_V3)
  setupZabbixAgent();
  #endif
  // Agente MQTT
  #if defined(MQTT_AGENT)
  setupMqttAgent();
  #endif
}

void loop() {
//  printSensors();
  printMemory();

  // Agente SNMP
  #if defined(SNMP_AGENT)
  loopSnmpAgent();
  #endif
  // Agente Zabbix
  #if defined(ZABBIX_AGENT) or defined(ZABBIX_AGENT_V2) or defined(ZABBIX_AGENT_V3)
  loopZabbixAgent();
  #endif
  // Agente Zabbix
  #if defined(MQTT_AGENT)
  loopMqttAgent();
  #endif
}
/*
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
*/
void printMemory() {
  // Imprime informações de memória a cada X milissegundos
  if (samples == 00 || millis() - lastMillis > 10000) { 
    uptime.compute();
    samples++;
    lastMillis = millis();
    unsigned int freeHeap = ESP.getFreeHeap();
    unsigned int total = 81920;
    unsigned int occup = total - freeHeap;
    Serial.print(samples);
    Serial.print(";");
    Serial.print(uptime.totalSeconds());
    Serial.print(";");
    Serial.print(occup);
    Serial.print(";");
    Serial.print(freeHeap);
    Serial.print(";");
    Serial.print(total);
    Serial.println(";");
  }
}
