/*
  ESP8266 WEMOS module
  
  >>>> there is NO diode beetween 5 V USB and 5 V pin <<<<
  
  1.- test de toutes les GPIOs avec un "blink"
  2.- test de la vitesse de réaction des ports, juste sur le port D4
  a.- avec un ordre standard Arduino (~1.5 usec)
  b.- avec une commande directe sur le registre de commande des périphériques (~80 nsec)
  -- voir la doc Espressif ou ce lien en raccourci: https://github.com/esp8266/esp8266-wiki/wiki/gpio-registers
  
  ce n'est pas utilisable n'importe comment car il faut désactiver les interrupts système, ce qui met hors service le RTOS de l'ESP. Mais pour qcq nanosecondes, ça doit passer si on a besoin d'une pulse aussi courte !

*/

// options pour compiler ce que l'on veut voir: l'une ou l'autre sinon les pulses ne sont pas visibles à l'oscillo...

#define Pulse false    // Pulse si true, Blink si false

/* **** ARDUINO use the "D" numbering for the ESP8266 pins ( GPIO 0 - GPIO 16 )!
	and are known from the environement (no need to define)

 #define D0   16	  // must be low at boot
 #define D1	   5   	// I2C: SCL
 #define D2 	 4   	// I2C: SDA
 #define D3 	 0   	// 10k pull-up
 #define D4 	 2		// 10k pull-up, with BuiltIn-LED
 #define D5 	14   	// SPI: SCK
 #define D6 	12   	// SPI: MISO
 #define D7 	13   	// SPI: MOSI
 #define D8 	15   	// SPI: SS or CS (= Slave_Select or Chip_Select), 10k pull-down, must be low at boot
 #define D9 	 3    // used for Rx UART USB
 #define D10   1	  // used for Tx UART USB

   A0: analog input max 3.2 volts
*/


#include <Arduino.h>
//#include "user_interface.h"       // pour les appels system de l'ESP


void display_1()
{
  Serial.println("-flash___1");
}

void display_2()
{
  Serial.println("-flash   2___");
}

void flash()
{
  digitalWrite(D0, LOW);
  delay(20);
  digitalWrite(D0, HIGH);

  digitalWrite(D1, LOW);
  delay(20);
  digitalWrite(D1, HIGH);

  digitalWrite(D2, LOW);
  delay(20);
  digitalWrite(D2, HIGH);

  digitalWrite(D3, LOW);
  delay(20);
  digitalWrite(D3, HIGH);
  
  digitalWrite(LED_BUILTIN, LOW);   // is D4, Turn the LED ON !
  delay(20);
  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(D5, LOW);
  delay(20);
  digitalWrite(D5, HIGH);

  digitalWrite(D6, LOW);
  delay(20);
  digitalWrite(D6, HIGH);

  digitalWrite(D7, LOW);
  delay(20);
  digitalWrite(D7, HIGH);
 
  digitalWrite(D8, LOW);
  delay(20);
  digitalWrite(D8, HIGH);

  // D9 & D10 are used for serial USB !!
}


// pour écrire directement dans le registre de contrôle des GPIO de l'ESP 

#define myPin D4               //   D4 => GPIO 2, (LED_BUILTIN)
#define myPinBit (1 << myPin)   // positionne le bit dans le registre de contrôle des ports
#define Port_Hi 0x60000304      // valeur du registre pour l'état HIGH
#define Port_Lo 0x60000308      // valeur du registre pour l'état LOW

// *********************************************

void setup()
{
  // tentative de passer la clock de 80 MHz(standard) à 160MHz
  // system_update_cpu_freq(160);    // ne semble pas fonctionner, même avec l'include "user_interface.h"
  //                                 // la clock reste à 80 MHz et ça pulse à  1,5 us
  
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(myPin,OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

 

  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print("Chip ID: ");
  Serial.println(ESP.getChipId());
  delay(1000);
  Serial.println("Runing from PlatformIO env");
  #if Pulse == true
    Serial.println("1+3 pulses sur D4");
  #endif
  delay(1000);
}

// the loop function runs over and over again forever
void loop()
{

#if Pulse == true
  // une pulse Arduino style
  //digitalWrite(LED_BUILTIN, LOW);   // clock standard à 80 MHz
  digitalWrite(LED_BUILTIN, HIGH);   // ça prend 1,55 us
  digitalWrite(LED_BUILTIN, LOW);   // soit du 319 KHz
 

  // écriture directe dans le registre des GPIO pour 4 pulses
  os_intr_lock();                           // <<<<<<<<<<<<<<<<< désactive les interrupts système, mais pas remarquable si 1 pulse... à tester

  // écriture "brute" pour 1 pulse
  WRITE_PERI_REG( 0x60000304, myPinBit );   // bit à 1
  WRITE_PERI_REG( 0x60000308, myPinBit );   // bit à 0, temps 88 ns, ou freq =~ 5,7 MHz

  // écriture "Arduino style" pour une même pulse
  WRITE_PERI_REG( Port_Hi, myPinBit ); 
  WRITE_PERI_REG( Port_Lo, myPinBit );

  // écriture "brute" pour 1 pulse
  WRITE_PERI_REG( 0x60000304, myPinBit );
  WRITE_PERI_REG( 0x60000308, myPinBit );

  os_intr_unlock();                         // <<<<<<<<<<<<<<<<< réactive les interrupts système, mais ce loop tourne en 8 us

  delay(1);

#else

  flash();
  display_1();
  delay(500);
  flash();
  delay(200);
  flash();
  display_2();
  delay(1000);

  #endif
}
