/******************** WIFI FUNÇOES ***********************************/
String byteArrayToString(byte* array, int size) {
  String result = "";
  for (int i = 0; i < size; i++) {
    result += String(array[i], HEX);
    if (i < size - 1) result += ":";
  }
  return result;
}


String intArrayToString(int* array, int size) {
  String result = "";
  for (int i = 0; i < size; i++) {
    result += String(array[i]);
    if (i < size - 1) result += ".";
  }
  return result;
}


void raiz(String index) {
}

void handleRoot() {
  if (logado) {
    String index = (const __FlashStringHelper*)MAIN_page;
    Serial.println("Redes escaneadas!");

    index.replace("<h3>serial_number</h3>", "S/N: " + SERIAL_NUMBER);
    index.replace("<h3>end_mac</h3>", "Wi-Fi MAC: " + WiFi.macAddress());

    String macString = byteArrayToString(mac_ethernet, sizeof(mac_ethernet));

    index.replace("<h3>end_mac_eth</h3>", "Ethernet MAC: " + macString);

    if (!conectado_ethernet) {
      index.replace("<br><br>", "<p id='statusDivEth'>Não Conectado!</p>");
    } else {

      if (!(Ethernet.linkStatus() == LinkOFF)) {
        index.replace("{{ethIp}}", Ethernet.localIP().toString());
        index.replace("{{EthGateway}}", Ethernet.gatewayIP().toString());
        index.replace("{{EthSubnet}}", Ethernet.subnetMask().toString());
        index.replace("<br><br>", "<p id='statusDivEth'> Conectado!</p>");
      }
    }

    if (!(WiFi.status() == WL_CONNECTED)) {
      index.replace("{{ip}}", "");
      index.replace("{{gateway}}", "");
      index.replace("{{mascara}}", "");
      index.replace("<br><br>", "<p id='status'>Não Conectado!</p>");
      server.send(200, "text/html", index);
      //  Serial.println("Não esta conectado ao wifi, conectando!");
      // connectEeprom(index);
    } else {

      String ip = WiFi.localIP().toString();
      String gateway = WiFi.gatewayIP().toString();



      // Substitua os marcadores de posição no HTML pelos valores das variáveis globais
      index.replace("{{ip}}", ip);
      index.replace("{{gateway}}", gateway);
      index.replace("{{mascara}}", String(24));




      String texto = "Conectado a rede ";
      texto.concat(ssid_wifi_global);
      texto.concat(" IP: ");
      texto.concat(ip);
      texto.concat(" Gateway: ");
      texto.concat(gateway);


      index.replace("<br><br>", "<p id='status'>Conectado!</p>");
      index.replace("<h3>rede_conectada</h3>", texto);
      Serial.println("Retornando pagina web!");
      server.send(200, "text/html", index);
    }
  } else {
    handleLogin();
  }
}

void scanWifiTaskEsp32(void* parameter) {
  int n = WiFi.scanNetworks();
  jsonResponse = "[";

  for (int i = 0; i < n; ++i) {
    if (i > 0) jsonResponse += ",";
    jsonResponse += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
  }

  jsonResponse += "]";
  Serial.print("Redes: ");
  Serial.println(jsonResponse);

  scanComplete = true;  // Marca a varredura como concluída
#ifdef ESP32
  vTaskDelete(NULL);  // Encerra a tarefa
#endif
}


void scanWifiTaskEsp8266() {
  int n = WiFi.scanNetworks();
  jsonResponse = "[";

  for (int i = 0; i < n; ++i) {
    if (i > 0) jsonResponse += ",";
    jsonResponse += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
  }

  jsonResponse += "]";
  Serial.print("Redes: ");
  Serial.println(jsonResponse);

  scanComplete = true;  // Marca a varredura como concluída
  if (scanComplete) {
    // Envia o resultado da varredura como resposta
    Serial.println("Enviando!");
    server.send(200, "application/json", jsonResponse);
    scanComplete = false;  // Resetar o estado após enviar os dados
  } else {
    Serial.println("Pendente!");
    // Se a varredura ainda não estiver completa, envia um status pendente
    server.send(200, "application/json", "{\"status\":\"Pendente\"}");
  }
}


