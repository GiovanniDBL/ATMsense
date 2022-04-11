
/*

  *** ZigbMax Technology ***
     03-ENERO_2022
     Cuenta AWS-IoT 876207246589

    
  Funciones ------------------------------ Real -------Test ------
  H11 Status      Pin 26(C3)--------->A3 = OUTPUT         OK!!
  H10 GAS         PIN 23 ------------>A0 = INPUT          OK!! Sensor de Gas. 
  H9 ShutDown     PIN 25(C2) -------->A2 = OUTPUT          XX!! Apagar/Reset de ATM Pendiente por verificar.
  H6 SENSOR18     PIN 12(B0)---------> 8 = INPUT          OK!!
  TBlock Fascia   PIN 19(ADC) ------->A6 = INPUT          OK!! Sensor fotosensible de Fascia Abierta.
  H8 SENSOR_VIB   PIN 22(ADC) --------A7 = INPUT          OK!! Sensor Vibracion.
  TXfox           PIN 13(PB2) ------->9 =  DATA           0K!!
  RXfox           PIN 14(PB3) ------->10 = DATA           OK!!
  controlOUT      PIN  9(PD5) -------> 5 = OUTPUT         OK!! Salida interna para panel del alarma.
  alarmOUT        PIN 10(PB3) -------> 6 = OUTPUT         OK!! Salida de sirena.
  keyZone         PIN  2(D4) --------  4 = INPUT          OK!! Zona2 - KeySwitch.
  alarmZone       PIN 11(D7) --------  7 = INPUT          OK!! Zona1 - ALARMA 24HRS.
  H1-2 tamper     PIN 24(ADC1) ------>A1 = INPUT          OK!! Tamper digital.
  H1-3            PIN 32(D2) -------->D2 = XXXX           OK!!
  H1-4            PIN 1(D3)---------->D3 = XXXX           OK!!
  LED3            PIN 17(B5)--------- 13 = OUTPUT         OK!! Led de Status CPU.
  H1-1 = VCC
  H1-5 = GND
*/


//-------------------------------------- SALIDA Y CONTROL SIGFOX ------------------------------------------------

#include <SoftwareSerial.h> // +++++++++++++++++++++++++++++++++++++++++++++++++++
SoftwareSerial mySerial (10,9);// RX, TX +++++++++++++++++++++++++++++++++++++++++



//-------------------------------------- Salidas y Entradas de Control ------------------------------------------------
const int alarmaOut = 6;    // PIN SALIDA DE SIRENA
const int controlOut = 5;  // PIN SALIDA PANEL DE ALARMA
const int ledRun = 13; // LED INDICADOR DE FUNCIONAMIENTO CPU
const int keyZone = 4; // ENTRADA KEYZONE
const int alarmZone = 7; // ENTRADA ALARMA-24 HRS


int DatoAlarmZone; // variable Alarma 24HRS
int DatoKeyZone;

//int keyswitch;
int KeyOn = 0;
int KeyOff = 0;
int puerta;





//--------------------------------------- DECLARACIONES Y VARIABLES - ShutDown --------------------------------------------------------------------------

const int shutDown = A2;




//--------------------------------------- DECLARACIONES Y VARIABLES - TAMPER/FASCIA --------------------------------------------------------------------------
const int tamper = A1; 
int valorTamper = 200; 
float datosTamper;
float fotoTamper;
const int thresholdTamper = 100; // se define el unbral del nivel de luz en el sensor tamper ++++++++++++++++++++++++++++++++++++++++++


const int fascia = A6; 
int valorFascia = 0; 






//--------------------------------------- DECLARACIONES Y VARIABLES - VIBRACION --------------------------------------------------------------------------


const int vibracion = A7; // VARIABLE PARA ALMACENAR VALOR LEIDO DE ENTRADA ANALOGICA A7
float DatoVibracion;      // VARIABLE PARA ALMACENAR LA LECTURA DEL SENSOR DE VIBRACION

