#if defined(ZABBIX_AGENT_V3)
#include <ZabbixPassiveWifi.h>  // Biblioteca que implementa o protocolo Agente Zabbix modo passivo

static char kPing[]    PROGMEM = "agent.ping";
static char kUname[]   PROGMEM = "system.uname";
static char kUptime[]  PROGMEM = "system.uptime";
static char kCpuId[]   PROGMEM = "system.stat[cpu,id]";
static char kFreeMem[] PROGMEM = "system.stat[memory,fre]";
static char kCpuTemp[] PROGMEM = "node.cputemp";
static char kVoltage[] PROGMEM = "node.voltage";

static char kSensorTemp[]  PROGMEM = "node.sensor.temp";
static char kSensorHumid[] PROGMEM = "node.sensor.humidity";

static char zbxPing[] = "1";
static char zbxName[] = "Sensor Node Zabbix Agent";

ZabbixPassiveWifi zabbix;

void keyReceived() {
  char* key = zabbix.getKey();
  DEBUG_PRINTLN2("keyReceived: ", key);
  DEBUG_PRINT("ZabbixAgentListener.received: ");
  DEBUG_PRINTLN(key);
  if (strcmp(key, ZABBIX_NO_KEY) != 0) {
    if (strcmp_P(key, kPing) == 0) {
      zabbix.respond(zbxPing);
    } else if (strcmp_P(key, kUname) == 0) {
      zabbix.respond(zbxName);
    } else if (strcmp_P(key, kUptime) == 0) {
      zabbix.respond(systemInfo.getUptime());
    } else if (strcmp_P(key, kCpuId) == 0) {
      zabbix.respond(systemInfo.getChipId());
    } else if (strcmp_P(key, kFreeMem) == 0) {
      zabbix.respond(systemInfo.getFreeHeap());
    } else if (strcmp_P(key, kCpuTemp) == 0) {
      zabbix.respond(systemInfo.getChipTemp(), 2);
    } else if (strcmp_P(key, kVoltage) == 0) {
      zabbix.respond(systemInfo.getVin(), 2);
    }

    if (strcmp_P(key, kSensorTemp) == 0) {
      zabbix.respond(sensors.getTemperature(), 2);
    } else if (strcmp_P(key, kSensorHumid) == 0) {
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
