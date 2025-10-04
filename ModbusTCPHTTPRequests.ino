#include <EEPROM.h>            // EEPROM para ESP32
#include <Ethernet.h>          // Ethernet (se estiver usando Ethernet com ESP32)
#include <SPI.h>               // SPI para ESP32
#include <ArduinoJson.h>       // JSON
#include <WiFi.h>              // Biblioteca WiFi para ESP32
#include <WiFiClientSecure.h>  // Cliente HTTPS para ESP32
#include <WebServer.h>         // WebServer para ESP32
#include <HTTPClient.h>        // Cliente HTTP para ESP32
#include <WiFiUdp.h>           // UDP para ESP32
#include <ModbusEthernet.h>    // Supondo que essa biblioteca seja compatível
#include <FS.h>                // Sistema de arquivos (base)
#include <time.h>              // Biblioteca para hora
#include <ArduinoHttpClient.h>
#include <SPIFFS.h>
#include <Update.h>
#include <esp_system.h>
#include <esp_task_wdt.h>

#define PATH_IP_WIFI "/path_ip_wifi.txt"
#define PATH_GAT_WIFI "/path_gat_wifi.txt"
#define PATH_SUB_WIFI "/path_sub_wifi.txt"
#define PATH_CONFIG_WIFI "/path_config_wifi.txt"
#define PATH_GUID "/path_guid.txt.txt"

#define PATH_IP_ETH "/path_ip_eth.txt"
#define PATH_GAT_ETH "/path_gat_eth.txt"
#define PATH_SUB_ETH "/path_sub_eth.txt"
#define PATH_CONFIG_ETH "/path_config_eth.txt"


#define PATH_PLC_SERVER "/path_plc_server.txt"
#define PATH_PLC_PORT "/path_plc_port.txt"

#define PATH_PLC_TIME "/path_plc_time.txt"



#define DELAY_SERVIDOR 500
String jsonResponse;
bool scanComplete = false;

// Defina o endereço IP manualmente

int arrayEthIP[4] = { 255, 255, 255, 255 };
int arrayGatewayIP[4] = { 255, 255, 255, 255 };
int arraySubnetIP[4] = { 255, 255, 255, 255 };



int arrayWifiIP[4] = { 255, 168, 100, 12 };
int arrayWifiGatewayIP[4] = { 255, 168, 100, 1 };
int arrayWifiSubnetIP[4] = { 255, 255, 255, 0 };




int contador_falhas_via_ethernet = 0;
int contador_reconect_ethernet = 0;


int contador_falhas_via_wifi = 0;
int contador_reconect_wifi = 0;

String senha_api = "1234";



String BASE_URL = "http://162.240.226.254:10050/";
String path_avisar_online = "v1/protected/modulopgm/informarOnlinePost/";
String path_buscar_acoes = "v1/protected/modulopgm/tarefas/listar/";
String path_responder_acao = "v1/protected/modulopgm/tarefas/responder/";
String path_informar_acao = "v1/protected/modulopgm/tarefas/informar/";


const char BASE_URL_ETH[] = "162.240.226.254";
const int BASE_PORT_ETH = 10050;

char path_buscar_acoes_eth[] = "/v1/protected/modulopgm/tarefas/listar/";
char path_responder_acao_eth[] = "/v1/protected/modulopgm/tarefas/responder/";
char path_informar_acao_eth[] = "/v1/protected/modulopgm/tarefas/informar/";
char path_avisar_online_eth[] = "/v1/protected/modulopgm/informarOnlinePost/";
char path_upload_imagem_eth[] = "/v1/protected/modulopgm/registrodisponibilidade/";




const char* BASE_URL_ETH_ATT = "162.240.226.254";
const unsigned short SERVER_PORT_ETH_ATT = 10050;



boolean logado = false;
#define POSICAO_PRIMEIRO_USO 110
#define POSICAO_CONFIGURADO_WIFI 111
#define POSICAO_CONFIGURADO_ETH 112
#define POSICAO_CONFIGURADO_PRIORIDADE 113
#define POSICAO_CONFIGURADO_SERVIDOR 115




String enderecoIpManualGlobal = "10.0.0.100";
String gatewayManualGlobal = "10.0.0.1";
String subMascaraManualGlobal = "24";


boolean rota1 = true;