int nivel_vibracionlow = 20;    // =AJUSTABLE=     VARIABLE NIVEL MINIMO DE VIBRACION
int nivel_vibracionmedium = 100; // =AJUSTABLE=   VARIABLE NIVEL MEDIO DE VIBRACION
int nivel_vibracionMax = 200;  // =AJUSTABLE=    VARIABLE NIVEL ALTO DE VIBRACION


int Golpesmax = 3;    //  =AJUSTABLE=  VARIABLE PARA DEFINIR LAS CANTIDAD DE GOLPES MAXIMOS ANTES DE LA ACTIVACION.
int Numgolpes = 0;      // VARIABLE PARA ALMACENAR LA CANTIDAD DE GOLPES REALIZADOS EN DETERMINADO TIEMPO.



//-------------------------------------------------------------  DECLARACIONES Y VARIABLES - TEMPERATURA DIGITAL ------------------------------------------------------------------
#include <OneWire.h>

#include <DallasTemperature.h>
const int pinDatosDQ = 8;     //VARIABLE PARA ALMACENAR VALOR LEIDO DE ENTRADA D8
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);
int datosTemp = 0;     // VALOR DE TEMPERATURA EN GRADOS CENTIGRADOS.
int datosTemp2 = 0;   // VALOR DE TEMPERATURA EN GRADOS CENTIGRADOS.
float resTemp;  // VARIABLE QUE ALMACENA OPERACION ARITMETICA ENTRE datosTemp Y datosTemp2




//---------------------------------------------------- DECLARACIONES Y VARIABLES - LED'S ---------------------------------------------------------------------------

#include <FastLED.h>
#define LED_PIN A3
#define NUM_LEDS 3

CRGB leds[NUM_LEDS];


//------------------------------------------------- DECLARACIONES Y VARIABLES - ACELEROMETRO ------------------------------------------------------------------------------

#include <Wire.h>
#include "SparkFun_MMA8452Q.h"
MMA8452Q accel;

 int keyposicion = 0;
 float valorX;
 float valorZ;
 float promX;
 float promZ;
 float resultX;
 float resultZ;
 float valoriableX;
 float valoriableZ;
 float resultXpos; 
 float resultZpos;
 float resultXneg;
 float resultZneg;
 int contador;
 int contador2;

 

//----------------------------------------------------------  DECLARACIONES Y VARIABLES - GAS -----------------------------------------------------------------------------------------------------------


int datosGas;  //VARIABLE PARA ALMACENAR VALOR LEIDO DE ENTRADA A0
int nivelGas = 200;  // =AJUSTABLE=  NIVEL DE GAS MAXIMA ANTES DE ACTIVACION.  "ajustar este valor,nivel maximo 600"






//-------------------------------------------------------  DECLARACIONES Y VARIABLES - TIMER'S -------------------------------------------------------------------------------------------------------


unsigned long PosicionMillis1 = 0;

long intervalo1 = 2500; // TIEMPO MAXIMO PARA ELIMINAR MEMORIA DE INCLINACION 

unsigned long PosicionMillis2 = 0;

long intervalo2 = 120000;  //    =AJUSTABLE=   ESTADO DE POSICION (NOTIFICACION DEL UNA MALA INSTALACION CADA DETERMINADO TIMEPO)


unsigned long PosicionMillis3 = 0;
long intervalo3 = 90000;        //AJUSTABLE=   ResetVibracion (CADA CUANTO RESETEA LOS GOLPES ALMACENADOS PARA ACTIVAR VIBRACION)


unsigned long PosicionMillis4 = 0;
long intervalo4 = 5700;      // 6 segundos en su posicion para guardar buena ubicacion.

unsigned long PosicionMillis5 = 0;
long intervalo5 = 120000;      // TEST DE VIDA - Se enviara cada 2 minutos.

unsigned long PosicionMillis6 = 0;
long intervalo6 = 120000;      // =AJUSTABLE=  notificacion de puerta abierta.







// -------------------------------------------------------------------------------- LED'S ------------------------------------------------------------------------------------------------------------


