const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>MODBUS RECEIVER WIFI/ETHERNET</title>
  <style>
    body {color: #434343; font-family: "Helvetica Neue",Helvetica,Arial,sans-serif; font-size: 14px; background-color: #eeeeee; margin-top: 100px;}
    .container {margin: 0 auto; max-width: 400px; padding: 30px; box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23); background-color: #ffffff; border-radius: 10px;}
    h2 {text-align: center; margin-bottom: 20px; margin-top: 0px; color: #0ee6b1; font-size: 35px;}
    #titleGreen {color: #00E1AA;}
    #titleBlack {color: #000000;}
    h3 {text-align: center; margin-bottom: 40px; margin-top: 0px; color: #336859; font-size: 35px;}
    form .field-group {box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0; width: 100%;}
    .text-field {font-size: 15px; margin-bottom: 4%; -webkit-appearance: none; display: block; background: #fafafa; color: #636363; width: 100%; padding: 15px 0px 15px 0px; text-indent: 10px; border-radius: 5px; border: 1px solid #e6e6e6; background-color: transparent;}
    .text-field:focus {border-color: #00bcd4; outline: 0;}
    .button-container {box-sizing: border-box; clear: both; margin: 1px 0 0; padding: 4px 0; position: relative; width: 100%;}
    .button {background: #00E1AA; border: none; border-radius: 5px; color: #ffffff; cursor: pointer; display: block; font-weight: bold; font-size: 16px; padding: 15px 0; text-align: center; text-transform: uppercase; width: 100%; -webkit-transition: background 250ms ease; -moz-transition: background 250ms ease; -o-transition: background 250ms ease; transition: background 250ms ease;}
    p {text-align: center; text-decoration: none; color: #87c1d3; font-size: 18px;}
    a {text-decoration: none; color: #ffffff; margin-top: 0%;}
    #status {text-align: center; text-decoration: none; color: #336859; font-size: 14px;}
    .side-panel {background-color: #add8e6; float: left; width: 200px; padding: 15px; height: 100%;}

    .tabs {
      text-align: center;
      margin-bottom: 20px;
    }

    .tablink {
      background-color: #00E1AA;
      color: #ffffff;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-weight: bold;
      font-size: 16px;
      padding: 15px 30px;
      text-align: center;
      text-transform: uppercase;
      margin-right: 10px;
      -webkit-transition: background 250ms ease;
      -moz-transition: background 250ms ease;
      -o-transition: background 250ms ease;
      transition: background 250ms ease;
    }

    .tablink:last-child {
      margin-right: 0;
    }

    .tabcontent {
      display: none;
    }

       .modal {
  display: none;
  position: fixed;
  z-index: 1;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0,0,0,0.7);
}

.modal-content {
  position: absolute;
  background-color: #fefefe;
  text-align: center;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  padding: 20px;
  border-radius: 10px;
}

#modalMessage {
  font-size: 18px;
}

#countdown {
  font-size: 24px;
}

  </style>
  <script>
   
    function validateFormEth() {
     
      var ip = document.forms["myFormEth"]["ethIp"].value;
      var gateway = document.forms["myFormEth"]["EthGateway"].value;
      var mascara = document.forms["myFormEth"]["EthSubnet"].value;


      
      var status = document.getElementById("statusDivEth");
     
        {
        status.innerHTML = "<p id='statusEth'>Conectando...</p>";

           // Exibir a janela modal
    var modal = document.getElementById("myModal");
    modal.style.display = "block";

    // Iniciar o contador de 20 a 0
    var countdown = 20;
    var countdownInterval = setInterval(function() {
      document.getElementById("countdown").innerText = countdown;
      countdown--;

      if (countdown < 0) {
        clearInterval(countdownInterval);
        
        // Após o contador atingir 0, atualize a página
        window.location.reload();
      }
    }, 1000);


        return true;
      }
    }


let tentativa = 0;
const maxTentativas = 5;

function checkWifiScanComplete() {
  if (tentativa >= maxTentativas) {
    console.error('Limite de tentativas atingido');
    return;
  }

  fetch('/get_wifi_list')
    .then(response => response.json())
    .then(data => {
      if (data.status !== "Pendente") {
        // Varredura concluída, atualiza a lista de redes Wi-Fi
        atualizarListaRedesWifi(data);
         // Reabilita o botão e altera o texto
        const botao = document.getElementById('listarRedesBtn');
        botao.disabled = false;
        botao.value = 'Listar Redes Wifi';

      } else {
        tentativa++;
        // Se ainda estiver pendente, verifica novamente em 1 segundo
        setTimeout(checkWifiScanComplete, 5000);
      }
    })
    .catch(error => console.error('Erro:', error));
}


  function listarRedesWifi() {
     // Desabilita o botão e altera o texto
  const botao = document.getElementById('listarRedesBtn');
  botao.disabled = true;
  botao.value = 'Buscando redes...';

    // Enviar a solicitação para iniciar a varredura
    fetch('/update_wifi_list')
      .then(response => response.json())
      .then(data => {
        console.log(data.status);
        // Começar a checar periodicamente se a varredura foi concluída
        checkWifiScanComplete();
      })
      .catch(error => console.error('Erro:', error));
  }

 
  function atualizarListaRedesWifi(redes) {
    const selectElement = document.querySelector('select[name="ssid"]');
    selectElement.innerHTML = ''; // Limpar a lista existente

    redes.forEach(rede => {
      const option = document.createElement('option');
      option.value = rede.ssid;
      option.textContent = rede.ssid;
      selectElement.appendChild(option);
    });
  }

    function validateForm() {
      var ssid = document.forms["myForm"]["ssid"].value;
      var password = document.forms["myForm"]["password"].value;

      var ip = document.forms["myForm"]["ip"].value;
      var gateway = document.forms["myForm"]["gate"].value;
      var mascara = document.forms["myForm"]["maska"].value;

      
      var status = document.getElementById("statusDiv");
      if (ssid == "" && password == "") {
        status.innerHTML = "<p id='status' style='color:red;'>Insira SSID e senha.</p>";
        return false;
      }
      else if (ssid == "") {
        status.innerHTML = "<p id='status' style='color:red;'>Insira SSID.</p>";
        return false;
      }
      else if (password == "") {
        status.innerHTML = "<p id='status' style='color:red;'>Insira senha.</p>";
        return false;
      }
      
      else {
        status.innerHTML = "<p id='status'>Conectando...</p>";
        return true;
      }
    }

    function openTab(tabName) {
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }
  document.getElementById(tabName + "Tab").style.display = "block";
}

  </script>
</head>
<body>
 <div>

 <div class="side-panel">
    <button class="button"  style="margin-top: 30px;" onclick="window.location.href='/'">Configurações do Cliente</button>
    <button class="button" style="margin-top: 30px;" onclick="window.location.href='/opcoes'" style="background-color: #008000;">Opções</button>
  </div>
  
<div class="container">
  <h2><span id="titleBlack">MODBUS RECEIVER WIFI/ETHERNET</span></h2>
   <h3>serial_number</h3>

 
  <!-- Tabs -->
      <div class="tabs">
        <button class="tablink" onclick="openTab('wifi')">Wi-Fi</button>
        <button class="tablink" onclick="openTab('ethernet')" style="background-color: #800080;">Ethernet</button>
      </div>

 

 <!-- Wi-Fi Tab Content -->
  <div id="wifiTab" class="tabcontent">
  <h3>rede_conectada</h3>
   <div class="button-container">
    <!-- Botão que chama a função listarRedesWifi() -->
    <input  id="listarRedesBtn" class="button" type="button" value="Listar Redes Wifi" onclick="listarRedesWifi()">
  </div>
 <h3>end_mac</h3>
     <form name="myForm" action="/action_new_connection" onsubmit="return validateForm()" method="post">
    <div class="field-group">
      <input class='text-field' type="text" name='ssid' length=64 placeholder="SSID"></select>
    </div>
    <br>
    <div class="field-group">
      <input class="text-field" type="password" name="password" length=64 placeholder="Password">
    </div>
  <br>
       <div class="field-group">
      <label for="ip">IP:</label>
      <input class="text-field" type="text" id="ip" name="ip" placeholder="Ip" value="{{ip}}">
    </div>
  <br>
    <div class="field-group">
      <label for="gateway">Gateway:</label>
      <input class="text-field" type="text" id="gateway" name="gate" placeholder="Gateway" value="{{gateway}}">
    </div>
  <br>
      <div class="field-group">
      <label for="mascara">Mascara:</label>
      <input class="text-field" type="text" id="mascara" name="maska" placeholder="Mascara" value="{{mascara}}">
    </div>

    
    <br>
    <div id="statusDiv">
      <br><br>
    </div>
    
    <div class="button-container">
      <input class="button" type="submit" value="Conectar">
    </div>

  </form>

  </div>
<!-- fecha o tab content wifi -->




<!-- Ethernet Tab Content -->
  <div id="ethernetTab" class="tabcontent">
 <h3>end_mac_eth</h3>
     <form name="myFormEth" action="/action_new_connection_eth" onsubmit="return validateFormEth()" method="post">
     <div class="field-group">
      <label for="ethIp">IP:</label>
      <input class="text-field" type="text" id="ethIp" name="ethIp" placeholder="Ip" value="{{ethIp}}">
    </div>
  <br>
    <div class="field-group">
      <label for="EthGateway">Gateway:</label>
      <input class="text-field" type="text" id="EthGateway" name="EthGateway" placeholder="EthGateway" value="{{EthGateway}}">
    </div>
  <br>
      <div class="field-group">
      <label for="EthSubnet">Sub Mascara:</label>
      <input class="text-field" type="text" id="EthSubnet" name="EthSubnet" placeholder="Sub mascara" value="{{EthSubnet}}">
    </div>
    
    <br>
    <div id="statusDivEth">
      <br><br>
    </div>
    
    <div class="button-container">
      <input class="button" type="submit" value="Conectar">
    </div>

  </form>

  </div>
<!-- fecha o tab content eth -->


 </div>
 <!-- fecha o container pai -->



   <!-- Janela modal -->
<div id="myModal" class="modal">
  <div class="modal-content">
    <p id="modalMessage">Aguardando Reinicialização</p>
    <p id="countdown">50</p>
  </div>
</div>


 </div>
<!-- fecha o div total -->

</body>
</html>
)=====";



const char Password_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>Configuração de Senha do Administrador - ESP8266</title>
  <style>
    body {
      color: #434343;
      font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
      font-size: 14px;
      background-color: #eeeeee;
      margin-top: 100px;
      text-align: center;
    }
    .container {
      margin: 0 auto;
      max-width: 400px;
      padding: 30px;
      box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23);
      background-color: #ffffff;
      border-radius: 10px;
    }
    h2 {
      margin-bottom: 20px;
      margin-top: 0px;
      color: #0ee6b1;
      font-size: 35px;
    }
    h3 {
      margin-bottom: 40px;
      margin-top: 0px;
      color: #336859;
      font-size: 20px;
    }
    .field-group {
      clear: both;
      padding: 10px 0;
      width: 100%;
    }
    .text-field {
      font-size: 15px;
      width: 100%;
      padding: 10px;
      border-radius: 5px;
      border: 1px solid #e6e6e6;
      background-color: #fafafa;
      box-sizing: border-box;
    }
    .button-container {
      margin-top: 20px;
    }
    .button {
      background: #00E1AA;
      border: none;
      border-radius: 5px;
      color: #ffffff;
      cursor: pointer;
      font-weight: bold;
      font-size: 16px;
      padding: 15px 30px;
      text-transform: uppercase;
      transition: background 250ms ease;
    }
    .button:hover {
      background: #00c196;
    }

   .modal {
  display: none;
  position: fixed;
  z-index: 1;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0,0,0,0.7);
}

.modal-content {
  position: absolute;
  background-color: #fefefe;
  text-align: center;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  padding: 20px;
  border-radius: 10px;
}

#modalMessage {
  font-size: 18px;
}

#countdown {
  font-size: 24px;
}
  </style>
  <script>
    function validateForm() {
      var password = document.forms["myForm"]["password"].value;
      var confirmPassword = document.forms["myForm"]["confirmPassword"].value;
      var status = document.getElementById("statusDiv");

      if (password === "") {
        status.innerHTML = "<p id='status' style='color:red;'>Insira uma senha.</p>";
        return false;
      }
      else if (password !== confirmPassword) {
        status.innerHTML = "<p id='status' style='color:red;'>As senhas não coincidem.</p>";
        return false;
      }
      else {
        status.innerHTML = "<p id='status'>Definindo senha...</p>";

              // Cria um objeto XMLHttpRequest
    var xhr = new XMLHttpRequest();
    
    // Configura a solicitação POST para a rota /action_add_mac
    xhr.open("POST", "/action_define_password", true);


      // Define o cabeçalho de tipo de conteúdo
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
// Define a função de retorno de chamada para processar a resposta da rota
    xhr.onload = function() {
      if (xhr.status == 200) {
      

        // Exibir a janela modal
    var modal = document.getElementById("myModal");
    modal.style.display = "block";

    // Iniciar o contador de 20 a 0
    var countdown = 20;
    var countdownInterval = setInterval(function() {
      document.getElementById("countdown").innerText = countdown;
      countdown--;

      if (countdown < 0) {
        clearInterval(countdownInterval);
        
        // Após o contador atingir 0, atualize a página
        window.location.reload();
      }
    }, 1000);


      } else {
        alert("Ocorreu um erro ao adicionar o endereço MAC.");
      }
    };

     // Envia o newMac como dados de formulário
    xhr.send("password=" + password);


        return true;
      }
    }

  
    
  </script>