unsigned long tempo_buscar_acoes = millis();

unsigned long tempo_buscar_acao_update = millis();

unsigned long tempo_ler_servidor = millis();



unsigned long tempo_reconexao = millis();

unsigned long tempo_update_status = millis();


unsigned long ultimo_aviso = millis();


unsigned long tempo_ligado = millis();


int primeiro_uso_global = -1;
int configurado_eth_global = -1;
int configurado_wifi_global = -1;
int configurado_servidor_global = -1;
int configurado_prioridade_global = -1;


String configurado_ip_plc_server_global = "";
String configurado_port_plc_server_global = "";
String configurado_time_plc_server_global = "";




boolean conectado_wifi = false;


boolean conectado_ethernet = false;


#include "MakeParamsLocalSetup.h"



/*************** CERTIFICADOS **************************************************/
#include "Certificados.h"
/*************** CERTIFICADOS **************************************************/


/***************** METODOS *************************/
#include "AllMetods.h"
/***************** METODOS *************************/

/*************** CLASSES **************************************************/
#include "WEBPAGES.h"
/*************** CLASSES **************************************************/


/***************** FilesWriter *************************/
#include "FilesWriter.h"
/***************** FilesWriter *************************/

/*************** CLASSES **************************************************/

/*************** SERVIDOR WEB VARIAVEIS E OBJETOS *************************/
const char* ssid_hostpot = "MODBUS RECEIVER ";
const char* password_hostpot = "MBreceiver7898";  // Senha HostPot


#ifdef ESP32
WebServer server(80);
#elif defined(ESP8266)
ESP8266WebServer server(80);
#else
#error "Placa não suportada. Escolha ESP32 ou ESP8266."
#endif

/*************** SERVIDOR WEB VARIAVEIS E OBJETOS *************************/

EthernetServer ethServer(90);



char* url_rota;



boolean cliente_wifi_salvo = false;
String ssid_wifi_global = "";
String passwd_wifi_global = "";

/*************** SETUPS *************************/
#include "WifiLocalSetup.h"
#include "WebServerLocalSetup.h"
/*************** SETUPS *************************/




/***************** ETHERNET *************************/
#include "EthernetLocalSetup.h"
/***************** ETHERNET *************************/

boolean informou_restart = false;




bool primeira_execucao = true;


ModbusEthernet modbusClient;

IPAddress plcServer;
int plcPort;
int plcTimeInterval;

void setup() {


  Serial.begin(115200);

  delay(2000);



  //true para formatar
  //true não funciona no nodemcu
  //Abre o sistema de arquivos
  if (!SPIFFS.begin(true)) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  } else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }

  firmwareVersion = String(FIRMWARE_VERSION);


  //padrao de fabrica
  padroesFabrica();




  obterConfiguracoes();


  iniciarHostpotWifi();  //Ininicar a Placa como HOSTPOT e Servidor WEB

  iniciarClienteWifi();  //Iniciar Cliente Wifi

  if (WiFi.status() == WL_CONNECTED) {

    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
      delay(500);
      Serial.print(".");
      now = time(nullptr);
    }
    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
    Serial.println("Time OK");
  }

   if (isETH)
   iniciarClienteEth();






  atualizarStatus();

  modbusClient.client();


  if (WiFi.status() == WL_CONNECTED) {
    // processarNovosRegistradores();
  }

  if (!informou_restart && WiFi.status() == WL_CONNECTED) {
    Serial.println("Informando Modulo restart");
    informarAcao(700, 0);
    informou_restart = true;
  } else {
    Serial.println("Não informado Modulo restart");
  }


  Serial.println("Fim do Setup!");
}


const uint16_t REG = 0;
uint16_t valorP = 0;

