/**
 * Classe que encapsula as informações do sistema que servem para o 
 * monitoramento do nó.
 * 
 * Atualmente ela tem informações de:
 * - Uptime (Segundos)
 * - Id do chip (número)
 * - Memória livre (Bytes)
 * - Temperatura do chip (Celsius)
 * - Tensão de alimentação (Volts)
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Uptime.h>

// Variaveis e constantes do Dallas DS18B20
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature chipSensor(&oneWire); // Sensor de temperatura do chip
Uptime uptime;

class SystemInfo {
  private:
    Uptime *_uptime;
    DallasTemperature *_chipSensor;

  public:
    SystemInfo(Uptime *uptime, DallasTemperature *chipSensor) {
      _uptime = uptime;
      _chipSensor = chipSensor;
    }
    void begin(void) {
#if defined(SLEEP_MODE) 
      _uptime->begin(true);
#endif
#if not defined(SLEEP_MODE) 
      _uptime->begin(false);
#endif
      _chipSensor->begin();
    }
    unsigned long getUptime(void) {
      return _uptime->totalSeconds();
    }
    unsigned int getChipId(void) {
      return ESP.getChipId();
    }
    unsigned int getFreeHeap(void) {
      return ESP.getFreeHeap();
    }
    float getChipTemp(void) {
      _chipSensor->requestTemperatures();
      return _chipSensor->getTempCByIndex(0);
    }
    float getVin() {
      int v = analogRead(A0);
      // faz a conversão pra volts e dobra (instalado na placa divisor de tensão por 2)
      return ((v * 3.3) / 1023.0) * 2;
    }
};

// Objeto que encapsula as informações do sistema
SystemInfo systemInfo(&uptime, &chipSensor);


