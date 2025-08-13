
// === SENSOR DE COR ESQUERDO (TCS3200) ===
  #define S0 52  // Controle de escala de frequência
  #define S1 53
  #define S2 50  // Seleção de filtro de cor
  #define S3 51
  #define OUT 22 // Saída do sensor

// === SENSOR DE COR DIREITO (TCS3200) ===
  #define S00 48
  #define S11 49
  #define S22 46
  #define S33 47
  #define OUTT 23

// === MOTORES COM PONTE H (L298N) ===
  // PWM motor esquerdo
  #define ENA 2     
  #define IN1 4
  #define IN2 3

  // PWM motor direito
  #define ENB 7 
  #define IN3 5
  #define IN4 6

void setup() {
  Serial.begin(9600);

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

  // === LEITURA DOS SENSORES DE COR ===

  // Sensor esquerdo
  int redE = readColor(S2, S3, OUT, LOW, LOW);
  int greenE = readColor(S2, S3, OUT, HIGH, HIGH);
  int blueE = readColor(S2, S3, OUT, LOW, HIGH);

  // Sensor direito
  int redD = readColor(S22, S33, OUTT, LOW, LOW);
  int greenD = readColor(S22, S33, OUTT, HIGH, HIGH);
  int blueD = readColor(S22, S33, OUTT, LOW, HIGH);
  // Identifica as cores detectadas
  String corE = identificarCor(redE, greenE, blueE);
  String corD = identificarCor(redD, greenD, blueD);

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


  // === LÓGICA DO SEGUIDOR DE LINHA ===

  if (corE == "Preto" && corD == "Preto") {
    motorFrente();
  } 
  else if (corE == "Preto" || corE == "Indefinido" && corD == "Branco") {
    motorEsquerda();
  } 
  else if (corE == "Branco" && corD == "Preto" || corD == "Indefinido") {
    motorDireita();
  } 
  else if (corE == "Branco" && corD == "Branco") {
    motorFrente();
  } 
  else if (corE != "Verde " && corD == "Verde"){
    motorDireita();
    delay(500);
  }
  else if (corE == "Verde" && corD != "Verde"){
    motorEsquerda();
    delay(500);
  }
  else if (corE == "Vermelho" && corD == "Vermelho"){
    pararMotores();
    delay(10000);
  }
  else{
    motorFrenteDevagar();
  }
}

// === FUNÇÃO PARA LER UMA COR COMBINANDO ESTADOS DE S2 E S3 ===
int readColor(int s2, int s3, int outPin, bool s2State, bool s3State) {

  digitalWrite(s2, s2State);
  digitalWrite(s3, s3State);
  delay(50); // Pequena pausa para estabilidade
  int tempo = pulseIn(outPin, LOW, 2500); // timeout de 25ms
  return tempo;

}

// === FUNÇÃO PARA IDENTIFICAR A COR ===
String identificarCor(int red, int green, int blue) {

  if (red <= 17 && green >= 19 && blue >= 15) {
    return "Vermelho";
  } 
  else if (red >= 18 && green >= 16 && blue < 20 ) {
    return "Preto";
  } 
  else if (blue <= green && green < red) {
    if (red < 10 && green < 10 && blue < 10){
      return "Branco";
    }
    else{
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
  Serial.println(">> Motores girando para FRENTE");

}
// Motor Esquerda
  void motorEsquerda() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 150);
    analogWrite(ENB, 250);
    Serial.println(">> Motores girando para a ESQUERDA");
  }

// Motor Direita
  void motorDireita() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, 250);
    analogWrite(ENB, 150);
    Serial.println(">> Motores girando para a DIREITA");
  }

// Parar Motor
  void pararMotores() {
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    Serial.println(">> Motores PARADOS");
  }

















