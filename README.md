<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/2392631d-987b-4a94-9b60-01c87eecf6ba" /># 🌐 ESP32 Modbus PLC Gateway

Este projeto implementa um **gateway inteligente entre um PLC (via Modbus TCP)** e um **servidor remoto HTTP/HTTPS**, utilizando um **ESP32** com suporte a **Ethernet e Wi-Fi**.  
O dispositivo também atua como **servidor web local (Hotspot)** para configuração e monitoramento, e é capaz de atualizar seu firmware e registradores remotamente.

<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/f6e326da-1be3-48eb-aadd-c893f0e1057c" />

---

## ⚙️ Funcionalidades principais

- 📡 **Conexão Wi-Fi e Ethernet** com IP manual configurável via interface web ou comandos remotos.  
- 🧭 **Servidor Web Local (Hotspot)** para configuração de rede, servidor e registradores.  
- 🔁 **Comunicação Modbus TCP** com leitura e escrita automática dos registradores configurados.  
- 🧠 **Armazenamento em SPIFFS e EEPROM**, preservando configurações entre reinicializações.  
- 🔒 **Sincronização NTP** automática para data/hora exatas.  
- 🔧 **Atualização remota de firmware** via endpoint configurável.  
- 📤 **Envio periódico de dados (registradores) ao servidor HTTP/HTTPS remoto.**  
- 🧩 **Estrutura modular** (dividida em múltiplos headers: `AllMetods.h`, `FilesWriter.h`, `WifiLocalSetup.h`, etc.).  
- ⚡ **Failover entre Wi-Fi e Ethernet** conforme disponibilidade.  
- 💾 **Criação e leitura dinâmica de registradores Modbus** via arquivos JSON no SPIFFS.

---

## 🧩 Estrutura do projeto

/src
├── main.cpp # Código principal (este arquivo)
├── AllMetods.h # Funções auxiliares e lógicas gerais
├── Certificados.h # Certificados HTTPS
├── EthernetLocalSetup.h # Inicialização e configuração do Ethernet
├── FilesWriter.h # Escrita e leitura de arquivos SPIFFS
├── MakeParamsLocalSetup.h # Definições de parâmetros e constantes
├── WEBPAGES.h # Páginas e endpoints do servidor web
├── WebServerLocalSetup.h # Inicialização do servidor web
├── WifiLocalSetup.h # Conexão e gerenciamento Wi-Fi



---

## 🧱 Estrutura de arquivos SPIFFS

| Caminho SPIFFS | Descrição |
|----------------|-----------|
| `/path_ip_wifi.txt` | IP manual da rede Wi-Fi |
| `/path_gat_wifi.txt` | Gateway Wi-Fi |
| `/path_sub_wifi.txt` | Máscara de sub-rede Wi-Fi |
| `/path_ip_eth.txt` | IP manual Ethernet |
| `/path_gat_eth.txt` | Gateway Ethernet |
| `/path_sub_eth.txt` | Máscara Ethernet |
| `/path_config_wifi.txt` | Status de configuração Wi-Fi |
| `/path_config_eth.txt` | Status de configuração Ethernet |
| `/path_plc_server.txt` | Endereço IP do PLC (Modbus Server) |
| `/path_plc_port.txt` | Porta TCP Modbus |
| `/path_plc_time.txt` | Intervalo de leitura Modbus (segundos) |
| `/path_guid.txt` | Identificador único do módulo |
| `/registradores/` | Diretório com arquivos JSON representando registradores Modbus |

---

## 📦 Dependências

Certifique-se de ter instaladas as seguintes bibliotecas na IDE Arduino:

- **WiFi.h** e **WiFiClientSecure.h**  
- **Ethernet.h**  
- **ModbusEthernet** (biblioteca Modbus TCP compatível com ESP32)  
- **ArduinoJson** (>= 6.x)  
- **ArduinoHttpClient**  
- **HTTPClient.h**  
- **WebServer.h**  
- **SPIFFS.h / FS.h**  
- **EEPROM.h**  
- **time.h**

---

## ⚡ Compilação e Upload

1. **Placa:**  
   - `ESP32 Dev Module` (ou compatível)

2. **Configurações recomendadas:**  
   - Flash Size: 4MB  
   - Partition Scheme: “Default with OTA” ou personalizada com SPIFFS ≥ 1MB  
   - CPU Frequency: 240MHz  
   - Upload Speed: 115200 baud  

