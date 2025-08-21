// === SENSOR DE COR ESQUERDO (TCS3200) ===
#define S0 23
#define S1 22
#define S2 25
#define S3 24
#define OUT 27

// === SENSOR DE COR DIREITO (TCS3200) ===
#define S00 50
#define S11 51
#define S22 52
#define S33 53
#define OUTT 49

// === MOTORES COM PONTE H (L298N) ===
#define ENA 2     // PWM motor esquerdo
#define IN1 4
#define IN2 3
#define ENB 7     // PWM motor direito
#define IN3 5
#define IN4 6

// Sensor ultrassônico
#define echo 41
#define trig 39

// Valores calibrados - ATUALIZE COM SEUS VALORES REAIS!
struct ColorCalibration {
  int white[3];
  int black[3];
  int green[3];
  int red[3];
};

ColorCalibration calLeft = {
  {5, 5, 5},       // BRANCO_E
  {15, 14, 11},    // PRETO_E
  {16, 10, 9},     // VERDE_E
  {22, 10, 8}      // VERM_E
};

ColorCalibration calRight = {
  {5, 5, 5},       // BRANCO_D
  {16, 15, 11},    // PRETO_D
  {16, 11, 9},     // VERDE_D
  {26, 12, 10}     // VERM_D
};

// Limiares ajustados
const int TH_WHITE_MAX = 10;
const int TH_BLACK_MIN = 12;
const int GREEN_DOMINANCE_THRESHOLD = 2;
const int BLACK_MAX_RGB = 18;

// Variáveis globais
String corE;
String corD;
int distancia;

// Filtro para suavizar leituras
const int FILTER_SIZE = 5;
int leftReadings[FILTER_SIZE][3] = {0};
int rightReadings[FILTER_SIZE][3] = {0};
int readingIndex = 0;

void setup() {
  Serial.begin(9600);

  // Sensor ultrassônico
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Sensores de cor - ESQUERDO
  pinMode(S0, OUTPUT);  
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);  
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  // Sensores de cor - DIREITO
  pinMode(S00, OUTPUT); 
  pinMode(S11, OUTPUT);
  pinMode(S22, OUTPUT);
  pinMode(S33, OUTPUT);
  pinMode(OUTT, INPUT);

  // Motores
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configura sensores para 100% da frequência
  digitalWrite(S0, HIGH);  
  digitalWrite(S1, HIGH);
  digitalWrite(S00, HIGH); 
  digitalWrite(S11, HIGH);

  Serial.println("Sistema Iniciado: sensores de cor + motores...");
}

void loop() {
  distancia = detectaDistancia();
  if (distancia < 15) {
    desviaObjetos();
  } else {
    leituraExibicao();   // Lê sensores e atualiza corE e corD
    seguidorDeLinha();   // Executa a lógica do seguidor de linha
  }
  delay(10); // Reduzido para melhor resposta
}

// ================== FUNÇÕES DE DETECÇÃO DE COR (APRIMORADAS) ==================
void applyFilter(int sensor, int r, int g, int b, int& filteredR, int& filteredG, int& filteredB) {
  if (sensor == 0) {
    leftReadings[readingIndex][0] = r;
    leftReadings[readingIndex][1] = g;
    leftReadings[readingIndex][2] = b;
    
    filteredR = filteredG = filteredB = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
      filteredR += leftReadings[i][0];
      filteredG += leftReadings[i][1];
      filteredB += leftReadings[i][2];
    }
    filteredR /= FILTER_SIZE;
    filteredG /= FILTER_SIZE;
    filteredB /= FILTER_SIZE;
  } else {
    rightReadings[readingIndex][0] = r;
    rightReadings[readingIndex][1] = g;
    rightReadings[readingIndex][2] = b;
    
    filteredR = filteredG = filteredB = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
      filteredR += rightReadings[i][0];
      filteredG += rightReadings[i][1];
      filteredB += rightReadings[i][2];
    }
    filteredR /= FILTER_SIZE;
    filteredG /= FILTER_SIZE;
    filteredB /= FILTER_SIZE;
  }
  
  readingIndex = (readingIndex + 1) % FILTER_SIZE;
}

int readColor(int s2, int s3, int outPin, bool s2State, bool s3State) {
  digitalWrite(s2, s2State);
  digitalWrite(s3, s3State);
  delayMicroseconds(200);
  return pulseIn(outPin, LOW, 20000);
}

int manhattanDistance(int r, int g, int b, const int ref[3]) {
  return abs(r - ref[0]) + abs(g - ref[1]) + abs(b - ref[2]);
}

