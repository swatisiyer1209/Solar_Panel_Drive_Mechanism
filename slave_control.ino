#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#define interruptPin 2 //Pin we are going to use to wake up the Arduino
#include <Wire.h>
#include <Servo.h> 
# define I2C_SLAVE_ADDRESS 11 // 12 pour l'esclave 2 et ainsi de suite

#define PAYLOAD_SIZE 2

void setup() {
  Serial.begin(9600);//Start Serial Comunication
  Wire.begin(I2C_SLAVE_ADDRESS);
  Serial.begin(9600); 
  Serial.println("-------------------I am Slave board-------------------");
  delay(1000);               
  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indicate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  delay(5000);//wait 5 seconds before going to sleep 
  Going_To_Sleep();
  
}

void loop() {

}

void Going_To_Sleep(){
    sleep_enable();//Enabling sleep mode
    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    Serial.println("Going to Sleep!! Good night!");
    delay(1000); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on
    
// Preparing slave board   
    Serial.begin(9600); 
    Serial.println(" Interrrupt Fired - Slave Arduino Woke up - Setup started :");
    delay (1000);
// Requesting Events
// Recieving Events
  
  Wire.onReceive(receiveEvents);
  Wire.onRequest(requestEvents);
  delay(10000);
  servo_control();  
//  Serial.println(F("---> recieved events"));
//  i_a = Wire.read();
//  d_a=Wire.read();
//  f_a=Wire.read();
//  Serial.print(F("intial value : "));
//  Serial.println(i_a);
//  Serial.print(F("desired value : "));
//  Serial.println(d_a);
//  Serial.print(F("final value : "));
//  Serial.println(f_a);
  

  }

void wakeUp(){
 
  sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;

}
int i_a,d_a,f_a,initial_feedbackValue,final_feedbackValue,flag1,flag2,sending,flag;
void requestEvents()
{
  Serial.print(F("sending value of error calculation  : "));
  Serial.println(sending);
  Wire.write(sending);
  
  
}



void receiveEvents(int numBytes)
{  
  Serial.println(F("---> recieved events"));
  i_a = Wire.read();
  d_a=Wire.read();
  f_a=Wire.read();
  initial_feedbackValue=Wire.read();
  final_feedbackValue=Wire.read();
  Serial.print(numBytes);
  Serial.println(F("bytes recieved"));
  Serial.print(F("intial value : "));
  Serial.println(i_a);
  Serial.print(F("desired value : "));
  Serial.println(d_a);
  Serial.print(F("final value : "));
  Serial.println(f_a);
  Serial.print(F("Flag value : "));
  Serial.println(flag1);
  Serial.print(F("Re-checking error calculation: "));
  Serial.print("\n");
  int x=initial_feedbackValue-final_feedbackValue;
   if (x==-1||x==0||x==1||(f_a-1)!=d_a)
   {flag2=0;}
   else
   {flag2=1;}
   if (flag1==flag2) {
    Serial.print(F("Error calculation Matches - Take control to rectify "));}
    else
    {Serial.print(F("Error calculation doesn't match - take control "));
   }
   if (flag2==0){
    sending=0;
  
}
else {sending=1;}
}

int servoPin = 9;
int feedbackPin = A2;
Servo myservo;

void servo_control(){
myservo.attach(servoPin);
Serial.print("\n");
Serial.print(F("Slave Arduino taking control from master arduino and put master arduino to sleep "));
Serial.print("\n");
int servoPos= myservo.read(); 
int initialPos=servoPos;
Serial.print("Initial Position = ");
Serial.print(servoPos);
 
// Home the servo motor
myservo.write(servoPos);
Serial.print("\n");

//enter the required angle
Serial.println("Enter desired position: "); //Prompt User for Input
while (Serial.available() == 0) {
// Wait for User to Input Data
  }
int desiredPos = Serial.parseInt();
Serial.print(desiredPos);
//check if the entered angle is within the servo limit 0-180 degrees
if (desiredPos < 0 || desiredPos >180)
  {
    Serial.print("\n");
    Serial.print("Desired Position not within servo limit ");
    Serial.print("\n");
  }
 //Check if the solar panel is already at the desired angle
else if (servoPos==desiredPos) 
 {
   Serial.print("\n");
   Serial.println("Solar Panel already at desired position");
 }
//clockwise rotation
else if (desiredPos-servoPos>0)  
  {
    int initial_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Initial feedbackValue=");
    Serial.println(initial_feedbackValue);
    Serial.print("\n");
    Serial.print("Desired Position > Current servo position : move clockwise ");
    Serial.print("\n");
    // Step through servo positions
    // Increment by 1 degrees
    for (servoPos; servoPos <=desiredPos; servoPos = servoPos + 1)
      {
        // Position servo motor
        myservo.write(servoPos);
        // Allow time to get there
        delay(1000);
        // Read value from feedback signal
        int feedbackValue = analogRead(feedbackPin); 
        // Write value to serial monitor
        Serial.print("Position = ");
        Serial.print(servoPos);
        Serial.print("\t");
        Serial.println(feedbackValue);
      } 
    int final_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Final feedbackValue=");
    Serial.println(final_feedbackValue);
    int x=initial_feedbackValue-final_feedbackValue;
    
    if (x==-1||x==0||x==1||(servoPos-1)!=desiredPos)
    {
      flag=0;
      Serial.println("Error code 404: Fire External ISR");
    }
    else 
    {
      flag=1;
      Serial.println("Desired position attained");
      servoPos= myservo.read();
    }
  }
    else 
    { 
      Serial.print("\n");
      int initial_feedbackValue = analogRead(feedbackPin);
      Serial.print("\n");
      Serial.println("Initial feedbackValue=");
      Serial.println(initial_feedbackValue);
      Serial.print("Desired Position < Current servo position : move anticlockwise ");
      Serial.print("\n");
      for (servoPos ; servoPos >=desiredPos; servoPos = servoPos -1)
      {    
        // Position servo motor
        myservo.write(servoPos);
        // Allow time to get there
        delay(1000);    
        // Read value from feedback signal
        int feedbackValue = analogRead(feedbackPin);  
        // Write value to serial monitor
        Serial.print("Position = ");
        Serial.print(servoPos);
        Serial.print("\t");
        Serial.println(feedbackValue);
              
  }
    int final_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Final feedbackValue=");
    Serial.println(final_feedbackValue);
   int x=initial_feedbackValue-final_feedbackValue;
   if (x==-1||x==0||x==1||(servoPos+1)!=desiredPos)
  {
    flag=0;
    Serial.println("Error code 404: Fire External ISR");
  }
    else 
    {
    flag=1;
    Serial.println("Desired position attained");
    servoPos= myservo.read();
    }
    }
}
