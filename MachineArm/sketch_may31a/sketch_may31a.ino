#include <Servo.h>
const int angle  = 500; //大臂和小臂要转多少度才能拿到物块（和物块高度一致）,参数可调
const int horizon = 1450; //水平角度
const int OriHandAngle = 300; 
const int ExpandHandAngle = 100;
Servo elbow;
Servo wrist;
Servo hand;

void release(bool DoRelease){
  if(DoRelease) hand.writeMicroseconds(OriHandAngle + ExpandHandAngle);
  delay(1000);
}

void hold(bool DoHold){
  if(DoHold) hand.writeMicroseconds(OriHandAngle);
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  elbow.attach(9);
  wrist.attach(10);
  //hand.attach(11);

  // Initialize
  elbow.writeMicroseconds(1450);
  wrist.writeMicroseconds(1450);
  //hand.writeMicroseconds(1450);
  delay(1000);
  
  // Hold and Release 
  // hold(1);
  // release(1);
//  delay(1000);
 for(int pos = 0; pos <= 400; pos++){
   elbow.writeMicroseconds(1450 - pos);
   wrist.writeMicroseconds(1450 - pos);
   delay(10);
 }
    
}


//void grab(){//降下来，合上爪子，升上去
//  int pos;
//  for (pos=0;pos<angle;pos++){
//    elbow.write(pos);
//    //wrist.write(pos);
//    delay(10);
//  }
  
//  //hand()
//  for (pos;pos>0;pos--){
//    elbow.write(pos);
//    wrist.write(pos);
//    delay(10);
//  }
//}

//void drop(){//降下来，松开爪子，升上去
//  int pos;
//  for (pos=0;pos<angle;pos++){
//    elbow.write(pos);
//    wrist.write(pos);
//    delay(10);
//  }
//  //hand()
//  for (pos;pos>0;pos--){
//    elbow.write(pos);
//    wrist.write(pos);
//    delay(10);
//  }
//}

void loop() {
  // put your main code here, to run repeatedly:
}
