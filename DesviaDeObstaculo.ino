#define echo 41
#define trig 39

// === MOTORES COM PONTE H (L298N) ===
// PWM motor esquerdo
#define ENA 2     
#define IN1 4
#define IN2 3

// PWM motor direito
#define ENB 7 
#define IN3 5
#define IN4 6
//F verde
int distancia;

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);  // put your setup code here, to run once:
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  distancia = detectaDistancia();
  desviaObjetos();

}
int detectaDistancia(){
    // Envia um pulso de trigger
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  //mede o tempodos inal do eco
  long duracao = pulseIn(echo,HIGH);
  int d = duracao / 58; // converte prwa cm

  Serial.print("Distância: ");
  Serial.print(d);
  Serial.println("cm");
  delay(400);
  return d;
}

void motorFrente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
  Serial.println(">> Motores girando para FRENTE");
}

void motorTras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
  Serial.println(">> Motores girando para FRENTE");
}

void parado(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println(">> Motores parados");
}
void motorEsquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);
  analogWrite(ENB, 250);
  Serial.println(">> Virando para a ESQUERDA");
}

void motorDireitaSuave() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 250);
  analogWrite(ENB, 170);
  Serial.println(">> Virando para a DIREITA");
}

void desviaObjetos(){
  if (distancia > 20){
    motorFrente();
  }
  else{
    parado();
    delay(500);
    motorTras();
    delay(1000);
    motorEsquerda();
    delay(600);
    motorFrente();
    delay(1000);
    motorDireitaSuave();
    delay(3000);
  }
}
