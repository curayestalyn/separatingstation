#include "include.h"
#include <SPI.h>    // Librería línea serie (Serial Peripherical Interface)
#include <Controllino.h>    // Librería para Controllino

#define MAX_ETAPAK 14
#define MAX_TRANTSIZIOAK 17
#define MAX_PIEZAK_AURREMANTENIMENDUA 10
#define MAX_ARRISKUA_MANTENIMENDUA  15



// == ALDAGAI GLOBALAK
char etapak[MAX_ETAPAK];
char trantsizioak[MAX_TRANTSIZIOAK];
IRTEERAK irteerak;
unsigned long timeHasiera = 0;
unsigned long timeAmaiera = 0;
int segundoak = 0;
int denboraBurututa = 0;
int kontagailua = 0;
int reseta = 0;

// DECLARAR ENTRADAS

void PORTUAK_sarreraPortuakHasieratu()
{
  pinMode(Part_AV, INPUT);    // Pieza en puesto de entrada
  pinMode(B_2, INPUT);    // Pieza en detenedor
  pinMode(B_3, INPUT);    // Pieza de tipo "housing"
  pinMode(B_4, INPUT);    // Switch retrasado
  pinMode(B_5, INPUT);    // No hay pieza en cinta 2
  pinMode(B_6, INPUT);    // No hay pieza en pick-up

  pinMode(S_1, INPUT);    // Pulsador de inicio
  pinMode(S_2, INPUT);    // Pulsador de parada (NC)
  pinMode(S_3, INPUT);    // Selector manual/automático
  pinMode(S_4, INPUT);    // Pulsador de reset
  pinMode(Em_Stop, INPUT);    // Seta de emergencia (NC)

  // investigar
  //pinMode(IP_F1_B, INPUT);  // Estación lateral B no presente
  //pinMode(IP_F1_A, INPUT);  // Estación lateral A no presente

}

void PORTUAK_irteeraPortuakHasieratu()
{
  pinMode(K_1, OUTPUT);   //
  pinMode(K_2, OUTPUT);   //
  pinMode(M_11, OUTPUT);   //
  pinMode(M_12, OUTPUT);   //

  pinMode(P_1, OUTPUT);   //
  pinMode(P_2, OUTPUT);   //
  pinMode(P_3, OUTPUT);   //
  pinMode(P_4, OUTPUT);   //


  // investigar
  //pinMode(IP_N_FO, OUTPUT);   //
  //pinMode(OBStat, OUTPUT);    //
  //pinMode(OBPan, OUTPUT); //
}


/* == HASIERAKO EGORA  == */
SARRERAK irakurriSentsoreSarrerak()
{
  SARRERAK sarrerak;
  sarrerak.readPiezaEnPuestoDeEntrada = digitalRead(Part_AV);
  sarrerak.readPiezaEnDetenedor = digitalRead(B_2);
  sarrerak.readPiezaDeTipo = digitalRead(B_3);
  sarrerak.readBarrera = digitalRead(B_4);
  sarrerak.readNoHayPiezaEnCintaDos =  digitalRead(B_5);
  sarrerak.readNoHayPiezaEnPickUp = digitalRead(B_6);

  sarrerak.readPulsadorDeInicio = digitalRead(S_1);
  sarrerak.readPulsadorDeParada = digitalRead(S_2);
  //sarrerak.readSelectorManualAutomatico = digitalRead(S_3);
  sarrerak.readPulsadorDeReset = digitalRead(S_4);
  sarrerak.readSetaDeEmergencia = digitalRead(Em_Stop);

  return sarrerak;
}


void idatziAktuadoreenBalioak(IRTEERAK irt)
{
  if (irt.writeMotorCintaUno) digitalWrite(K_1, HIGH);
  else digitalWrite(K_1, LOW);

  if (irt.writeMotorCintaDos) digitalWrite(K_2, HIGH);
  else digitalWrite(K_2, LOW);

  if (irt.writeDetenedor) digitalWrite(M_11, HIGH);
  else digitalWrite(M_11, LOW);

  if (irt.writeBarrera) digitalWrite(M_12, HIGH);
  else digitalWrite(M_12, LOW);

  if (irt.writeIndicadorDeInicio) digitalWrite(P_1, HIGH);
  else digitalWrite(P_1, LOW);

  if (irt.writeIndicadorDeReset) digitalWrite(P_2, HIGH);
  else digitalWrite(P_2, LOW);

  if (irt.writeIndicadorBufferLleno) digitalWrite(P_3, HIGH);
  else digitalWrite(P_3, LOW);

  if (irt.writeIndicadorPiezaEnPickUp) digitalWrite(P_4, HIGH);
  else digitalWrite(P_4, LOW);
}

