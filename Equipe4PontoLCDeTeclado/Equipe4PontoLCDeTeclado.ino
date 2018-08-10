//#include <SoftwareSerial.h>
//SoftwareSerial BTserial(0, 1);

#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define buzzer A1

// Declaracao dos pinos

const byte ROWS = 4;
const byte COLS = 3;
char teclas[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {7, 6, 5, 4 };
byte colPins[COLS] = {3, 2, A0};

Keypad teclado(makeKeymap(teclas), rowPins, colPins, ROWS, COLS);

// Variaveis

char linhaDigitada[4];
byte contTecl = 0;
char tecla;

int tempo104 = 0, tempo103 = 0;

int botao = 3;
bool invalido;

int erro;
int tempo = 0;

bool app = false;
int l = 0, p = 0;

void setup() {
  lcd.begin(20, 4);
  Serial.begin(9600);
  //BTserial.begin(9600);
  Wire.begin();
}

void loop() {
  pedeTempos(); // Comunicacao com outros controladores
  imprime(103, tempo103, 0); // Print
  imprime(104, tempo104, 2);  // Print
  TECL(); // Funcao onde eh analisada a informacao inserida no teclado matricial
}

void pedeTempos() {
  Wire.requestFrom(2, 3);
  tempo104 = Wire.read(); // Recebe dados sobre p tempo da linha 104
  tempo103 = Wire.read(); // Recebe dados sobre o tempo da linha 103
  erro = Wire.read();
  if (erro != 0) {
    enviaErro(erro);
  }
}

void imprime(int linha, int tempo, int row) {
  lcd.setCursor(0, row);
  lcd.print("Linha ");
  lcd.print(linha);
  lcd.print(" chega em:");
  lcd.setCursor(0, row + 1);
  if (tempo < 100) {
    lcd.print(tempo);
    lcd.print(" segundos            ");
  } else {
    lcd.print("Sem tempo disponivel");
  }

}

void TECL() {
  tecla = teclado.getKey();
  if (tecla) {
    tone(buzzer, 650, 200); // Reproduz um som sempre que alguma tecla eh pressionada

    linhaDigitada[contTecl] = tecla; // Armazena quantidade de teclas pressionadas no array
    contTecl++; // Incrementa variavel em 1 para armazenar novo char
  }

  if (contTecl == 3) { // Se foram pressionadas 3 teclas
    if (strcmp(linhaDigitada, "103") == 0) { // Se as teclas pressionadas representam "103"
      reproduzTempo(tempo103, 103);
    }
    else if (strcmp(linhaDigitada, "104") == 0) { // Se as teclas pressionadas representam "104"
      reproduzTempo(tempo104, 104);
    }
    else {
      int c = linhaDigitada[0]-'0';
      int d = linhaDigitada[1]-'0';
      int u = linhaDigitada[2]-'0';
      int n = (c*100)+(d*10)+(u);
      Serial.println (n);
      reproduzTempo(0, n); // Se nao foram pressionadas teclas referentes as linhas existentes
    }
    delay(500);
    clearData();
  }
}

void clearData() {
  while (contTecl != 0)
  {
    linhaDigitada[contTecl--] = 0; // "Limpa" o array para novas interacoes
  }
  return;
}

void reproduzTempo(int tempo, int linha) {
  if (tempo != 0) { // Se as linhas inseridas forem referentes as existentes
    Wire.beginTransmission(4);
    Wire.write(2); // Comunica com o outro controlador
    Wire.write(tempo); // O tempo da data de chegada
    Wire.write(linha);
    Wire.endTransmission();
  }
  else { // Se as linhas inseridas nao existem
    Wire.beginTransmission(4);
    Wire.write(2); // Comunica com o outro controlador
    Serial.println("mandando 6666");
    Wire.write(99); // Para reproduzir o audio de linha invalida
    Wire.write(linha);
    Wire.endTransmission();
  }
}

void enviaErro(int erro) {
  Wire.beginTransmission(4);
  Wire.write(1);
  Wire.write(erro);
  Wire.endTransmission();
}

