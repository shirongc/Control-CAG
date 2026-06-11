#include <SPI.h>
#include <math.h>

//
const int slaveSelectPin = 53; 
const int LDACpin = 48;


const unsigned short CanalD = 0x3000; 


//Inicialización
unsigned short Level = 0x0000;
unsigned short sRF=0x0000;  
byte r[2];


void setup() {
  Serial.begin(57600);         
  //Serial.println(ADC_STEPS);

   // Inicializar pines
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(LDACpin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH); // Asegurar que el esclavo esté desactivado
  digitalWrite(LDACpin, HIGH);

}

void loop() {


  // Inicializar SPI
  SPI.begin();
  SPI.beginTransaction (SPISettings (8000000, MSBFIRST, SPI_MODE0));

  sRF = Level | CanalD;

  //Escritura

  digitalWrite(slaveSelectPin, LOW);
  r[0] = SPI.transfer (highByte(sRF));
  r[1] = SPI.transfer (lowByte(sRF));
  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(LDACpin, LOW);
  delay(0.1);
  digitalWrite(LDACpin, HIGH);
  Serial.println(Level);

  SPI.endTransaction();

  delay(5000);

  if (Level < 4000) {
  Level = Level + 250;  
  }
} 
