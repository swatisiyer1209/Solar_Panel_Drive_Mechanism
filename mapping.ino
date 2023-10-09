#include <Servo.h> 

// Control and feedback pins
int servoPin = 9;
int feedbackPin = A0;

// Value from feedback signal
int feedbackValue;

// Create a servo object
Servo myservo; 

//Setup 
void setup() 
{ 
  // Setup Serial Monitor
  Serial.begin(9600);

  // Attach myservo object to control pin
  myservo.attach(servoPin); 

}

void loop()
{
  int val = analogRead(feedbackPin);            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
  myservo.write(val);   
  Serial.print(val);// sets the servo position according to the scaled value 
  delay(15);
}
