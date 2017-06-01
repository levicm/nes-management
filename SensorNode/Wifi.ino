/**
 * Code to handle with Wifi connection.
 */
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define W_SSID1 "LCM" // Rede de casa
#define W_KEY1  "safeplace"
#define W_SSID2 "LEIA-PROJ" // Rede do laboratório
#define W_KEY2  "l&1@79BkT_COM"

#if defined(SLEEP_MODE) 
#include <EEPROM.h>
#define EEPROM_ALOC_SIZE 12 // Espaço para 3 IPs

IPAddress ip;
IPAddress gateway;
IPAddress subnet;
#endif

ESP8266WiFiMulti WiFiMulti;

void connectWifi() {
  long start = millis();
#if defined(SLEEP_MODE) 
  EEPROM.begin(EEPROM_ALOC_SIZE);
  boolean sleepWake = ESP.getResetReason() == "Deep-Sleep Wake";
  if (sleepWake) {
    // Como veio de deep sleep, as informações de SSID e chave já são persistidas por WiFi
    // Mas os IPs não são, e se usarmos o anterior fica 40 vezes mais rápido, sem DHCP
    loadIps();
    WiFi.config(ip, gateway, subnet);
    DEBUG_PRINT("Conectando WiFi novamente.");
    while (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINT(".");
      delay(100);
    }
  } else 
#endif
  {
    WiFiMulti.addAP(W_SSID1, W_KEY1); // rede de casa
    WiFiMulti.addAP(W_SSID2, W_KEY2); // rede do laboratório
    DEBUG_PRINT("Procurando redes WiFi.");
    while (WiFiMulti.run() != WL_CONNECTED) {
      DEBUG_PRINT(".");
      delay(200);
    }
  }
  DEBUG_PRINTLN("\nWiFi conectado");

  DEBUG_PRINTLN2("IP: ", WiFi.localIP());
  DEBUG_PRINTLN2("Gateway: ", WiFi.gatewayIP());
  DEBUG_PRINTLN2("Mask: ", WiFi.subnetMask());
  DEBUG_PRINTLN2("Tempo total de configuração do Wifi: ", millis() - start);
#if defined(SLEEP_MODE) 
  if (!sleepWake)
    saveIps();
  // Libera memória usada pela EEPROM 
  EEPROM.end();
#endif
};

#if defined(SLEEP_MODE) 
void loadIps() {
  for (int i = 0; i < 4; ++i)
    ip[i] = EEPROM.read(i); 
  for (int i = 0; i < 4; ++i)
    gateway[i] = EEPROM.read(i + 4); 
  for (int i = 0; i < 4; ++i)
    subnet[i] = EEPROM.read(i + 8); 
}

void saveIps() {
  // Limpa os dados
  for (int i = 0; i < EEPROM_ALOC_SIZE; ++i) 
    EEPROM.write(i, 0);
  // Grava os IPs
  for (int i = 0; i < 4; ++i) 
    EEPROM.write(i, WiFi.localIP()[i]);
  for (int i = 0; i < 4; ++i)
    EEPROM.write(i + 4, WiFi.gatewayIP()[i]);
  for (int i = 0; i < 4; ++i) 
    EEPROM.write(i + 8, WiFi.subnetMask()[i]);
  EEPROM.commit();
  DEBUG_PRINT("Salvo na EEPROM: ");
  for (int i = 0; i < EEPROM_ALOC_SIZE; ++i)
    DEBUG_PRINT(EEPROM.read(i)); 
  DEBUG_PRINTLN(); 
}
#endif