3. **Upload:**  
   - Compile e envie o código via porta serial.  
   - Faça upload do sistema de arquivos SPIFFS contendo as pastas e arquivos de configuração.  

---

## 🌍 Comunicação com Servidor HTTP/HTTPS

Endpoints definidos:

| Função | Caminho |
|--------|----------|
| Informar módulo online | `/v1/protected/modulopgm/informarOnlinePost/` |
| Buscar ações | `/v1/protected/modulopgm/tarefas/listar/` |
| Responder ação | `/v1/protected/modulopgm/tarefas/responder/` |
| Informar ação | `/v1/protected/modulopgm/tarefas/informar/` |
| Upload de imagem / logs | `/v1/protected/modulopgm/registrodisponibilidade/` |

O servidor remoto padrão é:

IP: 162.240.123.456
Porta: 80


> ⚠️ É possível alterar esses parâmetros via interface web, arquivos SPIFFS ou comandos remotos.

---

## 🔧 Comandos via Serial

Durante o desenvolvimento, você pode enviar comandos pelo **Monitor Serial**:

| Comando | Ação |
|----------|------|
| `a` | Reinicia o ESP (`ESP.restart()`) |
| `l` | Lista os registradores armazenados |
| `g` | Busca ações pendentes do servidor |
| `h` | Recarrega registradores (requisição remota) |
| `u` | Executa upload de registradores Modbus |
| `d` | Limpa os registradores armazenados |

---

## 🧠 Lógica geral de funcionamento

1. **Inicialização:**  
   - Monta SPIFFS e EEPROM.  
   - Lê configurações salvas (IP, servidor, Wi-Fi, PLC).  
   - Inicia Hotspot e servidor web.  
   - Tenta conectar ao Wi-Fi e sincroniza NTP.  
   - Inicializa cliente Modbus TCP.  

2. **Loop principal (`tarefaGlobal()`):**  
   - Faz leitura periódica dos registradores do PLC.  
   - Envia dados ao servidor remoto via HTTP.  
   - Busca novas ações e processa requisições do servidor.  
   - Atualiza status de conexão a cada 20 segundos.  
   - Garante reconexão automática em caso de falhas.  

3. **Falhas e Reconexões:**  
   - Contadores de falhas (`contador_falhas_via_wifi`, `contador_falhas_via_ethernet`).  
   - Reinicialização automática se reconexões falharem.  

---

## 🔄 Atualização OTA

Quando o servidor envia uma requisição de tipo `70` (atualização), o ESP32 realiza:
1. Resposta de confirmação da ação.  
2. Chamada da função `buscarAtt()` para iniciar atualização OTA via Wi-Fi.  

---

## 🧮 Modbus TCP

O módulo atua como **cliente Modbus TCP**, comunicando-se com um PLC remoto:

- **Tipos de registradores suportados:**
  - `COIL`
  - `DISCRETE`
  - `INPUT_REGISTER`
  - `HOLDING_REGISTER`

- **Suporte a valores:**
  - `INTEGER`
  - `REAL` (float em duas words)

A cada intervalo configurado (`/path_plc_time.txt`), o ESP32:
1. Conecta-se ao PLC (`plcServer` / `plcPort`)  
2. Lê os registradores listados em `/registradores/*.json`  
3. Monta um JSON e envia ao servidor remoto.

---

## 💾 Exemplo de registrador (`/registradores/tempSensor.json`)

json
{
  "name": "tempSensor",
  "plcPath": "40001",
  "dataType": "HOLDING_REGISTER",
  "valueType": "INTEGER"
}


🛠️ Reset para padrão de fábrica

A função padroesFabrica() é responsável por:

Limpar EEPROM e SPIFFS.

Recriar os arquivos padrão.

Definir IPs e servidor Modbus padrão.

Salvar senha de administrador.

🔐 Segurança

As credenciais padrão do hotspot são:

SSID: MODBUS RECEIVER
Senha: Mbreceiver7898


Altere a senha padrão em produção.

Recomenda-se ativar HTTPS e restringir endpoints sensíveis no servidor remoto.

🧑‍💻 Autor

Aislan Silva Costa

🏗️ Licença

Este projeto está sob a licença MIT — consulte o arquivo LICENSE
para mais detalhes.

🧾 Notas finais

💡 Dica:
Para depuração, ative o monitor serial (115200 baud) e observe as mensagens de log.
O sistema exibe status detalhado de conexão, leitura Modbus e sincronização de hora.

