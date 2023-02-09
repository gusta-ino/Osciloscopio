// inventor[KR]

#include <Wire.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>

#include <avr/pgmspace.h>               

#include <EEPROM.h>



#define SCREEN_WIDTH 128               

#define SCREEN_HEIGHT 64               

#define REC_LENGTH 200            



#define OLED_RESET     -1      

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



//

const char vRangeName[10][5] PROGMEM = {"A50V", "A 5V", " 50V", " 20V", " 10V", "  5V", "  2V", "  1V", "0.5V", "0.2V"}; 

const char * const vstring_table[] PROGMEM = {vRangeName[0], vRangeName[1], vRangeName[2], vRangeName[3], vRangeName[4], vRangeName[5], vRangeName[6], vRangeName[7], vRangeName[8], vRangeName[9]};

const char hRangeName[8][6] PROGMEM = {" 50ms", " 20ms", " 10ms", "  5ms", "  2ms", "  1ms", "500us", "200us"};          

const char * const hstring_table[] PROGMEM = {hRangeName[0], hRangeName[1], hRangeName[2], hRangeName[3], hRangeName[4], hRangeName[5], hRangeName[6], hRangeName[7]};



int waveBuff[REC_LENGTH];      

char chrBuff[10];             

String hScale = "xxxAs";

String vScale = "xxxx";



float lsb5V = 0.0055549;       

float lsb50V = 0.051513;       



volatile int vRange;           

volatile int hRange;           

volatile int trigD;           

volatile int scopeP;           

volatile boolean hold = false; 

volatile boolean paraChanged = false; 

volatile int saveTimer;        

int timeExec;                  



int dataMin;                  

int dataMax;                   

int dataAve;                   

int rangeMax;                  

int rangeMin;                  

int rangeMaxDisp;              

int rangeMinDisp;             

int trigP;                    

boolean trigSync;             

int att10x;                    



void setup() {

  pinMode(2, INPUT_PULLUP);    

  pinMode(8, INPUT_PULLUP);    

  pinMode(9, INPUT_PULLUP);    

  pinMode(10, INPUT_PULLUP);   

  pinMode(11, INPUT_PULLUP);  

  pinMode(12, INPUT);          

  pinMode(13, OUTPUT);        



  

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 

  { 

   

    for (;;);                               

  }

  loadEEPROM();                            

  analogReference(INTERNAL);               

  attachInterrupt(0, pin2IRQ, FALLING);    

  startScreen();                           

}



void loop() {

  digitalWrite(13, HIGH);

  setConditions();                         

  readWave();                              

  digitalWrite(13, LOW);                   

  dataAnalize();                           

  writeCommonImage();                      

  plotData();                              

  dispInf();                               

  display.display();                        

  saveEEPROM();                            

  while (hold == true) {                   

    dispHold();

    delay(10);

  }

}



void setConditions() {   

  

  strcpy_P(chrBuff, (char*)pgm_read_word(&(hstring_table[hRange])));  

  hScale = chrBuff;                                                   



  

  strcpy_P(chrBuff, (char*)pgm_read_word(&(vstring_table[vRange]))); 

  vScale = chrBuff;                                                   



  switch (vRange) {             

    case 0: {                    

       

        att10x = 1;              

        break;

      }

    case 1: {                    

        

        att10x = 0;             

        break;

      }

    case 2: {                    

        rangeMax = 50 / lsb50V;  

        rangeMaxDisp = 5000;     

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 1;              

        break;

      }

    case 3: {                    

        rangeMax = 20 / lsb50V; 

        rangeMaxDisp = 2000;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 1;              

        break;

      }

    case 4: {                    

        rangeMax = 10 / lsb50V;  

        rangeMaxDisp = 1000;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 1;              

        break;

      }

    case 5: {                    

        rangeMax = 5 / lsb5V;    

        rangeMaxDisp = 500;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 0;              

        break;

      }

    case 6: {                    

        rangeMax = 2 / lsb5V;    

        rangeMaxDisp = 200;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 0;              

        break;

      }

    case 7: {                   

        rangeMax = 1 / lsb5V;    

        rangeMaxDisp = 100;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 0;             

        break;

      }

    case 8: {                    

        rangeMax = 0.5 / lsb5V; 

        rangeMaxDisp = 50;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 0;             

        break;

      }

    case 9: {                    

        rangeMax = 0.2 / lsb5V;  

        rangeMaxDisp = 20;

        rangeMin = 0;

        rangeMinDisp = 0;

        att10x = 0;              

        break;

      }

  }

}



