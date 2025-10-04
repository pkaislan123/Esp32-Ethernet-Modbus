void writeFile(String valor, String path) {
  Serial.print("Valor: ");
  Serial.println(valor);
  Serial.print("Path: ");
  Serial.println(path);

  File rFile = SPIFFS.open(path, FILE_WRITE);  // FILE_WRITE abre para escrita (cria se não existir)
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  } else {
    rFile.println(valor);
    Serial.print("gravou estado: ");
    Serial.println(valor);
  }
  rFile.close();
}

void salvarRegistrador(String nome, String plcPath, String dataType, String valueType) {
  String path = "/registradores/" + plcPath + ".json";

  File file = SPIFFS.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para escrita.");
    return;
  }

  StaticJsonDocument<128> doc;
  doc["name"] = nome;
  doc["plcPath"] = plcPath;
  doc["dataType"] = dataType;
  doc["valueType"] = valueType;

  serializeJson(doc, file);
  file.close();

  Serial.println("Registrador salvo: " + nome);
}

void listarRegistradores() {
  File root = SPIFFS.open("/registradores");
  if (!root) {
    Serial.println("Falha ao abrir o diretório");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("/registradores não é um diretório");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    String path = file.name();
    Serial.print("Arquivo: ");
    Serial.println(path);

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
    }

    file = root.openNextFile();  // Próximo arquivo
  }

  Serial.println("Listagem concluída.");
}

void limparRegistradores() {
  File root = SPIFFS.open("/registradores");
  if (!root) {
    Serial.println("Falha ao abrir o diretório");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("/registradores não é um diretório");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    String path = file.name();
    Serial.print("Removendo: ");
    Serial.println(path);

    if (SPIFFS.remove(path)) {
      Serial.println("Removido com sucesso.");
    } else {
      Serial.println("Erro ao remover.");
    }

    file = root.openNextFile();
  }

  Serial.println("Limpeza concluída.");
}