String identificarCorCalibrada(uint8_t sensorId, int r, int g, int b) {
  const ColorCalibration& cal = (sensorId == 0) ? calLeft : calRight;
  
  // Regras rápidas e robustas
  if (r < TH_WHITE_MAX && g < TH_WHITE_MAX && b < TH_WHITE_MAX) {
    return "Branco";
  }
  
  // Regra melhorada para preto
  if (r > TH_BLACK_MIN && g > TH_BLACK_MIN && b > TH_BLACK_MIN && 
      r < BLACK_MAX_RGB && g < BLACK_MAX_RGB && b < BLACK_MAX_RGB) {
    int dBlack = manhattanDistance(r, g, b, cal.black);
    int dGreen = manhattanDistance(r, g, b, cal.green);
    if (dBlack < dGreen) return "Preto";
  }
  
  // Regra para verde
  if (g > r + GREEN_DOMINANCE_THRESHOLD && g > b + GREEN_DOMINANCE_THRESHOLD) {
    int dGreen = manhattanDistance(r, g, b, cal.green);
    int dRed = manhattanDistance(r, g, b, cal.red);
    int dBlack = manhattanDistance(r, g, b, cal.black);
    if (dGreen < dRed && dGreen < dBlack) return "Verde";
  }
  
  // Fallback para distância de Manhattan
  int dWhite = manhattanDistance(r, g, b, cal.white);
  int dBlack = manhattanDistance(r, g, b, cal.black);
  int dGreen = manhattanDistance(r, g, b, cal.green);
  int dRed = manhattanDistance(r, g, b, cal.red);
  
  int minDist = dWhite;
  String color = "Branco";
  
  if (dBlack < minDist) { minDist = dBlack; color = "Preto"; }
  if (dGreen < minDist) { minDist = dGreen; color = "Verde"; }
  if (dRed < minDist) { minDist = dRed; color = "Vermelho"; }
  
  // Verificação de confiança
  const int CONFIDENCE_WHITE = 15;
  const int CONFIDENCE_BLACK = 20;
  const int CONFIDENCE_GREEN = 25;
  const int CONFIDENCE_RED = 25;
  
  bool confident = true;
  if (color == "Branco") confident = (minDist <= CONFIDENCE_WHITE);
  else if (color == "Preto") confident = (minDist <= CONFIDENCE_BLACK);
  else if (color == "Verde") confident = (minDist <= CONFIDENCE_GREEN);
  else if (color == "Vermelho") confident = (minDist <= CONFIDENCE_RED);
  
  return confident ? color : "Indefinido";
}

void leituraExibicao() {
  // Ler valores brutos
  int rLeft = readColor(S2, S3, OUT, LOW, LOW);
  int gLeft = readColor(S2, S3, OUT, HIGH, HIGH);
  int bLeft = readColor(S2, S3, OUT, LOW, HIGH);
  
  int rRight = readColor(S22, S33, OUTT, LOW, LOW);
  int gRight = readColor(S22, S33, OUTT, HIGH, HIGH);
  int bRight = readColor(S22, S33, OUTT, LOW, HIGH);
  
  // Aplicar filtro
  int filteredRLeft, filteredGLeft, filteredBLeft;
  int filteredRRight, filteredGRight, filteredBRight;
  
  applyFilter(0, rLeft, gLeft, bLeft, filteredRLeft, filteredGLeft, filteredBLeft);
  applyFilter(1, rRight, gRight, bRight, filteredRRight, filteredGRight, filteredBRight);
  
  // Identificar cores com valores filtrados
  corE = identificarCorCalibrada(0, filteredRLeft, filteredGLeft, filteredBLeft);
  corD = identificarCorCalibrada(1, filteredRRight, filteredGRight, filteredBRight);
  
  // Exibir resultados (opcional - pode comentar para melhor performance)
  Serial.println(">> Cor detectada:");
  Serial.print("[ESQUERDA] R: "); Serial.print(filteredRLeft);
  Serial.print(" | G: "); Serial.print(filteredGLeft);
  Serial.print(" | B: "); Serial.print(filteredBLeft);
  Serial.print("  --> Cor: "); Serial.println(corE);
  
  Serial.print("[DIREITA]  R: "); Serial.print(filteredRRight);
  Serial.print(" | G: "); Serial.print(filteredGRight);
  Serial.print(" | B: "); Serial.print(filteredBRight);
  Serial.print("  --> Cor: "); Serial.println(corD);
  Serial.println("-------------------");
}

// ================== FUNÇÕES DE MOVIMENTO (MANTIDAS) ==================
int detectaDistancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH, 30000);
  if (duracao == 0) return 999;

  int d = duracao / 58;
  return d;
}

void desviaObjetos() {
  parado();
  delay(500);
  motorTras();
  delay(500);
  motorEsquerda();
  delay(900);
  motorFrente();
  delay(2500);
  motorDireitaSuave();
  delay(700);
  motorFrente();
  delay(1000);
  motorDireitaSuave();
  delay(1500);
  
  while(corD != "Preto") {
    motorFrente();
    leituraExibicao();
  }
}

void seguidorDeLinha() {
  if (corE == "Preto" && corD == "Preto") {
    motorFrente();
  } else if (corE == "Preto" && corD == "Branco") {
    motorEsquerda();
  } else if (corE == "Branco" && corD == "Preto") {
    motorDireita();
  } else if (corE == "Branco" && corD == "Branco") {
    motorFrente();
  } else if (corE == "Verde" && corD != "Verde") {
    motorEsquerda();
    delay(100);
    leituraExibicao();
    if(corE == "Verde") {
      giraVerdeEsquerda();
    } else {
      motorFrente();
    }
  } else if (corE != "Verde" && corD == "Verde") {
    motorDireita();
    delay(100);
    leituraExibicao();
    if(corD == "Verde") {
      giraVerdeDireita();
    } else {
      motorFrente();
    }
  } else if (corE == "Vermelho" && corD == "Vermelho") {
    parado();
    delay(7000);
  } else {
    motorFrenteDevagar();
  }
}

void motorFrente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void motorFrenteDevagar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 100);
  analogWrite(ENB, 100);
  delay(20);
}

void motorEsquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 180);
  analogWrite(ENB, 250);
}

void motorDireita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 250);
  analogWrite(ENB, 180);
}

void motorDireitaSuave() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 250);
  analogWrite(ENB, 140);
}

void giraVerdeEsquerda() {
  motorFrente();
  delay(300);
  motorEsquerda();
  while (corE != "Preto") {
    motorEsquerda();
    leituraExibicao();
  }
  parado();
  delay(100);
}

void giraVerdeDireita() {
  motorFrente();
  delay(300);
  motorDireita();
  while (corD != "Preto") {
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
}

void parado() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