void writeCommonImage() {     

  display.clearDisplay();                  

  display.setTextColor(WHITE);              

  display.setCursor(86, 0);                 

  display.println(F("av    V"));            

  display.drawFastVLine(26, 9, 55, WHITE);  

  display.drawFastVLine(127, 9, 55, WHITE); 



  display.drawFastHLine(24, 9, 7, WHITE);  

  display.drawFastHLine(24, 36, 2, WHITE);  

  display.drawFastHLine(24, 63, 7, WHITE);  



  display.drawFastHLine(51, 9, 3, WHITE);   

  display.drawFastHLine(51, 63, 3, WHITE);  



  display.drawFastHLine(76, 9, 3, WHITE);   

  display.drawFastHLine(76, 63, 3, WHITE);  



  display.drawFastHLine(101, 9, 3, WHITE);  

  display.drawFastHLine(101, 63, 3, WHITE); 



  display.drawFastHLine(123, 9, 5, WHITE);  

  display.drawFastHLine(123, 63, 5, WHITE); 



  for (int x = 26; x <= 128; x += 5) {

    display.drawFastHLine(x, 36, 2, WHITE); 

  }

  for (int x = (127 - 25); x > 30; x -= 25) {

    for (int y = 10; y < 63; y += 5) {

      display.drawFastVLine(x, y, 2, WHITE); 

    }

  }

}



void readWave() {                            

  if (att10x == 1) {                        

    pinMode(12, OUTPUT);                     

    digitalWrite(12, LOW);                   

  } else {                                   

    pinMode(12, INPUT);                     

  }



  switch (hRange) {                          



    case 0: {                               

        timeExec = 400 + 50;                 

        ADCSRA = ADCSRA & 0xf8;              

        ADCSRA = ADCSRA | 0x07;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {     

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(1888);           

        }

        break;

      }



    case 1: {                                

        timeExec = 160 + 50;                 

        ADCSRA = ADCSRA & 0xf8;              

        ADCSRA = ADCSRA | 0x07;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {     

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(688);           

        }

        break;

      }



    case 2: {                               

        timeExec = 80 + 50;                  

        ADCSRA = ADCSRA & 0xf8;              

        ADCSRA = ADCSRA | 0x07;             

        for (int i = 0; i < REC_LENGTH; i++) 

        {     

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(288);            

        }

        break;

      }



    case 3: {                                

        timeExec = 40 + 50;                  

        ADCSRA = ADCSRA & 0xf8;             

        ADCSRA = ADCSRA | 0x07;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {     // 200

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(88);             

        }

        break;

      }



    case 4: {                                

        timeExec = 16 + 50;                  

        ADCSRA = ADCSRA & 0xf8;              

        ADCSRA = ADCSRA | 0x06;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {    

          waveBuff[i] = analogRead(0);       // MakeIdea

          delayMicroseconds(24);             // 

        }

        break;

      }



    case 5: {                                

        timeExec = 8 + 50;                   

        ADCSRA = ADCSRA & 0xf8;              

        ADCSRA = ADCSRA | 0x05;              

        for (int i = 0; i < REC_LENGTH; i++)

        {   

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(12);            

        }

        break;

      }



    case 6: {                                

        timeExec = 4 + 50;                   

        ADCSRA = ADCSRA & 0xf8;             

        ADCSRA = ADCSRA | 0x04;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {    

          waveBuff[i] = analogRead(0);       

          delayMicroseconds(4);              

          // MakeIdea

          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

          asm("nop"); asm("nop"); asm("nop");

        }

        break;

      }



    case 7: {                                

        timeExec = 2 + 50;                   

        ADCSRA = ADCSRA & 0xf8;             

        ADCSRA = ADCSRA | 0x02;              

        for (int i = 0; i < REC_LENGTH; i++) 

        {

          waveBuff[i] = analogRead(0);      

          // Hemant

          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

        }

        break;

      }

  }

}