void handleUpdateWifiList() {
#ifdef ESP32
  handleUpdateWifiListEsp32();
#elif defined(ESP8266)
  handleUpdateWifiListEsp8266();
#endif
}

void handleUpdateWifiListEsp32() {
  Serial.println("Iniciando varredura de redes Wi-Fi...");

// Cria uma tarefa para realizar a varredura de redes Wi-Fi
#ifdef ESP32
  xTaskCreatePinnedToCore(
    scanWifiTaskEsp32,  // Função da tarefa
    "ScanWifi",         // Nome da tarefa
    4096,               // Tamanho da stack da tarefa
    NULL,               // Parâmetro da tarefa
    1,                  // Prioridade da tarefa
    NULL,               // Handle da tarefa (não precisa)
    1                   // Núcleo para executar a tarefa
  );
#endif

  // Envia uma resposta imediata ao cliente, avisando que a varredura está em andamento
  server.send(200, "application/json", "{\"status\":\"Varredura iniciada\"}");
}


void handleUpdateWifiListEsp8266() {
  Serial.println("Iniciando varredura de redes Wi-Fi...");

  scanWifiTaskEsp8266();
}

void handleGetWifiList() {
  if (scanComplete) {
    // Envia o resultado da varredura como resposta
    Serial.println("Enviando!");
    server.send(200, "application/json", jsonResponse);
    scanComplete = false;  // Resetar o estado após enviar os dados
  } else {
    Serial.println("Pendente!");
    // Se a varredura ainda não estiver completa, envia um status pendente
    server.send(200, "application/json", "{\"status\":\"Pendente\"}");
  }
}

String mascaraParaSubmascara(const String& numBits) {
  // Converte a string para um número inteiro
  int bits = numBits.toInt();

  // Verifica se a conversão foi bem-sucedida
  if (bits == 0 && numBits != "0") {
    return "Valor inválido";
  }

  // Verifica se o número de bits está dentro do intervalo válido
  if (bits < 1 || bits > 32) {
    return "Máscara inválida";
  }

  // Calcula a submáscara
  unsigned long submascara = 0xFFFFFFFFUL << (32 - bits);

  // Converte a submáscara para o formato de string
  String submascaraStr = String((submascara >> 24) & 0xFF) + "." + String((submascara >> 16) & 0xFF) + "." + String((submascara >> 8) & 0xFF) + "." + String(submascara & 0xFF);

  return submascaraStr;
}

void handleForm() {


  String ssidWifi = server.arg("ssid");
  String passwordWifi = server.arg("password");

  String ip = server.arg("ip");
  String gateway = server.arg("gate");
  String mascara = server.arg("maska");



  Serial.printf("SSID: %s\n", ssidWifi);
  Serial.printf("Password: %s\n", passwordWifi);
  Serial.printf("Ip: %s\n", ip);
  Serial.printf("Gateway: %s\n", gateway);
  Serial.printf("Mascara: %s\n", mascara);


  String submascara = mascaraParaSubmascara(mascara);
  Serial.println("Submáscara para /" + mascara + ": " + submascara);


  // salvarConfiguracoesWifi(ip, gateway, submascara);
  salvarConfiguracoesWifiSpiffs(ip, gateway, submascara);

  //lerConfiguracoesWifi();
  lerConfiguracoesWifiSpiffs();
  configurado_wifi_global = 1;


  if (!ssidWifi.equals("") && !passwordWifi.equals("")) {
    connectToWiFi(ssidWifi, passwordWifi, "");
  }
}

