/*************** SERVIDOR WEB FUNCOES *************************/

void handleOpcoes() {


  if (logado) {
    String responsePage = (const __FlashStringHelper*)Opcoes_page;  //Leia o conteúdo HTML

    // Substitui o marcador de posição 'checked' com base no valor passado
    if (configurado_prioridade_global == 0) {
      responsePage.replace("{{ethChecked}}", "checked");
      responsePage.replace("{{wifiChecked}}", "");
    } else {
      responsePage.replace("{{ethChecked}}", "");
      responsePage.replace("{{wifiChecked}}", "checked");
    }

    server.send(200, "text/html", responsePage);
  } else {
    handleLogin();
  }
}



void handleExecuteReset() {
  String htmlContent = "<html><head><meta charset='UTF-8'><style>";
  htmlContent += "body { display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; background-color: #f0f0f0; }";
  htmlContent += "h1 { background-color: #007bff; color: #fff; padding: 20px; }";
  htmlContent += "</style></head><body>";
  htmlContent += "<h1>O dispositivo será reiniciado.</h1>";
  htmlContent += "<script>setTimeout(function(){ window.location.href = '/'; }, 10000);</script>";
  htmlContent += "</body></html>";

  server.send(200, "text/html", htmlContent);
  // Aguardar um pouco antes de reiniciar (10 segundos no exemplo)
  delay(1000);

  ESP.restart();
}

void handleConfigs() {

  if (logado) {
    String responsePage = (const __FlashStringHelper*)Configs_page;  //Leia o conteúdo HTML



    // Substitua os marcadores de posição no HTML pelos valores das variáveis globais
    responsePage.replace("{{enderecoIpCentral}}", configurado_ip_plc_server_global);
    responsePage.replace("{{timeInterval}}", configurado_time_plc_server_global);

    server.send(200, "text/html", responsePage);
  } else {
    handleLogin();
  }
}



void handleExecuteOpcoes() {
  String prioridade = server.arg("prioridadeConexao");

  Serial.println("Prioridade da Conexão: ");
  Serial.print(prioridade);

  int Iprioridade = prioridade.toInt();
  configurado_prioridade_global = Iprioridade;

  EEPROM.begin(200);
  EEPROM.write(POSICAO_CONFIGURADO_PRIORIDADE, Iprioridade);

  EEPROM.commit();
  EEPROM.end();
}

String lerSenhaAdmin() {
  String senha = "";

  EEPROM.begin(200);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes
  int senhaSize = (int)EEPROM.read(159);

  for (int i = 0; i < senhaSize; i++) {
    senha.concat(char(EEPROM.read(i + 160)));
  }

  Serial.println("senha admin lida: ");
  Serial.print(senha);


  EEPROM.end();

  return senha;
}








void handleExecuteConfigsEth() {
  //salvar a senha do administrador
  String enderecoip = server.arg("ethIp");
  String gatewayip = server.arg("EthGateway");
  String subnetip = server.arg("EthSubnet");


  //salvarConfiguracoesEth(enderecoip, gatewayip, subnetip);
  salvarConfiguracoesEthSpiffs(enderecoip, gatewayip, subnetip);


  obterConfiguracoes();

  // Enviar resposta para o cliente antes do reinício
  String htmlContent = "<html><head><meta charset='UTF-8'><style>";
  htmlContent += "body { display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; background-color: #f0f0f0; }";
  htmlContent += "h1 { background-color: #007bff; color: #fff; padding: 20px; }";
  htmlContent += "</style></head><body>";
  htmlContent += "<h1>Configurações de IP fixo Ethernet salvos. O dispositivo será reiniciado.</h1>";
  htmlContent += "<script>setTimeout(function(){ window.location.href = '/'; }, 10000);</script>";
  htmlContent += "</body></html>";

  server.send(200, "text/html", htmlContent);
  // Aguardar um pouco antes de reiniciar (10 segundos no exemplo)
  delay(1000);



  ESP.restart();
}


void handleLogin() {

  if (!logado) {
    String responsePage = (const __FlashStringHelper*)Login_page;  //Leia o conteúdo HTML
    server.send(200, "text/html", responsePage);
  } else {
    handleRoot();
  }
}



void handleExecuteLogin() {
  String senha_admin = server.arg("password");

  String responsePage = (const __FlashStringHelper*)Login_page;  //Leia o conteúdo HTML

  String senha_admin_armazenada = lerSenhaAdmin();


  if (senha_admin_armazenada == senha_admin) {
    logado = true;
    handleRoot();
    // return;
  } else {
    responsePage.replace("<br><br>", "<p id='status'>Senha Inválida!</p>");
    server.send(200, "text/html", responsePage);
  }
}