void dataAnalize() {                  

  int d;

  long sum = 0;



  

  dataMin = 1023;                         

  dataMax = 0;                            

  for (int i = 0; i < REC_LENGTH; i++)

  {  

    d = waveBuff[i];

    sum = sum + d;

    if (d < dataMin) {                    

      dataMin = d;

    }

    if (d > dataMax) {                    

      dataMax = d;

    }

  }



  

  dataAve = (sum + 10) / 20;              



  

  if (vRange <= 1) {                       

    rangeMin = dataMin - 20;               

    rangeMin = (rangeMin / 10) * 10;       

    if (rangeMin < 0) {

      rangeMin = 0;                       

    }

    rangeMax = dataMax + 20;              

    rangeMax = ((rangeMax / 10) + 1) * 10; 

    if (rangeMax > 1020) {

      rangeMax = 1023;                     

    }



    if (att10x == 1) {                            

      rangeMaxDisp = 100 * (rangeMax * lsb50V);   

      rangeMinDisp = 100 * (rangeMin * lsb50V);   

    } else {                                      

      rangeMaxDisp = 100 * (rangeMax * lsb5V);

      rangeMinDisp = 100 * (rangeMin * lsb5V);

    }

  } else {                                   

    

  }



  

  for (trigP = ((REC_LENGTH / 2) - 51); trigP < ((REC_LENGTH / 2) + 50); trigP++)

  { 

    if (trigD == 0) {                       

      if ((waveBuff[trigP - 1] < (dataMax + dataMin) / 2) && (waveBuff[trigP] >= (dataMax + dataMin) / 2)) {

        break;                             

      }

    } else {                                

      if ((waveBuff[trigP - 1] > (dataMax + dataMin) / 2) && (waveBuff[trigP] <= (dataMax + dataMin) / 2)) {

        break;

      }                                    

    }

  }

  trigSync = true;

  if (trigP >= ((REC_LENGTH / 2) + 50)) 

  { 

    trigP = (REC_LENGTH / 2);

    trigSync = false;                      

  }

}



void startScreen() {                

  display.clearDisplay();

  display.setTextSize(1);            

  display.setTextColor(WHITE);      

  display.setCursor(10, 25);        

  display.println(F("MakeIdea"));   

  display.setCursor(10, 45);         

  display.println(F("Pocket Oscilloscope"));

  display.display();                 

  delay(5000);

  display.clearDisplay();

  display.setTextSize(1);            

}



void dispHold() {                            

  display.fillRect(32, 12, 24, 8, BLACK);    

  display.setCursor(32, 12);

  display.print(F("Hold"));                  

  display.display();                         

}



