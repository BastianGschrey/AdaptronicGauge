//
// This program requires the UTFT library by Henning Carlsen
//uncomment line "#define CTE_DUE_SHIELD 1" UTFT\hardware\arm\HW_ARM_defines.h
//install ModBusMaster in IDE

#include <UTFT.h> //uncomment line "#define CTE_DUE_SHIELD 1" UTFT\hardware\arm\HW_ARM_defines.h 
#include <ModbusMaster.h>
#include <UTFT_Geometry.h>

#define TOUCH_ORIENTATION  LANDSCAPE

//Load RAW Images (Huge memory consumption!)
//extern unsigned short EFINI[0x11080];
//extern unsigned short efini_text[0x39F8];

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t Inconsola[];
extern uint8_t Ubuntubold[];
extern uint8_t SixteenSegment[];


UTFT myGLCD(CTE50, 25, 26, 27, 28); // Define Pins for LCD
UTFT_Geometry geo(&myGLCD);          //Pointer for Geo

ModbusMaster ecu;
bool inProgress = false;

uint16_t data[6]; //6 words for the 6 values displayed by gauge
uint8_t j, result;
float MAP; //variable to calcucate MAP
float AFR; // variable to calcuate AFR



void setup() {
  Serial.begin(57600); //open Serial1 Port


  ecu.begin(1, Serial); //open Modbus socket

  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  // Print Splash Screen
  int buf[798];
  int x, x2;
  int y, y2;
  int r;

  myGLCD.clrScr();
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(0, 0, 800, 480);

  //myGLCD.drawBitmap (240, 130, 320, 218, EFINI);
  //myGLCD.drawBitmap (285, 390, 212, 70, efini_text);

  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("Adaptronic(R) Multifunction Display v0.1", CENTER, 1);
  myGLCD.print("Bastian Gschrey - http://www.fd3s.de", RIGHT, 460);

  delay(3500);
  printLayout();
}



void loop()
{
  delay(50);
  //delay(1000);
  readModbusRegister();


}



void readModbusRegister()
{

  //Serial.flush();
  ecu.clearTransmitBuffer();
  ecu.readHoldingRegisters(4097, 6); //read 6 registers beginning from offest 4097

  if (result == ecu.ku8MBSuccess)  //if transmission is good, decode Data
  {
    decodeModbusRegister();

  }

}


void decodeModbusRegister()  //read single word registers and write into data array
{

  for (j = 0; j < 6; j++)
  {
    data[j] = ecu.getResponseBuffer(j);
  }

  printValues(); //after decoding, print data
}