void handleNewUse() {
  String index = (const __FlashStringHelper*)Password_page;  //Leia o conteúdo HTML
  server.send(200, "text/html", index);                      //Enviar pagina Web
}

void handleNewPassword() {

  //salvar a senha do administrador
  String senha_admin = server.arg("password");

  Serial.print("configurando nova senha admin: ");
  Serial.println(senha_admin);

  salvarSenhaAdminEeprom(senha_admin);
}


void salvarSenhaAdminEeprom(String senha_admin) {
  EEPROM.begin(200);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes

  EEPROM.write(159, senha_admin.length());

  for (int i = 0; i < senha_admin.length(); i++) {
    EEPROM.write(i + 160, senha_admin.charAt(i));
  }
  Serial.println("");

  EEPROM.write(POSICAO_PRIMEIRO_USO, 1);

  EEPROM.commit();
  EEPROM.end();

  delay(1000);
  server.send(200, "text/plain", "senha definida");
  Serial.println("[DEBUG] [SENHA] [AVISO] [salvarSenhaAdminEeprom] [NOVA SENHA DEFINIDA, REINICIANDO...] [REINICIAR ESP]");
  delay(2000);
  ESP.restart();
}



void handleExecuteConfigs() {
  //salvar a senha do administrador
  String enderecoip = server.arg("centralIP");
  String timeIntervael = server.arg("centralInterval");

  salvarConfiguracoesServidor(enderecoip, timeIntervael);

  server.send(200, "text/plain", "Configurações do Servidor foram salvas");

  obterConfiguracoes();
}



void iniciarServidorWeb() {
  //Tratamento de rotas

  if (primeiro_uso_global == 0) {
    server.on("/", handleNewUse);
    server.on("/action_define_password", handleNewPassword);
  } else {

    server.on("/", handleLogin);
    server.on("/action_login", handleExecuteLogin);
    server.on("/main", handleRoot);
    server.on("/action_new_connection", handleForm);
    server.on("/update_wifi_list", handleUpdateWifiList);
    server.on("/get_wifi_list", handleGetWifiList);
    server.on("/action_new_connection_eth", handleExecuteConfigsEth);
    server.on("/configs", handleConfigs);
    server.on("/opcoes", handleOpcoes);
    server.on("/action_reset", handleExecuteReset);
    server.on("/action_opcoes", handleExecuteOpcoes);
    server.on("/action_configs", handleExecuteConfigs);



    //server.on("/action_previous_connection", connectEeprom);
  }



  server.begin();
  Serial.println("[DEBUG] [WEB SERVER] [SUCESSO] [iniciarServidorWeb] [Servidor HTTP iniciado] []");
}

void iniciarHostpotWifi() {


  //Coloca o dispositivo no modo Wi-Fi Station
  WiFi.mode(WIFI_AP);  //Importante Definir para que o espnow funcione
  //WiFi.mode(WIFI_MODE_AP);

  String ssid_complete = String(ssid_hostpot) + SERIAL_NUMBER;
  Serial.println("[DEBUG] SSID do AP: " + ssid_complete);



  WiFi.softAP(ssid_complete.c_str(), password_hostpot);


  Serial.println("[aqui2]");

  Serial.println("[DEBUG] [WEB SERVER] [SUCESSO] [iniciarHostpotWifi] [HOSTPOT WIFI INICIADO] []");

  Serial.print("[DEBUG] [WEB SERVER] [SUCESSO] [iniciarHostpotWifi] [ENDEREÇO IP: ");
  Serial.print(WiFi.softAPIP());
  Serial.print("] []");

  Serial.print("[DEBUG] [WEB SERVER] [SUCESSO] [iniciarHostpotWifi] [REDE: ");
  Serial.print(ssid_complete.c_str());
  Serial.print("] []");


  iniciarServidorWeb();
}



void parseIPString(String ipString, int& part1, int& part2, int& part3, int& part4) {
  int dotCount = 0;
  String part;
  for (char c : ipString) {
    if (c == '.') {
      switch (dotCount) {
        case 0:
          part1 = part.toInt();
          break;
        case 1:
          part2 = part.toInt();
          break;
        case 2:
          part3 = part.toInt();
          break;
        case 3:
          part4 = part.toInt();
          break;
      }
      dotCount++;
      part = "";
    } else {
      part += c;
    }
  }
  // Observe que este bloco finaliza a última parte, após o último ponto.
  switch (dotCount) {
    case 0:
      part1 = part.toInt();
      break;
    case 1:
      part2 = part.toInt();
      break;
    case 2:
      part3 = part.toInt();
      break;
    case 3:
      part4 = part.toInt();
      break;
  }
}



