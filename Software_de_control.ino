#include <SPI.h>
#include <math.h>

// Constants
const float V_REF = 5.0;     // Analog reference voltage (e.g., 5V or 3.3V)
const float R_BITS = 10.0;   // ADC resolution (bits)
const float ADC_STEPS = (1 << int(R_BITS)) - 1; // Number of steps (2^R_BITS - 1)

const int slaveSelectPin = 53; 
const int LDACpin = 48;

const unsigned short CanalB = 0x1000; 
const unsigned short CanalC = 0x2000; //Ante es 256
const unsigned short CanalD = 0x3000; 

// GAIN INICIAL
// Amplificador del RF AD8367 requiere una tension de control entre 0V y +1V. por tanto N = [0 519] 
// Control FI1 es una palabra de 12bits
unsigned short GRF = 260;
// Amplificadores de FI VCA810 requieren una VC entre -2V y 0V, por tanto, como Vref=-5V -> N = [0 1038]
unsigned short GFI1 = 519; 
unsigned short GFI2 = 519; 


//Inicialización
unsigned short sRF=0x0000;  
unsigned short sFI1=0x1000;
unsigned short sFI2=0x2000;
byte r[2];


// Inicializan los pines de ADC
int analogPinRF = A0; 
int analogPinFI1 = A1;
int analogPinFI2 = A2;

// variable to store the value read
int valRF = 0; 
int valFI1 = 0;
int valFI2 = 0;


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

  // Lectura
  valRF = analogRead(analogPinRF);  // read the input pin
  float voltageRF = (valRF / ADC_STEPS) * V_REF; // Convert to voltage
  Serial.print("VRF=");
  Serial.println(voltageRF);
  float PRF = (voltageRF-1.954)/0.1019 ;
  Serial.print("PRF=");
  Serial.println(PRF);  

  valFI1 = analogRead(analogPinFI1);  // read the input pin
  float voltageFI1 = (valFI1 / ADC_STEPS) * V_REF; // Convert to voltage
  float PFI1 = 0.0;
  Serial.print("VFI1=");
  Serial.println(voltageFI1);
  if (voltageFI1 < 2.95)
  {
   PFI1 = (voltageFI1-3.2237)/0.1609 ;
  }
  else {
   PFI1 = 1.0;
  }
  Serial.print("PFI1=");
  Serial.println(PFI1);  

  valFI2 = analogRead(analogPinFI2);  // read the input pin
  float voltageFI2 = (valFI2 / ADC_STEPS) * V_REF; // Convert to voltage
  float PFI2 = 0.0;
  Serial.print("VFI2=");
  Serial.println(voltageFI2);
  if (voltageFI1 < 3.0)
  {
   PFI2 = (voltageFI1-3.188)/0.1617 ;
  }
  else {
   PFI2 = 1.0;
  }
  Serial.print("PFI2=");
  Serial.println(PFI2);  


// Calcula la nueva GRF
  if (voltageRF < 2.40 && GRF < 815) // 5V de 12 bits es 4096, para 1V es 819,2
    {
      GRF += 2;
    }
  else if (voltageRF > 2.20)
    {
      GRF -= 2;
      Serial.print("Se ha alcanzado Vmax de RF/n");
    } 
 sRF = GRF | CanalD;

  Serial.print("GRF=");
  Serial.println(GRF);  


  // Calcula la nueva GFI1
  if (voltageFI1 < 2.28 && GFI1 < 1638)
    {
      GFI1 += 2;
    }
  else if (voltageFI1 > 2.28)
    {
      GFI1 -= 2;
      Serial.print("Se ha alcanzado Vmax de FI1/n");
    }
   sFI1 = GFI1 | CanalC;
  Serial.print("GFI1=");
  Serial.println(GFI1);  


  // Calcula la nueva GFI2
  if (voltageFI2 < 2.5 && GFI2 < 1638)
    {
      GFI2 += 2;
    }
  else if (voltageFI2 > 2.5)
    {
      GFI2 -= 2;
      Serial.print("Se ha alcanzado Vmax de FI2/n");
    }
  sFI2 = GFI2 | CanalB;
  Serial.print("GFI2=");
  Serial.println(GFI2);  



  // Inicializar SPI
  SPI.begin();
  SPI.beginTransaction (SPISettings (8000000, MSBFIRST, SPI_MODE0));

  //Escritura
 
  digitalWrite(slaveSelectPin, LOW);
  r[0] = SPI.transfer (highByte(sRF));
  r[1] = SPI.transfer (lowByte(sRF));
  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(LDACpin, LOW);
  delay(0.1);
  digitalWrite(LDACpin, HIGH);
  //Serial.println(sRF);


  digitalWrite(slaveSelectPin, LOW);
  r[0] = SPI.transfer (highByte(sFI1));
  r[1] = SPI.transfer (lowByte(sFI1));
  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(LDACpin, LOW);
  delay(0.1);
  digitalWrite(LDACpin, HIGH);

  digitalWrite(slaveSelectPin, LOW);
  r[0] = SPI.transfer (highByte(sFI2));
  r[1] = SPI.transfer (lowByte(sFI2));
  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(LDACpin, LOW);
  delay(0.1);
  digitalWrite(LDACpin, HIGH);


  SPI.endTransaction();

  delay(40);
} 