void loop() {
  /*
  if (modbusClient.isConnected(plcServer)) {
    Serial.println("Servidor conectado!");
    // Lê 4 holding registers a partir do endereço 0
    modbusClient.readHreg(plcServer, REG, &valorP);
  } else {
    Serial.println("Servidor nao conectado, tentando conectar");
    modbusClient.connect(plcServer);  // Tenta conectar
  }
  delay(100);
  modbusClient.task();  // Executa as tarefas Modbus


  Serial.print("Contador: ");
  Serial.println(valorP);




  delay(3000);
  return;
*/

  if (Serial.available()) {
    char a = Serial.read();
    if (a == 'a') {
      Serial.println("restart recebido");
      ESP.restart();
    } else if (a == 'l') {
      Serial.println("Ler registradores");
      listarRegistradores();
    } else if (a == 'g') {
      String acao = getNeedUpdate();
      if (!acao.equals("errou")) {
        processarRequisicao(acao);
      }
    } else if (a == 'h') {
       processarNovosRegistradores();
    } else if (a == 'u') {
      setUploadRegistradores(processarUploadRegistradores());
    } else if (a == 'd') {
      limparRegistradores();
    }
  }





  //#ifdef ESP8266
  //  tarefaGlobal();
  //#endif
  tarefaGlobal();





  server.handleClient();  //Trata requisições de clientes do Servidor WEB
}




void processarRequisicao(String acao) {

  // Tamanho da string JSON
  const size_t capacity = JSON_OBJECT_SIZE(4) + 256;
  DynamicJsonDocument doc(capacity);

  // Parse da string JSON
  DeserializationError error = deserializeJson(doc, acao);

  // Verificar se houve erro no parse
  if (error) {
    Serial.print("Falha ao analisar JSON: ");
    Serial.println(error.c_str());
    tempo_buscar_acoes = millis();

    return;
  }

  // Obter os valores de "tipo_requisicao" e "id_tarefa" como inteiros
  int tipoRequisicao = doc["tipo_requisicao"].as<int>();
  int idTarefa = doc["id_tarefa"].as<int>();

  String command = doc["needUpdate"].as<String>();

  String valorString1 = "";
  String valorString2 = "";
  String valorString3 = "";
  String valorString4 = "";


  if (tipoRequisicao >= 90) {
    valorString1 = doc["valor_string1"].as<String>();
    valorString2 = doc["valor_string2"].as<String>();
    valorString3 = doc["valor_string3"].as<String>();
    valorString4 = doc["valor_string4"].as<String>();
  }

  /*
  Serial.print("Valor string 1: ");
  Serial.println(valorString1);
  Serial.print("Valor string 2: ");
  Serial.println(valorString2);


  Serial.print("Command: ");
  Serial.println(command);

  // Usar os valores conforme necessário
  Serial.print("Tipo de Requisicao: ");
  Serial.println(tipoRequisicao);
  Serial.print("ID da Tarefa: ");
  Serial.println(idTarefa);
  */

  if (command.equals("true")) {
    processarNovosRegistradores();
    setResponseUpdate(processarUploadRegistradores());
  } else if (tipoRequisicao == 60) {
    //resetar esp
    respostaAcao(idTarefa);
    ESP.restart();
  } else if (tipoRequisicao == 70) {
    //atualizar

    respostaAcao(idTarefa);


    if (WiFi.status() == WL_CONNECTED) {
      buscarAtt();
    }
  } else if (tipoRequisicao == 90) {
    //criarNovoArquivo
    //void writeFile(String valor, String path)
    writeFile(valorString1, valorString2);
  } else if (tipoRequisicao == 115) {

    //salvar rede wifi
    String ssidWifi = valorString1;
    String passwordWifi = valorString2;
    salvarEeprom(ssidWifi, passwordWifi);
    //Responder acao
    respostaAcao(idTarefa);
    EEPROM.begin(200);
    writeFile("1", PATH_CONFIG_WIFI);

    EEPROM.commit();  // Salva alterações na FLASH
    EEPROM.end();     // Apaga a cópia da EEPROM salva na RAM

    ESP.restart();
  } else if (tipoRequisicao == 116) {
    //salvar configuracos eth
    String ip_pgm_eth = valorString1;
    String gateway_rede_eth = valorString2;
    String submascara_eth = valorString3;
    salvarConfiguracoesEthSpiffs(ip_pgm_eth, gateway_rede_eth, submascara_eth);
    //Responder acao
    respostaAcao(idTarefa);
    ESP.restart();
  } else if (tipoRequisicao == 118) {
    //salvar configuracoes wifi
    String ip_pgm_wifi = valorString1;
    String gateway_rede_wifi = valorString2;
    String submascara_wifi = valorString3;
    salvarConfiguracoesWifiSpiffs(ip_pgm_wifi, gateway_rede_wifi, submascara_wifi);
    //Responder acao
    respostaAcao(idTarefa);
    ESP.restart();
  } else if (tipoRequisicao == 119) {
    //salvar configuracoes prioridade
    int prioridade = valorString1.toInt();
    EEPROM.begin(200);
    EEPROM.write(POSICAO_PRIMEIRO_USO, 1);
    EEPROM.write(POSICAO_CONFIGURADO_PRIORIDADE, prioridade);
    //Responder acao
    respostaAcao(idTarefa);
    ESP.restart();
  }





  //Responder acao
  respostaAcao(idTarefa);

  atualizarStatus();


  tempo_buscar_acoes = millis();
}