boolean reconnectWiFi() {

  if (configurado_wifi_global == 1) {


    IPAddress manualIP(arrayWifiIP[0], arrayWifiIP[1], arrayWifiIP[2], arrayWifiIP[3]);
    IPAddress manualGateway(arrayWifiGatewayIP[0], arrayWifiGatewayIP[1], arrayWifiGatewayIP[2], arrayWifiGatewayIP[3]);
    IPAddress manualSubnet(arrayWifiSubnetIP[0], arrayWifiSubnetIP[1], arrayWifiSubnetIP[2], arrayWifiSubnetIP[3]);

    IPAddress newDns(8, 8, 8, 8);


    WiFi.config(manualIP, newDns, manualGateway, manualSubnet);
  }


  int count = 0;
  while (count < 10) {
    Serial.println("Reconectando ao Wi-Fi...");
    if (WiFi.begin(ssid_wifi_global, passwd_wifi_global) == WL_CONNECTED) {

      conectado_wifi = true;

      //Se a conexão ocorrer com sucesso, mostre o endereço IP no monitor serial
      Serial.println("Reconectado ao WiFi");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("[DEBUG] [WIFI] [SUCESSO] [reconnectWifi] [RECONECTADO AO WIFI] []");
      return true;
    }
    count++;
    delay(1000);
  }

  Serial.print("[DEBUG] [WIFI] [ERRO] [reconnectWifi] [FALHA AO CONECTAR AO WIFI APÓS 10 TENTATIVAS] []");
  return false;
}

void connectToWiFi(String ssidWifi, String passwordWifi, String index) {
  int count = 0;


  if (index == "") {
    index = (const __FlashStringHelper*)MAIN_page;  //Leia o conteúdo HTML
  }

  if (configurado_wifi_global == 1) {
    Serial.println("Endereçamento Wifi Configurado!");
    if (validarEnderecamentoWifi()) {
      Serial.println("Endereçamento Wifi é Válido!");

      IPAddress manualIP(arrayWifiIP[0], arrayWifiIP[1], arrayWifiIP[2], arrayWifiIP[3]);
      IPAddress manualGateway(arrayWifiGatewayIP[0], arrayWifiGatewayIP[1], arrayWifiGatewayIP[2], arrayWifiGatewayIP[3]);
      IPAddress manualSubnet(arrayWifiSubnetIP[0], arrayWifiSubnetIP[1], arrayWifiSubnetIP[2], arrayWifiSubnetIP[3]);
      IPAddress newDns(8, 8, 8, 8);


      WiFi.config(manualIP, newDns, manualGateway, manualSubnet);

    } else {
      Serial.println("Endereçamento Wifi não é Válido!");
    }
  } else {
    Serial.println("Endereçamento Wifi não Configurado!");
  }



  WiFi.begin(ssidWifi.c_str(), passwordWifi.c_str());  //Conecta com seu roteador
  Serial.println("");

  //Espera por uma conexão
  while (count < 20) {
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      salvarEeprom(ssidWifi, passwordWifi);

      cliente_wifi_salvo = true;
      ssid_wifi_global = ssidWifi;
      passwd_wifi_global = passwordWifi;

      Serial.println("");
      conectado_wifi = true;

      //Se a conexão ocorrer com sucesso, mostre o endereço IP no monitor serial
      Serial.println("Conectado ao WiFi");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  //Endereço IP do ESP8266

      String ip = WiFi.localIP().toString();
      String gateway = WiFi.gatewayIP().toString();
      String texto = "Conectado a rede ";
      texto.concat(ssidWifi);
      texto.concat(" IP: ");
      texto.concat(ip);
      texto.concat(" Gateway: ");
      texto.concat(gateway);

      index.replace("<br><br>", "<p id='status'>Conectado!</p>");
      index.replace("<h3>rede_conectada</h3>", texto);


      String sn = String(SERIAL_NUMBER);
      index.replace("<h3>serial_number</h3>", "S/N: " + sn);
      index.replace("<h3>end_mac</h3>", "Wi-Fi MAC: " + WiFi.macAddress());

      String macString = byteArrayToString(mac_ethernet, sizeof(mac_ethernet));

      index.replace("<h3>end_mac_eth</h3>", "Ethernet MAC: " + macString);

      if (!conectado_ethernet) {
        index.replace("<br><br>", "<p id='statusDivEth'>Não Conectado!</p>");
      } else {

        // Substitua os marcadores de posição no HTML pelos valores das variáveis globais
        String ipEthString = intArrayToString(arrayEthIP, sizeof(arrayEthIP) / sizeof(arrayEthIP[0]));
        String gatewayEthString = intArrayToString(arrayGatewayIP, sizeof(arrayGatewayIP) / sizeof(arrayGatewayIP[0]));
        String subnetEthString = intArrayToString(arraySubnetIP, sizeof(arraySubnetIP) / sizeof(arraySubnetIP[0]));

        index.replace("{{ethIp}}", ipEthString);
        index.replace("{{EthGateway}}", gatewayEthString);
        index.replace("{{EthSubnet}}", subnetEthString);
        index.replace("<br><br>", "<p id='statusDivEth'> Conectado!</p>");
      }


      if (configurado_wifi_global == 1) {
        // Substitua os marcadores de posição no HTML pelos valores das variáveis globais
        ip = intArrayToString(arrayWifiIP, sizeof(arrayWifiIP) / sizeof(arrayWifiIP[0]));
        gateway = intArrayToString(arrayWifiGatewayIP, sizeof(arrayWifiGatewayIP) / sizeof(arrayWifiGatewayIP[0]));
      }


      // Substitua os marcadores de posição no HTML pelos valores das variáveis globais
      index.replace("{{ip}}", ip);
      index.replace("{{gateway}}", gateway);
      index.replace("{{mascara}}", String(24));


      index.replace("<br><br>", "<p id='status'>Conectado!</p>");
      index.replace("<h3>rede_conectada</h3>", texto);
      server.send(200, "text/html", index);

      return;
    }
    count++;
  }

  String responsePage = (const __FlashStringHelper*)MAIN_page;
  responsePage.replace("<br><br>", "<p id='status' style='color:red;'>Falha na conexão.</p>");
  server.send(200, "text/html", responsePage);
  return;
}

