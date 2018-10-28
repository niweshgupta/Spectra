#include <SoftwareSerial.h>
#include <OLED_I2C.h>
char state;
SoftwareSerial BT(10, 11);//TX, RX respetively //Androide Connection 
OLED  myOLED(SDA, SCL, 8);
extern uint8_t SmallFont[];
String readvoice;
#define BRAKE 0
#define CW    1
#define CCW   2
#define CS_THRESHOLD 15   // Definition of safety current (Check: "1.3 Monster Shield Example").

//MOTOR 1
#define MOTOR_A1_PIN 7
#define MOTOR_B1_PIN 8

//MOTOR 2
#define MOTOR_A2_PIN 4
#define MOTOR_B2_PIN 9

#define PWM_MOTOR_2 6
#define PWM_MOTOR_1 5

#define CURRENT_SEN_1 A2
#define CURRENT_SEN_2 A3

#define EN_PIN_1 A0
#define EN_PIN_2 A1

#define MOTOR_1 0
#define MOTOR_2 1

short usSpeed = 300;  //default motor speed
unsigned short usMotor_Status = BRAKE;
 
void setup()                         
{
  BT.begin(38400);      
  Serial.begin(38400);  
  myOLED.begin();
  myOLED.setFont(SmallFont);
  randomSeed(analogRead(7));
  
  myOLED.clrScr();
  myOLED.print("Smart Car", CENTER, 0);
  myOLED.print("Owner Avishek Biswas", CENTER, 28);
  myOLED.update();
  delay(500);
  
  pinMode(MOTOR_A1_PIN, OUTPUT);
  pinMode(MOTOR_B1_PIN, OUTPUT);
  
  pinMode(MOTOR_A2_PIN, OUTPUT);
  pinMode(MOTOR_B2_PIN, OUTPUT);

  pinMode(PWM_MOTOR_1, OUTPUT);
  pinMode(PWM_MOTOR_2, OUTPUT);
  
  pinMode(CURRENT_SEN_1, OUTPUT);
  pinMode(CURRENT_SEN_2, OUTPUT);
 
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(EN_PIN_2, OUTPUT);

  //Serial.begin(9600);              // Initiates the serial to do the monitoring 
  Serial.println("Begin motor control");
  Serial.println(); //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("1. STOP");
  Serial.println("2. FORWARD");
  Serial.println("3. REVERSE");
  Serial.println("+. INCREASE SPEED");
  Serial.println("-. DECREASE SPEED");
  Serial.println();

}

void loop() 
{
  char user_input;   

  
  
  while(Serial.available())
  {
     delay(10);
    user_input = Serial.read(); //Read user input and trigger appropriate function
    digitalWrite(EN_PIN_1, HIGH);
    digitalWrite(EN_PIN_2, HIGH);

     
    if (user_input =='S')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Stop", CENTER, 28);
    myOLED.update();
    Serial.write(state);
       Stop();
    }
    else if(user_input =='Q')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Forward", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      Forward();
    }
    else if(user_input =='B')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Reverse", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      Reverse();
    }
    else if(user_input =='R')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Right", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      Right();
    }
    else if(user_input =='I')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Increase Speed", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      IncreaseSpeed();
    }
    else if(user_input =='P')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Left", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      Left();
    }
    else if(user_input =='D')
    {
      myOLED.clrScr();
    myOLED.print("Smart Car", CENTER, 0);
    myOLED.print("Decrease Speed", CENTER, 28);
    myOLED.update();
    Serial.write(state);
      DecreaseSpeed();
    }
    /*else
    {
      Serial.println("Invalid option entered.");
    }*/
      
  }
}

void Stop()
{
  Serial.println("Stop");
  usMotor_Status = BRAKE;
  motorGo(MOTOR_1, usMotor_Status, 0);
  motorGo(MOTOR_2, usMotor_Status, 0);
}

void Forward()
{
  Serial.println("Forward");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, usSpeed);
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}
void Left()
{
  l();
  //r();
}
void l()
{
  Serial.println("Left");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, usSpeed);
}
void r()
{
  Serial.println("Right");
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}
void Right()
{
  //l1();
  r1();
}
void l1()
{
  Serial.println("Reverse");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, usSpeed);
}
void r1()
{
  Serial.println("Forward");
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void Reverse()
{
  Serial.println("Reverse");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, usSpeed);
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void IncreaseSpeed()
{
  usSpeed = usSpeed + 10;
  if(usSpeed > 255)
  {
    usSpeed = 255;  
  }
  
  Serial.print("Speed +: ");
  Serial.println(usSpeed);

  motorGo(MOTOR_1, usMotor_Status, usSpeed);
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void DecreaseSpeed()
{
  usSpeed = usSpeed - 10;
  if(usSpeed < 0)
  {
    usSpeed = 0;  
  }
  
  Serial.print("Speed -: ");
  Serial.println(usSpeed);

  motorGo(MOTOR_1, usMotor_Status, usSpeed);
  motorGo(MOTOR_2, usMotor_Status, usSpeed);
}

void motorGo(uint8_t motor, uint8_t direct, uint8_t pwm)         //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  if(motor == MOTOR_1)
  {
    if(direct == CW)
    {
      digitalWrite(MOTOR_A1_PIN, LOW); 
      digitalWrite(MOTOR_B1_PIN, HIGH);
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR_A1_PIN, HIGH);
      digitalWrite(MOTOR_B1_PIN, LOW);      
    }
    else
    {
      digitalWrite(MOTOR_A1_PIN, LOW);
      digitalWrite(MOTOR_B1_PIN, LOW);            
    }
    
    analogWrite(PWM_MOTOR_1, pwm); 
  }
  else if(motor == MOTOR_2)
  {
    if(direct == CW)
    {
      digitalWrite(MOTOR_A2_PIN, LOW);
      digitalWrite(MOTOR_B2_PIN, HIGH);
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR_A2_PIN, HIGH);
      digitalWrite(MOTOR_B2_PIN, LOW);      
    }
    else
    {
      digitalWrite(MOTOR_A2_PIN, LOW);
      digitalWrite(MOTOR_B2_PIN, LOW);            
    }
    
    analogWrite(PWM_MOTOR_2, pwm);
  }
}