void etapakHasieratu()
{
  for (int i = 0; i < MAX_ETAPAK; i++) {
        if(i == 0) etapak[i] = 1;
        else
           etapak[i] = 0;
    }
}

void trantsizioakHasieratu(){
  for(int i = 0; i < MAX_TRANTSIZIOAK; i++)
  {
    trantsizioak[i] = 0;
  }
}




void maketaGuztiaHasieratu()
{
  irteerak.writeMotorCintaUno = 0;
  irteerak.writeMotorCintaDos = 0;
  irteerak.writeDetenedor = 1;
  irteerak.writeBarrera = 0;

  irteerak.writeIndicadorDeInicio = 0;
  irteerak.writeIndicadorDeReset = 0;
  
  if(irakurriSentsoreSarrerak().readNoHayPiezaEnCintaDos) irteerak.writeIndicadorBufferLleno = 0;
  else  irteerak.writeIndicadorBufferLleno = 1;

  if (irakurriSentsoreSarrerak().readNoHayPiezaEnPickUp)  irteerak.writeIndicadorPiezaEnPickUp = 0;
  else  irteerak.writeIndicadorPiezaEnPickUp = 1;

  etapakHasieratu();
  trantsizioakHasieratu();
  kontagailua = 0;
  reseta = 0;
}

int tenporizadorea(int etapa, int ms)
{
  int denboraBurututa = 0;
  timeHasiera = millis();
  if(etapak[etapa]== 1)
    {     
      if((timeHasiera - timeAmaiera) >= ms) {
        denboraBurututa =  1;
        timeAmaiera = millis();
      }
    }
    return denboraBurututa;
}


void kalkulatuTrantsizioa(SARRERAK sarrera)
{
  trantsizioak[0] = etapak[0] && (sarrera.readPulsadorDeInicio && !sarrera.readPiezaEnPuestoDeEntrada);
  trantsizioak[1] = etapak[1] && sarrera.readPiezaEnPuestoDeEntrada;
  trantsizioak[2] = etapak[2] && sarrera.readPiezaEnDetenedor;
  trantsizioak[3] = etapak[3] && tenporizadorea(3,2000); 
  
  // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup desactivado
  trantsizioak[4] = etapak[4] && (sarrera.readPiezaDeTipo && sarrera.readNoHayPiezaEnPickUp); // No ha pieza en pickup Sensor a (1)
  trantsizioak[5] = etapak[5] && !sarrera.readNoHayPiezaEnPickUp; 
  trantsizioak[6] = etapak[6] && !sarrera.readNoHayPiezaEnPickUp;
  
   // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup activado
  trantsizioak[8] = etapak[4]  && (sarrera.readPiezaDeTipo && !sarrera.readNoHayPiezaEnPickUp && tenporizadorea(4,3000)); // Observaciones (Pieza en pickup más de 2 segundos)
  trantsizioak[9] = etapak[7]  && !sarrera.readNoHayPiezaEnPickUp;

  // Gestión de transiciones para pieza tipo Housing y buffer no lleno
  trantsizioak[10] = etapak[4] && (!sarrera.readPiezaDeTipo && sarrera.readNoHayPiezaEnCintaDos);
  trantsizioak[11] = etapak[8] && !sarrera.readNoHayPiezaEnCintaDos;
  trantsizioak[12] = etapak[9] && sarrera.readNoHayPiezaEnCintaDos && tenporizadorea(9,5000);
  trantsizioak[13] = etapak[10] && tenporizadorea(10,2000);
  trantsizioak[14] = etapak[9] && tenporizadorea(9,7000); 

  // Gestión de transiciones para pieza tipo Housing y buffer  lleno
 trantsizioak[15] = etapak[4] && (!sarrera.readPiezaDeTipo && !sarrera.readNoHayPiezaEnCintaDos && tenporizadorea(4,5000)); // Observaciones (Pieza en Cinta 2 más de 4 segundos)
 trantsizioak[16] = etapak[11]&& sarrera.readNoHayPiezaEnCintaDos;  
}


