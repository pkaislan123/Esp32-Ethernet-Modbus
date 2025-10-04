/*********************PARAMETROS DE FABRICAÇÃO *****************************/

#define FIRMWARE_VERSION 1

#define TMZ_CONTINENTE "America"
#define TMZ_CIDADE "Sao_Paulo"
//70:03:9F:8A:31:7E
//byte mac_ethernet[] = { 0x70, 0x03, 0x9F, 0x8A, 0x3D, 0x7E };
byte mac_ethernet[] = { 0x30, 0x03, 0xCF, 0xEF, 0xFE, 0x08 };

/*
-header 'x-api-key: Myz2sgtRl1ogVfbbdB75oog7GCv2Ji!q2rAWbqzK' \
--header 'x-usual-id: 5cc35301-5fcd-47fa-abb4-dd3f4d6cbf43' \
--header 'x-tenet-id: e987d790-4a76-33f5-8a0a-be965e11apf' \
--header 'x-function-id: WfsrrSLK5f74vYcZDrWIw44mu3aInFSU2i5TUsbU' \
*/

String x_security_id = "e987d790-4a76-33f5-8a0a-be965e11apf";
String x_api_key = "Myz2sgtRl1ogVfbbdB75oog7GCv2Ji!q2rAWbqzK";

String x_usual_id = "5cc35301-5fcd-47fa-abb4-dd3f4d6cbf43";
String x_tenet_id = "e987d790-4a76-33f5-8a0a-be965e11apf";

String x_function_id = "WfsrrSLK5f74vYcZDrWIw44mu3aInFSU2i5TUsbU";




const char *hostname = "iot";
const boolean isETH = true;
const boolean flag = true;

const boolean isZones = true;
#define BUFFER_SIZE 512  // Tamanho do buffer para leitura

#define TEMPO_ISPEIRA 2000
#define DELAY_CONEXAO 500

#ifdef ESP32
#define ETH_PHY_TYPE        ETH_PHY_W5500
#define ETH_PHY_ADDR        1
#define ETH_PHY_CS          10
#define ETH_PHY_IRQ         9 // -1 if you won't wire
#define ETH_PHY_RST         3 // -1 if you won't wire
#define ETH_SPI_SCK         SCK
#define ETH_SPI_MISO        MISO
#define ETH_SPI_MOSI        MOSI
const int rele_id1 = 27;
const int rele_id2 = 26;
const int on = 0;
const int off = 1;
String path_buscar_att = "v1/protected/modulopgm/buscarattpgmrosinetosesp32";
char path_buscar_att_eth[] = "/v1/protected/modulopgm/buscarattpgmrosinetosesp32";
#elif defined(ESP8266)
#define CSPIN D4
const int rele_id1 = D0;
const int rele_id2 = D1;
const int on = 0;
const int off = 1;
String path_buscar_att = "v1/protected/modulopgm/buscarattpgmrosinetosesp8266";
char path_buscar_att_eth[] = "/v1/protected/modulopgm/buscarattpgmrosinetosesp8266";
#else
#error "Placa não suportada. Escolha ESP32 ou ESP8266."
#endif



String firmwareVersion = "";
String SERIAL_NUMBER = "";
/*********************PARAMETROS DE FABRICAÇÃO *****************************/
