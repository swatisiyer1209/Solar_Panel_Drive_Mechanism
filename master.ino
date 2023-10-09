#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#define interruptPin 2 //Pin we are going to use to wake up the Arduino
#include <Wire.h>
#include <Servo.h> 
# define I2C_SLAVE1_ADDRESS 11
# define I2C_SLAVE2_ADDRESS 12

#define PAYLOAD_SIZE 2

int n=0;
int servoPin = 9;
int feedbackPin = A2;
int flag=1;
// Value from feedback signal
int feedbackValue;

// Create a servo object
Servo myservo; 

void setup()
{
  Wire.begin();        
  Serial.begin(9600);  
  Serial.println(F("********************* I am the Master board *********************"));
  Serial.print("\n");
  delay(1000);
  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  delay(5000);//wait 5 seconds before going to sleep

 myservo.attach(servoPin); 

 //read the servos initial position 
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
  if (desiredPos < 0 || desiredPos >180){
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
  else if (desiredPos-servoPos>0)  {
    int initial_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Initial feedbackValue=");
    Serial.println(initial_feedbackValue);
    Serial.print("\n");
    Serial.print("Desired Position > Current servo position : move clockwise ");
    Serial.print("\n");
    // Step through servo positions
    // Increment by 1 degrees
    for (servoPos; servoPos <=100; servoPos = servoPos + 1){
        // Position servo motor
    myservo.write(servoPos);
    // Allow time to get there
    delay(1000);
    
    // Read value from feedback signal
    feedbackValue = analogRead(feedbackPin);
    
    // Writing Position and corresponding feedback value to serial monitor
    Serial.print("Position = ");
    Serial.print(servoPos);
    Serial.print(" degrees");
    Serial.print("\t");
    Serial.print("\t");
    Serial.print("Feedback Value = ");
    Serial.println(feedbackValue);
  } 
    int final_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Final feedbackValue=");
    Serial.println(final_feedbackValue);
  // Print to serial monitor when done
    // Print to serial monitor when done
   int x=initial_feedbackValue-final_feedbackValue;
    Serial.println(x);
      
  if (x==-1||x==0||x==1||(servoPos-1)!=desiredPos)
  {flag=0;
    Serial.println("Error code 404: Fire External ISR"); 
    delay(1000);
     //Send value 12 to slave
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
    Wire.write(initialPos); 
    Serial.print(F("sending value of initial position: "));
    Serial.println(initialPos);   
    Wire.write(desiredPos); 
    Serial.print(F("sending value of desired position: "));
    Serial.println(desiredPos);     
    Wire.write(servoPos-1); 
    Serial.print(F("sending value of final position: "));
    Serial.println(servoPos);      
    Wire.endTransmission();   

    Serial.print(" ");
    delay(5000);
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
    Wire.write(initialPos); 
    Serial.print(F("sending value of initial position: "));
    Serial.println(initialPos);   
    Wire.write(desiredPos); 
    Serial.print(F("sending value of desired position: "));
    Serial.println(desiredPos);     
    Wire.write(servoPos-1); 
    Serial.print(F("sending value of final position: "));
    Serial.println(servoPos-1);  
    Wire.write(initial_feedbackValue); 
    Serial.print(F("sending initial feedback value: "));
    Serial.println(initial_feedbackValue);  
    Wire.write(final_feedbackValue); 
    Serial.print(F("sending final feedback value: "));
    Serial.println(final_feedbackValue);     
    Wire.endTransmission();
    Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1); 
    int n = Wire.read();
    Serial.print(F(" recieved value : "));
    Serial.println(n); 
  if (n==0)
  { Going_To_Sleep();}
  else  if (n==1) {
    Serial.print("Servo position correct");
    }
  }
  else {
    flag=1;
  Serial.println("Desired position attained");
  servoPos= myservo.read();
  }
    Serial.println(flag);
  }
  

  //anticlockwise rotation
  else  { 
    Serial.print("\n");
    int initial_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Initial feedbackValue=");
    Serial.println(initial_feedbackValue);
    Serial.print("Desired Position < Current servo position : move anticlockwise ");
    Serial.print("\n");
   for (servoPos ; servoPos >=desiredPos; servoPos = servoPos -1){
        // Position servo motor
    myservo.write(servoPos);
    // Allow time to get there
    delay(1000);
    
    // Read value from feedback signal
    feedbackValue = analogRead(feedbackPin);
    
    // Write value to serial monitor
    Serial.print("Position = ");
    Serial.print(servoPos);
    Serial.print(" degrees");
    Serial.print("\t");
    Serial.print("\t");
    Serial.print("Feedback Value = ");
    Serial.println(feedbackValue);
       
  }
    int final_feedbackValue = analogRead(feedbackPin);
    Serial.print("\n");
    Serial.println("Final feedbackValue=");
    Serial.println(final_feedbackValue);
    int x=initial_feedbackValue-final_feedbackValue;
    
  if (x==-1||x==0||x==1||(servoPos+1)!=desiredPos)
  {flag=0;
    Serial.println("Error code 404: Fire External ISR"); 
    delay(1000);
     //Send value 12 to slave
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
    Wire.write(initialPos); 
    Serial.print(F("sending value of initial position: "));
    Serial.println(initialPos);   
    Wire.write(desiredPos); 
    Serial.print(F("sending value of desired position: "));
    Serial.println(desiredPos);     
    Wire.write(servoPos+1); 
    Serial.print(F("sending value of final position: "));
    Serial.println(servoPos+1);      
    Wire.endTransmission();    

    Serial.print(" ");
    delay(5000);
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
    Wire.write(initialPos); 
    Serial.print(F("sending value of initial position: "));
    Serial.println(initialPos);   
    Wire.write(desiredPos); 
    Serial.print(F("sending value of desired position: "));
    Serial.println(desiredPos);     
    Wire.write(servoPos+1); 
    Serial.print(F("sending value of final position: "));
    Serial.println(servoPos+1);  
    Wire.write(initial_feedbackValue); 
    Serial.print(F("sending initial feedback value: "));
    Serial.println(initial_feedbackValue);  
    Wire.write(final_feedbackValue); 
    Serial.print(F("sending final feedback value: "));
    Serial.println(final_feedbackValue); 
    Wire.endTransmission();
    Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1); 
    int n = Wire.read();
    Serial.print(F(" recieved value : "));
    Serial.println(n); 
  if (n==0)
  { Going_To_Sleep();}
  
  else  if (n==1){
    Serial.print("Servo position correct");
    }
    
  }
  else {
    flag=1;
  Serial.println("Desired position attained");
  servoPos= myservo.read();
  }
  
  }
  

}
void Going_To_Sleep(){
    sleep_enable();//Enabling sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    Serial.println("Master board going to Sleep!!");
    delay(100); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    Serial.println("just woke up!");//next line of code executed after the interrupt 
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on
    
  

  }

void loop()
{
  delay(100);
}
