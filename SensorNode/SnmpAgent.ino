#if defined(SNMP_AGENT)
#include <AgentuinoWifi.h>  // Biblioteca que implementa o protocolo SNMP

// Variaveis e constantes do SNMP
// RFC1213-MIB OIDs
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
static char sysDescr[] PROGMEM      = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
static char sysUpTime[] PROGMEM     = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)
// User defined
// .iso.org.dod.internet.mgmt.mib-2.system.node (.1.3.6.1.2.1.1.9)
// .iso.org.dod.internet.mgmt.mib-2.system.node.nodeCpuId (.1.3.6.1.2.1.1.9.1)
static char nodeCpuId[] PROGMEM    = "1.3.6.1.2.1.1.9.1.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.node.nodeFreeMem (.1.3.6.1.2.1.1.9.2)
static char nodeFreeMem[] PROGMEM   = "1.3.6.1.2.1.1.9.2.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.node.nodeCpuTemp (.1.3.6.1.2.1.1.9.3)
static char nodeCpuTemp[] PROGMEM      = "1.3.6.1.2.1.1.9.3.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.node.nodeVoltage (.1.3.6.1.2.1.1.9.4)
static char nodeVin[] PROGMEM       = "1.3.6.1.2.1.1.9.4.0";  // read-only  (DisplayString)
// User defined
// .iso.org.dod.internet.mgmt.mib-2.system.sensor (.1.3.6.1.2.1.1.10)
// .iso.org.dod.internet.mgmt.mib-2.system.sensor.temp (.1.3.6.1.2.1.1.10.1)
// .iso.org.dod.internet.mgmt.mib-2.system.sensor.humid (.1.3.6.1.2.1.1.10.2)

// RFC1213 local values
static char snmpName[]              = "Sensor Node SNMP Agent.";  // read-only (static)


char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_API_STAT_CODES response_status;
SNMP_ERR_CODES status;
SNMP_PDU pdu;

void pduReceived() {
  DEBUG_PRINTLN("pduReceived");
  api_status = Agentuino.requestPdu(&pdu);

  if (pdu.type == SNMP_PDU_GET
      || pdu.type == SNMP_PDU_GET_NEXT
      || pdu.type == SNMP_PDU_SET
      && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS) {

    pdu.OID.toString(oid);
    DEBUG_PRINT("oid ");
    DEBUG_PRINTLN(oid);
    if (pdu.type == SNMP_PDU_SET) {
      // Assume que não suporta a função set pra nenhum oid
      pdu.error = SNMP_ERR_READ_ONLY;
    } else {
      // Trata cada oid da função get
      if (strcmp_P(oid, sysDescr) == 0) {
        DEBUG_PRINTLN2("Recebeu pedido de sysDescr: ", snmpName);
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, snmpName);
        pdu.error = status;
      } else if (strcmp_P(oid, sysUpTime) == 0) {
        DEBUG_PRINTLN2("Recebeu pedido de uptime: ", (int) systemInfo.getUptime());
        status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, (uint32_t) (systemInfo.getUptime()));
        pdu.error = status;
      } else if (strcmp_P(oid, nodeCpuId) == 0) {
        DEBUG_PRINTLN2("Recebeu pedido de nodeChipId: ", systemInfo.getChipId());
        String chipId = String(systemInfo.getChipId());
        char b[chipId.length()];
        chipId.toCharArray(b, chipId.length());
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, b);
        pdu.error = status;
      } else if (strcmp_P(oid, nodeFreeMem) == 0) {
        DEBUG_PRINTLN2("Recebeu pedido de nodeFreeMem: ", systemInfo.getFreeHeap());
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, (int) systemInfo.getFreeHeap());
        pdu.error = status;
      } else if (strcmp_P(oid, nodeCpuTemp) == 0) {
        int value = (int) (systemInfo.getChipTemp() * 10);
        DEBUG_PRINTLN2("Recebeu pedido de nodeTemp: ", value);
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, value);
        pdu.error = status;
      } else if (strcmp_P(oid, nodeVin) == 0) {
        int value = (int) (systemInfo.getVin() * 10);
        DEBUG_PRINTLN2("Recebeu pedido de nodeVin: ", value);
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, value);
        pdu.error = status;
      } else {
        pdu.error = SNMP_ERR_NO_SUCH_NAME;
      }
    }
    pdu.type = SNMP_PDU_RESPONSE;
    DEBUG_PRINTLN("Enviando resposta...");
    Agentuino.responsePdu(&pdu);
  }
  DEBUG_PRINTLN("Liberando PDU...");
  Agentuino.freePdu(&pdu);
}

void setupSnmpAgent() {
  Serial.println("SnmpAgent.setup()");
  api_status = Agentuino.begin();
  if (api_status == SNMP_API_STAT_SUCCESS) {
    Agentuino.onPduReceive(pduReceived);
    Serial.println("Escutando...");
  } else {
    Serial.println("Erro iniciando Agente SNMP.");
  }
}

void loopSnmpAgent() {
  Agentuino.listen();
}
#endif
