#include <avr/sleep.h>

// Pinos do sensor ultrassônico
#define echo 8
#define trig 9

// Pinos dos LEDs
#define vermelho 11
#define amarelo 53

// Sensor infravermelho
const int sensorPin = A0;

// Buzzer
const int buzzer = 40;

// Ponte H L298N (Motores)
#define ENA 5     // PWM motor esquerdo
#define IN1 6
#define IN2 7
#define ENB 10    // PWM motor direito
#define IN3 12
#define IN4 13

void setup() {
  Serial.begin(9600);

  // Configurações dos sensores e atuadores
  pinMode(amarelo, OUTPUT); 
  pinMode(vermelho, OUTPUT); 
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);

  // Configuração dos motores
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Sistema Iniciado: Sensor IR + Ultrassônico + Motores");
  Serial.println("=====================================================");
}

void loop() {
  // ------------------------------
  // LEITURA DO SENSOR ULTRASSÔNICO
  // ------------------------------
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH);
  int distancia = duracao / 58; // converte para cm

  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println("cm");

  // Avisa se objeto está próximo
  if (distancia < 15) {
    digitalWrite(vermelho, HIGH);
    digitalWrite(amarelo, LOW);
    Serial.println(">> Objeto identificado por perto!");
    tone(buzzer, 800);
  } else {
    digitalWrite(vermelho, LOW);
    digitalWrite(amarelo, HIGH);
    Serial.println(">> Área livre, continuando...");
    noTone(buzzer);
  }

  Serial.println("------------------------------");

  // ------------------------------
  // LEITURA DO SENSOR INFRAVERMELHO
  // ------------------------------
  int sensorValue = analogRead(sensorPin);
  Serial.print("Valor do sensor IR: ");
  Serial.println(sensorValue);

  if (sensorValue > 800) {
    Serial.println(">> Superfície PRETA detectada");
  } else {
    Serial.println(">> Superfície BRANCA detectada");
  }

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");

  // ------------------------------
  // LÓGICA DE MOVIMENTO DOS MOTORES
  // ------------------------------
  if (distancia >= 15 && sensorValue < 800) {
    // Caminho livre e solo branco → anda para frente
    motorFrente();
  } else {
    // Objeto próximo ou solo preto → parar
    motorParar();
  }

  delay(500); // Aguarda antes da próxima leitura
}

// ------------------------------
// FUNÇÕES AUXILIARES
// ------------------------------

void motorFrente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200); // Velocidade ajustável
  analogWrite(ENB, 200);
  Serial.println(">> Motores girando para FRENTE");
}

void motorParar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println(">> Motores PARADOS");
}