void salvarConfiguracoesEthSpiffs(String enderecoIp, String gateway, String subnet) {

  writeFile(enderecoIp, PATH_IP_ETH);
  writeFile(gateway, PATH_GAT_ETH);
  writeFile(subnet, PATH_SUB_ETH);
  writeFile("1", PATH_CONFIG_ETH);
}

void lerConfiguracoesEthSpiffs() {


  File Fconfig_eth = SPIFFS.open(PATH_CONFIG_ETH, "r");
  if (!Fconfig_eth) {
    Serial.println("Erro ao abrir arquivo com Configurado?  Ethernet!");

    configurado_eth_global = 0;
    return;
  }
  String configurado = Fconfig_eth.readStringUntil('\r');
  Serial.print("IP ETH Configurado?: ");
  Serial.println(configurado);
  Fconfig_eth.close();

  int iconfig = configurado.toInt();

  if (iconfig == 1) {

    File FipEth = SPIFFS.open(PATH_IP_ETH, "r");
    if (!FipEth) {
      Serial.println("Erro ao abrir arquivo com IP do Ethernet!");
    }
    String ipEth = FipEth.readStringUntil('\r');
    Serial.print("Ip ethernet em Spiffs: ");
    Serial.println(ipEth);
    FipEth.close();

    parseIPString(ipEth, arrayEthIP[0], arrayEthIP[1], arrayEthIP[2], arrayEthIP[3]);

    ////
    File FgatEth = SPIFFS.open(PATH_GAT_ETH, "r");
    if (!FgatEth) {
      Serial.println("Erro ao abrir arquivo com Gateway do Ethernet!");
    }
    String gatEth = FgatEth.readStringUntil('\r');
    Serial.print("Gate ethernet em Spiffs: ");
    Serial.println(gatEth);
    FgatEth.close();

    parseIPString(gatEth, arrayGatewayIP[0], arrayGatewayIP[1], arrayGatewayIP[2], arrayGatewayIP[3]);

    ////
    File FsubEth = SPIFFS.open(PATH_SUB_ETH, "r");
    if (!FsubEth) {
      Serial.println("Erro ao abrir arquivo com Submascara do Ethernet!");
    }
    String subEth = FsubEth.readStringUntil('\r');
    Serial.print("Submascara ethernet em Spiffs: ");
    Serial.println(subEth);
    FsubEth.close();

    parseIPString(subEth, arraySubnetIP[0], arraySubnetIP[1], arraySubnetIP[2], arraySubnetIP[3]);

    configurado_eth_global = 1;
  } else {
    Serial.println("ETH não configurado com SPIFFS");
    configurado_eth_global = 0;
    return;
  }
}



void salvarConfiguracoesWifiSpiffs(String enderecoIp, String gateway, String subnet) {

  writeFile(enderecoIp, PATH_IP_WIFI);
  writeFile(gateway, PATH_GAT_WIFI);
  writeFile(subnet, PATH_SUB_WIFI);
  writeFile("1", PATH_CONFIG_WIFI);
}

void lerConfiguracoesWifiSpiffs() {


  File Fconfig_wifi = SPIFFS.open(PATH_CONFIG_WIFI, "r");
  if (!Fconfig_wifi) {
    Serial.println("Erro ao abrir arquivo com Configurado?  Wifi!");

    configurado_wifi_global = 0;
    return;
  }
  String configurado = Fconfig_wifi.readStringUntil('\r');
  Serial.print("IP Wifi Configurado?: ");
  Serial.println(configurado);
  Fconfig_wifi.close();

  int iconfig = configurado.toInt();

  if (iconfig == 1) {

    File FipWifi = SPIFFS.open(PATH_IP_WIFI, "r");
    if (!FipWifi) {
      Serial.println("Erro ao abrir arquivo com IP do Wifi!");
    }
    String ipWifi = FipWifi.readStringUntil('\r');
    Serial.print("Ip wifi em Spiffs: ");
    Serial.println(ipWifi);
    FipWifi.close();

    parseIPString(ipWifi, arrayWifiIP[0], arrayWifiIP[1], arrayWifiIP[2], arrayWifiIP[3]);

    ////
    File FgatWifi = SPIFFS.open(PATH_GAT_WIFI, "r");
    if (!FgatWifi) {
      Serial.println("Erro ao abrir arquivo com Gateway do Wifi!");
    }
    String gatWifi = FgatWifi.readStringUntil('\r');
    Serial.print("Gate wifi em Spiffs: ");
    Serial.println(gatWifi);
    FgatWifi.close();

    parseIPString(gatWifi, arrayWifiGatewayIP[0], arrayWifiGatewayIP[1], arrayWifiGatewayIP[2], arrayWifiGatewayIP[3]);

    ////
    File FsubWifi = SPIFFS.open(PATH_SUB_WIFI, "r");
    if (!FsubWifi) {
      Serial.println("Erro ao abrir arquivo com Submascara do Wifi!");
    }
    String subWifi = FsubWifi.readStringUntil('\r');
    Serial.print("Submascara wifi em Spiffs: ");
    Serial.println(subWifi);
    FsubWifi.close();

    parseIPString(subWifi, arrayWifiSubnetIP[0], arrayWifiSubnetIP[1], arrayWifiSubnetIP[2], arrayWifiSubnetIP[3]);

    configurado_wifi_global = 1;
  } else {
    Serial.println("Wifi não configurado com SPIFFS");
    configurado_wifi_global = 0;
    return;
  }
}