boolean validarDadosServidor() {
  if (configurado_ip_plc_server_global.equals("0.0.0.0")) {
    return false;
  }
  return true;
}


void tarefaGlobal() {

  if (millis() - tempo_ler_servidor > (plcTimeInterval * 1000) && WiFi.status() == WL_CONNECTED && !(Ethernet.linkStatus() == LinkOFF)) {
    if (validarDadosServidor()) {
      setUploadRegistradores(processarUploadRegistradores());
      tempo_ler_servidor = millis();
    } else {
      tempo_ler_servidor = millis();
    }
  }


  if (millis() - tempo_buscar_acao_update > 10000 && WiFi.status() == WL_CONNECTED) {
    String acao = getNeedUpdate();
    if (!acao.equals("errou")) {
      processarRequisicao(acao);
    }
    tempo_buscar_acao_update = millis();
  }



  if (millis() - tempo_buscar_acoes > 1500 && WiFi.status() == WL_CONNECTED) {


    boolean processar = false;
    String acao = buscarAcoesWifi();
    if (acao.equals("errou")) {
      processar = false;
    } else {
      processar = true;
    }


    if (processar) {
      if (acao != "" && acao.length() > 5) {
        processarRequisicao(acao);
      }
    }


    tempo_buscar_acoes = millis();
  }





  if (millis() - tempo_update_status > 20000) {
    if (WiFi.status() == WL_CONNECTED) {
      atualizarStatus();
      tempo_update_status = millis();
    }
  }





  if (primeiro_uso_global == 1) {
    if (configurado_wifi_global == 1) {
      if (contador_reconect_wifi > 3) {
        Serial.println("Reiniciar ESP ...");
        delay(1000);
        ESP.restart();
      }
      if (contador_falhas_via_wifi > 10) {
        Serial.println("Reconectando Wifi...");
        iniciarClienteWifi();
      }
    }
  }
}






void atualizarStatus() {

  if (WiFi.status() == WL_CONNECTED) {
    String acao = updateStatusWifi();
  }
}






void padroesFabrica() {
  EEPROM.begin(200);
  EEPROM.write(POSICAO_PRIMEIRO_USO, 1);
  EEPROM.write(POSICAO_CONFIGURADO_SERVIDOR, 0);
  // EEPROM.write(POSICAO_CONFIGURADO_ETH, 0);
  // EEPROM.write(POSICAO_CONFIGURADO_WIFI, 0);
  EEPROM.write(POSICAO_CONFIGURADO_PRIORIDADE, 0);


  EEPROM.write(0, 30);
  EEPROM.write(1, 30);


  EEPROM.commit();  // Salva alterações na FLASH
  EEPROM.end();     // Apaga a cópia da EEPROM salva na RAM


  writeFile("0", PATH_CONFIG_ETH);
  writeFile("0", PATH_CONFIG_WIFI);
  writeFile("2025-7-100", PATH_GUID);

  writeFile("192.168.10.11", PATH_PLC_SERVER);
  writeFile("502", PATH_PLC_PORT);

  writeFile("60", PATH_PLC_TIME);

  salvarSenhaAdminEeprom("modbus7898");
}




int lerEeprom(int posicao) {
  EEPROM.begin(200);
  int valor = EEPROM.read(posicao);
  EEPROM.end();
  return valor;
}

void salvarEeprom(int posicao, int valor) {
  EEPROM.begin(200);
  EEPROM.write(posicao, valor);
  EEPROM.commit();
  obterConfiguracoes();
}



