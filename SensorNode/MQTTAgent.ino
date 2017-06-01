#if defined(MQTT_AGENT)
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_INTERVAL 10
#define MQTT_TOPIC_PREFIX "zabbix/item/sensor-node"

#define MQTT_KEY_UNAME "system.uname"
#define MQTT_KEY_UPTIME "system.uptime"
#define MQTT_KEY_CPU_ID "system.stat[cpu,id]"
#define MQTT_KEY_FREE_MEM "system.stat[memory,fre]"
#define MQTT_KEY_CPU_TEMP "node.cputemp"
#define MQTT_KEY_VOLTAGE "node.voltage"

#define MQTT_KEY_SENSOR_TEMP "node.sensor.temp"
#define MQTT_KEY_SENSOR_HUMID "node.sensor.humidity"

#define MQTT_NAME "Sensor Node MQTT Agent."

// IP do broker
IPAddress serverIP(192, 168, 1, 62);
WiFiClient wclient;
PubSubClient client(wclient, serverIP);
//PubSubClient client(wclient, "localhost");

long lastPublishMillis = 0;

void callback(const MQTT::Publish& pub) {
  Serial.print("Callback invoked");
  Serial.println(ESP.getFreeHeap());  
}

void publishItem(const String key, const String value) {
  String item = String();
  item += MQTT_TOPIC_PREFIX;
  item += WiFi.localIP()[3];
  item += '/';
  item += key;
  DEBUG_PRINT("Publishing: ");
  DEBUG_PRINT(item);
  DEBUG_PRINTLN2(", ", value);
  client.publish(item, value);
}

void setupMqttAgent() {
  client.set_callback(callback);
  if (client.connect("MQTTAgent")) {
    DEBUG_PRINTLN("MQTTAgent connected to Broker!");
  } else {
    DEBUG_PRINTLN("MQTTAgent NOT Connected!");
#if defined(SLEEP_MODE) 
    sleep();
#endif
  }
}

void loopMqttAgent() {
  if (client.connected()) {
    client.loop();

#if not defined(SLEEP_MODE) 
    // Se passou o intervalo, envia os indicadores
    if (millis() - lastPublishMillis > MQTT_INTERVAL * 1000) 
#endif
    {
      // Envia dados do sistema
      publishItem(MQTT_KEY_UNAME, MQTT_NAME);
      publishItem(MQTT_KEY_UPTIME, String(systemInfo.getUptime(), DEC));
      publishItem(MQTT_KEY_CPU_ID, String(systemInfo.getChipId(), DEC));
      publishItem(MQTT_KEY_FREE_MEM, String(systemInfo.getFreeHeap(), DEC));
      publishItem(MQTT_KEY_CPU_TEMP, String(systemInfo.getChipTemp(), 2));
      publishItem(MQTT_KEY_VOLTAGE, String(systemInfo.getVin(), 2));
      // Envia dados dos sensores
      publishItem(MQTT_KEY_SENSOR_TEMP, String(sensors.getTemperature(), 2));
      publishItem(MQTT_KEY_SENSOR_HUMID, String(sensors.getHumidity(), 2));
      lastPublishMillis = millis();

#if defined(SLEEP_MODE) 
      sleep();
#endif
    }
  }
}

#if defined(SLEEP_MODE) 
void sleep() {
  DEBUG_PRINTLN("Entering in slpeed mode...");
  uptime.persist(MQTT_INTERVAL * 1000);
  ESP.deepSleep(MQTT_INTERVAL * 1000000, WAKE_RF_DEFAULT);
}
#endif

#endif

