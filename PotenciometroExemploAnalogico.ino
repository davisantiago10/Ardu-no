#define vermelho 10
const int giroscopio = A0;

void setup() {
  pinMode(vermelho, OUTPUT);
  pinMode(giroscopio, INPUT);
}

void loop() {
  int grau = analogRead(giroscopio);
  int brilho = map(grau, 0, 1023, 0, 180);
  analogWrite(vermelho, brilho);
}