void desaktibatuEtapak()
{
    if(reseta) maketaGuztiaHasieratu(); 
    else
    {
      if(trantsizioak[0]) etapak[0] = 0; 
      if(trantsizioak[1]) {
        if(etapak[1]) kontagailua = kontagailua + 1; // Contador de mantenimiento
        etapak[1] = 0; 
      }
      if(trantsizioak[2]) etapak[2] = 0; 
      if(trantsizioak[3]) etapak[3] = 0; 

      // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup desactivado
      if(trantsizioak[4]) etapak[4] = 0; 
      if(trantsizioak[5]) etapak[5] = 0;
      if(trantsizioak[6]) etapak[6] = 0;
      if(trantsizioak[7]) etapak[6] = 0;
      // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup activado

      if(trantsizioak[8]) etapak[4] = 0; 
      if(trantsizioak[9]) etapak[7] = 0;

      // Gestión de transiciones para pieza tipo Housing y buffer no lleno
      if(trantsizioak[10]) etapak[4] = 0; 
      if(trantsizioak[11]) etapak[8] = 0; 
      if(trantsizioak[12]) etapak[9] = 0; 
      if(trantsizioak[13]) etapak[10] = 0; 
      if(trantsizioak[14]) etapak[9] = 0;
      
      // Gestión de transiciones para pieza tipo Housing y buffer  lleno
      if(trantsizioak[15]) etapak[4] = 0;
      if(trantsizioak[16]) etapak[11] = 0;
    }
}


void aktibatuEtapak()
{

  if(reseta) maketaGuztiaHasieratu();
  else
  {
    if(trantsizioak[0]) etapak[1] = 1; 
    if(trantsizioak[1]) etapak[2] = 1; 
    if(trantsizioak[2]) etapak[3] = 1; 
    if(trantsizioak[3]) etapak[4] = 1; 

      // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup desactivado
    if(trantsizioak[4]) etapak[5] = 1; 
    if(trantsizioak[5]) etapak[6] = 1;
    if(trantsizioak[6]) etapak[1] = 1;

      // Gestión de transiciones para pieza tipo Cylinder body y sensor pickup activado      
    if(trantsizioak[8]) etapak[7] = 1; 
    if(trantsizioak[9]) etapak[5] = 1; 
     
     // Gestión de transiciones para pieza tipo Housing y buffer no lleno
    if(trantsizioak[10]) etapak[8] = 1; 
    if(trantsizioak[11]) etapak[9] = 1; 
    if(trantsizioak[12]) etapak[10] = 1; 
    if(trantsizioak[13]) etapak[1] = 1; 
    if(trantsizioak[14]) etapak[1] = 1;

     // Gestión de transiciones para pieza tipo Housing y buffer  lleno
    if(trantsizioak[15]) etapak[11] = 1;
    if(trantsizioak[16]) etapak[8] = 1;
  }
}




void ekintzak()
{
  // == ETAPA 1 ==
  if(etapak[0]) 
  {
    Serial.println("etapa 1");      
    irteerak.writeBarrera = 0;
    irteerak.writeDetenedor = 1;
    irteerak.writeMotorCintaUno = 0;
    irteerak.writeMotorCintaDos = 0;
  }

  // == ETAPA 2 ==
  if(etapak[1]) { 
    Serial.println("Etapa 2");
    irteerak.writeIndicadorDeInicio = 1;
    irteerak.writeMotorCintaDos = 0 ;
  }

  // == ETAPA 3 ==
  if(etapak[2]){
    Serial.println("Etapa 3");
    irteerak.writeMotorCintaUno = 1; 
    irteerak.writeDetenedor = 1;
  }

  // == ETAPA 4 ==
  if(etapak[3]){
    delay(300);
    Serial.println("Etapa 4");
    irteerak.writeMotorCintaUno = 0;
   }
  
  // == ETAPA 5 ==
  
  if(etapak[4])
    {
      Serial.println("Etapa  5");
    }

    // == ETAPA 6 ==
    if(etapak[5])
    {
    Serial.println("Etapa 6");
    irteerak.writeMotorCintaUno = 1;
    irteerak.writeDetenedor = 0;
    irteerak.writeIndicadorPiezaEnPickUp = 0;
    }
  
    // == ETAPA 7 ==
    if(etapak[6])
    {
    Serial.println("Etapa 7");
    irteerak.writeDetenedor = 0;
    irteerak.writeMotorCintaUno = 0;
    irteerak.writeIndicadorPiezaEnPickUp = 1;
    irteerak.writeIndicadorDeInicio = 0;
    }
  
    // == ETAPA 8 ==
   if(etapak[7])
   {
       Serial.println("Etapa 8");
       irteerak.writeMotorCintaUno = 0;
       irteerak.writeIndicadorPiezaEnPickUp = 1;
       irteerak.writeIndicadorBufferLleno = 0;
   }
 
   // == ETAPA 9 ==
    if(etapak[8])
    {
      Serial.println("Etapa 9");
      irteerak.writeDetenedor = 0;
      irteerak.writeBarrera = 1;
      irteerak.writeMotorCintaUno = 1; 
      irteerak.writeMotorCintaDos = 1;
      irteerak.writeIndicadorBufferLleno = 0;
    }
 
  // == ETAPA 10 ==

   if(etapak[9])
   {
      Serial.println("Etapa 10");
      irteerak.writeMotorCintaDos = 1;
      irteerak.writeDetenedor = 1;
      irteerak.writeMotorCintaUno = 0;
      irteerak.writeBarrera = 0;
      irteerak.writeIndicadorBufferLleno = 1;
   }
  
  // == ETAPA 11 ==
  if(etapak[10])
  {
    Serial.println("Etapa 11");
     irteerak.writeIndicadorBufferLleno = 0;
     irteerak.writeIndicadorDeInicio = 1;
     irteerak.writeMotorCintaDos = 0;
  }
  
  // == ETAPA 12 ==
 if(etapak[11])
 {
   Serial.println("Etapa 12");
   irteerak.writeDetenedor = 1; 
   irteerak.writeMotorCintaUno = 0;
   irteerak.writeMotorCintaDos = 0;
   irteerak.writeIndicadorBufferLleno = 1; 
   irteerak.writeIndicadorDeInicio = 1;
 }  
}