</head>
<body>
 <div>
  <div class="container">
    <h2>MODBUS RECEIVER WIFI/ETHERNET</h2>
    <h3>Defina a senha do administrador</h3>
     <form name="myForm" onsubmit="return false;" method="post">
      <div class="field-group">
        <input class="text-field" type="password" name="password" placeholder="Senha">
      </div>
      <div class="field-group">
        <input class="text-field" type="password" name="confirmPassword" placeholder="Repita a Senha">
      </div>
      <div id="statusDiv">
        <br>
      </div>
      <div class="button-container">
        <input class="button" type="submit" value="OK" onclick="validateForm()">
      </div>
    </form>
  </div>


    <!-- Janela modal -->
<div id="myModal" class="modal">
  <div class="modal-content">
    <p id="modalMessage">Aguardando Reinicialização</p>
    <p id="countdown">50</p>
  </div>
</div>



   </div>
</body>
</html>
)=====";


const char Login_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>Login -MODBUS RECEIVER WIFI/ETHERNET</title>
  <style>
    body {
      color: #434343;
      font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
      font-size: 14px;
      background-color: #eeeeee;
      margin-top: 100px;
      text-align: center;
    }
    .container {
      margin: 0 auto;
      max-width: 400px;
      padding: 30px;
      box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23);
      background-color: #ffffff;
      border-radius: 10px;
    }
    h2 {
      margin-bottom: 20px;
      margin-top: 0px;
      color: #0ee6b1;
      font-size: 35px;
    }
    h3 {
      margin-bottom: 40px;
      margin-top: 0px;
      color: #336859;
      font-size: 20px;
    }
    .field-group {
      clear: both;
      padding: 10px 0;
      width: 100%;
    }
    .text-field {
      font-size: 15px;
      width: 100%;
      padding: 10px;
      border-radius: 5px;
      border: 1px solid #e6e6e6;
      background-color: #fafafa;
      box-sizing: border-box;
    }
    .button-container {
      margin-top: 20px;
    }
    .button {
      background: #00E1AA;
      border: none;
      border-radius: 5px;
      color: #ffffff;
      cursor: pointer;
      font-weight: bold;
      font-size: 16px;
      padding: 15px 30px;
      text-transform: uppercase;
      transition: background 250ms ease;
    }
    .button:hover {
      background: #00c196;
    }
  </style>
  <script>
    function validateForm() {
      var password = document.forms["myForm"]["password"].value;
      var status = document.getElementById("statusDiv");

      if (password === "") {
        status.innerHTML = "<p id='status' style='color:red;'>Insira uma senha.</p>";
        return false;
      }
      else {
        status.innerHTML = "<p id='status'>Login...</p>";
        return true;
      }
    }
  </script>
