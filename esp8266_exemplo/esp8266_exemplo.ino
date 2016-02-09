#include<stdlib.h>
#include <SoftwareSerial.h>

#define WIFI "alterar"
#define PASS "alterar"

SoftwareSerial conexao(8, 11); // RX, TX

int numCon = 0;

void setup() {
  conexao.begin(9600);
  Serial.begin(9600);
  
  if (executarComando("AT", "OK")) {
    if (connectWiFi()) {
      Serial.println("\n****\nCONECTADO COM SUCESSO...\n***\n\n");
    } else {
      Serial.println("Erro ao conectar na rede wifi");
    }
  } else {
    Serial.println("NAO FOI POSSIVEL CONECTAR NO DISPOSITIVO DE REDE ESP8266");
  }

}

void loop() {
  enviarGet("192.168.1.3", 8080, "TESTE");
  delay(5000);
}

/*
 * Envia uma requisicao get com o pacote para um determinado ip e uma determinada porta.
 * 
 */
boolean enviarGet(String ip, int porta, String pacote){
  conexao.flush();
  String msg = "GET /esp8266web/sensor?"+pacote+" HTTP/1.1\r\n";
  msg += ("Host: " + ip + ":" + String(porta) + "\r\n");
  msg += "Connection: close\r\n\r\n";
  
  int _numCon = numCon;
  if(executarComando("AT+CIPSTART="+String(numCon)+",\"TCP\",\""+ip+"\"," + String(porta), "OK", 2000)){
    String cmd = "AT+CIPSEND="+String(numCon)+"," + String(msg.length() + 2);  
    if(executarComando(cmd, ">", 2000) && executarComando(msg, "SEND OK", 2000)){
      Serial.println("Msg enviada com sucesso....");
      conexao.find("\r\n\r\n");
      Serial.println(conexao.readString());
    }else{
      Serial.println("\nFalha ao enviar dados. Tentando novamente em 3s");
      delay(3000);
      executarComando("AT+CWMODE=1", "OK");
      executarComando("AT+CIPMUX=1", "OK");
    }
  }else{
    Serial.println("\nFalha ao conectar no servidor. Tentando novamente em 3s...");
    delay(3000);
    executarComando("AT+CWMODE=1", "OK");
    executarComando("AT+CIPMUX=1", "OK");
  }
  executarComando("AT+CIPCLOSE=5", "OK", 2000);
  
  conexao.flush();
}

/*
 * Executa um comando AT no esp8266 e aguarda o esp8266 retornar uma string igual a variavel esperar
 */
boolean executarComando(String comando, String esperar) {
  return executarComando(comando, esperar, 100000);
}

/*
 * Executa um comando AT no esp8266 e aguarda o esp8266 retornar uma string igual a variavel esperar.
 * Tambem permite ajustar o timeOut para a execucao
 */
boolean executarComando(String comando, String esperar, int timeOut) {
  conexao.setTimeout(timeOut);
  Serial.print("\nCMD: " + comando + " E: *" + esperar + "*");
  conexao.println(comando);
  Serial.print("[executado]");
  
  char e[esperar.length()];
  esperar.toCharArray(e, esperar.length());
  if (conexao.find(e)) {
    Serial.print("[SUCESSO]");
    return true;
  } else {
    Serial.print("[FALHOU]");
    return false;
  }
}

/*
 * Conecta na rede wifi
 */
boolean connectWiFi() {
  String cmd = "AT+CWJAP=\"";
  cmd += WIFI;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";


  return 
         executarComando("AT+CWMODE=1", "OK") &&
         executarComando(cmd, "OK") &&
         executarComando("AT+CIPMUX=1", "OK");
}