void salvarConfiguracoesWifi(String enderecoIp, String gateway, String subnet) {
  EEPROM.begin(700);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes


  int part1, part2, part3, part4;
  parseIPString(enderecoIp, part1, part2, part3, part4);


  Serial.print("IP Wifi: ");
  Serial.print(part1);
  Serial.print('.');
  Serial.print(part2);
  Serial.print('.');
  Serial.print(part3);
  Serial.print('.');
  Serial.println(part4);

  EEPROM.write(620, part1);
  EEPROM.write(621, part2);
  EEPROM.write(622, part3);
  EEPROM.write(623, part4);


  parseIPString(gateway, part1, part2, part3, part4);


  Serial.print("Gateway Wifi: ");
  Serial.print(part1);
  Serial.print('.');
  Serial.print(part2);
  Serial.print('.');
  Serial.print(part3);
  Serial.print('.');
  Serial.println(part4);

  EEPROM.write(624, part1);
  EEPROM.write(625, part2);
  EEPROM.write(626, part3);
  EEPROM.write(627, part4);

  parseIPString(subnet, part1, part2, part3, part4);


  Serial.print("Submascara Wifi: ");
  Serial.print(part1);
  Serial.print('.');
  Serial.print(part2);
  Serial.print('.');
  Serial.print(part3);
  Serial.print('.');
  Serial.println(part4);

  EEPROM.write(628, part1);
  EEPROM.write(629, part2);
  EEPROM.write(630, part3);
  EEPROM.write(631, part4);

  EEPROM.write(POSICAO_CONFIGURADO_WIFI, 1);




  EEPROM.commit();
  EEPROM.end();
}

void lerConfiguracoesWifi() {
  EEPROM.begin(700);

  Serial.println("Lendo Configurações de Wifi");

  arrayWifiIP[0] = (int)EEPROM.read(620);
  arrayWifiIP[1] = (int)EEPROM.read(621);
  arrayWifiIP[2] = (int)EEPROM.read(622);
  arrayWifiIP[3] = (int)EEPROM.read(623);

  arrayWifiGatewayIP[0] = (int)EEPROM.read(624);
  arrayWifiGatewayIP[1] = (int)EEPROM.read(625);
  arrayWifiGatewayIP[2] = (int)EEPROM.read(626);
  arrayWifiGatewayIP[3] = (int)EEPROM.read(627);


  arrayWifiSubnetIP[0] = (int)EEPROM.read(628);
  arrayWifiSubnetIP[1] = (int)EEPROM.read(629);
  arrayWifiSubnetIP[2] = (int)EEPROM.read(630);
  arrayWifiSubnetIP[3] = (int)EEPROM.read(631);


  Serial.print("Ip Wifi: ");
  Serial.print(arrayWifiIP[0]);
  Serial.print('.');
  Serial.print(arrayWifiIP[1]);
  Serial.print('.');
  Serial.print(arrayWifiIP[2]);
  Serial.print('.');
  Serial.println(arrayWifiIP[3]);


  Serial.print("Gateway Wifi: ");
  Serial.print(arrayWifiGatewayIP[0]);
  Serial.print('.');
  Serial.print(arrayWifiGatewayIP[1]);
  Serial.print('.');
  Serial.print(arrayWifiGatewayIP[2]);
  Serial.print('.');
  Serial.println(arrayWifiGatewayIP[3]);

  Serial.print("Submascara Wifi: ");
  Serial.print(arrayWifiSubnetIP[0]);
  Serial.print('.');
  Serial.print(arrayWifiSubnetIP[1]);
  Serial.print('.');
  Serial.print(arrayWifiSubnetIP[2]);
  Serial.print('.');
  Serial.println(arrayWifiSubnetIP[3]);


  EEPROM.end();
}


