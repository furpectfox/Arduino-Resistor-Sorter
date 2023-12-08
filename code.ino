#include <tinySPI.h>            // https://github.com/JChristensen/tinySPI

// pin definitions
const int
    LATCH_PIN(0),               // storage register clock (slave select)
    DATA_PIN(1),                // data in
    CLOCK_PIN(2);               // shift register clock

unsigned long previousMillis = 0;    
const long interval = 500;     // how long to display number until next measurement 
                                //(how long before update display)
byte d1, d2, d3, d4, dp;        //store 4 digits and 1 decimal point
unsigned int R = 0;             //value of R to find


//*****************************************************************************************************
void setup() {
  // put your setup code here, to run once:
  SPI.begin();                   // start hardware SPI (the library sets up the clock and data pins)
  pinMode(LATCH_PIN, OUTPUT);    // latch pin needs to be set up for hardware or software
  digitalWrite(LATCH_PIN, HIGH);
  pinMode(4, INPUT_PULLUP);      //pullups on for unused pins
}


//*****************************************************************************************************
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  display_num(d1, d2, d3, d4, dp);

  if (currentMillis - previousMillis >= interval) {
    measure_resistance();
    previousMillis = millis();
  }
}


//*****************************************************************************************************
void display_num(byte d1, byte d2, byte d3, byte d4, byte dp)
{
  byte chars[12]
  {
    0b11011101, //0
    0b00000101, //1
    0b10011011, //2
    0b10010111, //3
    0b01000111, //4
    0b11010110, //5
    0b11011110, //6
    0b10000101, //7
    0b11011111, //8
    0b11010111, //9
    0b00000000, //empty
    0b11011010, //E
  };
  
  byte pos = 0; // select which digit on the display to be turned ON
  byte data = 0;// binary code for a digit

  for(byte i = 1; i <= 8; i = i << 1)
  {
    switch(i)
    {
      case 1:
        data = chars[d1] | dp;  //add decimal point after first digit if is asked
        break;
      case 2:
        data = chars[d2];
        break;
      case 4: //case 3
        data = chars[d3];
        break;
      case 8:
        data = chars[d4];
        break;
    } 
    
    pos = i ^ B1111; // (ex: turn 0010 to 1101) 0 mean that pin is connected to GND
  
    digitalWrite(LATCH_PIN, LOW);
    SPI.transfer(pos << 1); //first transistor starts at second bit
    SPI.transfer(data);
    digitalWrite(LATCH_PIN, HIGH); 
  }
}


//*****************************************************************************************************
void measure_resistance()
{
  unsigned int raw = 0, R = 0, multiplier;
  float buffer, vout;
  byte Pow=0;


  for(unsigned int b = 1; b <= 16; b = b << 1)
  {
    digitalWrite(LATCH_PIN, LOW);
    SPI.transfer(b << 1); // b starts from second bit
    SPI.transfer(0b00000000);
    digitalWrite(LATCH_PIN, HIGH);

    for(int i=0; i<3; ++i) //read three times and discard first two values
    {
      raw = analogRead(A3);
    }

    if     (b==1) //1Mohms
    {
      if(raw > 682) continue;
      Pow = 5;
    }
    else if(b==2) //100Kohms
    {
      if(raw < 170 || raw > 682) continue;
      Pow = 4;
    }
    else if(b==4) //10Kohms
    {
      if(raw < 170 || raw > 682) continue;     
      Pow = 3;
    }
    else if(b==8) //1Kohms
    {
      if(raw < 170 || raw > 682) continue;
      multiplier = 1000;
      Pow = 0;
    }
    else          //100ohms
    {
      if(raw < 170) continue;
      multiplier = 100;
      Pow = 0;
    }


    if(Pow!=0)
    {
      if(raw>512)
      {
        multiplier = 100;
        dp = 0b00100000;  //turn on decimal point
      }
      else
      {
        multiplier = 10;
        dp = 0b00000000;  //turn off decimal point
      }
    }
    
   
    //calculation:
    buffer = raw * 5.0;
    vout = buffer / 1024.0;
    buffer = 5.0 / vout - 1;
    R = multiplier*buffer;  
      

    if(Pow !=0)
    {
      d1 = R/10 % 10;
      d2 = R % 10;
      d3 = 11;
      d4 = Pow;
    }
    else
    {
      d1 = R/1000 % 10;
      d2 = R/100 % 10;
      d3 = R/10 % 10;
      d4 = R % 10;
    }
    //delay(1000);
  }
}
