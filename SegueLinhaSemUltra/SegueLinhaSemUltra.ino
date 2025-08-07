#include <avr/sleep.h>

// Sensores infravermelhos
const int sensorPinD = A0; // Sensor da direita
const int sensorPinE = A1; // Sensor da esquerda

// Ponte H L298N (Motores)
#define ENA 2     // PWM motor esquerdo
#define IN1 3
#define IN2 4

#define ENB 7    // PWM motor direito
#define IN3 5
#define IN4 6

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
.
  if (sensorValorEsquerda > 600 && sensorValorDireita > 600) {
    Serial.println(">> Superfície PRETA detectada");
    motorFrente();
  } 

  else if (sensorValorEsquerda < 200 && sensorValorDireita < 200){
    Serial.println(">> Superfície BRANCA detectada");
    motorFrente();
  }

  else if (sensorValorEsquerda < 200 && sensorValorDireita > 600) {
    Serial.println(">> Virando para a direita");
    motorDireita();
  }

  else if (sensorValorEsquerda > 600 && sensorValorDireita < 200){
    Serial.println("Virando para a esquerda");
    motorEsquerda();
  }
  else{
    Serial.println("Cor não identificada");
    motorFrente();
  }
  // ------------------------------
  // LÓGICA DE MOVIMENTO DOS MOTORES
  delay(40); // Aguarda antes da próxima leitura
}

// ------------------------------
// FUNÇÕES AUXILIARES
// ------------------------------

void motorFrente() {
  digitalWrite(IN1, HIGH);  // motor esquerdo para frente
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // motor direito para frente
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 90);
  analogWrite(ENB, 90);
  Serial.println(">> Motores girando para FRENTE");
}

void motorEsquerda(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 40); // Inverte o movimento
  analogWrite(ENB, 150);
  Serial.println(">> Motores girando para a ESQUERDA");
}


void motorDireita(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 40);
  Serial.println(">> Motores girando para a DIREITA");
}