/*
String buscarAcoesWifi() {
 
  WiFiClient client;
  HTTPClient http;  // must be declared after WiFiClient for correct destruction order, because used by http.begin(client,...)

  Serial.println("Buscando ações via Wi-Fi...");

  // Serial.println("Buscando acoes...");
  String sn = String(SERIAL_NUMBER);
  http.begin(client, BASE_URL + path_buscar_acoes + sn + "/" + senha_api);
  http.addHeader("content-type", "application/json");


  int httpCode = http.GET();
  if (httpCode < 0) {
    Serial.println("request error - " + httpCode);
    return "request error - " + httpCode;
  }
  if (httpCode != HTTP_CODE_OK) {
    Serial.println("request error - " + httpCode);
    return "";
  }
  String response = http.getString();
  Serial.print("Acao Recebida: ");
  Serial.println(response);
  http.end();



  return response;
 
  return "";
}
 */



/*
String responderAcao(int id_requisicao) {

  String sn = String(SERIAL_NUMBER);

  WiFiClient client;
  HTTPClient http;  // must be declared after WiFiClient for correct destruction order, because used by http.begin(client,...)


  Serial.println("Respondendo requisicao de acao");
  String Sid_requisicao = String(id_requisicao);
  Serial.print("Id da requisicao: ");
  Serial.println(Sid_requisicao);
  http.begin(client, BASE_URL + path_responder_acao + Sid_requisicao + "/" + sn + "/" + senha_api);
  http.addHeader("content-type", "application/json");


  int httpCode = http.GET();

  if (httpCode < 0) {
    Serial.println("request error - " + httpCode);
    return "errou";
  }
  if (httpCode != HTTP_CODE_OK) {
    Serial.println("request error - " + httpCode);
    return "errou";
  }
  String response = http.getString();
  Serial.print("Responda da api: ");
  Serial.println(response);
  http.end();

  Serial.println("Feito");


  return "sucesso";
  
  return "";
}
*/





