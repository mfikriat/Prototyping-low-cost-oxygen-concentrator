/*
   Receive data sent by the OCS-3F sensor and display it on the 1602LCD
  Compatible with the Arduino IDE 1.8.5
  Library version:1.1
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

int Sieve_A_Valve = 5; //Defined Pin as Variable
int Sieve_B_Valve = 6; //Defined Pin as Variable
int PreCharge_Valve = 7; //Defined Pin as Variable
// VARIABLE CREATION
//**************************************************************************
unsigned long Relay_Test_Delay; //delay variable creation
unsigned long Startup_Purge_Delay; //delay variable creation
unsigned long Production_Delay; //delay variable creation
unsigned long Flush_Delay; //delay variable creation
unsigned long PreCharge_Delay; //delay variable creation

void setup()
{
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  Serial.begin(9600);

   // STARTUP
  //**************************************************************************
  // Serial Port initialization
  Serial.begin(9600);


  // SET PIN MODE FOR PINS IN PROGRAM
  //**************************************************************************
  pinMode(Sieve_A_Valve, OUTPUT);
  pinMode(Sieve_B_Valve, OUTPUT);
  pinMode(PreCharge_Valve, OUTPUT);

  //  SET DELAY TIMING HERE
  //**************************************************************************
  Relay_Test_Delay = 0;
  Startup_Purge_Delay = 1000;
  Production_Delay = 15000;
  Flush_Delay = 450;
  PreCharge_Delay = 700;

  // VALVE RELAY TEST SEQUENCE
  //**************************************************************************
  Serial.println("Relay Test Sequence");
  digitalWrite(Sieve_A_Valve, HIGH); //Turn on relay
  delay(Relay_Test_Delay);
  digitalWrite(Sieve_B_Valve, HIGH); //Turn on relay
  delay(Relay_Test_Delay);
  digitalWrite(PreCharge_Valve, HIGH); //Turn on relay
  delay(Relay_Test_Delay);
  Serial.println("Valve Relay Test Sequence Complete");
  delay(Relay_Test_Delay);


  // STARTUP PURGE
  //**************************************************************************
  Serial.println("Relay Test Sequence");
  digitalWrite(Sieve_A_Valve, HIGH); //Turn on relay
  digitalWrite(Sieve_B_Valve, HIGH); //Turn on relay
  digitalWrite(PreCharge_Valve, HIGH); //Turn on relay
  delay(Startup_Purge_Delay);

}

void loop()
{
  typedef unsigned char u8;
  typedef unsigned int u16;
  int inByte;
  u8 temp;
  u8 i, j, o2[12];
  u16 o2c, o2f, o2t; //Define oxygen concentration, flow rate and temperature

  //CYCLE 1
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Purge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, LOW);
  digitalWrite(PreCharge_Valve, LOW);
  delay(Production_Delay);


  //CYCLE 2
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Purge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, LOW);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(Flush_Delay) ;


  //CYCLE 3
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(PreCharge_Delay);

  //CYCLE 4
  //**************************************************************************
  Serial.println("Sieve A Purge / Sieve B Charge");
  digitalWrite(Sieve_A_Valve, LOW);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, LOW);
  delay(Production_Delay);

  //CYCLE 5
  //**************************************************************************
  Serial.println("Sieve A Purge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, LOW);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(Flush_Delay);


  //CYCLE 6
  //**************************************************************************
  Serial.println("Sieve A Charge / Sieve B Charge / Flush/PreCharge");
  digitalWrite(Sieve_A_Valve, HIGH);
  digitalWrite(Sieve_B_Valve, HIGH);
  digitalWrite(PreCharge_Valve, HIGH);
  delay(PreCharge_Delay) ;

  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {

      inByte = Serial.read();
      
      //----Receiving part----
      if ((o2[0] == 0x16) && (o2[1] == 0x09) && (o2[2] == 0x01)) //Determine if the first two bytes are received correctly, I is the global variable
      {
        o2[i] = inByte;       
        i++;
      }
      else           //If one of the first three bytes received is incorrect, the first two bytes will be judged
      {
        if ((o2[0] == 0x16) && (o2[1] == 0x09))
        {
          if ( inByte == 0x01)   
          {
            o2[2] =  inByte;   
            i++;
          }
          else                                
          {
            i = 0;                
            for (j = 0; j < 12; j++)          
            {
              o2[j] = 0;
            }
          }
        }
        else     
        {
          if (o2[0] == 0x16)
          {
            if ( inByte == 0x09)
            {
              o2[1] =  inByte; 
              i++;
            }
            else        
            {
              i = 0;                
              for (j = 0; j < 12; j++)          
              {
                o2[j] = 0;
              }
            }
          }
          else     
          {
            if ( inByte == 0x16) 
            {
              o2[0] =  inByte; 
              i++;
            }
            else         
            {
              i = 0;                 
              for (j = 0; j < 12; j++)         
              {
                o2[j] = 0;
              }
            }
          }
        }
      }
     //----Receiving part----



      if (i == 12)   //Data received complete, start calibration
      {
        temp = 0;
        for (j = 0; j < 12; j++)
        {
          temp += o2[j];
        }
        if (temp == 0)     //Check passed, calculate oxygen concentration, flow, temperature value
        {
          o2c = o2[3] * 256 + o2[4];     //Oxygen concentration
          o2f = o2[5] * 256 + o2[6];     //Oxygen flow value
          o2t = o2[7] * 256 + o2[8];     //Oxygen temperature
        }

      

        i = 0;                             
        for (j = 0; j < 12; j++)           //Initialize array
        {
          o2[j] = 0;
        }

    //---O2
     lcd.setCursor(0, 0);               //Set the display pointer
     lcd.print("O2:");
     lcd.setCursor(5, 0);              
     lcd.print(o2c/100);
     lcd.print(o2c/10%10);
     lcd.print(".");
     lcd.print(o2c%10);
     lcd.print("%");

    //---Flow
     lcd.setCursor(0, 1);               
     lcd.print("Flow:");
     lcd.setCursor(6, 1);              
     lcd.print(o2f/10%10);
     lcd.print(".");
     lcd.print(o2f%10);
     lcd.print("L/min");
      }


    }
  }
}
