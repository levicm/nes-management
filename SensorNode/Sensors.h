/**
 * Classe que encapsula as informações dos sensores
 * Atualmente ela tem informações de:
 * - Temperatura
 * - Umidade
 * - Índice de calor
 */
#include <DHT.h>

// Variaveis e constantes do DHT22
#define DHTPIN 5
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE); // Sensor de temperatura e humidade do ambiente

class Sensors {
  private:
    DHT* dht;
    float t;
    float h;
    float hic;

    void read(void) {
      float _t = dht->readTemperature();
      float _h = dht->readHumidity();
      // Verifica se alguma leitura falhou e tenta novamente
      if (isnan(_t) || isnan(_h)) {
        DEBUG_PRINTLN("[Sensors] Primeira leitura invalida. Tentando novamente...");
        delay(10);
        _t = dht->readTemperature();
        _h = dht->readHumidity();
      }
      if (!isnan(_t) && !isnan(_h)) {
        h = _h;
        t = _t;
        hic = dht->computeHeatIndex(t, h, false);
      } else {
        DEBUG_PRINTLN("[Sensors] Não foi possível ler DHT");
      }
    }

  public:
    Sensors(DHT* _dht) {
      dht = _dht;
    }
    void begin(void) {
      dht->begin();
    }
    float getTemperature(void) {
      read();
      return t;
    }
    float getHumidity(void) {
      read();
      return h;
    }
    float getHeatIndex(void) {
      read();
      return hic;
    }
};

// Objeto que encapsula as informações dos sensores
Sensors sensors(&dht);