String getNeedUpdate() {
  Serial.println("[DEBUG] [] [AVISO] [getNeedUpdate] [Verificar por atualização...] []");

  WiFiClientSecure testClient;

  HTTPClient http;
  testClient.setCACert(root_ca);

  String resposta = "";
  String url = "https://machine.mach-wise.com/app/v1/iot/need-update";

  if (http.begin(testClient, url)) {
    http.addHeader("x-api-key", x_api_key);
    http.addHeader("x-security-id", x_security_id);
    http.addHeader("Host", "machine.mach-wise.com");

    int httpCode = http.GET();
    Serial.printf("Código HTTP: %d\n", httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("[DEBUG] [] [AVISO] [getNeedUpdate] [Requisição HTTPS COMPLETA] []");

      Serial.println("Payload:");
      Serial.println(payload);

      int indexI = payload.indexOf("{");
      int indexF = payload.lastIndexOf("}");

      if (indexI >= 0 && indexF > indexI) {
        resposta = payload.substring(indexI, indexF + 1);
      }
      Serial.println("Tratado:");
      Serial.println(resposta);

    } else {
      Serial.printf("Erro na requisição: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("[DEBUG] [] [FATAL] [getNeedUpdate] [Erro ao conectado ao servidor] []");
      resposta = "errou";
    }

    http.end();
  } else {
    Serial.println("Erro na requisicao");
    resposta = "errou";
  }



  return resposta;
}





String setResponseUpdate(String body) {
  Serial.println("[DEBUG] [] [AVISO] [setResponseUpdate] [Responder sobre Update de Tabela de Registradores] []");

  WiFiClientSecure testClient;

  HTTPClient http;
  testClient.setCACert(root_ca);

  String resposta = "";
  String url = "https://machine.mach-wise.com/app/v1/iot/updated";


  Serial.println("Body: ");
  Serial.println(body);

  if (http.begin(testClient, url)) {
    http.addHeader("Content-Type", "application/json");  // Header necessário para JSON
    http.addHeader("x-api-key", x_api_key);
    http.addHeader("x-security-id", x_security_id);
    http.addHeader("Host", "machine.mach-wise.com");

    int httpCode = http.PATCH(body);
    Serial.printf("Código HTTP: %d\n", httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("[DEBUG] [] [AVISO] [setResponseUpdate] [Requisição HTTPS COMPLETA] []");

      Serial.println("Payload:");
      Serial.println(payload);

      int indexI = payload.indexOf("{");
      int indexF = payload.lastIndexOf("}");

      if (indexI >= 0 && indexF > indexI) {
        resposta = payload.substring(indexI, indexF + 1);
      }
      Serial.println("Tratado:");
      Serial.println(resposta);
    } else {
      Serial.printf("Erro na requisição: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("[DEBUG] [] [FATAL] [setResponseUpdate] [Erro ao conectar ao servidor] []");
      resposta = "errou";
    }

    http.end();
  } else {
    Serial.println("Erro ao iniciar a conexão HTTP");
    resposta = "errou";
  }

  return resposta;
}





String getRegistradores() {
  Serial.println("[DEBUG] [] [AVISO] [getRegistradores] [Buscando tabela de registradores..] []");

  WiFiClientSecure testClient;

  HTTPClient http;
  testClient.setCACert(root_ca);

  String resposta = "";
  String url = "https://machine.mach-wise.com/app/v1/iot/plc-path";

  if (http.begin(testClient, url)) {
    http.addHeader("x-api-key", x_api_key);
    http.addHeader("x-security-id", x_security_id);
    http.addHeader("Host", "machine.mach-wise.com");

    int httpCode = http.GET();
    Serial.printf("Código HTTPS: %d\n", httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("[DEBUG] [] [AVISO] [getRegistradores] [Requisição HTTPS COMPLETA] []");

      Serial.println("Payload:");
      Serial.println(payload);

      int indexI = payload.indexOf("{");
      int indexF = payload.lastIndexOf("}");

      if (indexI >= 0 && indexF > indexI) {
        // resposta = payload.substring(indexI, indexF + 1);
      }
      resposta = payload;

      Serial.println("Tratado:");
      Serial.println(resposta);

    } else {
      Serial.printf("Erro na requisição: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("[DEBUG] [] [FATAL] [getRegistradores] [Erro ao conectado ao servidor] []");
      resposta = "errou";
    }

    http.end();
  } else {
    Serial.println("Erro na requisicao");
    resposta = "errou";
  }



  return resposta;
}


String setUploadRegistradores(String body) {
  Serial.println("[DEBUG] [] [AVISO] [setUploadRegistradores] [Upload de Registradores] []");

  WiFiClientSecure testClient;

  HTTPClient http;
  testClient.setCACert(root_ca);

  String resposta = "";
  String url = "https://zjsuhjl0tg.execute-api.us-east-1.amazonaws.com/receiver/";


  Serial.println("Body: ");
  Serial.println(body);

  if (http.begin(testClient, url)) {
    http.addHeader("Content-Type", "application/json");  // Header necessário para JSON
    http.addHeader("x-api-key", "WfsrrSLK5f74vYcZDrWIw44mu3aInFSU2i5TUsbU");
    http.addHeader("x-usual-id", "5cc35301-5fcd-47fa-abb4-dd3f4d6cbf43");
    http.addHeader("x-tenet-id", "e987d790-4a76-33f5-8a0a-be965e11apf");
    http.addHeader("Host", "zjsuhjl0tg.execute-api.us-east-1.amazonaws.com");

    int httpCode = http.POST(body);
    Serial.printf("Código HTTP: %d\n", httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("[DEBUG] [] [AVISO] [setUploadRegistradores] [Requisição HTTPS COMPLETA] []");

      Serial.println("Payload:");
      Serial.println(payload);

      int indexI = payload.indexOf("{");
      int indexF = payload.lastIndexOf("}");

      if (indexI >= 0 && indexF > indexI) {
        resposta = payload.substring(indexI, indexF + 1);
      }
      Serial.println("Tratado:");
      Serial.println(resposta);
    } else {
      Serial.printf("Erro na requisição: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("[DEBUG] [] [FATAL] [setUploadRegistradores] [Erro ao conectar ao servidor] []");
      resposta = "errou";
    }

    http.end();
  } else {
    Serial.println("Erro ao iniciar a conexão HTTP");
    resposta = "errou";
  }

  return resposta;
}


String buscarAcoesWifi() {
  //  Serial.println("Buscando ações via WiFi...");

  String response = "";
  WiFiClient client;

  String sn = String(SERIAL_NUMBER);
  String url = String(path_buscar_acoes_eth) + sn + "/" + senha_api;

  if (client.connect(BASE_URL_ETH, BASE_PORT_ETH)) {
    //Serial.println(url);

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL_ETH));
    client.println("Content-Type: application/json");
    client.println("Connection: keep-alive");
    client.println();

    unsigned long tempoInicio = millis();  // Marca o tempo inicial
    String respostaServidor = "";

    // Aguarda a resposta sem travar o loop
    while (millis() - tempoInicio < TEMPO_ISPEIRA) {  // Timeout de 3 segundos
      while (client.available()) {
        char c = client.read();
        respostaServidor += c;
        tempoInicio = millis();  // Reseta o tempo se receber dados
      }
      if (!client.connected()) break;  // Sai se a conexão for fechada
      yield();                         // Evita o watchdog reset no ESP8266
    }

    int indexI = respostaServidor.indexOf("{");
    int indexF = respostaServidor.lastIndexOf("}");

    if (indexI >= 0 && indexF > indexI) {
      response = respostaServidor.substring(indexI, indexF + 1);
    }

  } else {
    Serial.println("Falha na conexão ao buscar ações via WiFi");
    contador_falhas_via_wifi++;
    return "errou";
  }

  client.stop();
  return response;
}

String responderAcao(int id_requisicao) {
  Serial.println("Respondendo requisição de ação via WiFi");

  String Sid_requisicao = String(id_requisicao);
  String sn = String(SERIAL_NUMBER);
  String url = String(path_responder_acao_eth) + Sid_requisicao + "/" + sn + "/" + senha_api;

  WiFiClient client;

  if (client.connect(BASE_URL_ETH, BASE_PORT_ETH)) {
    Serial.println(url);

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL_ETH));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();

    unsigned long tempoInicio = millis();  // Marca o tempo inicial
    String respostaServidor = "";

    // Aguarda a resposta sem bloquear
    while (millis() - tempoInicio < TEMPO_ISPEIRA) {  // Timeout de 3 segundos
      while (client.available()) {
        char c = client.read();
        respostaServidor += c;
        tempoInicio = millis();  // Reseta o tempo se receber dados
      }
      if (!client.connected()) break;  // Sai se a conexão for fechada
      yield();                         // Evita watchdog reset no ESP8266
    }

    if (respostaServidor.length() > 0) {
      //Serial.println("Resposta do servidor:");
      //Serial.println(respostaServidor);
    } else {
      Serial.println("Nenhuma resposta recebida.");
    }

  } else {
    Serial.println("Falha na conexão ao responder ação via WiFi");
    contador_falhas_via_wifi++;
    return "errou";
  }

  client.stop();
  return "sucesso";
}