String listSSID() {
  String index = (const __FlashStringHelper*)MAIN_page;  //Leia o conteúdo HTML
  String networks = "";
  int n = WiFi.scanNetworks();
  Serial.println("Scan done.");
  if (n == 0) {
    Serial.println("Nenhuma rede encontrada.");
    index.replace("<select class='text-field' name='ssid'></select>", "<select class='text-field' name='ssid'><option value='' disabled selected>Nenhuma rede encontrada</option></select>");
    index.replace("<br><br>", "<p id='status' style='color:red;'>Rede não encontrada.</p>");
    return index;
  } else {
    Serial.printf("%d networks found.\n", n);
    networks += "<select class='text-field' name='ssid'><option value='' disabled selected>SSID</option>";
    for (int i = 0; i < n; ++i) {
      // Imprime o SSID de cada rede encontrada
      networks += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";
    }
    networks += "</select>";
  }
  index.replace("<select class='text-field' name='ssid'></select>", networks);
  return index;
}


void salvarEeprom(String ssidWifi, String passwordWifi) {
  EEPROM.begin(200);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes

  if (!compareEeprom(ssidWifi, passwordWifi)) {
    Serial.println("Salvando:");
    EEPROM.write(0, ssidWifi.length());
    Serial.println(ssidWifi.length());

    for (int i = 2; i < 2 + ssidWifi.length(); i++) {
      Serial.print(ssidWifi.charAt(i - 2));
      EEPROM.write(i, ssidWifi.charAt(i - 2));
    }
    Serial.println("");

    Serial.println("Salvando:");
    EEPROM.write(1, passwordWifi.length());
    Serial.println(passwordWifi.length());

    for (int j = 2 + ssidWifi.length(); j < 2 + ssidWifi.length() + passwordWifi.length(); j++) {
      Serial.print(passwordWifi.charAt(j - 2 - ssidWifi.length()));
      EEPROM.write(j, passwordWifi.charAt(j - 2 - ssidWifi.length()));
    }
    Serial.println("");

    EEPROM.commit();  // Salva alterações na FLASH
  }
  EEPROM.end();  // Apaga a cópia da EEPROM salva na RAM
}

