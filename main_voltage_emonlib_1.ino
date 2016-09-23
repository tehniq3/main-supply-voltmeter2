// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
// https://github.com/openenergymonitor/EmonLib
// https://openenergymonitor.org/emon/buildingblocks/measuring-voltage-with-an-acac-power-adapter

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

// adapted sketch by niq_ro from http://arduinotehniq.blogspot.com
// and http://nicuflorica.blogspot.ro
// version 1.0 in 22.09.2016, Craiova - Romania
// source for LEDControl: http://embedded-lab.com/blog/?p=6862
#include "LedControl.h" 
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);

float vtot, tensiune;

void setup()
{  
  Serial.begin(9600);

  emon1.voltage(2, 250, 1.7);  // Voltage: input pin, calibration, phase_shift
//  emon1.current(1, 111.1);       // Current: input pin, calibration.

// Initialize MAX7219 device
lc.shutdown(0,false); // Enable display 
lc.setIntensity(0,8); // Set brightness level (0 is min, 15 is max) 
lc.clearDisplay(0); // Clear display register 

                                                 
lc.setRow(0,6,B1100010);  // afisez litera "n" pe coloane 6
lc.setRow(0,5,B0100000);  // afisez litera "i" pe coloane 5
lc.setRow(0,4,B1110011);  // afisez litera "q" pe coloane 4
lc.setRow(0,3,B0000001);  // afisez semnul "-" pe coloane 3
lc.setRow(0,2,B1000010);  // afisez litera "r" pe coloane 2
lc.setRow(0,1,B1100011);  // afisez litera "o" pe coloane 1
delay(2000);
lc.clearDisplay(0); // Clear display register 

}

void loop()
{
for (byte i=0; i < 20; i++){    
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  tensiune   = emon1.Vrms;             //extract Vrms into Variable
  vtot = vtot + tensiune; 
delay(200);
}
tensiune = vtot/20;
 Serial.println("--------------------");
 Serial.print(tensiune);
 Serial.println("V");

//tensiune = 231.4;  / for test display style
tensiune = 10*tensiune;

int vs = tensiune/1000;
int vzu = tensiune - 1000*vs;
int vz = vzu/100;
int vum = vzu - 100*vz;
int vu = vum/10;
int vm = vum - 10*vu;
  
lc.setDigit(0,3,vs, false); // afisez sute pe coloana 3
lc.setDigit(0,2,vz, false); // afisez zeci pe coloana 2
lc.setDigit(0,1,vu, true);  // afisez unitati pe coloana 1 si virgula (punct)
lc.setDigit(0,0,vm, false); // afisez cifra dupa virgula pe coloana 0
delay(1000);
//lc.clearDisplay(0); // Clear display register 
}
