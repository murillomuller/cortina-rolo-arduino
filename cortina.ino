#include <SoftwareSerial.h>
SoftwareSerial DT06(9, 8);
char serialdt06;

#include <IRremote.h>

//PINAGEM DO MOTOR
#define MOTOR1 12
#define MOTOR2 11 


#define SENSORDECIMA 5
#define SENSORDEBAIXO 6


#define BOTAODEBAIXO 3
#define BOTAODECIMA 4

int estadobotaocima;
int ultimoestadobotaocima;


int estadobotaobaixo;
int ultimoestadobotaobaixo;

int temposubida;
int tempodescida;
int tempodadescida = 90;
int tempodasubida = 147;
int posicaodacortina;

int RECV_PIN = 10; //Ligado ao pino do receptor IR (pino S) 
float armazenavalorcomando; 


IRrecv irrecv(RECV_PIN);  
decode_results results; 

void setup() {

  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);

  pinMode(SENSORDECIMA, INPUT); //SE 1 MANDAR PARAR
  pinMode(SENSORDEBAIXO, INPUT); //SE 0 MANDAR PARAR
  digitalWrite(SENSORDEBAIXO, HIGH);
  digitalWrite(SENSORDECIMA, HIGH);
  
  pinMode(BOTAODEBAIXO, INPUT_PULLUP); //DEFINE O PINO COMO ENTRADA
  pinMode(BOTAODECIMA, INPUT_PULLUP); //DEFINE O PINO COMO ENTRADA

 

  Serial.begin(9600); //Inicializa a serial  

  DT06.begin(9600);
  DT06.println("Teste wifi");
  
  irrecv.enableIRIn(); // Inicializa o receptor IR  
}

void loop() {
  if (DT06.available())
  {
    serialdt06=(DT06.read());
    if (serialdt06=='1')
    {
      
      subirCortina();
      DT06.println("Subindo cortina");
    }
    if (serialdt06=='2')
    {
      descerCortina();
      DT06.println("Descendo cortina");
    }
    if (serialdt06=='3')
    {
      pararCortina();
      DT06.println("Parar cortina");
    }
  }
  
if(digitalRead(SENSORDEBAIXO) == 0) {
    if(statusMotor() != 2 && armazenavalorcomando != 0x511DBB && digitalRead(BOTAODECIMA) != 0 && serialdt06 == '2') {
      pararCortina();
    }
    if(armazenavalorcomando == 0x511DBB) {
      subirCortina();
    }
    
}

if(digitalRead(SENSORDECIMA) == 0) {
    if(statusMotor() != 2 && armazenavalorcomando != 0xA3C8EDDB && digitalRead(BOTAODEBAIXO) != 0 && serialdt06 == '1') {
      pararCortina();
    }
    if(armazenavalorcomando == 0xA3C8EDDB) {
      descerCortina();
    }
}
  
if (irrecv.decode(&results))  {  
    Serial.print("Valor lido : ");  
    Serial.println(results.value, HEX); 
    if(results.value != 0xFFFFFFFF) {
    armazenavalorcomando = (results.value);
    }
    if(results.value == 0xA3C8EDDB) {
      serialdt06 = '2';
      descerCortina();
    }
    if(results.value == 0x511DBB) {
      serialdt06 = '1';
      subirCortina();
    }
    if(results.value == 0xD7E84B1B) {
      if(statusMotor() != 2) {
       serialdt06 = '3';
      pararCortina();
      }
    }
    irrecv.resume();
}
  
  
 estadobotaocima = digitalRead(BOTAODECIMA);
 if(estadobotaocima != ultimoestadobotaocima) {
   if(digitalRead(BOTAODECIMA) == 0 && digitalRead(SENSORDECIMA) != 0) {
    if(statusMotor() == 2) {
      serialdt06 = '1';
      subirCortina();
      }
    else {
      pararCortina();
    }
    }
  ultimoestadobotaocima = estadobotaocima;
}

estadobotaobaixo = digitalRead(BOTAODEBAIXO);
 if(estadobotaobaixo != ultimoestadobotaobaixo) {
   if(digitalRead(BOTAODEBAIXO) == 0 && digitalRead(SENSORDEBAIXO) != 0) {
    if(statusMotor() == 2) {
      serialdt06 = '2';
      descerCortina();
     }
    else {
      pararCortina();
    }
   }
  ultimoestadobotaobaixo = estadobotaobaixo;
  }
  posicaodaCortina();
}


void posicaodaCortina() {
  if(digitalRead(MOTOR1) == 1 && digitalRead(MOTOR2) == 0) {
    tempodescida++;
    temposubida = 0;
  }
  if(digitalRead(MOTOR1) == 0 && digitalRead(MOTOR2) == 1) {
    temposubida++;
    tempodescida = 0;
  }
  if(digitalRead(SENSORDECIMA) == 0) {
    posicaodacortina = 0;
    tempodescida = 0;
  }
  if(digitalRead(SENSORDEBAIXO) == 0) {
    posicaodacortina = 100;
    temposubida = 0;
  }
  if(temposubida != 0 && tempodescida == 0 && digitalRead(SENSORDECIMA) != 0) {
    
   posicaodacortina = (100-(100*temposubida)/tempodasubida);
    
  }
  if(tempodescida != 0 && temposubida == 0 && digitalRead(SENSORDEBAIXO) != 0) {
    
   posicaodacortina = (100*tempodescida)/tempodadescida;
    
  }
}


void descerCortina() {
  digitalWrite(MOTOR1, 1);
  digitalWrite(MOTOR2, 0);
}
void subirCortina() {
  digitalWrite(MOTOR1, 0);
  digitalWrite(MOTOR2, 1);
  }

void pararCortina() {
  digitalWrite(MOTOR1, 0);
  digitalWrite(MOTOR2, 0);
}


int statusMotor() {
  if(digitalRead(MOTOR1) == 0 && digitalRead(MOTOR2) == 1) {
    //CORTINA SUBINDO//ABRINDO
    return 1;
}
else if (digitalRead(MOTOR1) == 1 && digitalRead(MOTOR2) == 0) {
  //CORTINA DESCENDO/FECHANDO
  return 0;
}
else if (digitalRead(MOTOR1) == 0 && digitalRead(MOTOR2) == 0) {
  //CORTINA PARADA
  return 2;
}
}