boolean compareEeprom(String ssidWifi, String passwordWifi) {
  int idLength = int(EEPROM.read(0));    // Tamanho do SSID armazenado (número de bytes)
  int passLength = int(EEPROM.read(1));  // Tamanho do Password armazenado (número de bytes)
  String id = "";
  String pass = "";

  //Serial.println("Lendo SSID:");
  //Serial.print("Tamanho:");
  //Serial.println(idLength);
  for (int i = 2; i < 2 + idLength; i++) {
    // Serial.print("Posição ");
    // Serial.print(i);
    // Serial.print(": ");
    id = id + char(EEPROM.read(i));
    // Serial.println(id[i - 2]);
  }
  //Serial.println("");

  // Serial.println("Lendo senha:");
  // Serial.print("Tamanho:");
  // Serial.println(passLength);
  for (int j = 2 + idLength; j < 2 + idLength + passLength; j++) {
    // Serial.print("Posição ");
    // Serial.print(j);
    // Serial.print(": ");
    pass = pass + char(EEPROM.read(j));
    //   Serial.println(pass[j - 2 - idLength]);
    //  Serial.println(pass);
  }
  //Serial.println("");

  Serial.print("SSID é igual: ");
  Serial.println(id.equals(ssidWifi));

  Serial.print("Senha é igual: ");
  Serial.println(pass.equals(passwordWifi));

  if (id.equals(ssidWifi) && pass.equals(passwordWifi)) {
    Serial.println("Dados já presentes na memória.");
    return true;
  } else {
    return false;
  }
}



void connectEeprom() {
  EEPROM.begin(200);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes

  int ssidSize = (int)EEPROM.read(0);      // Tamanho do SSID armazenado (número de bytes)
  int passwordSize = (int)EEPROM.read(1);  // Tamanho do Password armazenado (número de bytes)
  String ssidWifi = "";
  String passwordWifi = "";

  Serial.println("Lendo:");
  for (int i = 2; i < 2 + ssidSize; i++) {
    Serial.print(char(EEPROM.read(i)));
    ssidWifi.concat(char(EEPROM.read(i)));
  }
  Serial.println("");

  Serial.println("Lendo:");
  for (int j = 2 + ssidSize; j < 2 + ssidSize + passwordSize; j++) {
    Serial.print(char(EEPROM.read(j)));
    passwordWifi.concat(char(EEPROM.read(j)));
  }
  Serial.println("");

  EEPROM.end();  // Apaga a cópia da EEPROM salva na RAM

  Serial.println("Leu:");
  Serial.println(ssidWifi);
  Serial.println(passwordWifi);

  cliente_wifi_salvo = true;
  ssid_wifi_global = ssidWifi;
  passwd_wifi_global = passwordWifi;



  if (cliente_wifi_salvo) {

    bool prosseguir = true;

    if (ssidWifi.length() > 30 || passwordWifi.length() > 30) {
      prosseguir = false;
    } else {
      //Validar rede e senha salvos
      for (int i = 0; i < ssidWifi.length(); i++) {
        if (isPrintable(ssidWifi.charAt(i))) {
        } else {
          prosseguir = false;
          break;
        }
      }
    }


    if (prosseguir) {
      Serial.println("[DEBUG] [CLIENTE WIFI] [AVISO] [connectEeprom] [REDE LIDA NA EEPROM] []");
      connectToWiFi(ssidWifi, passwordWifi);

    } else {
      Serial.println("[DEBUG] [CLIENTE WIFI] [FALHA] [connectEeprom] [REDE LIDA CORROMPIDA] []");
    }
  }
}

bool validarEnderecamentoWifi() {

  for (int i = 0; i < 4; i++) {
    if (arrayWifiIP[i] < 0 || arrayWifiIP[i] >= 255) {
      return false;
    }
  }


  for (int i = 0; i < 4; i++) {
    if (arrayWifiGatewayIP[i] < 0 || arrayWifiGatewayIP[i] >= 255) {
      return false;
    }
  }



  for (int i = 0; i < 4; i++) {
    if (arrayWifiSubnetIP[i] < 0 || arrayWifiSubnetIP[i] > 255) {
      return false;
    }
  }


  return true;
}

