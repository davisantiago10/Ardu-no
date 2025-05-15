#include <avr/sleep.h>
#define echo 8
#define trig 9
#define vermelho 11
#define amarelo 53

const int sensorPin = A0;
int sensorValue = 0; 
const int buzzer = 40;


// Acho que os leds estão desconectados né
// l3c3dts1-01
void setup() {
  Serial.begin(9600);
  pinMode(amarelo, OUTPUT); 
  pinMode(vermelho, OUTPUT); 
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(sensorPin, INPUT);
  Serial.println("Teste de Sensor Infravermelho - Preto e Branco");
  Serial.println("============================================");
}

void loop() {
  // Distância
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duracao = pulseIn(echo,HIGH);
  int distancia = duracao / 58; // converte prwa cm
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println("cm");
  if (distancia < 15) {
    // Se a distância for menor que 10 cm, acende o LED vermelho
    digitalWrite(vermelho, HIGH);
    digitalWrite(amarelo, LOW);
    Serial.println("Objeto identificado por perto!");
    tone(buzzer, 800);
  }
  else {
    // Se a distância for maior que 15 cm, acende o LED verde
    digitalWrite(vermelho, LOW);
    digitalWrite(amarelo, HIGH);
    Serial.println("Área livre, continuando...");
    noTone(buzzer);
  }
  Serial.println("------------------------------");
  // Preto ou branco
  sensorValue = analogRead(sensorPin);
  Serial.print("Valor do sensor: ");
  Serial.println(sensorValue);
  if (sensorValue > 800) {  // Ajuste este valor conforme necessário
    Serial.println("Superficie PRETA detectada");
  } else {
    Serial.println("Superficie BRANCA detectada");
  }
  
  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
  
  // Pequena pausa entre leituras
  delay(1500);
  
}
