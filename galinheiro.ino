/**
***************************************************************************************** 
* Galinheiro IoT 
* A Intenção desse projeto é colocar automação de um Galinheiro, aproveitando peças de uma impressora 
* Contato para maiores detalhes: cezarantsouza@gmail.com ou ouvidoperfeito@gmail.com
* Fique a vontade pra criticar. Projeto para alinhar aprendizado. 
*/
// Bibliotecas 
#include <AFMotor.h>
//  Sensores de Controle
#define SENSOR_BALANCA       1 
#define SENSOR_DISTANCIA     2
#define SENSOR_FIM_DE_CURSO  3
#define MOTOR_CARRO          1  
AF_DCMotor motor(MOTOR_CARRO);
// Controladores de Quantidade de Tentativas
int QUANTIDADE_TENT_BALANCA   = 3; 
int QUANTIDADE_TENT_DISTANCIA = 3;
// Variaveis de Carro de Impressão
#define TEMPO_VOTAR_CARRO 5000 
#define TEMPO_PARA_LIBERAR_MILHO 800
// Contadores de Tentativas 
int contTentativasBalanca   = 0; 
int contTentativasDistancia = 0; 
// Posição do Carro de Impressão 
int posicaoAtual = 0; 
int posicaoFinal = 3; 
// Controladores de Fluxo
bool motorMilhoJaLigou   = false; 
bool milhoCaiu           = false;
bool carroAndou          = false;   
bool realizarTelemetria  = true;
bool circuitBreak        = false;
bool esperandoTimer      = false;     

void setup(){  
  Serial.begin(9600);
  motor.setSpeed(200);
  motor.run(RELEASE);
  moveCarroProInicio();
  //setupInicialFluxo();    
}

void moveCarroProInicio(){
  motor.setSpeed(255);
  // Volta o Carro até o Inicio
  motor.run(BACKWARD);
  for (int i=0;i< 4;i++){
      delay(TEMPO_VOTAR_CARRO);
      // Anda um pouco  
      motor.run(FORWARD);
      delay(TEMPO_PARA_LIBERAR_MILHO);
      // Para para soltar milho
      motor.run(RELEASE);  
  }
  delay(TEMPO_VOTAR_CARRO);
  // Volta o Carro até o Inicio
  motor.run(BACKWARD);  
}

void loop(){
  if((!circuitBreak) && (!esperandoTimer))
     fluxoPrincipal();
}

void setupInicialFluxo(){
  motorMilhoJaLigou = false; 
  milhoCaiu   = false; 
  carroAndou  = false;  
  telemetria("Setup Inicial do Fluxo", true);  
} 

void fluxoPrincipal(){
  // Liga a rosca para liberar o Milho
  if(!motorMilhoJaLigou)
      ligaMotorRosca();
   
  if ((motorMilhoJaLigou) && (!milhoCaiu))
      verificaSeMilhoCaiu();
  
  if ((motorMilhoJaLigou) && (milhoCaiu) && (!carroAndou))
      ligaMotorCarroImpressao();

   if ((motorMilhoJaLigou) && (milhoCaiu) && (!carroAndou))
       verificaCarroAndou();    

   if ((motorMilhoJaLigou) && (milhoCaiu) && (carroAndou) && (!chegouFimCurso()))
       setupInicialFluxo(); 
   else if((motorMilhoJaLigou) && (milhoCaiu) && (carroAndou) && (chegouFimCurso())) 
       religaTimer();                     
}

void telemetria(String mensagem, bool quebraLinha){
   if(realizarTelemetria){ 
      delay(2000);
      if(quebraLinha) 
        Serial.println(mensagem);
      else 
        Serial.print(mensagem);
   }
}
bool chegouFimCurso(){
  telemetria("Verifica Fim de Curso...", true);
  delay(500);
  return posicaoAtual == posicaoFinal; 
}
void verificaSeMilhoCaiu(){
  // Verifica se o milho caiu na balança 
  if ((recuperaValorSensor(SENSOR_BALANCA) == 0) && (contTentativasBalanca < 3)){      
      contTentativasBalanca++;        
      telemetria("Tentativa de ler sensor de milho frustrada...", false);
      telemetria(String(contTentativasBalanca), true);
  }else if(recuperaValorSensor(SENSOR_BALANCA) == 1){
      telemetria("Milho Caiu...",true);
      milhoCaiu = true; 
  }else if(contTentativasBalanca == 3){
      paraTudo();
  } 
}
void verificaCarroAndou(){
  // Verifica se o milho caiu na balança 
  if ((recuperaValorSensor(SENSOR_DISTANCIA) == 0) && (contTentativasDistancia < 3)){      
      contTentativasDistancia++;        
      telemetria("Tentativa de ler sensor de Carro frustrada...", false);
      telemetria(contTentativasBalanca + "",true);
  }else if(recuperaValorSensor(SENSOR_DISTANCIA) == 1){
      telemetria("Carro andou...", true);
      carroAndou = true;
      telemetria("Distancia:", false); 
      telemetria(String(posicaoAtual), true);
      posicaoAtual++; 
  }else if(contTentativasDistancia == 3){
      paraTudo();
  } 
}
void ligaMotorRosca(){
  // Codigo que liga o motor e dá um delay. 
  motorMilhoJaLigou=true;
  telemetria("Motor de Rosca Ligado...", true);
}
int recuperaValorSensor(int CODIGOSENSOR){
  switch(CODIGOSENSOR){
     case SENSOR_BALANCA: 
          return 1;
     break; 
     case SENSOR_DISTANCIA: 
          return 1;
     break;
     default: 
          return 0;   
     break;     
  }  
}
void paraTudo(){ 
  telemetria("Circuit Break!!", true);
  circuitBreak = true; 
}
void religaTimer(){
  esperandoTimer = true;
  telemetria("Religando Timer...Fim de papo", true); 
}
void ligaMotorCarroImpressao(){
  // Liga o motor do carro de Impressão
  telemetria("Ligando Motor de Carro de Impressão...", true);
}