/* === INICIO DE SETUP  Y LOOP === */

void setup()
{
  Serial.begin(9600); // visualizar en pantalla
  PORTUAK_sarreraPortuakHasieratu();
  PORTUAK_irteeraPortuakHasieratu();      
  etapakHasieratu();
  delay(1500);
}


void loop()
{
  SARRERAK sarrera = irakurriSentsoreSarrerak();
 

  // == CALCULAR ETAPA VALIDA 
  kalkulatuTrantsizioa(sarrera);
  desaktibatuEtapak();
  aktibatuEtapak();
  
 if(!sarrera.readSetaDeEmergencia)  // Seta de emergencia (NC)
 {
    Serial.println("Seta de Emergia activada");
    irteerak.writeMotorCintaUno = 0;
    irteerak.writeMotorCintaDos = 0;
    irteerak.writeDetenedor = 0;
    irteerak.writeBarrera = 0;
    irteerak.writeIndicadorDeInicio = 1;
    irteerak.writeIndicadorDeReset = 1;
    idatziAktuadoreenBalioak(irteerak);
    reseta = 1;
   do {
    Serial.println("LARRIALDEKO PULTSADOREA: Sakatu RESET botoia");
   }while(irakurriSentsoreSarrerak().readPulsadorDeReset != 1);
   
   if(irakurriSentsoreSarrerak().readPulsadorDeReset)  maketaGuztiaHasieratu();
 }
 else if (sarrera.readPulsadorDeReset) // Pulsador de Reset
 {
  Serial.println("RESET");
    reseta = 1;
    maketaGuztiaHasieratu();
    if(irakurriSentsoreSarrerak().readPulsadorDeReset)  maketaGuztiaHasieratu();
 } 
 else {
   if (!irakurriSentsoreSarrerak().readPulsadorDeParada) // Pulsador de parada (NC)
   {
    Serial.println("GELDIUNEA");
     reseta = 1;
     maketaGuztiaHasieratu();
   } else ekintzak(); 
 }

// MANTENIMENDUA
  if(kontagailua >= MAX_PIEZAK_AURREMANTENIMENDUA)
   {
     irteerak.writeIndicadorDeReset = 1;
     Serial.print("SEPERATING STATION: ");
     Serial.print(kontagailua);
     Serial.println(" pieza jaso ditu");
     Serial.println("MANTENIMENDUA: Sakatu RESET botoia");
     if (kontagailua >= MAX_ARRISKUA_MANTENIMENDUA)
     {
       do
       {
         Serial.println("MAKINA ARRISKUAN: Sakatu RESET botoia");
       } while (irakurriSentsoreSarrerak().readPulsadorDeReset != 1); 
        if(irakurriSentsoreSarrerak().readPulsadorDeReset)  maketaGuztiaHasieratu();     
     }
   }
  
  idatziAktuadoreenBalioak(irteerak);
  delay(10);
}
