#ifndef INCLUDE
#define INCLUDE

#include <SPI.h>    // Librería línea serie (Serial Peripherical Interface)
#include <Controllino.h>    // Librería para Controllino




/* === ETAPAK === */


/*  ==== ENTRADAS CONTROLLINO A ARDUINO === */


 int  Part_AV = CONTROLLINO_A0;    // Pieza en puesto de entrada
 int  B_2 = CONTROLLINO_A1;    // Pieza en detenedor
 int  B_3 = CONTROLLINO_A2;   // Pieza de tipo "housing"
 int  B_4 = CONTROLLINO_A3;    // Switch retrasado
 int  B_5 = CONTROLLINO_A4;  // No hay pieza en cinta 2
 int  B_6 = CONTROLLINO_A5;    // No hay pieza en pick-up

 int  S_1 = CONTROLLINO_A6;    // Pulsador de inicio
 int  S_2 = CONTROLLINO_A7;    // Pulsador de parada (NC)
 int  S_3 = CONTROLLINO_A8;   // Selector manual/automático PREGUNTAR ESTO
 int  S_4 = CONTROLLINO_A9;   // Pulsador de reset
 int  Em_Stop = CONTROLLINO_IN0;    // Seta de emergencia (NC)

/*  ==== SALIDAS CONTROLLINO A ARDUINO === */


 int  K_1 =  CONTROLLINO_R0;    // Accionar motor de la cinta 1
 int  K_2 =  CONTROLLINO_R1;    // Accionar motor de la cinta 2
 int  M_11 =  CONTROLLINO_R2;    // Retrasar detenedor  -- 1 atrás -- 0 adelante
 int  M_12 =  CONTROLLINO_R3;    // Adelantar switch  -- 1 adelante -- 0 atras

 int  P_1 =  CONTROLLINO_D0;    // Indicador de inicio
 int  P_2 =  CONTROLLINO_D1;    // Indicador de reset CONTROLLINO_R5
 int  P_3 =  CONTROLLINO_D2;    // Indicador de buffer lleno CONTROLINO_CMN (a)
 int  P_4 =  CONTROLLINO_D3;    // Indicador de pieza en pick-up CONTROLLINO_CMN (b)


/* ==== DATUEN EGITURA === */

typedef struct SARRERAK {

  int readPiezaEnPuestoDeEntrada;
  int readPiezaEnDetenedor;
  int readPiezaDeTipo;
  int readBarrera;
  int readNoHayPiezaEnCintaDos;
  int readNoHayPiezaEnPickUp;

  int readPulsadorDeInicio;
  int readPulsadorDeParada;
  int readSelectorManualAutomatico;
  int readPulsadorDeReset;
  int readSetaDeEmergencia;
    
}SARRERAK;


typedef struct IRTEERAK
{
    int writeMotorCintaUno;
    int writeMotorCintaDos;
    int writeDetenedor; 
    int writeBarrera;

    int writeIndicadorDeInicio;
    int writeIndicadorDeReset;
    int writeIndicadorBufferLleno;
    int writeIndicadorPiezaEnPickUp;
}IRTEERAK;

/* == PORTUAK == */
void PORTUAK_sarreraPortuakHasieratu();
void PORTUAK_irteeraPortuakHasieratu();

/* == ETAPAK == */
/* === DEFINIZIOAK === */

SARRERAK irakurriSentsoreSarrerak();
void idatziAktuadoreenBalioak(IRTEERAK irt);
void kalkulatuTrantsizioa(SARRERAK sarrera);
void desaktibatuEtapak(int emergentzia, int parada);
void aktibatuEtapak();
void ekintzak();

int tenporizadorea(int etapa, int ms);
void desaktibatuTenporizadorea();
void maketaGuztiaHasieratu();

#endif