String informarAcao(int id_requisicao, int origem) {
  Serial.println("Informando requisição de ação via WiFi");

  String sn = String(SERIAL_NUMBER);
  String Sid_requisicao = String(id_requisicao);
  String Sorigem = String(origem);

  String url = String(path_informar_acao_eth) + sn + "/" + senha_api + "/" + Sid_requisicao + "/" + Sorigem;
  WiFiClient client;

  if (client.connect(BASE_URL_ETH, BASE_PORT_ETH)) {
    Serial.println("Conectado ao servidor [informarAcaoWifi]");
    Serial.println(url);

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL_ETH));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();

    unsigned long tempoInicio = millis();  // Marca o tempo inicial
    String respostaServidor = "";

    // Aguarda a resposta sem bloquear a execução
    while (millis() - tempoInicio < TEMPO_ISPEIRA) {  // Timeout de 3 segundos
      while (client.available()) {
        char c = client.read();
        respostaServidor += c;
        tempoInicio = millis();  // Reseta o tempo se receber dados
      }
      if (!client.connected()) break;  // Sai se a conexão for fechada
      yield();                         // Evita watchdog reset no ESP8266
    }

    if (respostaServidor.length() > 0) {
      Serial.println("Resposta do servidor:");
      Serial.println(respostaServidor);
    } else {
      Serial.println("Nenhuma resposta recebida.");
    }

  } else {
    Serial.println("Falha ao informar ação via WiFi");
    contador_falhas_via_wifi++;
    return "errou";
  }

  client.stop();
  return "sucesso";
}


String lerConfiguracaoSpiff(String path) {

  File file = SPIFFS.open(path, "r");
  if (!file) {
    Serial.println("Erro ao abrir arquivo com Configuração ");
    Serial.println(path);
    return "";
  } else {
    Serial.println("arquivo aberto!");
  }


  String valor = file.readStringUntil('\r');
  Serial.print("Valor: ");
  Serial.println(valor);

  file.close();
  return valor;
}