void LedBandera() {

    digitalWrite(ledRun, HIGH); 
    leds[0] = CRGB::Green;
    leds[1] = CRGB::White;
    leds[2] = CRGB::Red;
    FastLED.show();
    delay(9000);
    digitalWrite(ledRun, LOW);
    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    FastLED.show();
    delay(1000);

      
}

void LedArranque(){

for (int x = 0; x < 1 ; x++) {

    leds[2] = CRGB::Black;
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(100);

    leds[0] = CRGB::Black;
    leds[1] = CRGB::Green;
    FastLED.show();
    delay(100);

    leds[1] = CRGB::Black;
    leds[2] = CRGB::Green;
    FastLED.show();
    delay(100);
    leds[2] = CRGB::Black;
    FastLED.show();

  }
  
}

void Ledblanco() {

  leds[0] = CRGB::White;
  leds[1] = CRGB::White;
  leds[2] = CRGB::White;
  FastLED.show();
  delay(300);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(300);
}

void LedAmarillo() {

  leds[0] = CRGB::Yellow;
  leds[1] = CRGB::Yellow;
  leds[2] = CRGB::Yellow;
  FastLED.show();
  delay(200);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(200);
}

void LedVerde() {

  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;


  FastLED.show();
  delay(200);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(200);
}

void LedVerdeFast() {

  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;


  FastLED.show();
  delay(20);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  
}

void LedRojoFast() {

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  FastLED.show();
  delay(20);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  
}

void LedRojo() {

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(100);
}

void LedAzul() {         
  

  for (int x = 1; x < 4 ; x++) {

  leds[0] = CRGB::Blue;
  leds[1] = CRGB::Blue;
  leds[2] = CRGB::Blue;
  FastLED.show();
  delay(70);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(70);
    
  }



}


void LedMorado() {

  leds[0] = CRGB::Purple;
  leds[1] = CRGB::Purple;
  leds[2] = CRGB::Purple;
  FastLED.show();
  delay(70);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(70);

  leds[0] = CRGB::Purple;
  leds[1] = CRGB::Purple;
  leds[2] = CRGB::Purple;
  FastLED.show();
  delay(70);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(70);

  leds[0] = CRGB::Purple;
  leds[1] = CRGB::Purple;
  leds[2] = CRGB::Purple;
  FastLED.show();
  delay(70);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(70);

}


void LedNaranja() {

  leds[0] = CRGB::Magenta;
  leds[1] = CRGB::Magenta;
  leds[2] = CRGB::Magenta;
  FastLED.show();
  delay(200);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(200);
}

void LedMensaje() {

  for (int x = 1; x < 4 ; x++) {

  leds[0] = CRGB::White;
  leds[1] = CRGB::White;
  leds[2] = CRGB::White;
  FastLED.show();
  delay(70);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(70);
    
  }
}



// ---------------------------------------------------------------------------------- FUNCIONES ------------------------------------------------------------------------------------------------------------



