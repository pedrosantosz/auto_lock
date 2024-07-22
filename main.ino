#include <WiFi.h>  // Inclui a biblioteca WiFi
#define gaveta 14  // Pino para controle da gaveta

const char *ssid = "";  // Nome da rede WiFi
const char *password = "";   // Senha da rede

WiFiServer server(80);  // Define a porta que o servidor irá utilizar

void fechadura();  // Função para gerar web server e controlar o piano

void setup() {
  Serial.begin(9600);
  pinMode(gaveta, OUTPUT);  // Configura como saída
  digitalWrite(gaveta, 0);

  WiFi.begin(ssid, password);  // Inicializa WiFi, passando o nome da rede e a senha

  while (WiFi.status() != WL_CONNECTED) {  // Aguarda conexão
    delay(741);
  }

  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());  // Mostra o endereço IP

  server.begin();  // Inicializa o servidor web
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {  // Verifica se está conectado
    WiFi.begin(ssid, password);  // Tenta se conectar novamente
    delay(741);
  }

  fechadura();  // Chama a função para controle do piano por WiFi
}

void fechadura() {
  WiFiClient client = server.available();  // Verifica se existe um cliente conectado

  if (client) {  // Existe um cliente?
    String currentLine = "";  // String para armazenar a mensagem do cliente

    while (client.connected()) {  // Repete enquanto o cliente estiver conectado
      if (client.available()) {  // Existem dados a serem lidos?
        char c = client.read();  // Salva em c

        if (c == '\n') {  // É um caractere de nova linha?
          if (currentLine.length() == 0) {  // A mensagem terminou?
            // Gera a página HTML
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // código da página
            client.print("<head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" lang=\"pt-BR\">");
            client.print("<title>Controle de Gaveta</title>");
            client.print("<style>body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f0f0f0; font-family: Arial, sans-serif;}");
            client.print(".container { text-align: center;}");
            client.print(".cabinet { width: 150px; height: 100px; background-color: #8b4513; position: relative;}");
            client.print(".drawer { position: absolute; width: 100%; height: 100%; background-color: #a0522d; transition: transform 1s;}");
            client.print(".handle { position: absolute; width: 30px; height: 10px; background-color: gold; border-radius: 5px; left: 50%; top: 45%; transform: translateX(-50%);}");
            client.print(".drawer.closed { transform: translateX(0);}");
            client.print(".drawer.open { transform: translateX(100%);}");
            client.print("button { margin-top: 20px; padding: 10px 20px; font-size: 16px; cursor: pointer;}</style>");

            client.print("</head><body>");
            client.print("<div class=\"container\">");
            client.print("<div class=\"cabinet\">");
            client.print("<div id=\"drawer\" class=\"drawer closed\">");
            client.print("<div class=\"handle\"></div>");
            client.print("</div>");
            client.print("</div>");
            client.print("<button id=\"toggleButton\" onclick=\"abrirGaveta('abrir')\">Abrir Gaveta</button>");
            client.print("</div>");

            client.print("<script>");
            client.print("let abriu = 0;");
            client.print("function abrirGaveta(abrir) {");
            client.print("  if (abriu == 0) {abriu = 1; fetch('/' + abrir);}");
            client.print("    const drawer = document.getElementById('drawer');");
            client.print("    drawer.classList.remove('closed');");
            client.print("    drawer.classList.add('open');");
            client.print("    setTimeout(() => {");
            client.print("      drawer.classList.remove('open');");
            client.print("      drawer.classList.add('closed');");
            client.print("      abriu = 0;");
            client.print("    }, 5000);");
            client.print("}");

            client.print("</script>");
            client.print("</body>");

            client.println();
            break;  // Encerra o laço
          } else {
            currentLine = "";  // Reseta a linha atual
          }
        } else if (c != '\r') {
          currentLine += c;  // Adiciona o caractere à mensagem
        }

        // Verifica se o botão foi pressionado e aciona o transistor para abrir a gaveta
        if (currentLine.endsWith("GET /abrir")) { 
          digitalWrite(gaveta, 1);  // abre gaveta
          delay(5000);
          digitalWrite(gaveta, 0);
        }
      }
    }
    client.stop();  // Finaliza conexão
  }
}
