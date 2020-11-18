#include <Servo.h>
const int Ini_hand = 1800;
const int Ini_wrist = 2000;
const int Ini_elbow = 1300;
const int Ini_disk = 1850;
const int high_grab = 1200;
const int low_grab = 1500;
Servo elbow;
Servo wrist;
Servo hand;
Servo disk;

void release(int grab_angle, bool DoRelease){
  if(!DoRelease) return;

  for(int angle = 0; angle < 500; angle++){
    wrist.writeMicroseconds(Ini_wrist - angle);
    delay(2.5);
  }
  for(int angle = 500; angle < grab_angle; angle++){
    wrist.writeMicroseconds(Ini_wrist - angle);
    elbow.writeMicroseconds(Ini_wrist - angle - 200);
    delay(2.5);
  }
  
  // Realize the hand
  for(int i = 0; i < 400; i++){
    hand.writeMicroseconds(Ini_hand - 400 + i);
    delay(2.5);
  }
  delay(1000);

  // Back to the initial position
  int wrist_angle = Ini_wrist - grab_angle;
  for(int angle = 0; angle < grab_angle - 500; angle++){
    wrist.writeMicroseconds(wrist_angle + angle);
    elbow.writeMicroseconds(wrist_angle - 200 + angle);
    delay(2.5);
  }
  for(int angle = grab_angle - 500; angle < grab_angle; angle++){
    wrist.writeMicroseconds(wrist_angle + angle);
    delay(2.5);
  }
} 


void hold(int grab_angle, bool DoHold){  
  if(!DoHold) return;

  for(int angle = 0; angle < 500; angle++){
    wrist.writeMicroseconds(Ini_wrist - angle); // 1500
    delay(2.5);
  }
  for(int angle = 500; angle < grab_angle; angle++){
    wrist.writeMicroseconds(Ini_wrist - angle); // Start: 1500
    elbow.writeMicroseconds(Ini_wrist - angle - 200); // Start: 1300
    delay(2.5);
  }

  // Close the hand
  for(int i = 0; i < 400; i++){
    hand.writeMicroseconds(Ini_hand - i);
    delay(2.5);
  }
  delay(1000);

  // Lift the item up
  int wrist_angle = Ini_wrist - grab_angle;
  for(int angle = 0; angle < grab_angle - 500; angle++){
    wrist.writeMicroseconds(wrist_angle + angle);
    elbow.writeMicroseconds(wrist_angle - 200 + angle);
    delay(2.5);
  }
  for(int angle = grab_angle - 500; angle < grab_angle; angle++){
    wrist.writeMicroseconds(wrist_angle + angle);
    delay(2.5);
  }   
}

void setup() {
  Serial.begin(9600);
  elbow.attach(9);
  wrist.attach(6);
  hand.attach(11);
  disk.attach(10);
  
  // Initialize
  elbow.writeMicroseconds(Ini_elbow); // 1300
  wrist.writeMicroseconds(Ini_wrist); // 2000
  hand.writeMicroseconds(Ini_hand);
  disk.writeMicroseconds(Ini_disk);
  delay(2000);

  // Grab from a high position
  hold(high_grab, 1);
  delay(2000);

  // Release to the floor
  release(low_grab, 1);
  delay(2000);

  // Grab from a low position
  hold(low_grab, 1);
  delay(2000);

  // Release to the floow
  release(low_grab, 1);
  delay(2000);

}

void loop(){}