void connectToWiFi(String ssidWifi, String passwordWifi) {
  int count = 0;

  /*

  if (configurado_wifi_global == 1) {
    Serial.println("Endereçamento Wifi Configurado!");
    if (validarEnderecamentoWifi()) {
      Serial.println("Endereçamento Wifi é Válido!");

      IPAddress manualIP(arrayWifiIP[0], arrayWifiIP[1], arrayWifiIP[2], arrayWifiIP[3]);
      IPAddress manualGateway(arrayWifiGatewayIP[0], arrayWifiGatewayIP[1], arrayWifiGatewayIP[2], arrayWifiGatewayIP[3]);
      IPAddress manualSubnet(arrayWifiSubnetIP[0], arrayWifiSubnetIP[1], arrayWifiSubnetIP[2], arrayWifiSubnetIP[3]);

      IPAddress newDns(8, 8, 8, 8);

      WiFi.config(manualIP, newDns, manualGateway, manualSubnet);
    } else {
      Serial.println("Endereçamento Wifi não é Válido!");
    }
  } else {
    Serial.println("Endereçamento Wifi não Configurado!");
  }
*/


  WiFi.begin(ssidWifi.c_str(), passwordWifi.c_str());  //Conecta com seu roteador
  Serial.println("");

  //Espera por uma conexão
  while (count < 20) {
    if (count > 15) {
//tentar com ip automatico
#ifdef ESP32
      WiFi.disconnect(true);
#elif defined(ESP8266)
      WiFi.config(0U, 0U, 0U);
#else
#error "Placa não suportada. Escolha ESP32 ou ESP8266."
#endif
      WiFi.begin(ssidWifi.c_str(), passwordWifi.c_str());  //
      Serial.println("[DEBUG] [CLIENTE WIFI] [FALHA] [connectToWiFi] [TENTANDO POR IP AUTOMATICO]");
    }
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      salvarEeprom(ssidWifi, passwordWifi);
      Serial.println("");
      //Se a conexão ocorrer com sucesso, mostre o endereço IP no monitor serial
      Serial.println("Conectado ao WiFi");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  //Endereço IP do ESP8266

      conectado_wifi = true;
      contador_reconect_wifi = 0;
      return;
    } else if (WiFi.status() == WL_CONNECT_FAILED) {
    }
    count++;
  }

  Serial.println("[DEBUG] [CLIENTE WIFI] [FALHA] [connectToWiFi] [FALHA AO SE CONECTAR A REDE WIFI SALVA]");
  conectado_wifi = false;
  contador_reconect_wifi++;

  //delay(1000);
  //ESP.restart();
}



void connectEeprom(String index) {
  EEPROM.begin(200);  // Tamanho da FLASH reservado para EEPROM. Pode ser de 4 a 4096 bytes

  int ssidSize = (int)EEPROM.read(0);      // Tamanho do SSID armazenado (número de bytes)
  int passwordSize = (int)EEPROM.read(1);  // Tamanho do Password armazenado (número de bytes)
  String ssidWifi = "";
  String passwordWifi = "";

  //Serial.println("Lendo:");
  for (int i = 2; i < 2 + ssidSize; i++) {
    //Serial.print(char(EEPROM.read(i)));
    ssidWifi.concat(char(EEPROM.read(i)));
  }
  // Serial.println("");

  // Serial.println("Lendo:");
  for (int j = 2 + ssidSize; j < 2 + ssidSize + passwordSize; j++) {
    //Serial.print(char(EEPROM.read(j)));
    passwordWifi.concat(char(EEPROM.read(j)));
  }
  //  Serial.println("");

  EEPROM.end();  // Apaga a cópia da EEPROM salva na RAM

  Serial.println("Leu:");
  Serial.println(ssidWifi);
  Serial.println(passwordWifi);

  cliente_wifi_salvo = true;
  ssid_wifi_global = ssidWifi;
  passwd_wifi_global = passwordWifi;



  if (cliente_wifi_salvo) {

    bool prosseguir = true;

    if (ssidWifi.length() > 30 || passwordWifi.length() > 30) {
      prosseguir = false;
    } else {
      //Validar rede e senha salvos
      for (int i = 0; i < ssidWifi.length(); i++) {
        if (isPrintable(ssidWifi.charAt(i))) {
        } else {
          prosseguir = false;
          break;
        }
      }
    }


    if (prosseguir) {
      Serial.println("[DEBUG] [CLIENTE WIFI] [AVISO] [connectEeprom] [REDE LIDA NA EEPROM] []");
      connectToWiFi(ssidWifi, passwordWifi, index);

    } else {

      cliente_wifi_salvo = false;

      index.replace("<br><br>", "<p id='status'>Não Conectado!</p>");
      index.replace("<h3>rede_conectada</h3>", "Nenhum Cliente Wifi Conectado ");
      server.send(200, "text/html", index);
      Serial.println("[DEBUG] [CLIENTE WIFI] [FALHA] [connectEeprom] [REDE LIDA CORROMPIDA] []");
    }
  }
}




