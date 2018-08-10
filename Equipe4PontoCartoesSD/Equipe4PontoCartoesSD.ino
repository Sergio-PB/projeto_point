#include <Wire.h>
#include <Wtv020sd16p.h>

int resetPin = 4;  //Pino Reset
int clockPin = 5;  //Pino clock
int dataPin = 6;   //Pino data (DI)
int busyPin = 7;   //Pino busy

Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

// Declaracao das variaveis
int tempo;
int unidade, dezena;

bool toca;

int erro;
int linha;

void setup() {
  Serial.begin(9600);
  Wire.begin(4);
  Wire.onReceive(recebeDado);
  wtv020sd16p.reset();
  
}

void loop() {
  if(toca == true){ // Responsável por reproduzir de forma sonora os tempos das linhas quando digitadas
    som();
    toca = false;
  }
}

void recebeDado(int howMany){
  int ordem;
  ordem = Wire.read();
  if(ordem == 1){ //probelma -> gravar no log
    erro = Wire.read();
    gravar(erro);
  }
  
  if(ordem == 2){ //teclado -> reproduzir audio
    toca = true;
    tempo = Wire.read();
    linha = Wire.read();
    Serial.println(linha, DEC);
    Serial.println(tempo, DEC);
    if (tempo != 99) {
      // Organiza o tempo para poder ser reproduzido pelo módulo wtv020sd
      converteTempo(tempo);
    }
  }
}

void som(){
  if (tempo == 99){         //////////////////////Nao Passa por este ponto
    Serial.print(linha/100);
    Serial.print((linha%100)-(linha%10));
    Serial.println(linha%10);
    
    // Audio de inicializacao
    wtv020sd16p.playVoice(0);

    wtv020sd16p.asyncPlayVoice(3333); //A linha
    delay(1000);

    wtv020sd16p.asyncPlayVoice(linha/100); //1
    delay(1000);

    wtv020sd16p.asyncPlayVoice((linha%100)-(linha%10)); //0
    delay(1000);

    wtv020sd16p.asyncPlayVoice(linha%10); //3 ou 4
    delay(1000);
    
    
    // Reproduz o arquivo 6666, responsavel pelo audio de invalidez "nao passsa por este ponto"
    wtv020sd16p.asyncPlayVoice(6666);
    delay(4000);

    // Para os audios
    wtv020sd16p.stopVoice();

    
  }else if(tempo>79){   /////////////////////// Nao possuimos info
    wtv020sd16p.playVoice(0);
    delay(100);

    // Reproduz o arquivo 4444, responsavel pelo audio de falta de informacao sobre a linha
    wtv020sd16p.asyncPlayVoice(4444);
    delay(5500);

    Serial.print(linha/100);
    wtv020sd16p.asyncPlayVoice(linha/100); //1
    delay(1000);

    Serial.print((linha%100)-(linha%10));
    wtv020sd16p.asyncPlayVoice((linha%100)-(linha%10)); //0
    delay(1000);

    Serial.print(linha%10);
    wtv020sd16p.asyncPlayVoice(linha%10); //3 ou 4
    delay(1000);

    wtv020sd16p.stopVoice();
    
  }else if(tempo != 99 && tempo<100){ ///////////////////////Normal
    wtv020sd16p.playVoice(0);

    // Audio auxiliar para inicializacao
    wtv020sd16p.asyncPlayVoice(1);
    delay(1000);
    
    // Reproduz o arquivo 9999, responsavel pelo audio de "faltam"
    wtv020sd16p.asyncPlayVoice(9999);
    delay(1000);
   
    // Muda para os arquivos respectivos a dezena do tempo atual
    wtv020sd16p.asyncPlayVoice(dezena);
    delay(1500);
    
    // Muda para os arquivos respectivos a unidade do tempo atual
    wtv020sd16p.asyncPlayVoice(unidade);
    delay(1000);
    
    // Reproduz o arquivo 8888, responsavel pelo audio de "segundos para a linha"
    wtv020sd16p.asyncPlayVoice(8888);
    delay(2500);

    

    wtv020sd16p.asyncPlayVoice(linha/100); //1
    delay(1000);

    wtv020sd16p.asyncPlayVoice((linha%100)-(linha%10)); //0
    delay(1000);

    wtv020sd16p.asyncPlayVoice(linha%10); //3 ou 4
    delay(1000);

    wtv020sd16p.asyncPlayVoice(7777); //Chegar
    delay(1000);

    
      
    // Para audios
    wtv020sd16p.stopVoice();
  }  
}

void converteTempo(int tempo){
  unidade = 0000 + (tempo % 10);
  dezena = 0000 + (tempo - unidade);
}

void gravar(int erro){
  Serial.print(" Ponto: ");
  Serial.print((erro/100)%10); // Organiza o "print" para mostrar em ordem: ponto/carro/erro
  Serial.print(" Carro: ");
  Serial.print((erro/10) % 10);
  Serial.print(" Tipo de Erro: ");
  Serial.println(erro%10);
}