</head>
<body>
  <div class="container">
    <h2>MODBUS RECEIVER WIFI/ETHERNET</h2>
    <h3>Entre com a senha do administrador</h3>
     <form name="myForm" action="/action_login" onsubmit="return validateForm()" method="post">
      <div class="field-group">
        <input class="text-field" type="password" name="password" placeholder="Senha">
      </div>
      <div id="statusDiv">
        <br>
      </div>
      <div class="button-container">
        <input class="button" type="submit" value="OK">
      </div>
    </form>
  </div>
</body>
</html>
)=====";





const char Configs_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>MODBUS RECEIVER WIFI/ETHERNET</title>
  <style>
    body {color: #434343; font-family: "Helvetica Neue",Helvetica,Arial,sans-serif; font-size: 14px; background-color: #eeeeee; margin-top: 100px;}
    .container {margin: 0 auto; max-width: 400px; padding: 30px; box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23); background-color: #ffffff; border-radius: 10px;}
    h2 {text-align: center; margin-bottom: 20px; margin-top: 0px; color: #0ee6b1; font-size: 35px;}
    #titleGreen {color: #00E1AA;}
    #titleBlack {color: #000000;}
    h3 {text-align: center; margin-bottom: 40px; margin-top: 0px; color: #336859; font-size: 35px;}
    form .field-group {box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0; width: 100%;}
    .text-field {font-size: 15px; margin-bottom: 4%; -webkit-appearance: none; display: block; background: #fafafa; color: #636363; width: 100%; padding: 15px 0px 15px 0px; text-indent: 10px; border-radius: 5px; border: 1px solid #e6e6e6; background-color: transparent;}
    .text-field:focus {border-color: #00bcd4; outline: 0;}
    .button-container {box-sizing: border-box; clear: both; margin: 1px 0 0; padding: 4px 0; position: relative; width: 100%;}
    .button {background: #00E1AA; border: none; border-radius: 5px; color: #ffffff; cursor: pointer; display: block; font-weight: bold; font-size: 16px; padding: 15px 0; text-align: center; text-transform: uppercase; width: 100%; -webkit-transition: background 250ms ease; -moz-transition: background 250ms ease; -o-transition: background 250ms ease; transition: background 250ms ease;}
    p {text-align: center; text-decoration: none; color: #87c1d3; font-size: 18px;}
    a {text-decoration: none; color: #ffffff; margin-top: 0%;}
    #status {text-align: center; text-decoration: none; color: #336859; font-size: 14px;}
    .side-panel {background-color: #add8e6; float: left; width: 200px; padding: 15px; height: 100%;}

 .field-group {
      clear: both;
      padding: 10px 0;
      width: 100%;
    }
    .text-field {
      font-size: 15px;
      width: 100%;
      padding: 10px;
      border-radius: 5px;
      border: 1px solid #e6e6e6;
      background-color: #fafafa;
      box-sizing: border-box;
    }
    .button-container {
      margin-top: 20px;
    }
    .button {
      background: #00E1AA;
      border: none;
      border-radius: 5px;
      color: #ffffff;
      cursor: pointer;
      font-weight: bold;
      font-size: 16px;
      padding: 15px 30px;
      text-transform: uppercase;
      transition: background 250ms ease;
    }
    .button:hover {
      background: #00c196;
    }

    
  </style>
  <script>

  function configureCentralIP() {
      var centralIP = document.getElementById("centralIP").value;
         var centralInterval = document.getElementById("centralInterval").value;

      if (centralIP !== "" && centralInterval) {
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/action_configs", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.onload = function() {
          if (xhr.status == 200) {
            alert("Configurações salvas!");
            window.location.reload(); // Atualiza a página após a configuração
          } else {
            alert("Ocorreu um erro ao configurar o endereço IP da central.");
          }
        };
        var dados = "centralIP=" + centralIP + "&centralInterval=" + centralInterval;

        xhr.send(dados);
      } else {
        alert("Revise as informações");
      }
    }

  </script>
</head>
<body>
 <div>

 <div class="side-panel">
    <button class="button"  style="margin-top: 30px;" onclick="window.location.href='/'">Configurações do Cliente</button>
    <button class="button" style="margin-top: 30px;" onclick="window.location.href='/configs'" style="background-color: #008000;">Configurações Servidor</button>
     <button class="button" style="margin-top: 30px;" onclick="window.location.href='/opcoes'" style="background-color: #008000;">Opções</button>
  </div>
  
   <div class="container">
  <h2>MODBUS RECEIVER WIFI/ETHERNET</h2>
    <h3>Configure Acesso ao servidor Modbus</h3>
    <div class="field-group">
      <label for="centralIP">Endereço IP do Servidor modbus:</label>
      <input class="text-field" type="text" id="centralIP" name="centralIP" placeholder="Digite o endereço da central, ex: 10.0.0.1"  value="{{enderecoIpCentral}}">
    </div>
     <div class="field-group">
      <label for="centralInterval">Intervalo de leitura do servidor modbus(em segundos):</label>
      <input class="text-field" type="text" id="centralInterval" name="centralInterval" placeholder="Intervalo de leitura do servidor modbus (em segundos), ex: 60"  value="{{timeInterval}}">
    </div>
     
    
    <div class="button-container">
      <input class="button" type="button" value="OK" onclick="configureCentralIP()">
    </div>
  </div>



 </div>
</body>
</html>
)=====";

 








const char Opcoes_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <title>MODBUS RECEIVER WIFI/ETHERNET</title>
  <style>
    body {color: #434343; font-family: "Helvetica Neue",Helvetica,Arial,sans-serif; font-size: 14px; background-color: #eeeeee; margin-top: 100px;}
    .container {margin: 0 auto; max-width: 400px; padding: 30px; box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23); background-color: #ffffff; border-radius: 10px;}
    h2 {text-align: center; margin-bottom: 20px; margin-top: 0px; color: #0ee6b1; font-size: 35px;}
    #titleGreen {color: #00E1AA;}
    #titleBlack {color: #000000;}
    h3 {text-align: center; margin-bottom: 40px; margin-top: 0px; color: #336859; font-size: 35px;}
    form .field-group {box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0; width: 100%;}
    .text-field {font-size: 15px; margin-bottom: 4%; -webkit-appearance: none; display: block; background: #fafafa; color: #636363; width: 100%; padding: 15px 0px 15px 0px; text-indent: 10px; border-radius: 5px; border: 1px solid #e6e6e6; background-color: transparent;}
    .text-field:focus {border-color: #00bcd4; outline: 0;}
    .button-container {box-sizing: border-box; clear: both; margin: 1px 0 0; padding: 4px 0; position: relative; width: 100%;}
    .button {background: #00E1AA; border: none; border-radius: 5px; color: #ffffff; cursor: pointer; display: block; font-weight: bold; font-size: 16px; padding: 15px 0; text-align: center; text-transform: uppercase; width: 100%; -webkit-transition: background 250ms ease; -moz-transition: background 250ms ease; -o-transition: background 250ms ease; transition: background 250ms ease;}
    p {text-align: center; text-decoration: none; color: #87c1d3; font-size: 18px;}
    a {text-decoration: none; color: #ffffff; margin-top: 0%;}
    #status {text-align: center; text-decoration: none; color: #336859; font-size: 14px;}
    .side-panel {background-color: #add8e6; float: left; width: 200px; padding: 15px; height: 100%;}

 .field-group {
      clear: both;
      padding: 10px 0;
      width: 100%;
    }
    .text-field {
      font-size: 15px;
      width: 100%;
      padding: 10px;
      border-radius: 5px;
      border: 1px solid #e6e6e6;
      background-color: #fafafa;
      box-sizing: border-box;
    }
    .button-container {
      margin-top: 20px;
    }
    .button {
      background: #00E1AA;
      border: none;
      border-radius: 5px;
      color: #ffffff;
      cursor: pointer;
      font-weight: bold;
      font-size: 16px;
      padding: 15px 30px;
      text-transform: uppercase;
      transition: background 250ms ease;
    }
    .button:hover {
      background: #00c196;
    }

    
  </style>
  <script>

 function salvarOpcoes() {
    var prioridadeConexao = document.querySelector('input[name="conexaoPrioridade"]:checked').value;

    fetch('/action_opcoes', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: 'prioridadeConexao=' + prioridadeConexao,
    }).then(response => {
      // Lógica para lidar com a resposta, se necessário
    }).catch(error => {
      console.error('Erro ao enviar a solicitação:', error);
    });
  }

  </script>
</head>
<body>
 <div>

 <div class="side-panel">
    <button class="button"  style="margin-top: 30px;" onclick="window.location.href='/'">Configurações do Cliente</button>
    <button class="button" style="margin-top: 30px;" onclick="window.location.href='/configs'" style="background-color: #008000;">Configurações Servidor</button>
     <button class="button" style="margin-top: 30px;" onclick="window.location.href='/opcoes'" style="background-color: #008000;">Opções</button>
  </div>
  
   <div class="container">
  <h2>MODBUS RECEIVER WIFI/ETHERNET</h2>
    <h3>Opções Gerais</h3>
     <button style="background-color: red; color: white;" onclick="window.location.href='/action_reset'">Reiniciar</button>

 <!-- Radio buttons para a ordem de prioridade da conexão -->
  <label for="conexaoPrioridade">Ordem Prioridade da Conexão:</label><br>
<input type="radio" id="conexaoEthernetWifi" name="conexaoPrioridade" value="0" {{ethChecked}}>
<label for="conexaoEthernetWifi">Ethernet/Wifi</label><br>
<input type="radio" id="conexaoWifiEthernet" name="conexaoPrioridade" value="1" {{wifiChecked}}>
<label for="conexaoWifiEthernet">Wifi/Ethernet</label><br><br>

  <!-- Botão salvar -->
  <button style="background-color: blue; color: white; padding: 10px 20px; margin-bottom: 20px;" onclick="salvarOpcoes()">Salvar</button>


  </div>



 </div>
</body>
</html>
)=====";




