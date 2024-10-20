#include <SoftwareSerial.h>
#include <Servo.h>
#define trigpin A0
#define echopin A1
#define servopin A2

#define right_motor_speed 5  
#define rigth_motor_pin_a 6  
#define rigth_motor_pin_b 7  
#define left_motor_speed 11  
#define left_motor_pin_a 8   
#define left_motor_pin_b 9   
// IR Sensor
#define Right_IR_Sens_pin A3
#define Left_IR_Sens_pin A4
// Bluetooth
#define TX 2
#define RX 3

Servo myservo;                     
SoftwareSerial Bluetooth(RX, TX);  

int speed = 180;
int speed_2 = 130;
float distance_L, distance_F, distance_R;
float distance;
int ping_travel_time;
float ping_travel_distance;
float set = 0.17;
String mycommand;
int data_BT;  
int mode = 0;

void setup() {
  Serial.begin(9600);  
  Bluetooth.begin(9600);
  pinMode(right_motor_speed, OUTPUT);
  pinMode(rigth_motor_pin_a, OUTPUT);
  pinMode(rigth_motor_pin_b, OUTPUT);
  pinMode(left_motor_speed, OUTPUT);
  pinMode(left_motor_pin_a, OUTPUT);
  pinMode(left_motor_pin_b, OUTPUT);

  pinMode(servopin, OUTPUT);
  myservo.attach(servopin);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(Right_IR_Sens_pin, INPUT);
  pinMode(Left_IR_Sens_pin, INPUT);
}
void loop() {
  if (Bluetooth.available()) {
    data_BT = Bluetooth.read();
    Serial.println(data_BT);
  }
  // manual control
  if (data_BT == 8)
  {
    mode = 0;
    stop();
  // automatic line follow mode
  } else if (data_BT == 6)  
  {
    mode = 1;
    // set the speed
  // automatic obistacle avoidence
  } else if (data_BT == 7)  
  {
    mode = 2;
    // set speed
  }
  if (mode == 0) {
    if (data_BT == 1) {
      forward();
    } else if (data_BT == 2) {
      backward();
    } else if (data_BT == 3) {
      left();
    } else if (data_BT == 4) {
      right();
    } else if (data_BT == 5) {
      stop();
    }
  }
  if (mode == 1) {
    // line Follow (Tracing)
    if ((digitalRead(Right_IR_Sens_pin) == 0) && (digitalRead(Left_IR_Sens_pin) == 0)) {
      forward();
    }  //if Right Sensor and Left Sensor are at White color then it will call forword function
    if ((digitalRead(Right_IR_Sens_pin) == 1) && (digitalRead(Left_IR_Sens_pin) == 0)) {
      right();
    }  //if Right Sensor is Black and Left Sensor is White then it will call turn Right function
    if ((digitalRead(Right_IR_Sens_pin) == 0) && (digitalRead(Left_IR_Sens_pin) == 1)) {
      left();
    }  //if Right Sensor is White and Left Sensor is Black then it will call turn Left function
    if ((digitalRead(Right_IR_Sens_pin) == 1) && (digitalRead(Left_IR_Sens_pin) == 1)) {
      stop();
    }  //if Right Sensor and Left Sensor are at Black color then it will call Stop function
  }
  if (mode == 2) {
    // Obstacle Avoidence
    distance_F = data();
    Serial.print("S=");
    Serial.println(distance_F);
    if (distance_F >= set) {
      Serial.println("Forward");
      forward();
    } else {
      check_side();
    }
  }
}
// read distance function
float data() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(10); //2
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  ping_travel_time = pulseIn(echopin, HIGH);
  ping_travel_distance = ping_travel_time * pow(10, -6) * 343;
  distance = ping_travel_distance / 2;
  return distance;
}
// Read the distance from right and left
void check_side() {
  stop();
  myservo.write(0);
  delay(400);
  distance_R = data();
  myservo.write(180);
  delay(400);
  distance_L = data();
  myservo.write(90);
  delay(300);
  compareDistance();
}
// compare distance function between right and left
void compareDistance() {
  if (distance_L > distance_R) {
    left();
    delay(250);
    stop();
  } else if (distance_R > distance_L) {
    right();
    delay(250);
    stop();
  } else {
    backward();
    delay(400);
    left();
    delay(105);
    stop();
    delay(450);
  }
}
// move the robot forward
void forward() {
  analogWrite(right_motor_speed, speed_2);
  analogWrite(left_motor_speed, speed_2);
  digitalWrite(rigth_motor_pin_a, HIGH);
  digitalWrite(rigth_motor_pin_b, LOW);
  digitalWrite(left_motor_pin_a, HIGH);
  digitalWrite(left_motor_pin_b, LOW);
}
// move the robot backward
void backward() {
  analogWrite(right_motor_speed, speed_2);
  analogWrite(left_motor_speed, speed_2);
  digitalWrite(rigth_motor_pin_a, LOW);
  digitalWrite(rigth_motor_pin_b, HIGH);
  digitalWrite(left_motor_pin_a, LOW);
  digitalWrite(left_motor_pin_b, HIGH);
}
// stop the robot
void stop() {
  analogWrite(right_motor_speed, 0);
  analogWrite(left_motor_speed, 0);
  digitalWrite(rigth_motor_pin_a, LOW);
  digitalWrite(rigth_motor_pin_b, LOW);
  digitalWrite(left_motor_pin_a, LOW);
  digitalWrite(left_motor_pin_b, LOW);
}
// move the robot right
void right() {
  analogWrite(right_motor_speed, speed);
  analogWrite(left_motor_speed, speed);
  digitalWrite(rigth_motor_pin_a, LOW);
  digitalWrite(rigth_motor_pin_b, HIGH);
  digitalWrite(left_motor_pin_a, HIGH);
  digitalWrite(left_motor_pin_b, LOW);
}
// move the robot left
void left() {
  analogWrite(right_motor_speed, speed);
  analogWrite(left_motor_speed, speed);
  digitalWrite(rigth_motor_pin_a, HIGH);
  digitalWrite(rigth_motor_pin_b, LOW);
  digitalWrite(left_motor_pin_a, LOW);
  digitalWrite(left_motor_pin_b, HIGH);
}