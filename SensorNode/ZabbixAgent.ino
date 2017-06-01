#if defined(ZABBIX_AGENT)
#include <ZabbixPassiveWifi.h>  // Biblioteca que implementa o protocolo Agente Zabbix modo passivo

#define ZABBIX_KEY_PING "agent.ping"
#define ZABBIX_KEY_UNAME "system.uname"
#define ZABBIX_KEY_UPTIME "system.uptime"
#define ZABBIX_KEY_CPU_ID "system.stat[cpu,id]"
#define ZABBIX_KEY_FREE_MEM "system.stat[memory,fre]"
#define ZABBIX_KEY_CPU_TEMP "node.cputemp"
#define ZABBIX_KEY_VOLTAGE "node.voltage"

#define ZABBIX_KEY_SENSOR_TEMP "node.sensor.temp"
#define ZABBIX_KEY_SENSOR_HUMID "node.sensor.humidity"

#define ZABBIX_NO_RESPONSE ""

#define ZBX_PING "1"
#define ZBX_NAME "Sensor Node Zabbix Agent."

ZabbixPassiveWifi zabbix;

void keyReceived() {
  char* key = zabbix.getKey();
  Serial.print("keyReceived: ");
  Serial.println(key);
  Serial.println(ESP.getFreeHeap());
  DEBUG_PRINT("ZabbixAgentListener.received: ");
  DEBUG_PRINTLN(key);
  if (strcmp(key, ZABBIX_NO_KEY) != 0) {
    if (strcmp(key, ZABBIX_KEY_PING) == 0) {
      zabbix.respond(ZBX_PING);
    } else if (strcmp(key, ZABBIX_KEY_UNAME) == 0) {
      zabbix.respond(ZBX_NAME);
    } else if (strcmp(key, ZABBIX_KEY_UPTIME) == 0) {
      zabbix.respond(systemInfo.getUptime());
    } else if (strcmp(key, ZABBIX_KEY_CPU_ID) == 0) {
      zabbix.respond(systemInfo.getChipId());
    } else if (strcmp(key, ZABBIX_KEY_FREE_MEM) == 0) {
      zabbix.respond(systemInfo.getFreeHeap());
    } else if (strcmp(key, ZABBIX_KEY_CPU_TEMP) == 0) {
      zabbix.respond(systemInfo.getChipTemp(), 2);
    } else if (strcmp(key, ZABBIX_KEY_VOLTAGE) == 0) {
      zabbix.respond(systemInfo.getVin(), 2);
    }

    if (strcmp(key, ZABBIX_KEY_SENSOR_TEMP) == 0) {
      zabbix.respond(sensors.getTemperature(), 2);
    } else if (strcmp(key, ZABBIX_KEY_SENSOR_HUMID) == 0) {
      zabbix.respond(sensors.getHumidity(), 2);
    }
  }
}

void setupZabbixAgent() {
  Serial.println("ZabbixAgent.setup()");
  zabbix.begin();
  zabbix.onKeyReceive(keyReceived);
  Serial.println("Escutando...");
}

void loopZabbixAgent() {
  zabbix.listen();
}
#endif
