// main voltmeter on MAX7219 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/
// use info from http://nicuflorica.blogspot.ro/2016/09/releu-de-protectie-la-minima-si-maxima_21.html
// version 3, 24.09.2016, Craiova - Romania  
// ver.3a - clear empty digits
// ver.3b - display voltage with number after comma xxx.x
// ver.3c - put U = xxx.x
// ver.4 - add LCD1602 with i2c interface

// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
// https://github.com/openenergymonitor/EmonLib
// https://openenergymonitor.org/emon/buildingblocks/measuring-voltage-with-an-acac-power-adapter

#include <EEPROM.h> // http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>   // https://github.com/tehniq3/used_library: https://github.com/tehniq3/used_library/tree/master/LiquidCrystal_I2C

// Set the LCD address to 0x3F (or 0x27) for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define sub 7     // pin for undervoltage led
#define bun 6     // pin for ok voltage led
#define peste 5   // pin for overvoltage led
#define releu 8   // pin for relay
#define meniu 4   // pin for MENU button
#define minus 3   
#define plus 2

float tensiune, voltage, voltagetr, vtot;
int vs, vsz, vz, vzu, vu, vv;
float u1, u0;

float under;  // under voltage value
float over;   // over voltage value
int intarziere;  // value in second for delay after disconect relay because voltage is not ok
int intarziere2 = 1;  // value in second for delay after reading when voltage is ok
int nivel = 0;
int underzu, overzu;
int under1 = 190; 
int under2 = 207;
int over1 = 240;
int over2 = 255;

// uncomment block first time, for write datas in EEPROM memory
/*
// under voltage is 190V ->
EEPROM.write(101,1);
EEPROM.write(102,9);
EEPROM.write(103,0);
// overvoltage is 250V ->
EEPROM.write(104,2);
EEPROM.write(105,5);
EEPROM.write(106,0);
// delay time for reconnect relay -> 9 seconds
EEPROM.write(107,0);
EEPROM.write(108,9);
*/

int unders = EEPROM.read(101);
int underz = EEPROM.read(102);
int underu = EEPROM.read(103);
int overs = EEPROM.read(104);
int overz = EEPROM.read(105);
int overu = EEPROM.read(106);
int intarzierez = EEPROM.read(107);
int intarziereu = EEPROM.read(108);


void setup(){    
  lcd.begin();  // initialize the LCD
  lcd.backlight();  // Turn on the blacklight and print a message.
  lcd.clear();  // clear display
  
 pinMode(A2, INPUT); 
 pinMode(meniu, INPUT); 
 pinMode(plus, INPUT); 
 pinMode(minus, INPUT); 
 pinMode(sub, OUTPUT);
 pinMode(bun, OUTPUT);
 pinMode(peste, OUTPUT);
 pinMode(releu, OUTPUT);

 digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
 digitalWrite(plus, HIGH);
 digitalWrite(minus, HIGH);
 digitalWrite(sub, LOW);
 digitalWrite(bun, LOW);
 digitalWrite(peste, LOW); 
 digitalWrite(releu, LOW); 
  

under = 100*unders + 10*underz + underu;
if (under < under1) under = under1;
if (under > under2) under = under2;
over = 100*overs + 10*overz + overu;
if (over < over1) over = over1;
if (over > over2) over = over2;
intarziere = 10*intarzierez + intarziereu;
if (intarziere < 1) intarziere = 1;
if (intarziere > 99) intarziere = 99;

emon1.voltage(2, 255, 1.7);  // Voltage: input pin, calibration, phase_shift

lcd.setCursor(0, 0);
lcd.print("  Under & over  ");     
lcd.setCursor(0, 1);
lcd.print("  voltage relay ");
delay (1500);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print(" original sketch");     
lcd.setCursor(0, 1);
lcd.print(" by Nicu FLORICA");
delay (1500);
lcd.clear();
vtot = 0;

}//end "setup()"
 
 
 
void loop(){     //Start of Program 

if (nivel == 0) {     // measurement state 
for (byte i=0; i < 10; i++){  
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  tensiune   = emon1.Vrms;             //extract Vrms into Variable
  vtot = vtot + tensiune; 
delay(200);
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  i = 10;
  delay(500);
}    
}

delay(50);
voltage = vtot/10;
// calculate number for each figure number
vs = vtot /1000;
vsz = vtot - 1000*vs;
vz = vsz /100;
vzu = vsz - 100*vz;
vu = vzu /10; 
vv = vzu - 10*vu; 
vtot = 0;

lcd,setCursor(0,0);
lcd.print("U = ");
lcd.print(vs);
lcd.print(vu);
lcd.print(",");
lcd.print(vu);
lcd.print("V");

if (digitalRead(meniu) == LOW) {
  nivel = 1;
  delay(500);
  lcd.clear();
}    
}

