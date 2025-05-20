#include <avr/sleep.h>

// Sensores infravermelhos
const int sensorPinD = A0; // Sensor da direita
const int sensorPinE = A1; // Sensor da esquerda

// Ponte H L298N (Motores)
#define ENA 5     // PWM motor esquerdo
#define IN1 6
#define IN2 10

#define ENB 11    // PWM motor direito
#define IN3 12
#define IN4 13

void setup() {
  Serial.begin(9600);

  // Configurações dos sensores e atuadores
  pinMode(sensorPinD, INPUT); // Modificado
  pinMode(sensorPinE, INPUT);

  // Configuração dos motores
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Sistema Iniciado: Sensor IR + Ultrassônico + Motores");
  Serial.println("=======================KKKBURROKKK==============================");
}

void loop() {
  Serial.println("------------------------------");
  // ------------------------------
  // LEITURA DOS SENSORES IR
  // ------------------------------
  int sensorValorDireita = analogRead(sensorPinD);
  int sensorValorEsquerda = analogRead(sensorPinE);

  Serial.print("Valor do sensor IR Direita: ");
  Serial.println(sensorValorDireita);
  Serial.print("Valor do sensor IR Esquerda: ");
  Serial.println(sensorValorEsquerda);

  if (sensorValorEsquerda > 800 && sensorValorDireita > 800) {
    Serial.println(">> Superfície PRETA detectada");
    motorFrente();
  } 

  else if (sensorValorEsquerda < 100 && sensorValorDireita < 100){
    Serial.println(">> Superfície BRANCA detectada");
    motorFrente();
  }

  else if (sensorValorEsquerda < 100 && sensorValorDireita > 800) {
    Serial.println(">> Virando para a direita");
    motorDireita();
  }

  else if (sensorValorEsquerda > 800 && sensorValorDireita < 100){
    Serial.println("Virando para a esquerda");
    motorEsquerda();
  }
  // ------------------------------
  // LÓGICA DE MOVIMENTO DOS MOTORES
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

void motorEsquerda(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150); // Reduz um pouco a velocidade para curva
  analogWrite(ENB, 200);
  Serial.println(">> Motores girando para a ESQUERDA");
}


void motorDireita(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 150);
  Serial.println(">> Motores girando para a DIREITA");
}
