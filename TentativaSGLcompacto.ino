// === SENSOR DE COR ESQUERDO (TCS3200) ===
#define S0 23  // Controle de escala de frequência
#define S1 22
#define S2 25  // Seleção de filtro de cor
#define S3 24
#define OUT 27 // Saída do sensor

// === SENSOR DE COR DIREITO (TCS3200) ===
#define S00 51
#define S11 50
#define S22 53
#define S33 52
#define OUTT 49

// === MOTORES COM PONTE H (L298N) ===
// PWM motor esquerdo
#define ENA 2     
#define IN1 4
#define IN2 3

// PWM motor direito
#define ENB 7 
#define IN3 5
#define IN4 6

// Sensor ultrassonico: 
#define echo 41
#define trig 39

// Variáveis globais para armazenar cores e distancia
String corE;
String corD;
int distancia;


void setup() {
  Serial.begin(9600);

  //Pinos do sensor ultrassonico
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Pinos do sensor esquerdo
  pinMode(S0, OUTPUT);  
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);  
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  // Pinos do sensor direito
  pinMode(S00, OUTPUT); 
  pinMode(S11, OUTPUT);
  pinMode(S22, OUTPUT);
  pinMode(S33, OUTPUT);
  pinMode(OUTT, INPUT);

  // Pinos dos motores
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Sistema Iniciado: sensores de cor + motores...");

  // Configura sensores para 100% da frequência de saída
  digitalWrite(S0, HIGH);  
  digitalWrite(S1, HIGH);
  digitalWrite(S00, HIGH); 
  digitalWrite(S11, HIGH);
}

void loop() {
  distancia = detectaDistancia();
  if (distancia < 15){
    desviaObjetos();
  }
  else{
    leituraExibicao();   // Lê sensores e atualiza corE e corD
    seguidorDeLinha();   // Executa a lógica do seguidor de linha
  }
  delay(100);
}

// Função de leitura e exibição dos valores
void leituraExibicao() {
  // Sensor esquerdo
  int redE = readColor(S2, S3, OUT, LOW, LOW);
  int greenE = readColor(S2, S3, OUT, HIGH, HIGH);
  int blueE = readColor(S2, S3, OUT, LOW, HIGH);

  // Sensor direito
  int redD = readColor(S22, S33, OUTT, LOW, LOW);
  int greenD = readColor(S22, S33, OUTT, HIGH, HIGH);
  int blueD = readColor(S22, S33, OUTT, LOW, HIGH);

  // Identifica as cores detectadas
  corE = identificarCor(redE, greenE, blueE);
  corD = identificarCor(redD, greenD, blueD);

  // Exibe valores no monitor serial
  Serial.println(">> Cor detectada:");
  Serial.print("[ESQUERDA] R: "); Serial.print(redE);
  Serial.print(" | G: "); Serial.print(greenE);
  Serial.print(" | B: "); Serial.println(blueE);
  Serial.println(corE);
  Serial.println("-=-=-=--=");

  Serial.print("[DIREITA] R: "); Serial.print(redD);
  Serial.print(" | G: "); Serial.print(greenD);
  Serial.print(" | B: "); Serial.println(blueD);
  Serial.println(corD);
  Serial.println("-=-=-=--=");
}

// Função para ler cor do TCS3200
int readColor(int s2, int s3, int outPin, bool s2State, bool s3State) {
  digitalWrite(s2, s2State);
  digitalWrite(s3, s3State);
  delayMicroseconds(200); // Pequena pausa para estabilidade
  return pulseInLong(outPin, LOW, 20000); // timeout de 20ms
}

// Função para identificar cor
String identificarCor(int red, int green, int blue) {
  if (red <= 17 && green >= 19 && blue >= 15) {
    return "Vermelho";
  } 
  else if (red >= 18 && green >= 16 && blue < 20) {
    return "Preto";
  } 
  else if (blue <= green && green < red) {
    if (red < 10 && green < 10 && blue < 10) {
      return "Branco";
    }
    else {
      return "Verde";
    }
  }
  else if (red <= 13 && green <= 13 && blue < 10) {
    return "Branco";
  } 
  else {
    return "Indefinido";
  }
}

//Função para detectar a distancia 
int detectaDistancia(){
    // Envia um pulso de trigger
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  //mede o tempodos inal do eco
  long duracao = pulseIn(echo, HIGH, 30000);
  if (duracao == 0) return 999; // Sem leitura, assume longe

  int d = duracao / 58; // converte prwa cm

  Serial.print("Distância: ");
  Serial.print(d);
  Serial.println("cm");
  delay(400);
  return d;
}

//Função de desviar do objeto
void desviaObjetos(){
    parado();
    delay(500);
    motorTras();
    delay(1000);
    motorEsquerda();
    delay(860);
    motorFrente();
    delay(2700);
    motorDireitaSuave();
    delay(1000);
    motorFrente();
    delay(1000);
    motorDireitaSuave();
    delay(2200);
    motorFrente();
  }

// Função para seguir a linha
void seguidorDeLinha() {
  if (corE == "Preto" && corD == "Preto") {
    motorFrente();
  } 

  else if (corE == "Preto" && corD == "Branco") {
    motorEsquerda();
  } 

  else if (corE == "Branco" && corD == "Preto") {
    motorDireita();
  } 

  else if (corE == "Branco" && corD == "Branco") {
    motorFrente();
  } 

  else if (corE == "Verde" && corD != "Verde"){
    motorEsquerda();
    delay(100);
    leituraExibicao();
    if(corE == "Verde"){ //Verifica se realmente é verde ou se estava entre o preto e o branco
      giraVerdeEsquerda();
    }
    else{
      motorFrente();
    }
  }

  else if (corE != "Verde" && corD == "Verde"){
    motorDireita();
    delay(100);
    leituraExibicao();
    if(corD == "Verde"){
      giraVerdeDireita();
    }
    else{
      motorFrente();
    }

  }
  else if (corE == "Vermelho" && corD == "Vermelho") {
    parado();
    delay(7000);
  }
  else {
    motorFrenteDevagar();
  }
}

// === CONTROLE DOS MOTORES ===
void motorFrente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
  Serial.println(">> Motores girando para FRENTE");
}

void motorFrenteDevagar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 100);
  analogWrite(ENB, 100);
  delay(20);
  Serial.println(">> Motores girando para FRENTE DEVAGAR");
}

void motorEsquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 180);
  analogWrite(ENB, 250);
  Serial.println(">> Virando para a ESQUERDA");
}

void motorDireita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 250);
  analogWrite(ENB, 180);
  Serial.println(">> Virando para a DIREITA");
}

void motorDireitaSuave() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 250);
  analogWrite(ENB, 140);
  Serial.println(">> Virando para a DIREITA SUAVE");
}

void giraVerdeEsquerda(){
  motorFrente();
  delay(300); //Anda um pouco para frente para passar do verde
  motorEsquerda(); // gira pra esquerda, onde viu o verde
  while (corE != "Preto"){
    motorEsquerda();
    leituraExibicao();
  }
  parado();
  delay(100);
}

void giraVerdeDireita(){
  motorFrente();
  delay(300);
  motorDireita();
  while (corD != "Preto"){
    motorDireita();
    leituraExibicao();
  }
  parado();
  delay(100);
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
