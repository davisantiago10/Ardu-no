#define vermelho 8
#define botao 7

void setup() {
  pinMode(vermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
}

void loop() {
  int estado = digitalRead(botao); // HIGH ou LOW
  if(estado == LOW){
    digitalWrite(vermelho, HIGH);
  }
  else{
    digitalWrite(vermelho, LOW);
  }
}
