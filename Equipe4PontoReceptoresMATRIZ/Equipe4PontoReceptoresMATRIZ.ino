#include <Wire.h>

const int qntCarros = 3;
int carros[qntCarros] = {1041, 1042, 1031};

int pontos[4][qntCarros]= { //Matriz para os pinos digitais
  { 3,  4, 5},
  { 6,  7, 8},
  { 0,  0, 9},
  {10, 11, 0}
};
bool f = false;

int posicoes[qntCarros] = {0, 2, 1};

const int qntPontos = 3;
int trajeto104[qntPontos] = {36, 27, 27};
int trajeto103[qntPontos] = {38, 18, 16};
int adiantadoRes, atrasadoRes;

const int qntErros = 3;
int errosAd[qntPontos][qntCarros][qntErros]= {
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
};

int errosAt[qntPontos][qntCarros][qntErros]= {
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
  {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
};

int to104;
int tRestante104, tRestante103;
int tNecessario104, tNecessario103;
int tempos[qntCarros] = {0, 0, 0};
int erro;
int erros3[qntCarros] = {0, 0, 0};

int margem = 5;

bool chamouApp = false;
int l = 0, p = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onRequest(enviaTempo);
  Wire.onReceive(app);
  //Habilita resistor interno do Arduino para utilizar o botoes
  for(int i = 0; i<4; i++){
    for(int j = 0; j<qntCarros; j++){
      int p = pontos[i][j];
      if(p != 0){
        pinMode(p, INPUT);
        digitalWrite(p, HIGH);
      }
    }
  }

  tNecessario103 = somaTempos(0, posicoes[2], trajeto103);
}

void loop() {
  for(int i = 0; i<4; i++){ //Verifica cada PONTO
    for(int j = 0; j<qntCarros; j++){ //Verifica cada CARRO
      if(pontos[i][j] != 0 && digitalRead(pontos[i][j]) == LOW){
        if(i == 3){
          i = 2;
          f = true;
        }
        if(j == 2){
          adiantadoRes = adiantado(i, j, trajeto103);
          atrasadoRes  = atrasado(i, j, trajeto103);
        }else{
          adiantadoRes = adiantado(i, j, trajeto104);
          atrasadoRes  = atrasado(i, j, trajeto104);
        }
        if(adiantadoRes != 0){
          erro = 001 + (j*10) + (i*100);
          adiantadoRes = 0;
          if (not f){
            botaoSegurado(pontos[i][j]);
          }else{
            botaoSegurado(pontos[3][j]);
            f = false;
          }
          break;
        }
        if(atrasadoRes != 0){
          erro = 002 + (j*10) + (i*100);
        }

        posicoes[j] = i;
        tempos[j] = millis()/1000;

        if(i == 0 && (j == 0 || j == 1)){
          tNecessario104 = somaTempos(0, posicoes[(j+1) % 2], trajeto104);
          to104 = tempos[(j+1) % 2];
        }else if(j == 2){
          tNecessario103 = somaTempos(0, i, trajeto103);
        }else if (not (alguemFrente(j))){
          tNecessario104 = somaTempos(0, i, trajeto104);
          to104 = tempos[j];
        }
        if (not f){
          botaoSegurado(pontos[i][j]);
        }else{
          botaoSegurado(pontos[3][j]);
          f = false;
        }
        
          
      }
    }
  }
  naoPassou(); //Checa por leituras que deviam ter ocorrido e nao ocorreram
}
void app(int howMany){
  int  x = Wire.read();
  if(x == 5){
    chamouApp = true;
    l = Wire.read();
    p = Wire.read();
    x = 0;
  }
}

void botaoSegurado(int botao){ //Impede multiplas leituras no sensor
  while(digitalRead(botao) == LOW){
    ;
  }
  delay(50);
  while(digitalRead(botao) == LOW){
    ;
  }
}