void dispInf() {                             

  float voltage;

  // 

  display.setCursor(2, 0);                   

  display.print(vScale);                     

  if (scopeP == 0) {                         

    display.drawFastHLine(0, 7, 27, WHITE);  

    display.drawFastVLine(0, 5, 2, WHITE);

    display.drawFastVLine(26, 5, 2, WHITE);

  }



  

  display.setCursor(34, 0);                  

  display.print(hScale);                     

  if (scopeP == 1) {                         

    display.drawFastHLine(32, 7, 33, WHITE); 

    display.drawFastVLine(32, 5, 2, WHITE);

    display.drawFastVLine(64, 5, 2, WHITE);

  }



 

  display.setCursor(75, 0);                  

  if (trigD == 0) {

    display.print(char(0x18));               

  } else {

    display.print(char(0x19));                            

  }

  if (scopeP == 2) {      

    display.drawFastHLine(71, 7, 13, WHITE); 

    display.drawFastVLine(71, 5, 2, WHITE);

    display.drawFastVLine(83, 5, 2, WHITE);

  }



 

  if (att10x == 1) {                         

    voltage = dataAve * lsb50V / 10.0;       

  } else {

    voltage = dataAve * lsb5V / 10.0;        

  }

  dtostrf(voltage, 4, 2, chrBuff);           

  display.setCursor(98, 0);                  

  display.print(chrBuff);                    

  //  display.print(saveTimer);                 



  // 

  voltage = rangeMaxDisp / 100.0;            

  if (vRange == 1 || vRange > 4) {           

    dtostrf(voltage, 4, 2, chrBuff);         

  } else {                                   

    dtostrf(voltage, 4, 1, chrBuff);        

  }

  display.setCursor(0, 9);

  display.print(chrBuff);                    



  voltage = (rangeMaxDisp + rangeMinDisp) / 200.0;

  if (vRange == 1 || vRange > 4) {           

    dtostrf(voltage, 4, 2, chrBuff);         

  } else {                                  

    dtostrf(voltage, 4, 1, chrBuff);        

  }

  display.setCursor(0, 33);

  display.print(chrBuff);                    



  voltage = rangeMinDisp / 100.0;            

  if (vRange == 1 || vRange > 4) {           

    dtostrf(voltage, 4, 2, chrBuff);         

  } else {

    dtostrf(voltage, 4, 1, chrBuff);         

  }

  display.setCursor(0, 57);

  display.print(chrBuff);                    



  

  if (trigSync == false) {                  

    display.setCursor(60, 55);               

    display.print(F("Unsync"));              

  }

}



void plotData() {                   

  long y1, y2;

  for (int x = 0; x <= 98; x++) {

    y1 = map(waveBuff[x + trigP - 50], rangeMin, rangeMax, 63, 9);  

    y1 = constrain(y1, 9, 63);                                      

    y2 = map(waveBuff[x + trigP - 49], rangeMin, rangeMax, 63, 9); 

    y2 = constrain(y2, 9, 63);                                     

    display.drawLine(x + 27, y1, x + 28, y2, WHITE);                

  }

}



void saveEEPROM() {                    

  if (paraChanged == true) {           

    saveTimer = saveTimer - timeExec;  

    if (saveTimer < 0) {               

      paraChanged = false;             

      EEPROM.write(0, vRange);        

      EEPROM.write(1, hRange);

      EEPROM.write(2, trigD);

      EEPROM.write(3, scopeP);

    }

  }

}



void loadEEPROM() {                

  int x;

  x = EEPROM.read(0);             

  if ((x < 0) || (x > 9)) {        

    x = 3;                         

  }

  vRange = x;



  x = EEPROM.read(1);             

  if ((x < 0) || (x > 7)) {        

    x = 3;                         

  }

  hRange = x;

  x = EEPROM.read(2);             

  if ((x < 0) || (x > 1)) {        

    x = 1;                         

  }

  trigD = x;

  x = EEPROM.read(3);             

  if ((x < 0) || (x > 2)) {        

    x = 1;                         

  }

  scopeP = x;

}



void pin2IRQ() {                  

  

 

  int x;                           

  x = PINB;                        



  if ( (x & 0x07) != 0x07) {      

    saveTimer = 5000;              

    paraChanged = true;           

  }



  if ((x & 0x01) == 0) {

    scopeP++;

    if (scopeP > 2) {

      scopeP = 0;

    }

  }



  if ((x & 0x02) == 0) {           

    if (scopeP == 0) {            

      vRange++;

      if (vRange > 9) {

        vRange = 9;

      }

    }

    if (scopeP == 1) {             

      hRange++;

      if (hRange > 7) {

        hRange = 7;

      }

    }

    if (scopeP == 2) {            

      trigD = 0;                  

    }

  }



  if ((x & 0x04) == 0) {           

    if (scopeP == 0) {            

      vRange--;

      if (vRange < 0) {

        vRange = 0;

      }

    }

    if (scopeP == 1) {             

      hRange--;

      if (hRange < 0) {

        hRange = 0;

      }

    }

    if (scopeP == 2) {            

      trigD = 1;                  

    }

  }



  if ((x & 0x08) == 0) {           

    hold = ! hold;                 

  }


}