void salvarConfiguracoesServidor(String enderecoIp, String timeInterval) {

  writeFile(enderecoIp, PATH_PLC_SERVER);

  writeFile(timeInterval, PATH_PLC_TIME);

  EEPROM.begin(300);
  EEPROM.write(POSICAO_CONFIGURADO_SERVIDOR, 1);

  EEPROM.commit();  // Salva alterações na FLASH
  EEPROM.end();     // Apaga a cópia da EEPROM salva na RAM
}



void obterConfiguracoes() {
  delay(500);
  EEPROM.begin(900);
  primeiro_uso_global = EEPROM.read(POSICAO_PRIMEIRO_USO);

  configurado_servidor_global = EEPROM.read(POSICAO_CONFIGURADO_SERVIDOR);


  configurado_prioridade_global = EEPROM.read(POSICAO_CONFIGURADO_PRIORIDADE);






  EEPROM.end();





  //lerConfiguracoesWifi();
  lerConfiguracoesWifiSpiffs();
  delay(500);
  //Serial.print("Configurado eth?: ");
  //Serial.println(configurado_eth_global);
  lerConfiguracoesEthSpiffs();





  SERIAL_NUMBER = lerConfiguracaoSpiff(PATH_GUID);



  configurado_ip_plc_server_global = lerConfiguracaoSpiff(PATH_PLC_SERVER);
  configurado_port_plc_server_global = lerConfiguracaoSpiff(PATH_PLC_PORT);
  configurado_time_plc_server_global = lerConfiguracaoSpiff(PATH_PLC_TIME);

  if (!plcServer.fromString(configurado_ip_plc_server_global)) {
    Serial.println("[FATAL] IP do Servidor Modbus inválido.");
  }

  plcTimeInterval = configurado_time_plc_server_global.toInt();

  Serial.print("PLC Server configurado: ");
  Serial.println(plcServer);
  Serial.print("PLC Porta configurada: ");
  Serial.println(plcPort);
  Serial.print("PLC Intervalo leitura: ");
  Serial.println(plcTimeInterval);
}




boolean respostaAcao(int id_acao) {

  //Busca primeiro por wifi
  if (WiFi.status() == WL_CONNECTED) {
    String acao = responderAcao(id_acao);
    if (acao.equals("sucesso"))
      return true;
  }


  return false;
}



void processarNovosRegistradores() {
  Serial.println("[DEBUG] [] [AVISO] [processarNovosRegistradores] [Obtendo novos registradores] []");

  String acao = getRegistradores();


  if (!acao.equals("errou")) {
    limparRegistradores();

    const size_t capacity = JSON_ARRAY_SIZE(30) + 30 * JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);

    // Parse do JSON
    DeserializationError error = deserializeJson(doc, acao);
    if (error) {
      Serial.print("Falha ao analisar JSON de Registradores ");
      Serial.println(error.c_str());
      return;
    }

    JsonArray array = doc.as<JsonArray>();

    // Iterar pelos registradores
    for (JsonObject obj : array) {
      String name = obj["name"] | "N/A";
      String plcPath = obj["plcPath"] | "N/A";
      String dataType = obj["dataType"] | "N/A";
      String valueType = obj["valueType"] | "N/A";

      Serial.println("---- Registrador ----");
      Serial.println("Nome: " + name);
      Serial.println("PLC Path: " + plcPath);
      Serial.println("Tipo: " + dataType);
      Serial.println("Value: " + valueType);

      salvarRegistrador(name, plcPath, dataType, valueType);
    }
    Serial.println("[DEBUG] [] [AVISO] [processarNovosRegistradores] [Tabela de Registradores salva] []");

  } else {
    Serial.println("[DEBUG] [] [FATAL] [processarNovosRegistradores] [Erro ao obter respostas com novos registradores] []");
  }
}