void naoPassou(){ //Verificacao para caso leitura nao ocorra em determinado ponto
  int tMax;
  for(int n = 0; n<qntCarros; n++){
    if(n==0 || n==1){
      tMax = trajeto104[posicoes[n]] + margem;
    }else{
      tMax = trajeto103[posicoes[n]] + margem;
    }
    if (erros3[n] == 0 && (int(millis()/1000) - tempos[n]) > tMax){
      erro = 003 + (n*10) + (posicoes[n]*100);
      erros3[n] = 1;
    }
    if ((int(millis()/1000) - tempos[n]) < tMax){
      erros3[n] = 0;
    }
  }
}

void guardaErros(int ponto, int carro, int matriz[][qntCarros][3], int valor){
  int i = 0;
  int p = 0;
  for (; i<3; i++){
    if(matriz[ponto][carro][i] == 0){
      break;
    }
  }
  matriz[ponto][carro][i] = valor;
  if(i==2){
    if (ponto - 1 < 0){
      p = 2;
    }else{
      p = ponto-1;
    }
    
    if(carro == 2){
      trajeto103[p] = trajeto103[p]+((matriz[ponto][carro][0]+matriz[ponto][carro][1]+matriz[ponto][carro][2])/3);
    }else{
      trajeto104[p] = trajeto104[p]+((matriz[ponto][carro][0]+matriz[ponto][carro][1]+matriz[ponto][carro][2])/3);
    }
    limpaErros(ponto, carro, matriz);
  }
}

void limpaErros(int ponto, int carro, int matriz[][qntCarros][3]){
  for(int i = 0; i<3; i++){
    matriz[ponto][carro][i] = 0;
  }
}

int adiantado(int ponto, int carro, int trajeto[]){ //Verificacao contra leituras adiantadas
  int v = (int(millis()/1000)-tempos[carro]) - somaTempos(ponto, posicoes[carro], trajeto);
//  Serial.println(v);
  if (v < -(margem) ){
    guardaErros(ponto, carro, errosAd, v);
    return 1;
  }
  limpaErros(ponto, carro, errosAd);
  return 0;
}

int atrasado(int ponto, int carro, int trajeto[]){ //Verificacao para leituras atrasadas
  int v = (int(millis()/1000)-tempos[carro]) - somaTempos(ponto, posicoes[carro], trajeto);
  if ( v > (margem)){
    guardaErros(ponto, carro, errosAt, v);
    return 2;
  }
  limpaErros(ponto, carro, errosAt);
  return 0;
}

bool alguemFrente(int carro){ //Verificacao para linha 104, o tempo a ser mostrado eh do carro mais a frente
  int outro = (carro + 1) % 2;
  return posicoes[carro] <= posicoes[outro];
}

int somaTempos(int pFinal, int pInicial, int trajeto[]){ //Somatorio dos tempos necessarios de pInicial ate pFinal em determinada linha
  int soma = 0;
  if(pFinal == pInicial){
    for(int i=0;i<qntPontos;i++){
      soma = soma + trajeto[i];
    }
  }
  for(; pFinal!=pInicial;){
    soma = soma + trajeto[pInicial];
    pInicial = (pInicial+1)%qntPontos;
  }
  return soma;
}

void enviaTempo(){ //Envia ao mestre os tempos a serem mostrados
  if(chamouApp){
    if(l == 103){
      Wire.write(somaTempos(p, posicoes[2], trajeto103)-(millis()/1000) + tempos[2]);
    }else{
      int t0 = somaTempos(p, posicoes[0], trajeto104) - (millis()/1000) + tempos[0];
      int t1 = somaTempos(p, posicoes[1], trajeto104) - (millis()/1000) + tempos[1];
      Wire.write(min(t0,t1));
    }
    chamouApp = false;
  }else{
    int tempo104 = tNecessario104 -(millis()/1000) + to104;
    int tempo103 = tNecessario103 -(millis()/1000) + tempos[2];

    Wire.write(tempo104);
    Wire.write(tempo103);

    Wire.write(erro);
    erro = 0;
  }
}