if (nivel == 0) {     // measurement state 
if (voltage <= under)  // undervoltage
{
digitalWrite(sub, HIGH);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
digitalWrite(releu, LOW);
lcd,setCursor(0,1);
lcd.print(" Undervoltage ! ");
for (byte j=0; j < intarziere; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere;
  delay(500);
}    
delay(1000);
}
}
if (voltage >= over)  // overvoltage
{
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, HIGH);
digitalWrite(releu, LOW);
lcd,setCursor(0,1);
lcd.print(" Overvoltage !  ");
for (byte j=0; j < intarziere; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere;
  delay(500);
}    
delay(1000);
}

}  
if ((voltage > under) && (voltage < over)) 
{ 
digitalWrite(sub, LOW);
digitalWrite(bun, HIGH);
digitalWrite(peste, LOW);
digitalWrite(releu, HIGH);
lcd,setCursor(0,1);
lcd.print(" Voltage is OK! ");
for (byte j=0; j < intarziere2; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere2;
  delay(500);
}    
delay(1000);
}

}
}    // end loop for measurement

if (nivel == 1) {    // for change the value
lcd.clear(); 
lcd,setCursor(0,0);
lcd.print("Adjust values ! ");
delay(2000);
nivel = 2;
lcd.clear(); // Clear display register 
}

if (nivel == 2) {    // for change the undervoltage value
unders = under / 100;
underzu = under - 100*unders;
underz = underzu/10;
underu = underzu - 10*underz;

lcd,setCursor(0,0);
lcd.print("Minimum voltage:");
lcd,setCursor(0,1);
lcd.print("     ");
lcd.print(unders);
lcd.print(underz);
lcd.print(underu);
lcd.print("V ");

if (digitalRead(meniu) == LOW) {
  nivel = 3;
EEPROM.write(101,unders);
EEPROM.write(102,underz);
EEPROM.write(103,underu);
  delay(500);
  lcd.clear(); // Clear display register 
}    

if (digitalRead(plus) == LOW) {
  under = under + 1;
  if (under > under2) under = under2;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  under = under - 1;
  if (under < under1) under = under1;
  delay(250);
}    

}  // end loop for change undervoltage value   
   
if (nivel == 3) {    // for change the overrvoltage value
overs = over / 100;
overzu = over - 100*overs;
overz = overzu/10;

lcd,setCursor(0,0);
lcd.print("Maximum voltage:");
lcd,setCursor(0,1);
lcd.print("     ");
lcd.print(overs);
lcd.print(overz);
lcd.print(overu);
lcd.print("V ");

if (digitalRead(meniu) == LOW) {
  nivel = 4;
EEPROM.write(104,overs);
EEPROM.write(105,overz);
EEPROM.write(106,overu);
  delay(500);
 lcd.clear(); // Clear display register 
}    

if (digitalRead(plus) == LOW) {
  over = over + 1;
  if (over > over2) over = over2;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  over = over - 1;
  if (over < over1) over = over1;
  delay(250);
}    

}  // end loop for change overvoltage value   

if (nivel == 4) {    // for change delay time for nook voltage
intarzierez = intarziere / 10;
intarziereu = intarziere - 10*intarzierez;

lcd.setCursor(0,0);
lcd.print("Delay on:");
lcd,setCursor(0,1);
lcd.print("      ");
lcd.print(intarzierez);
lcd.print(intarzierez);
lcd.print("sec ");
  
if (digitalRead(meniu) == LOW) {
  nivel = 0;
EEPROM.write(107,intarzierez);
EEPROM.write(108,intarziereu);
  delay(500);
  lcd.clear(); // Clear display register 
}    

if (digitalRead(plus) == LOW) {
  intarziere = intarziere + 1;
  if (intarziere > 99) intarziere = 99;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  intarziere = intarziere - 1;
  if (intarziere < 1) intarziere = 1;
  delay(250);
}    

}  // end loop for change delay value  for nook voltage
   
 
}// end loop() 