String processarUploadRegistradores() {
  Serial.println("[DEBUG] [] [AVISO] [processarUploadRegistradores] [Processar Upload de Registradores] []");

  File dir = SPIFFS.open("/registradores");
  if (!dir || !dir.isDirectory()) {
    Serial.println("Erro: /registradores não é um diretório.");
    return "errou";
  }

  File file = dir.openNextFile();  // primeiro arquivo
  String body = "{\"data\": \"{";

  int contador = 0;

  if (modbusClient.isConnected(plcServer)) {
    Serial.println("Conectado ao PLC");
  } else {
    Serial.print("Não conectado ao PLC, tentao conexão no ip ");
    Serial.println(plcServer);
    modbusClient.connect(plcServer);

    modbusClient.task();
    delay(1000);
    if (modbusClient.isConnected(plcServer)) {
      Serial.println("Conectado ao PLC");
    } else {
      Serial.println("[DEBUG] [] [FATAL] [processarUploadRegistradores] [Erro ao conectar ao servidor modbus] []");
      modbusClient.connect(plcServer);
      return "errou";
    }
  }

  while (file) {
    String path = file.name();
    Serial.print("Arquivo: ");
    Serial.println(path);

    if (!file) {
      Serial.println("Erro ao abrir o arquivo.");
      file = dir.openNextFile();  // próximo arquivo
      continue;
    }

    // Ler e processar o conteúdo do JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
      Serial.print("Erro ao ler JSON: ");
      Serial.println(error.c_str());
    } else {
      String name = doc["name"] | "N/A";
      String plcPath = doc["plcPath"] | "N/A";
      String dataType = doc["dataType"] | "N/A";
      String valueType = doc["valueType"] | "N/A";

      Serial.println("  → name: " + name);
      Serial.println("  → plcPath: " + plcPath);
      Serial.println("  → dataType: " + dataType);
      Serial.println("  → valueType: " + valueType);

      bool coilValue[1];
      bool bolRes = false;
      uint16_t intRes = 0;
      uint16_t realRes[2];

      String valor = "";

      int numero = plcPath.toInt();
      int offset = numero % 100;

      if (dataType.equals("COIL")) {
        if (offset >= 1)
          --offset;
        Serial.println("Processando registrador COIL");
        modbusClient.readCoil(plcServer, (uint16_t)offset, coilValue, 1);
        delay(DELAY_CONEXAO);
        modbusClient.task();
        delay(DELAY_CONEXAO);
        valor = coilValue[0] ? "true" : "false";
      } else if (dataType.equals("DISCRETE")) {
        if (offset >= 1)
          --offset;

        Serial.println("Processando registrador DISCRETE");
        modbusClient.readIsts(plcServer, (uint16_t)offset, &bolRes);

        delay(DELAY_CONEXAO);
        modbusClient.task();
        delay(DELAY_CONEXAO);

        valor = String(bolRes);

      } else if (dataType.equals("INPUT_REGISTER")) {
        if (offset >= 1)
          --offset;

        Serial.println("Processando registrador INPUT_REGISTER");
        if (valueType.equals("INTEGER")) {
          modbusClient.readIreg(plcServer, (uint16_t)offset, &intRes);
        } else if (valueType.equals("REAL")) {
          modbusClient.readIreg(plcServer, (uint16_t)offset, realRes, 2);
        }
        delay(DELAY_CONEXAO);
        modbusClient.task();
        delay(DELAY_CONEXAO);

        if (valueType.equals("INTEGER")) {
          valor = String(intRes);
        } else if (valueType.equals("REAL")) {
          float resultado = realRes[0] + (realRes[1] / 100.0);
          valor = String(resultado);
        }
      }

      else if (dataType.equals("HOLDING_REGISTER")) {
        if (offset >= 1)
          --offset;

        Serial.println("Processando registrador HOLDING_REGISTER");
        if (valueType.equals("INTEGER")) {
          modbusClient.readHreg(plcServer, (uint16_t)offset, &intRes);
        } else if (valueType.equals("REAL")) {
          modbusClient.readHreg(plcServer, (uint16_t)offset, realRes, 2);
        }
        delay(DELAY_CONEXAO);
        modbusClient.task();
        delay(DELAY_CONEXAO);

        if (valueType.equals("INTEGER")) {
          valor = String(intRes);
        } else if (valueType.equals("REAL")) {
          float resultado = realRes[0] + (realRes[1] / 100.0);
          valor = String(resultado);
        }
      }

      Serial.print("Offset: ");
      Serial.println(offset);

      Serial.print("Valor: ");
      Serial.println(valor);

      String local = jsonPair(name, valor);
      body += local;
      body += ",";
    }

    file = dir.openNextFile();  // próximo arquivo
  }

  body += jsonPair("dt_time", "\\\"" + getDateTime());
  body += "\\\"}\"}";

  return body;
}


String jsonPair(String chave, String valor) {
  return "\\\"" + chave + "\\\":" + valor;
}

String getDateTime() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", timeinfo);

  return String(buffer);
}