void printValues()
{
  //print Values to screen:
  myGLCD.setFont(Inconsola);

  //----------MAP Print----------

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(43, 47, 56);


  if (data[0] > 500) {
    //do nothing
  }
  else {
    myGLCD.printNumI(data[0], 284, 110, 3, ' ');


    myGLCD.fillRect(27, 102, 27 + (data[0] / 2), 138);
    if (data[0] < 497) {
      myGLCD.setColor(0, 0, 0);
      myGLCD.fillRect(27 + (data[0] / 2), 102, 274, 138);
    }
  }

  //---------Air Temp Print---------

if(data[1] > 125){
  
}
else{
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(43, 47, 56);

  myGLCD.printNumI(data[1], 295, 250, 3, ' ');
  myGLCD.fillRect(27, 242, 27 + (data[1] * 2), 279);
  myGLCD.setColor(0, 0, 0);
  if (data[1] < 125) {
    myGLCD.fillRect(27 + (data[1] * 2), 242, 274, 279);
  }
}
  //------------Oiltemp------------


  if (data[3] < 60) myGLCD.setColor(VGA_BLUE);
  if (data[3] > 110) myGLCD.setColor(VGA_RED);
  if (data[3] >= 60 && data[3] <= 110) myGLCD.setColor(VGA_GREEN);
  myGLCD.fillRect(27, 382, 27 + (data[3] * 1.67), 418);
  myGLCD.printNumI(data[3], 295, 390, 3, ' ');

  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(27 + (data[3] * 1.67), 382, 274, 418);





  //--------AFR Print---------


  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(43, 47, 56);

  AFR = data[4] / 2570.00;

  //DEBUG!!!
  //myGLCD.printNumF(AFR, 2 , 680, 110, '.', 5, ' ');





  if (AFR <= 7.35) {
    myGLCD.setColor(255, 140, 0);
    myGLCD.print("RICH ", 680, 110);
  }
  if (AFR >= 22.00) {
    myGLCD.setColor(VGA_RED);
    myGLCD.print("LEAN ", 680, 110);
    myGLCD.fillRect(427, 102, 674, 138);
  }



  if (AFR > 7.36 && AFR < 22.00) {

    if (AFR < 10) myGLCD.setColor(255, 140, 0);
    if (AFR >= 10 && AFR <= 14.7) myGLCD.setColor(VGA_GREEN);
    if (AFR > 14.8) myGLCD.setColor(VGA_RED);

    myGLCD.printNumF(AFR, 1 , 680, 110, '.', 5, ' ');
    myGLCD.fillRect(427, 102, 427 + ((AFR - 7.36) * 17), 138);
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(427 + ((AFR - 7.36) * 17), 102, 674, 138);
  }



  //------Water Temp----




  //myGLCD.setColor(255, 255, 255);

  if (data[2] < 85) myGLCD.setColor(VGA_BLUE);
  if (data[2] > 99) myGLCD.setColor(VGA_RED);
  if (data[2] >= 85 && data[2] <= 99) myGLCD.setColor(VGA_GREEN);

  myGLCD.setBackColor(43, 47, 56);

  myGLCD.printNumI(data[2], 680, 250, 3, ' ');
  myGLCD.fillRect(427, 242, 427 + (data[2] * 1.675), 279);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(427 + (data[2] * 1.67), 242, 674, 279);




  //----- Knock Print-----------
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(43, 47, 56);

  if ((data[5] / 256) > 60) myGLCD.setColor(VGA_RED);
  else myGLCD.setColor(255, 255, 255);
  if (data[5] == 0) {
    myGLCD.printNumI(0, 680, 390, 3, ' ');
  }
  else
  {
    myGLCD.printNumI(data[5] / 256, 680, 390, 3, ' ');

    //if ((data[5] / 256) > 60) myGLCD.setColor(VGA_RED);
    myGLCD.printNumI(data[5] / 256, 680, 390, 3, ' ');
    myGLCD.fillRect(427, 382, 427 + ((data[5] / 256)), 418);
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(427 + ((data[5] / 256)), 382, 674, 418);
  }


}


void printLayout()
{
  // This Function Print the 3x2 Rectangle Menu
  myGLCD.setColor(43, 47, 56);
  myGLCD.fillRect(0, 0, 800, 480);



  myGLCD.setColor(VGA_WHITE);

  myGLCD.drawRect(25, 100, 276, 140);
  myGLCD.drawRect(26, 101, 277, 139);

  myGLCD.drawRect(425, 100, 676, 140);
  myGLCD.drawRect(426, 101, 677, 139);


  myGLCD.drawRect(25, 240, 276, 280);
  myGLCD.drawRect(26, 241, 277, 281);

  myGLCD.drawRect(425, 240, 676, 280);
  myGLCD.drawRect(426, 241, 677, 281);


  myGLCD.drawRect(25, 380, 276, 420);
  myGLCD.drawRect(26, 381, 277, 419);

  myGLCD.drawRect(425, 380, 676, 420);
  myGLCD.drawRect(426, 381, 677, 419);

  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRect(27, 102, 274, 138);
  myGLCD.fillRect(27, 242, 274, 278);
  myGLCD.fillRect(27, 382, 274, 418);

  myGLCD.fillRect(427, 102, 674, 138);
  myGLCD.fillRect(427, 242, 674, 278);
  myGLCD.fillRect(427, 382, 674, 418);
  myGLCD.setColor(VGA_WHITE);
  /*myGLCD.drawLine(126,95,126,144);
    myGLCD.drawLine(127,95,127,144);*/
  myGLCD.setFont(Ubuntubold);
  myGLCD.setBackColor(43, 47, 56);


  myGLCD.print("MAP", 25, 60);

  myGLCD.print("Airtemp", 25, 200);

  myGLCD.print("Oiltemp", 25, 340);

  myGLCD.print("Air/Fuel", 425, 60);

  myGLCD.print("Watertemp", 425, 200);

  myGLCD.print("Knock", 425, 340);

  myGLCD.setFont(Inconsola);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(43, 47, 56);

  //Left units
  myGLCD.print("kPa", 353, 110);
  myGLCD.print("`C", 360, 390);
  myGLCD.print("`C", 360, 250);
  //right units
  myGLCD.print("`C", 750, 250);
}