void Vibracion() {

  TimerVibracion();

  DatoVibracion = analogRead(vibracion);



  if (DatoVibracion >= nivel_vibracionlow) {

    if (DatoVibracion >= nivel_vibracionmedium) {

      if (DatoVibracion >= nivel_vibracionMax) {

        
       Serial.print("*** GOLPE MAXIMO ***:");
       LedRojo();

       
     mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
     delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
     /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
     Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
     cadena_byte(1,6,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
     
       LedMensaje();
        
        LEDVibracion();
         
      }


      

      else {
        
        Numgolpes ++;
        Serial.print("\n");
        Serial.print("\n");
        Serial.print("*** GOLPE MEDIA ***:");
        Serial.println(DatoVibracion);
        Serial.print("\n");
        Serial.print("\n");

        for (int x = 0; x < 2 ; x++) {
          LedAmarillo();
        }
      }


    }



    else {
      Serial.print("\n");
      Serial.print("\n");
      Serial.print("*** GOLPE BAJA *** :");
      Serial.println(DatoVibracion);
      Serial.print("\n");
      Serial.print("\n");
      for (int x = 0; x < 2 ; x++) {
        LedVerde();
      }
    }





  }



  if (Numgolpes == Golpesmax) {

  
    Numgolpes = 0;
    Serial.print("*** SUMA DE 3 GOLPES MEDIA  ***:");
    
    mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
    delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
    /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
    
    cadena_byte(1,6,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
    
    LedMensaje();
    
    LEDVibracion();
    

  }


}


void Temp_Dig1() {

  sensorDS18B20.requestTemperatures();
  datosTemp = (sensorDS18B20.getTempCByIndex(0));
  Serial.print("\n");
  Serial.print("Temp Digital 1: ");
  Serial.println(datosTemp);

  ///Implementación de una alarma de cambio brusco de temperatura.

}


void Temp_Dig2() {

  sensorDS18B20.requestTemperatures();
  datosTemp2 = (sensorDS18B20.getTempCByIndex(0));

  Serial.print("\n");
  Serial.print("Temp Digital 2: ");
  Serial.println(datosTemp2);
  Serial.print("\n");
   Serial.print("\n");
 
}

 void Gas() {

  datosGas = analogRead(A0);

  if (datosGas >= nivelGas ) {
      
  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  
  cadena_byte(1,5,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  LedMensaje();
   
    LEDGas();
    
  }
 }


void Tamper(){ // REVISAR ESTA FUNCION. 

  
  datosTamper = analogRead(tamper);
   if (datosTamper >= fotoTamper ) {
    
    Serial.println("*** Tamper ALARMA  ***:");
     mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
     delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
     /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
     Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
     cadena_byte(1,7,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
    
   LEDTamper();
    
  }
  
}




void ControlOut() {  // TIMPO PARA ACTIVACION DE PANEL ALARMA Y ENCENDIDO DE SIRENA. 

  digitalWrite(controlOut, HIGH);

  digitalWrite(alarmaOut, HIGH);
  
  delay(1500); // TIEMPO DE PULSO PARA PENEL 2 SEG

  digitalWrite(alarmaOut, LOW);
 
  delay(10000); // TIEPO DE ACTIVACION SIRENA 

  digitalWrite(controlOut, LOW);
  

}



void AlarmZone() {

  DatoAlarmZone = digitalRead(alarmZone); // DETALLES CON LA ENTRADA KEYZONE


  if (DatoAlarmZone == HIGH) {

    Serial.print("\n");
    Serial.print("*** ALARMA ATM - 24HRS *** "); 
    
     mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
    delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
    /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  
  cadena_byte(1,9,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  
   LedMensaje();
    
   
    
    LED24hrs();
    
  }

}


void ShutDow() {




}


void PosicionInstalacion(){

  
  

while (keyposicion < 10){

TimerPosicionInstalacion();
TimerPosicion();

 promX = 0;
 promZ = 0;

for (int x = 0; x < 200 ; x++) {

   if (accel.available()) { 
     
     valorX = accel.getCalculatedX();
     valorZ = accel.getCalculatedZ();

     promX = valorX + promX;
     promZ = valorZ + promZ;
     
     }
}


  resultX = promX / 200;
  resultZ = promZ / 200;

/*Serial.print("\n");
Serial.print("VALOR X :");
Serial.print(resultX);
Serial.print("\t");
Serial.print("VALOR Z :");
Serial.print(resultZ);
Serial.print("\n");
 delay(500);
*/

if(resultX < .10 && resultX > -.10 && resultZ < .15 &&  resultZ > -.15 ){
  
LedArranque();
keyposicion ++;
Serial.print(keyposicion);
Serial.print("\n");
Serial.print("\n");
Serial.print("\n");
}

else{
LedRojo();
 
}


}
  
}



void ObtencionPosicionInclinacion(){
  

promX = 0;
promZ = 0;
resultX = 0;
resultZ = 0;

for (int x = 0; x < 100 ; x++) {

   if (accel.available()) { 
     
     valorX = accel.getCalculatedX();
     valorZ = accel.getCalculatedZ();

     promX = valorX + promX;
     promZ = valorZ + promZ;
     
     }
}


 resultX = promX / 100;
 resultZ = promZ / 100;


resultXpos = resultX + .10; // es igual a 6º de inclinacion
resultXneg = resultX - .10;

resultZpos = resultZ + .10; // es igual a 6º de inclinacion
resultZneg = resultZ - .10; // es igual a 6º de inclinacion
 
 Serial.println();
 Serial.print( "Valor de eje X:");
 Serial.println(valorX);
 Serial.println();
 Serial.print("Valor promedio POSITIVO de la X:");
 Serial.print(resultXpos);
 Serial.println();
 Serial.print("Valor promedio NEGATICO de la X:");
 Serial.print(resultXneg);
Serial.println();
Serial.println();
Serial.println();
Serial.print( "Valor de eje Z:");
Serial.println(valorZ);
Serial.println();
Serial.print("Valor promedio POSITIVO de la Z:");
Serial.print(resultZpos);
Serial.println();
Serial.print("Valor promedio NEGATICO de la Z:");
Serial.print(resultZneg);


  
}



void Iclinacion(){


 TimerInclinacion();

if (accel.available()) {     
    
    valoriableX = accel.getCalculatedX();  
    valoriableZ = accel.getCalculatedZ(); 
  }

  
 if(valoriableX > resultXpos || valoriableX < resultXneg ){

      contador++;
      Serial.print("X:");
      Serial.print(contador);
      Serial.println();
      
    }


    if(valoriableZ > resultZpos || valoriableZ < resultZneg ){

      contador2++;
       Serial.print("Z:");
      Serial.print(contador2);
      Serial.println();
      
    }

   

if(contador >= 200){

      
      Serial.print("***INCLINACION FRONTAL ACTIVADA***");
      
       mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
     delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
     Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
     cadena_byte(1,3,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
     LedMensaje();

     
      LEDInclina();
    }

    if(contador2 >= 200){

      
       Serial.print("***INCLINACION LATERAL ACTIVADA ***");
       mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
       delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
        /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
       Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
      cadena_byte(1,3,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
      LedMensaje();
      
      LEDInclina();
    }



  
}





//----------------------------------------------------------------- LED'S 2 ------------------------------------------------------------------------

void LEDVibracion() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Blue;
  leds[2] = CRGB::Red;
  FastLED.show();
  
  ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();


  

}



void LEDTemperatura() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::White;
  leds[2] = CRGB::Red;
  FastLED.show();
  
 ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();

  
  

}



void LEDGas() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  FastLED.show();
  
   ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();

  
  

}


void LEDTamper() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Red;
  FastLED.show();
  
   
 ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();

  
  

}


void LED24hrs() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Orange;
  leds[2] = CRGB::Red;
  FastLED.show();
  
  ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();


  

}


void LEDInclina() { 

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Magenta;
  leds[2] = CRGB::Red;
  FastLED.show();
  
   ControlOut();
  
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();

 
  

}




// ---------------------------------------------------------------------------- TIMER'S --------------------------------------------------------------------------------------------------------------------



void TimerInclinacion(){

 unsigned long VariableMillis1 = millis();

  if ((VariableMillis1 - PosicionMillis1) >= intervalo1) {
    

    PosicionMillis1 = VariableMillis1;

    contador = 0;
    contador2 = 0;
    Serial.println("RESET INCLINACION");
    }
    


  
}



 
void TimerPosicion() { // REVISAR ESTA PARTE PARA QUE SUENE LA SIRENA, ESTA MAL ESTA PARTE DERIVADO A LA CONBINACION DE CONTROL CON ACTIVACION DE PANEL. 
  
  unsigned long VariableMillis2 = millis();

  if ((VariableMillis2 - PosicionMillis2) >= intervalo2) {

     PosicionMillis2 = VariableMillis2;

  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  cadena_byte(1,19,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+

   
   Serial.println("*SENSOR MAL INSTALADO*");
   digitalWrite(controlOut, HIGH);  // AQUI SE NOTIFICARA CUANDO EL SENSOR ESTA MAL activando la salida de alarma
   delay(100); 
   digitalWrite(controlOut, LOW);
   delay(100); 
   digitalWrite(controlOut, HIGH);
   delay(100); 
   digitalWrite(controlOut, LOW);
   delay(100); 
   digitalWrite(controlOut, HIGH);
   delay(100); 
   digitalWrite(controlOut, LOW); 
   

  }

}



void TimerVibracion() {


  unsigned long VariableMillis3 = millis();

  if ((VariableMillis3 - PosicionMillis3) >= intervalo3) {
    

    PosicionMillis3 = VariableMillis3;

    Numgolpes = 0;
    Serial.print("\n");
    Serial.print("\n");
    Serial.println("=RESET DE GOLPES=");
    Serial.print("\n");
    Serial.print("\n");


  }

}




void TimerPosicionInstalacion() { 


  unsigned long VariableMillis4 = millis();

  if ((VariableMillis4 - PosicionMillis4) >= intervalo4) {

    PosicionMillis4 = VariableMillis4;

  
    
    keyposicion = 0;
    Serial.print("\n");
    Serial.print("\n");
    Serial.println("=RESET keyposicion =");
    Serial.print("\n");
    Serial.print("\n");


  }
  }




void TimePuertaAbierta() { // REVISAR ESTA PARTE PARA QUE SUENE LA SIRENA, ESTA MAL ESTA PARTE DERIVADO A LA CONBINACION DE CONTROL CON ACTIVACION DE PANEL.   


  unsigned long VariableMillis6 = millis();

  if ((VariableMillis6 - PosicionMillis6) >= intervalo6) {

    PosicionMillis6 = VariableMillis6;

    Serial.println("ALARMA DE PUERTA  NO CERRADA");

  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  cadena_byte(1,18,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  LedMensaje();
  
   digitalWrite(controlOut, HIGH);  
   delay(100); 
   digitalWrite(controlOut, LOW);
   delay(100); 
   digitalWrite(controlOut, HIGH);
   delay(100); 
   digitalWrite(controlOut, LOW);
   delay(100); 
   digitalWrite(controlOut, HIGH);
   delay(100); 
   digitalWrite(controlOut, LOW); 
     

     


  }
  }





void TestVida(){
  

unsigned long VariableMillis5 = millis();

  if ((VariableMillis5 - PosicionMillis5) >= intervalo5) {

    PosicionMillis5 = VariableMillis5;

  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  cadena_byte(1,4,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  LedMensaje();


    
}
}




// ---------------------------------------------------------------------------- CICLOS -----------------------------------------------------------------------------------------------




void Principal() {


  if (DatoKeyZone == LOW) {
    LedAzul();
  }
  else {
    LedMorado();
  }

  Temp_Dig1();
   
  
  
  for (int x = 1; x < 6250 ; x++) { // (10 seg programados) 625 = 1 segundos del ciclo. 

    TestVida();
    Vibracion();
    //Gas();  
    //Tamper(); 
    Vibracion();
    Iclinacion();
    AlarmZone();
  }

  
  Temp_Dig2();
   

  resTemp = datosTemp2 - datosTemp;

  if (resTemp >= 2) { //  =AJUSTABLE= GRADOS DE DIFERENCIA PARA ACTIVACIOn, crear variable para cambio de temperatura. 
  
   
   LEDTemperatura();
    



  }

}




///-------------------------------------------------------  PROTOCOLO COMUNICACION --------------------------------------------------------------------------


void cadena_byte(int m,int n,int o,int p,int q,int r,int s,int t){

    //Variable bytes especificos de la cadena y comandos AT
    String disp="";
    String accion="";
    String gas="";
    String impacto="";  
    String cambio=""; 
    String red="";
    String at = "AT$SF=";
    
    byte cadena[8];  /////longitud de la cadena -es modificable- Actualmente con 8 bytes 
    
    ///Asignación del orden por datos dentro de la cadena
    cadena[0] = (byte) (m);
    cadena[1] = (byte) (n);
    cadena[2] = (byte) (o);
    cadena[3] = (byte) (p);
    cadena[4] = (byte) (q);
    cadena[5] = (byte) (r);
    cadena[6] = (byte) (s);
    cadena[7] = (byte) (t);

    ///////////////////Analisis de variables especificas para formato
    disp += "0" + String(cadena[0], HEX);
    gas += "0" + String(cadena[4], HEX);
    impacto += "0" + String(cadena[5], HEX);
    cambio += "0" + String(cadena[6], HEX);
    red += "0" + String(cadena[7], HEX);
    //////////////////////Proceso de validación del byte de la acción
    if(cadena[1]< 10){
      accion += "0" + String(cadena[1], HEX);
    }else{
      accion += String(cadena[1]); 
    }
    ////////////////////////////////////////////////////////////////

   //////////////////////Armando cadena con el formato de comandos AT
   mySerial.print(at); ///comando AT
   mySerial.print(disp);
   mySerial.print(accion);
   mySerial.print(cadena[2],HEX);
   mySerial.print(cadena[3],HEX);
   mySerial.print(gas);
   mySerial.print(impacto);
   mySerial.print(cambio);
   mySerial.print(red);
   mySerial.print('\n');

}///MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM FIN DEL MÉTODO


void setup() {
  
  Serial.begin(9600);
  mySerial.begin(9600);//comunicacion con el radio Sigfox ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  cadena_byte(1,15,datosTemp,nivel_vibracionlow,nivelGas,12,datosTemp2,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  LedMensaje();

  
  pinMode(alarmaOut, OUTPUT);
  pinMode (controlOut, OUTPUT);
  pinMode(ledRun, OUTPUT);
  pinMode (keyZone, INPUT);
  pinMode(alarmZone, INPUT);
  pinMode(shutDown, OUTPUT); 
  //digitalWrite(shutDown, LOW); 


  sensorDS18B20.begin();
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  Wire.begin();


  if (accel.begin() == false) { // FUTURA MODIFICACION.

    Serial.println("*** Falla de Acelerometro, No Conectado ***");
    while (1);
  }

  Serial.println("ATMsense: VERS 0.3 CON COMUNICAICON");
 
  LedBandera();

  PosicionInstalacion();
  ObtencionPosicionInclinacion();
 
  
}


void loop() {

  


  DatoKeyZone = digitalRead(keyZone); 


  if (DatoKeyZone == LOW) { 
    KeyOn = 1;

  }



  else {
    KeyOn = 2;
  }



  if ( puerta == 2) {


    Serial.print("\n");
    Serial.print("\n");
    Serial.print("** PUERTA ATM CERRADA **  ");
    
     mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
    delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
     /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  
    cadena_byte(1,2,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
    LedMensaje();
   
    Serial.print("\n");
      puerta = 0;
  }




  if (KeyOn == 1) {
    Principal();

    KeyOn = 0 ;
    KeyOff = 0;
  }






  if (KeyOff == 1) {
    
    TimePuertaAbierta(); // ACTIVARA ALARMA cada x tiempo que la puerta esta abierta.
  
    Principal();
    KeyOn = 0 ;


    DatoKeyZone = digitalRead(keyZone); 

    if ( DatoKeyZone == LOW) {
      puerta = 2;
    }
    
 }





  if (KeyOn == 2) {

    Serial.print("\n");
    Serial.print("\n");
    Serial.print("** ATMSENSE - APERTURA DE PUERTA ** ");
    Serial.print("\n");
    Serial.print("\n");

  mySerial.print("AT$RC\n"); // Comando para el reset del modulo x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x
  delay(200); // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
  /// --Implementación del método de la cadena de bytes -- MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Temp_Dig1(); ///Se llama a los métodos de temperatura para obtener la información actual
  cadena_byte(1,1,datosTemp,DatoVibracion,datosGas,0,0,1);// x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+x+
  LedMensaje();
     
    leds[0] = CRGB::Yellow;
    leds[1] = CRGB::Yellow;
    leds[2] = CRGB::Yellow;
    FastLED.show();
    
    delay(30000);  // TIEMPO DE DESACTIVACION DE ATM *** hay que crear una variable para almacenar el tiempo.
    
    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    FastLED.show();

    DatoKeyZone = digitalRead(keyZone);

    if (DatoKeyZone == LOW) {
      puerta = 2;

    }

    KeyOff = 1;

  }


 

} //  Fin de codigo
