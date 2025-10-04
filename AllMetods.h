
void iniciarServidorWeb();
void iniciarHostpotWifi();
void handleRoot();
void handleForm();
void connectToWiFi(String ssidWifi, String passwordWifi, String index);
void connectToWiFi(String ssidWifi, String passwordWifi);
String listSSID();
void handleGetWifiList();
void handleUpdateWifiList();
String informarAcao(int id_requisicao, int origem);
boolean compareEeprom(String ssidWifi, String passwordWifi);
void iniciarClienteWifi();
void connectEeprom(String index);
void connectEeprom();
void handleUpdateWifiListEsp32();
void handleUpdateWifiListEsp8266();
void salvarEeprom(String ssidWifi, String passwordWifi);
boolean reconnectWiFi();
void handleNewPassword();
void salvarSenhaAdminEeprom(String senha_admin);
void handleLogin();
String lerSenhaAdmin();
void handleExecuteLogin();
void handleConfigs();
void obterConfiguracoes();
void salvarConfiguracoesEth(String enderecoIp, String gateway, String subnet);

void salvarConfiguracoesServidor(String enderecoIp, String timeInterval);

void lerConfiguracoesEth();
void lerConfiguracoesEthSpiffs();
void salvarConfiguracoesEthSpiffs(String enderecoIp, String gateway, String subnet);
bool validarEnderecamentoWifi();

void lerConfiguracoesWifi();

String updateStatusWifi();
boolean respostaAcao(int id_acao);
void lerConfiguracoesWifiSpiffs();
void salvarConfiguracoesWifi(String enderecoIp, String gateway, String subnet);
void salvarConfiguracoesWifiSpiffs(String enderecoIp, String gateway, String subnet);
String lerConfiguracaoSpiff(String path);
void processarRequisicao(String acao);
String buscarAcoes();
String responderAcao(int id_requisicao);
void tarefaGlobal();
String buscarAcoesEthernet();
String responderAcaoEth(int id_requisicao);




String getNeedUpdate();

void atualizarStatus();

String setResponseUpdate(String body);
void salvarRegistrador(String nome, String plcPath, String dataType);
void listarRegistradores();
void processarNovosRegistradores();
void limparRegistradores();
String setUploadRegistradores(String body);
String processarUploadRegistradores();