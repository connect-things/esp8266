#include<stdlib.h>
#include <SoftwareSerial.h>

SoftwareSerial conexao(8, 11); // RX, TX

void setup() {
   conexao.begin(9600);
   Serial.begin(9600);
}

void loop() {
  if (Serial.available()){
    String comando = Serial.readString(); 
    Serial.println(comando);
    Serial.println("-----");
    conexao.println(comando);
  }

  if(conexao.available()){
      Serial.println(conexao.readString());
  }
}
