# ESP32 Industrial Modbus TCP to Cloud Gateway

### Gateway IIoT Dual-Network (Ethernet W5500 + Wi-Fi) para Coleta Dinâmica de CLPs, Telemetria HTTPS, OTA e Gabinete 3D

![Hardware](https://img.shields.io/badge/Hardware-ESP32--S3%20%2B%20W5500-1F3A5F?style=flat-square)
![Modbus](https://img.shields.io/badge/Campo%20OT-Modbus%20TCP%20Client-2B4C7E?style=flat-square)
![Cloud](https://img.shields.io/badge/Cloud%20IT-HTTPS%20TLS%20%2B%20OTA-2B4C7E?style=flat-square)
![Storage](https://img.shields.io/badge/Storage-SPIFFS%20JSON%20%2B%20EEPROM-334E68?style=flat-square)
![Licença](https://img.shields.io/badge/Licen%C3%A7a-MIT-1B6B52?style=flat-square)

---

Gateway de borda **IIoT (Industrial Internet of Things)** baseado em **ESP32-S3** e controlador Ethernet **W5500**, projetado para atuar como ponte segura entre **CLPs industriais (Modbus TCP)** e plataformas em nuvem (**HTTPS/TLS** e gestão remota).

O projeto entrega uma solução completa de campo — combinando **hardware com alimentação segregada**, **coleta dinâmica de registradores Modbus via JSON no SPIFFS**, **sincronização temporal NTP**, **atualização remota de firmware OTA**, **portal web local autenticado com varredura Wi-Fi assíncrona em FreeRTOS** e **gabinete mecânico 3D** pronto para manufatura.

<p align="center">
  <img src="https://github.com/user-attachments/assets/f6e326da-1be3-48eb-aadd-c893f0e1057c" alt="Visão conceitual do Gateway Industrial ESP32 Modbus TCP" width="800">
  <br>
  <em>Arquitetura de borda integrando rede industrial Modbus TCP (OT) a serviços em nuvem HTTPS/REST (IT) com provisionamento web local.</em>
</p>

---

## Sumário

1. [Objetivo](#objetivo)
2. [Justificativa Técnica](#justificativa-técnica)
3. [Cenários de Uso](#cenários-de-uso)
4. [Arquitetura do Sistema](#arquitetura-do-sistema)
5. [Hardware e Esquema Elétrico](#hardware-e-esquema-elétrico)
6. [Coleta Dinâmica Modbus TCP e SPIFFS](#coleta-dinâmica-modbus-tcp-e-spiffs)
7. [Integração em Nuvem HTTPS e Gestão Remota](#integração-em-nuvem-https-e-gestão-remota)
8. [Portal Web Embarcado e Provisionamento](#portal-web-embarcado-e-provisionamento)
9. [Atualização OTA e Console de Diagnóstico](#atualização-ota-e-console-de-diagnóstico)
10. [Gabinete Mecânico 3D](#gabinete-mecânico-3d)
11. [Requisitos de Compilação e Estrutura do Repositório](#requisitos-de-compilação-e-estrutura-do-repositório)
12. [Licença e Contato](#licença-e-contato)

---

## Objetivo

O objetivo deste projeto é fornecer um **gateway de borda industrial autônomo, compacto e remotamente gerenciável**, capaz de coletar variáveis de processo diretamente de **Controladores Lógicos Programáveis (CLPs)** via **Modbus TCP** e transmiti-las com segurança para plataformas de telemetria e análise em nuvem.

Em termos práticos, a solução foi concebida para:

- **Integrar CLPs existentes à nuvem sem alterar a lógica de máquina:** realizar leitura cíclica e não invasiva de registradores digitais e analógicos (`COIL`, `DISCRETE`, `INPUT_REGISTER` e `HOLDING_REGISTER`) com timestamp padronizado em ISO-8601 via NTP.
- **Eliminar manutenções presenciais recorrentes:** permitir que a lista de registradores monitorados, o endereçamento IP das redes OT/IT e até o próprio firmware do gateway sejam atualizados remotamente sem retirar o equipamento do painel elétrico.
- **Simplificar o comissionamento em campo:** oferecer um portal web embarcado protegido por senha para técnicos configurarem credenciais Wi-Fi, IP estático Ethernet e parâmetros do CLP diretamente pelo navegador, sem depender de cabo serial ou IDE.

---

## Justificativa Técnica

Grande parte dos parques industriais opera com máquinas e CLPs confiáveis que, embora suportem **Modbus TCP**, permanecem isolados em redes locais de chão de fábrica (OT). Quando surge a demanda por visibilidade em tempo real, cálculo de OEE ou manutenção preditiva, equipes de engenharia costumam enfrentar três obstáculos recorrentes: **custo elevado** de gateways industriais proprietários, **exposição indevida de segurança** ao conectar CLPs diretamente à rede corporativa e **engessamento operacional** quando cada nova variável de máquina exige recompilar firmware.

As decisões arquiteturais deste projeto atacam diretamente cada um desses gargalos:

- **Segregação Física entre Rede Industrial (OT) e Nuvem (IT):** o barramento Ethernet cabeado (**W5500 via SPI**) comunica-se exclusivamente com a sub-rede do CLP, enquanto a interface **Wi-Fi** do `ESP32-S3` estabelece conexões de saída **HTTPS/TLS** com validação de certificado raiz (`root_ca`). O CLP permanece isolado da internet.
- **Desacoplamento entre Firmware e Mapa de Registradores (SPIFFS):** em soluções embarcadas convencionais, os endereços Modbus ficam fixos no código. Aqui, cada registrador é descrito em JSON no diretório `/registradores/` e sincronizado dinamicamente com a nuvem — permitindo escalar ou ajustar medições em produção sem recompilação.
- **Robustez Elétrica e Operacional em Campo:** o módulo Ethernet `W5500` recebe alimentação dedicada de **`3,3 V` via conversor buck `LM2596`** (preservando o regulador onboard do ESP32 sob picos de tráfego TCP), o firmware implementa autorecuperação por contadores de falha de conexão, a varredura Wi-Fi roda de forma assíncrona em **FreeRTOS** (sem travar o ciclo principal) e as atualizações **OTA** gerenciam explicitamente o Watchdog para evitar corrupção na gravação da flash.
- **Entrega Orientada a Produto:** além do código modular, o repositório inclui o esquema elétrico de montagem e o gabinete mecânico em `.stl`/`.blend`, cobrindo desde a eletrônica até o acondicionamento físico em painel.

---

## Cenários de Uso

A arquitetura flexível de mapa dinâmico e dupla interface de rede permite aplicar o gateway em diferentes contextos industriais e comerciais:

| Cenário de Aplicação | Como o Gateway Atua | Benefício Direto |
| :--- | :--- | :--- |
| **Monitoramento de OEE e Chão de Fábrica** | Lê periodicamente contadores de produção, estados de operação/parada (`COIL`/`DISCRETE`) e tempos de ciclo diretamente do CLP, enviando payloads com timestamp NTP para dashboards em nuvem. | Visibilidade em tempo real de disponibilidade, performance e gargalos de linha sem substituir CLPs existentes. |
| **Retrofit IIoT de Máquinas Legadas** | Acopla-se via cabo Ethernet à porta Modbus TCP do CLP sem alterar o programa ladder nem exigir licenças SCADA locais ou computadores industriais (IPCs) por máquina. | Digitalização rápida e de baixo custo em plantas com parque heterogêneo de equipamentos. |
| **Supervisão de Utilidades e Processos Contínuos** | Monitora registradores analógicos (`INPUT_REGISTER` e `HOLDING_REGISTER` em `INTEGER` e `REAL`), como temperaturas, pressões, vazões, níveis e grandezas elétricas em chillers, compressores, bombas e caldeiras. | Detecção antecipada de desvios operacionais e histórico estruturado para manutenção preditiva. |
| **Gestão Remota de Frotas por OEMs e Integradores** | Equipamentos entregues a clientes remotos mantêm heartbeat contínuo, aceitam alteração remota de tags/IPs e recebem melhorias de firmware via OTA (`tipo 70`). | Redução drástica de chamados presenciais e suporte proativo a ativos distribuídos geograficamente. |

---

## Arquitetura do Sistema

O diagrama abaixo resume o fluxo completo entre o CLP no lado industrial (OT), os serviços internos executados no `ESP32-S3` e os endpoints em nuvem no lado corporativo (IT):

```mermaid
flowchart TB
    subgraph OT["Rede Industrial (OT)"]
        PLC["CLP / Servidor Modbus TCP (Porta 502)"]
        W5500["Interface Ethernet W5500 (SPI — IP Estático)"]
        PLC <-->|Modbus TCP| W5500
    end

    subgraph EDGE["Gateway de Borda — ESP32-S3"]
        CORE["Orquestrador tarefaGlobal() + ModbusEthernet Client"]
        FS["SPIFFS (/registradores/*.json + configs) & EEPROM"]
        NTP["Sincronização NTP (ISO-8601)"]
        WEB["Portal Web Local Autenticado (SoftAP :80 + FreeRTOS Scan)"]
        OTA["Motor OTA (Update U_FLASH — chunks 512 B)"]
        CORE <--> FS
        NTP --> CORE
        WEB <--> FS
        CORE --> OTA
    end

    subgraph IT["Nuvem e Gestão Remota (IT — Wi-Fi)"]
        TLS["API Cloud HTTPS/TLS (root_ca): Mapa de Tags + Ingestão JSON"]
        MGMT["Servidor de Gestão: Comandos Remotos, Heartbeat e Binário OTA"]
    end

    W5500 <--> CORE
    CORE <-->|WiFiClientSecure HTTPS| TLS
    CORE <-->|HTTP Gestão / OTA| MGMT
```

### Cadência Operacional (`tarefaGlobal()`)

O loop principal executa um agendador não-bloqueante baseado em `millis()`, coordenando cinco rotinas independentes sem interromper o atendimento ao portal web local:

| Rotina | Intervalo | Função |
| :--- | :--- | :--- |
| **Leitura Modbus + Upload Cloud** | Configurável (`/path_plc_time.txt`, padrão `60 s`) | Lê todos os registradores em `/registradores/*.json`, anexa `dt_time` (ISO-8601) e envia via HTTPS POST |
| **Sincronização de Mapa de Tags** | `10 s` | Consulta `getNeedUpdate()` e, se `needUpdate == "true"`, recarrega a tabela de registradores no SPIFFS |
| **Polling de Comandos Remotos** | `1,5 s` | Busca tarefas pendentes (`buscarAcoesWifi()`), executa a ação correspondente e confirma execução |
| **Heartbeat de Disponibilidade** | `20 s` | Reporta versão de firmware, IPs Wi-Fi/Ethernet, prioridade ativa e estado de I/O (`updateStatusWifi()`) |
| **Autorecuperação de Rede** | Contínuo | Monitora `contador_falhas_via_wifi` (`> 10` tenta reconexão) e `contador_reconect_wifi` (`> 3` executa `ESP.restart()`) |

---

## Hardware e Esquema Elétrico

O projeto elétrico organiza o dispositivo em três blocos isolados — **Energia**, **Ethernet** e **Controle** — priorizando estabilidade de alimentação sob carga contínua de rede e facilidade de montagem em campo:

<p align="center">
  <img src="./Esquema_Eletrico.png" alt="Esquema elétrico do Gateway ESP32-S3 com módulo W5500 e buck LM2596" width="820">
  <br>
  <em>Esquema elétrico de interligação: entrada 5 VDC com chave SPST, buck dedicado LM2596 (3,3 V para o W5500), interface Ethernet SPI W5500 e núcleo ESP32-S3.</em>
</p>

| Bloco | Componentes | Especificação e Decisão de Engenharia |
| :--- | :--- | :--- |
| **Energia** | Power Jack `+5 VDC` + Chave `SPST` + Conversor Buck `LM2596` (`5 V → 3,3 V`) | O rail `5 V` alimenta o `ESP32-S3`, enquanto o módulo Ethernet `W5500` recebe alimentação **`3,3 V` dedicada** via `LM2596`, evitando sobrecarga no LDO interno da placa controladora durante tráfego TCP contínuo. |
| **Ethernet OT** | Módulo `ADIY W5500 HAT` (Controlador `W5500`) | Interface **SPI** (`CS = GPIO10`, `SCK`, `MISO`, `MOSI`; `IRQ = GPIO9` e `RST = GPIO3` mapeados em `MakeParamsLocalSetup.h`) com referência `GND` comum. |
| **Controle** | `ESP32-S3-WROOM-1` (compatível com ESP32 / ESP8266 via `#ifdef`) | Processamento dual-core FreeRTOS, conectividade Wi-Fi integrada, suporte TLS (`WiFiClientSecure`) e partições SPIFFS/OTA. |

---

## Coleta Dinâmica Modbus TCP e SPIFFS

A coleta de dados no CLP é orientada por metadados persistidos no **SPIFFS**. Cada variável monitorada corresponde a um arquivo JSON em `/registradores/<plcPath>.json`. A cada ciclo, o firmware estabelece ou reutiliza a sessão TCP com `plcServer` (`plcPort`), percorre o diretório, realiza a leitura adequada ao tipo do registrador e consolida todas as medições em um único payload JSON acompanhado do carimbo temporal `dt_time` (sincronizado via `pool.ntp.org` / `time.nist.gov`).

### Tipos Modbus e Conversões Suportadas (`processarUploadRegistradores()`)

| `dataType` | Função `ModbusEthernet` | `valueType` Suportado | Tratamento de Leitura |
| :--- | :--- | :--- | :--- |
| **`COIL`** | `readCoil()` | Booleano | Leitura 1-bit (`true` / `false`), offset 0-based (`plcPath % 100 - 1`) |
| **`DISCRETE`** | `readIsts()` | Booleano | Leitura de entrada discreta (1-bit), offset 0-based |
| **`INPUT_REGISTER`** | `readIreg()` | `INTEGER` / `REAL` | `INTEGER`: 1 word (`uint16_t`); `REAL`: 2 words combinadas em ponto flutuante |
| **`HOLDING_REGISTER`** | `readHreg()` | `INTEGER` / `REAL` | `INTEGER`: 1 word (`uint16_t`); `REAL`: 2 words combinadas em ponto flutuante |

### Exemplo de Descritor no SPIFFS (`/registradores/40001.json`)

```json
{
  "name": "tempSensor",
  "plcPath": "40001",
  "dataType": "HOLDING_REGISTER",
  "valueType": "INTEGER"
}
```

### Mapa de Arquivos Persistidos no SPIFFS

| Caminho SPIFFS | Finalidade |
| :--- | :--- |
| `/path_plc_server.txt` / `/path_plc_port.txt` / `/path_plc_time.txt` | IP do CLP Modbus (padrão `192.168.10.11`), porta (`502`) e cadência de leitura em segundos (`60`) |
| `/path_ip_eth.txt` / `/path_gat_eth.txt` / `/path_sub_eth.txt` / `/path_config_eth.txt` | Endereçamento estático Ethernet (`W5500`) e flag de ativação |
| `/path_ip_wifi.txt` / `/path_gat_wifi.txt` / `/path_sub_wifi.txt` / `/path_config_wifi.txt` | Endereçamento estático Wi-Fi e flag de ativação |
| `/path_guid.txt.txt` | Número de série / identificador único do gateway (`SERIAL_NUMBER`) |
| `/registradores/` | Diretório dinâmico contendo os descritores JSON de cada registrador monitorado |

---

## Integração em Nuvem HTTPS e Gestão Remota

A comunicação externa é dividida em dois fluxos complementares — um canal criptografado dedicado aos dados de processo e um canal operacional para comandos de administração, disponibilidade e manutenção remota:

1. **Canal Seguro HTTPS/TLS (`WiFiClientSecure` + `root_ca` em `Certificados.h`):**
   - Download da tabela de registradores (`getRegistradores()`) autenticado via headers `x-api-key` e `x-security-id`.
   - Ingestão de telemetria Modbus serializada com timestamp NTP ISO-8601 (`setUploadRegistradores()`) via `HTTPS POST` autenticado por `x-api-key`, `x-usual-id` e `x-tenet-id`.
2. **Canal de Gestão, Disponibilidade e Tarefas Remotas:**
   - Heartbeat periódico (`/v1/protected/modulopgm/informarOnlinePost/`), consulta de tarefas (`/v1/protected/modulopgm/tarefas/listar/`), confirmação de execução (`/v1/protected/modulopgm/tarefas/responder/`) e notificação de eventos/boot (`/v1/protected/modulopgm/tarefas/informar/`).

### Tabela de Comandos Remotos (`processarRequisicao()`)

| Gatilho / `tipo_requisicao` | Ação Executada no Gateway |
| :--- | :--- |
| `needUpdate == "true"` | Sincroniza novo mapa de registradores via HTTPS e dispara leitura/upload imediato |
| **`60`** | Confirma a tarefa e executa reinicialização remota (`ESP.restart()`) |
| **`70`** | Confirma a tarefa e inicia atualização de firmware **OTA** via Wi-Fi (`buscarAtt()`) |
| **`90`** | Cria ou sobrescreve arquivo arbitrário no SPIFFS (`writeFile(valorString1, valorString2)`) |
| **`115`** | Atualiza credenciais Wi-Fi (SSID/senha) na EEPROM e reinicia |
| **`116`** | Reconfigura IP estático, gateway e sub-rede **Ethernet** no SPIFFS e reinicia |
| **`118`** | Reconfigura IP estático, gateway e sub-rede **Wi-Fi** no SPIFFS e reinicia |
| **`119`** | Atualiza prioridade de interface de rede na EEPROM e reinicia |

---

## Portal Web Embarcado e Provisionamento

Para eliminar dependência de programação manual na instalação em campo, o gateway inicializa automaticamente o ponto de acesso **`MODBUS RECEIVER <SERIAL_NUMBER>`** e expõe uma interface web administrativa na porta **`80`** (`WebServerLocalSetup.h` e `WEBPAGES.h`).

- **Fluxo de Primeiro Uso e Autenticação:** quando `primeiro_uso_global == 0`, o portal exige o cadastro da senha administrativa (armazenada em `EEPROM`). Nos acessos subsequentes, todas as telas de configuração são protegidas pela rota de login.
- **Varredura Wi-Fi Assíncrona (FreeRTOS):** no ESP32, a rota `/update_wifi_list` executa `WiFi.scanNetworks()` em uma task dedicada (`xTaskCreatePinnedToCore` no Core 1), retornando os SSIDs via JSON em `/get_wifi_list` sem bloquear o loop de automação.
- **Conversor CIDR Integrado:** o formulário Wi-Fi aceita máscara em notação CIDR (ex.: `/24`) e realiza a conversão bit-a-bit via `mascaraParaSubmascara()` antes da gravação no SPIFFS.

| Rota HTTP | Função |
| :--- | :--- |
| `/` e `/action_define_password` | Definição inicial da senha administrativa (primeiro uso) ou tela de login |
| `/action_login` e `/main` | Autenticação de sessão e painel principal com status Wi-Fi/Ethernet, MACs e S/N |
| `/update_wifi_list` e `/get_wifi_list` | Disparo assíncrono (FreeRTOS) e consulta JSON da lista de redes Wi-Fi |
| `/action_new_connection` | Gravação de SSID/senha em EEPROM e IP estático Wi-Fi no SPIFFS |
| `/action_new_connection_eth` | Configuração de IP estático, gateway e máscara da interface Ethernet `W5500` |
| `/configs` e `/action_configs` | Configuração do endereço IP do servidor Modbus (CLP) e intervalo de coleta |
| `/opcoes` e `/action_opcoes` | Seleção de prioridade entre interfaces de rede |
| `/action_reset` | Reinicialização controlada com redirecionamento automático |

---

## Atualização OTA e Console de Diagnóstico

### Motor de Atualização OTA (`buscarAtt()`)

Ao receber o comando remoto **`70`**, o gateway executa uma atualização segura em flash sem acesso físico à porta USB:

1. Confirma o recebimento da tarefa no servidor de gestão.
2. Desabilita temporariamente o Task Watchdog (`esp_task_wdt_delete(NULL)` no ESP32) para evitar resets durante o download da imagem.
3. Baixa o binário via HTTP validando `Content-Length`, inicializa `Update.begin(length, U_FLASH)` e grava em streaming por blocos de **`512 bytes`** (`BUFFER_SIZE`).
4. Finaliza com `Update.end()` e reinicia automaticamente executando o novo firmware.

### Console de Diagnóstico Serial (`115200 bps`)

Durante bancada e comissionamento, o Monitor Serial aceita comandos rápidos de inspeção e teste manual:

| Tecla | Ação Executada |
| :---: | :--- |
| `a` | Reinicia o microcontrolador (`ESP.restart()`) |
| `l` | Lista no console todos os registradores JSON salvos em `/registradores/` |
| `g` | Força consulta imediata de atualização (`getNeedUpdate()`) |
| `h` | Recarrega remotamente a tabela de registradores (`processarNovosRegistradores()`) |
| `u` | Executa ciclo manual de leitura Modbus TCP e upload HTTPS |
| `d` | Limpa todos os arquivos de `/registradores/` |

---

## Gabinete Mecânico 3D

O repositório inclui o projeto mecânico completo do invólucro protetivo do gateway para impressão 3D e adaptação a painéis elétricos industriais:

| Arquivo | Formato | Descrição |
| :--- | :---: | :--- |
| [`case_esp32.stl`](./case_esp32.stl) | `.stl` | Malha pronta para fatiamento e impressão 3D do gabinete do gateway |
| [`case.blend`](./case.blend) | `.blend` | Projeto-fonte 3D editável no Blender para adaptação de recortes, fixação DIN ou dimensões |

---

## Requisitos de Compilação e Estrutura do Repositório

### Ambiente e Dependências

- **Placa alvo:** `ESP32-S3 Dev Module` / `ESP32 Dev Module` (Flash ≥ 4 MB, esquema de partição com suporte a **OTA** + **SPIFFS ≥ 1 MB**, `115200 baud`).
- **Bibliotecas:** `WiFi.h`, `WiFiClientSecure.h`, `WebServer.h`, `HTTPClient.h`, `Ethernet.h`, `SPI.h`, `ModbusEthernet`, `ArduinoJson` (v6+), `ArduinoHttpClient`, `SPIFFS.h`, `EEPROM.h`, `Update.h`.

> [!IMPORTANT]
> **Segurança em Produção:** antes de implantar o firmware em ambiente produtivo, substitua as URLs de API, chaves de autenticação (`x-api-key`, `x-security-id`, `x-usual-id`, `x-tenet-id`), senha administrativa padrão (`padroesFabrica()`) e credenciais do SoftAP em `ModbusTCPHTTPRequests.ino`, `MakeParamsLocalSetup.h` e `WebServerLocalSetup.h`, além de validar os certificados raiz em `Certificados.h`.

### Arquivos do Repositório

| Arquivo | Descrição |
| :--- | :--- |
| [`ModbusTCPHTTPRequests.ino`](./ModbusTCPHTTPRequests.ino) | Firmware principal: inicialização SPIFFS/NTP/Modbus, `tarefaGlobal()`, processamento de tarefas e leitor Modbus TCP |
| [`WebServerLocalSetup.h`](./WebServerLocalSetup.h) | Rotas HTTP do portal web, autenticação administrativa, clientes HTTPS/HTTP e motor OTA (`buscarAtt()`) |
| [`WifiLocalSetup.h`](./WifiLocalSetup.h) | Gestão Wi-Fi STA/EEPROM, task FreeRTOS de scan de redes, validação de IPs e inicialização Ethernet `W5500` |
| [`WEBPAGES.h`](./WEBPAGES.h) | Páginas HTML/CSS/JS embarcadas em memória Flash (`PROGMEM`) |
| [`FilesWriter.h`](./FilesWriter.h) | Camada de leitura, gravação, listagem e limpeza de arquivos e registradores JSON no SPIFFS |
| [`MakeParamsLocalSetup.h`](./MakeParamsLocalSetup.h) | Definições de pinagem `W5500` SPI, versão de firmware, MAC Ethernet e parâmetros de integração |
| [`AllMetods.h`](./AllMetods.h) | Protótipos centralizados das funções do projeto |
| [`Certificados.h`](./Certificados.h) / [`certs.h`](./certs.h) / [`certUpdate`](./certUpdate) | Certificados raiz TLS (`root_ca`) e utilitário auxiliar de geração de certificados |
| [`EthernetLocalSetup.h`](./EthernetLocalSetup.h) | Header reservado para extensões específicas de Ethernet |
| [`Esquema_Eletrico.png`](./Esquema_Eletrico.png) | Esquema elétrico completo (`ESP32-S3` + `W5500` + `LM2596`) |
| [`case_esp32.stl`](./case_esp32.stl) / [`case.blend`](./case.blend) | Gabinete mecânico 3D pronto para impressão e fonte Blender |
| [`README.md`](./README.md) | Documentação técnica do projeto |

---

## Licença e Contato

Distribuído sob a **MIT License** — livre para uso, modificação e distribuição mantendo os créditos originais.

- **Autor:** Aislan Silva Costa — Engenharia de Hardware, Sistemas Embarcados e IIoT
- **E-mail:** [comprasaislan@gmail.com](mailto:comprasaislan@gmail.com)
- **LinkedIn:** [linkedin.com/in/aislan-silva-costa-660814341](https://www.linkedin.com/in/aislan-silva-costa-660814341/)