void buscarAtt() {


#ifdef ESP32
  esp_task_wdt_delete(NULL);
#elif defined(ESP8266)
  ESP.wdtDisable();
#else
#error "Placa não suportada. Escolha ESP32 ou ESP8266."
#endif

  WiFiClient transport;
  HttpClient client(transport, BASE_URL_ETH_ATT, SERVER_PORT_ETH_ATT);


  client.setTimeout(15000);  // 10 segundos

  client.get(path_buscar_att_eth);

  int statusCode = client.responseStatusCode();
  Serial.print("Update status code: ");
  Serial.println(statusCode);
  if (statusCode != 200) {

    client.stop();
    return;
  }

  long length = client.contentLength();
  if (length == HttpClient::kNoContentLengthHeader) {
    client.stop();
    Serial.println("Server didn't provide Content-length header. Can't continue with update.");
    return;
  }
  Serial.print("Server returned update file of size ");
  Serial.print(length);
  Serial.println(" bytes");

  if (!Update.begin(length, U_FLASH)) {
    client.stop();
    Serial.println("There is not enough space to store the update. Can't continue with update.");
    return;
  }
  byte buffer[BUFFER_SIZE];

  while (length > 0) {
    int readSize = min((long)BUFFER_SIZE, length);  // Converte BUFFER_SIZE para long
    int bytesRead = client.readBytes(buffer, readSize);
    if (bytesRead <= 0) {
      Serial.println("Erro ao ler os dados do servidor");
      break;
    }
    Update.write(buffer, bytesRead);
    length -= bytesRead;
  }
  Update.end(false);
  client.stop();
  if (length > 0) {
    Serial.print("Timeout downloading update file at ");
    Serial.print(length);
    Serial.println(" bytes. Can't continue with update.");
    return;
  }

  Serial.println("Sketch update apply and reset.");
  Serial.flush();
  ESP.restart();
}



String updateStatusWifi() {
  // Serial.println("Atualizando status via WiFi...");

  String url = String(path_avisar_online_eth) + SERIAL_NUMBER + "/" + senha_api;
  WiFiClient client;

  StaticJsonDocument<4096> doc;
  doc["firmware_version"] = firmwareVersion;
  doc["conectado_wifi"] = WiFi.status() == WL_CONNECTED;

  if (WiFi.status() == WL_CONNECTED) {
    doc["ssid"] = ssid_wifi_global;
    doc["passwd"] = passwd_wifi_global;
    doc["ip_wifi"] = WiFi.localIP().toString();
    doc["submascara_wifi"] = WiFi.subnetMask().toString();
    doc["gateway_wifi"] = WiFi.gatewayIP().toString();
  }

  doc["prioridade"] = String(configurado_prioridade_global);
  doc["ip_eth"] = Ethernet.localIP().toString();
  doc["gateway_eth"] = Ethernet.gatewayIP().toString();
  doc["submascara_eth"] = Ethernet.subnetMask().toString();

  doc["conectado_eth"] = (isETH && flag && Ethernet.linkStatus() != LinkOFF) ? "true" : "false";

  doc["status_rele1"] = (digitalRead(rele_id1) == on) ? "on" : "off";
  doc["status_rele2"] = (digitalRead(rele_id2) == on) ? "on" : "off";

  doc["atualizado_via"] = "wifi";
  doc["log"] = "";



  String requestBody;
  serializeJson(doc, requestBody);

  //  Serial.print("Body da atualização de status: ");
  //Serial.println(requestBody);

  if (client.connect(BASE_URL_ETH, BASE_PORT_ETH)) {
    //  Serial.println("Conectado ao servidor [updateStatusWifi]");

    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + String(BASE_URL_ETH));
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(requestBody.length()));
    client.println();
    client.println(requestBody);

    unsigned long tempoInicio = millis();  // Marca o tempo inicial
    String respostaServidor = "";

    // Aguarda a resposta sem bloquear a execução
    while (millis() - tempoInicio < TEMPO_ISPEIRA) {  // Timeout de 3 segundos
      while (client.available()) {
        char c = client.read();
        respostaServidor += c;
        tempoInicio = millis();  // Reseta o tempo se receber dados
      }
      if (!client.connected()) break;  // Sai se a conexão for fechada
      yield();                         // Evita watchdog reset no ESP8266
    }

    if (respostaServidor.length() > 0) {
      // Serial.println("Resposta do servidor:");
      // Serial.println(respostaServidor);
      contador_falhas_via_wifi = 0;
    } else {
      Serial.println("Falha ao atualizar status via WiFi (sem resposta)");
      contador_falhas_via_wifi++;
      client.stop();
      return "errou";
    }
  } else {
    Serial.println("Falha ao conectar ao servidor para atualizar status via WiFi");
    contador_falhas_via_wifi++;
    client.stop();
    return "errou";
  }

  client.stop();
  Serial.println("Feito [updateStatusWifi]");
  return "sucesso";
}



/*************** SERVIDOR WEB FUNCOES *************************/