void iniciarClienteWifi() {
  connectEeprom();
}




bool validarEnderecamentEth() {

  for (int i = 0; i < 4; i++) {
    if (arrayEthIP[i] < 0 || arrayEthIP[i] >= 255) {
      return false;
    }
  }


  for (int i = 0; i < 4; i++) {
    if (arrayGatewayIP[i] < 0 || arrayGatewayIP[i] >= 255) {
      return false;
    }
  }



  for (int i = 0; i < 4; i++) {
    if (arraySubnetIP[i] < 0 || arraySubnetIP[i] > 255) {
      return false;
    }
  }


  return true;
}


void iniciarClienteEth() {

  contador_reconect_ethernet++;

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);


  Ethernet.init(ETH_PHY_CS);  // MKR ETH shield
  delay(500);

  IPAddress manualEthIP(arrayEthIP[0], arrayEthIP[1], arrayEthIP[2], arrayEthIP[3]);
  IPAddress gatewayEth(arrayGatewayIP[0], arrayGatewayIP[1], arrayGatewayIP[2], arrayGatewayIP[3]);
  IPAddress subnetEth(arraySubnetIP[0], arraySubnetIP[1], arraySubnetIP[2], arraySubnetIP[3]);

  IPAddress newDns(8, 8, 8, 8);



  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    conectado_ethernet = false;
  } else {
    Serial.println("[DEBUG] [CLIENTE ETH] [SUCESSO] [iniciarClienteEth] [MODULO ETHERNET OK] []");
  }


  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    conectado_ethernet = false;
    return;
  } else {
    Serial.println("[DEBUG] [CLIENTE ETH] [SUCESSO] [iniciarClienteEth] [REDE CABEADA CONECTADA] []");
  }


  if (configurado_eth_global == 1) {
    Serial.println("Endereçamento Ethernet Manual Configurado!");
    if (validarEnderecamentEth()) {
      Serial.println("Endereçamento Ethernet Manual é valido!");

      // Ethernet.begin(mac_ethernet, manualEthIP, gatewayEth);
      Ethernet.begin(mac_ethernet, manualEthIP, newDns, gatewayEth, subnetEth);

      Serial.println(Ethernet.localIP());
      //Ethernet.setDnsServerIP(newDns);
      contador_falhas_via_ethernet = 0;

      conectado_ethernet = true;

    } else {
      Serial.println("Endereçamento Ethernet Manual não é valido");
    }
  } else {
    Serial.println("Endereçamento Ethernet Manual não configurado!");

    if (!Ethernet.begin(mac_ethernet)) {
      Serial.println("Falha ao Configurar o Endereço Ethernet por DHCP");
      conectado_ethernet = false;
    } else {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
      conectado_ethernet = true;
      contador_falhas_via_ethernet = 0;
    }
  }





  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");

  ethServer.begin();
}

/******************** WIFI FUNÇOES ***********************************/
