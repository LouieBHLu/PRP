#include <FlexiTimer2.h>
#define pwr_left 49
#define QTI1 A0
#define pwr_for 50
#define QTI4 A3
#define pwr_middle 51
#define QTI2 A1
#define pwr_beh 52
#define QTI5 A4
#define pwr_right 53
#define QTI3 A2
#define guard 12

#define PULA 30
#define PULB 32 
#define PULC 34 
#define PULD 36
#define DIRA 31
#define DIRB 33
#define DIRC 35
#define DIRD 37


#define ready_line_lr 0 //先准备左右移动
#define run_line_lr 1 //开始移动
#define ready_line_fb 2//准备前后移动
#define run_line_fb 3
#define run_end 4
#define stop_line 5//临时的，用于调试



const double aver=600;
int direction_key=1;
double val1, val2,val3,val4,val5; 
unsigned long long int timerM=0;
unsigned long long int p_timerM=0;//MOTOR
unsigned long long int timerQ=0;
unsigned long long int p_timerQ=0;//QTI
static int finish=0;//标志上一个任务做完

typedef struct POINT{
  int x;
  int y;
} point;
point p_car;


typedef struct TASK{
  point target;//where to go
  int op;//0 is to grab, 1 is to release
} task;

int red=0,green=0,blue=0;//记录当前物块在哪个功能区, 0为原料区，1位加工区，2为成品区
const point RED[3]={{3,1},{2,5},{5,2}};//离点位最近的安全区点
const point GREEN[3]={{3,1},{3,5},{5,3}};
const point BLUE[3]={{3,1},{4,5},{5,4}};

//const task tasklist[12]={{RED[0],0},{RED[1],1},{GREEN[0],0},{GREEN[1],1},{BLUE[0],0},{BLUE[1],1},{RED[1],0},{RED[2],1},{GREEN[1],0},{GREEN[2],1},{BLUE[1],0},{BLUE[2],1}};
const task tasklist[6]={{RED[1],0},{RED[2],1},{GREEN[1],0},{GREEN[2],1},{BLUE[1],0},{BLUE[2],1}};

void setup() {
  pinMode(pwr_left,OUTPUT);
  pinMode(pwr_middle,OUTPUT);
  pinMode(pwr_right,OUTPUT);
  pinMode(pwr_for,OUTPUT);
  pinMode(pwr_beh,OUTPUT);
  pinMode(QTI1,INPUT);
  pinMode(QTI2,INPUT);
  pinMode(QTI3,INPUT);
  pinMode(QTI4,INPUT);
  pinMode(QTI5,INPUT);
  
  pinMode(PULA,OUTPUT);
  pinMode(PULB,OUTPUT);
  pinMode(PULC,OUTPUT);
  pinMode(PULD,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
  pinMode(DIRC,OUTPUT);
  pinMode(DIRD,OUTPUT);
  
  setP32( ); // Prescaler = 32
  Serial.begin(115200);
  FlexiTimer2::set(1, timer); 
  FlexiTimer2::start();
  p_car.x=2;
  p_car.y=5;

}

void timer(){
    static bool i=0;
    i=!i;
    timerM++;
    if (i)
    timerQ++;
}

void setP32( ) {
  Serial.println("ADC Prescaler = 32");  // 101
  ADCSRA |=  (1 << ADPS2);   // 1
  ADCSRA &=  ~(1 << ADPS1);  // 0
  ADCSRA |=  (1 << ADPS0);   // 1
}

int direction_judge(){//返回-1为左偏，0为不偏，1为右偏，2为经过交点，3为彻底出轨，不修改偏向标志位
  int left,middle,right; static int ret;
  //根据需要可将判断细化，分为小修正和大修正
    
  
if (direction_key==1 || direction_key==-1){
  digitalWrite(pwr_left,HIGH);digitalWrite(pwr_middle,HIGH);digitalWrite(pwr_right,HIGH);
  digitalWrite(QTI1,HIGH);digitalWrite(QTI2,HIGH);digitalWrite(QTI3,HIGH);
  val1=analogRead(QTI1);val2=analogRead(QTI2);val3=analogRead(QTI3);
  //if(val1>aver && val2>aver && val3>aver) Serial.println(1);

    if (direction_key==1){
      //left=digitalRead(QTI1);middle=digitalRead(QTI2);right=digitalRead(QTI3);
      left=val1;middle=val2;right=val3;
    }
    else{//-1
      //left=digitalRead(QTI3);middle=digitalRead(QTI2);right=digitalRead(QTI1);
      left=val3;middle=val2;right=val1;
    }
  return ((-(left<aver)*(right>=aver))+((left>=aver)*(right<aver)));
  }
  else{//direction_key==+-2
  digitalWrite(pwr_for,HIGH);digitalWrite(pwr_middle,HIGH);digitalWrite(pwr_beh,HIGH);
  digitalWrite(QTI5,HIGH);digitalWrite(QTI2,HIGH);digitalWrite(QTI4,HIGH);
  val5=analogRead(QTI5);val2=analogRead(QTI2);val4=analogRead(QTI4);
    //Serial.println(val2);
    if(direction_key==-2){
      left=val4;middle=val2;right=val5;
    }
    else{
      left=val5;middle=val2;right=val4;
    }
        return ((-(left<aver)*(right>=aver))+((left>=aver)*(right<aver)));
  }
    
}

void Correction(){
  int i;
  static int correction_left=0; 
  static int correction_right=0;
  static int recorre_left=0;
  static int recorre_right=0;
  static int counter=0;
  static int key_left=0;
  static int key_right=0;//correction需要修正的量，counter一次积累的偏差量，key左右偏标志位
  if(timerQ>p_timerQ){
  //Serial.println(direction_judge());
  switch (direction_judge()){
  case -1:{//传感器显示左偏
    //if (key_right==1) throw();//如果直接从右偏进入左偏，,抛出异常
    key_left=1;//标志左偏
    correction_left++;//左侧轮多输出一些，具体多少需要调试
    counter++;
    break;
  }
  case 1:{//传感器显示右偏
    //if (key_left==1) throw();
    key_right=1;
    correction_right++;
    counter++;
    break;
  }
  case 0:{//不偏，指两个传感器都不压线
    if (key_left==1){//从左偏修正回来
      key_left=0;
      correction_left=0;
      recorre_left+=counter*0.5; //方向回正，counter为从偏离线到回到线总共累计的偏差量，具体回正多少需要调试
      counter=0; //此时处于不偏状态
    }
    else if (key_right==1){//从右偏修正
      key_right=0;
      correction_right=0;
      recorre_right+=counter*0.5;
      counter=0;
    }
    else{//本来就不偏
    //直行，不进行操作  
    }
  key_left=0;
  key_right=0;correction_left=0;correction_right=0;
  counter=0;
  break;
  }  
  case 2:{//两个传感器都压线，经过交叉
    //直行，不进行操作
    break;
  }
  default: {
    //直行，不进行操作
    break;
  }
  }

  if (correction_left!=0||correction_right!=0 || recorre_left!=0 || recorre_right!=0){//修正输出
    if (correction_left!=0){//左侧轮额外发出一次脉冲，一次修正一步，具体一次修正多少需要调试
      turn_right_step();
      //correction_left--;
      //不设置方向，方向与当前运动方向一致
      //根据counter的值不同，表示误差累积量不同，随着counter变大可逐渐增大一次调整的值
//      for (i=0;i<=floor(counter/2);i++){ 
//          turn_right_step();
//      }
    }
    else if (recorre_left!=0) {turn_left_step();recorre_left--;}
    else if (recorre_right!=0) {turn_right_step();recorre_right--;}
    else{    //(correction_right!=0)
      turn_left_step();
      //correction_right--;
//      for (i=0;i<=floor(counter/2);i++){       
//          turn_left_step();
//      }
    } 
  }
  //清除同时要向左修正和向右修正的情况。只能同时存在一种
   while( recorre_left!=0 && recorre_right!=0){
    recorre_left-- ; recorre_right-- ;
  }
  while( recorre_left!=0 && correction_right!=0){
    recorre_left-- ; correction_right-- ;
  }
  while( recorre_right!=0 && recorre_left!=0){
    recorre_right-- ; recorre_left-- ;
  }
  while( recorre_right!=0 && correction_left!=0){
    recorre_right-- ; correction_left-- ;
  }
  
  p_timerQ=timerQ;
  }
}


//front side 返回左中右偏标志位

void forward_step(int sp=10) {//向前走一步
  direction_key=1;
if (p_timerM+10-sp<timerM){
    digitalWrite(DIRA,HIGH);
    digitalWrite(DIRB,HIGH);
    digitalWrite(DIRC,HIGH);
    digitalWrite(DIRD,HIGH);
    digitalWrite(PULA,HIGH);
    digitalWrite(PULB,HIGH);
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
    
    delayMicroseconds(10);
    
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
  
    delayMicroseconds(10);
    p_timerM=timerM;
}
}

void backward_step(int sp=10) {//向后走一步
  direction_key=-1;
if (p_timerM+10-sp<timerM){  
    digitalWrite(DIRA,LOW);
    digitalWrite(DIRB,LOW);
    digitalWrite(DIRC,LOW);
    digitalWrite(DIRD,LOW);
    digitalWrite(PULA,HIGH);
    digitalWrite(PULB,HIGH);
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
     
    delayMicroseconds(10);
    
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
  
    delayMicroseconds(10);
    p_timerM=timerM;
  }
}

void left_step(int sp=10) {//向左走一步
  direction_key=2;
if (p_timerM+10-sp<timerM){
    digitalWrite(DIRA,LOW);
    digitalWrite(DIRB,HIGH);
    digitalWrite(DIRC,HIGH);
    digitalWrite(DIRD,LOW);
    digitalWrite(PULA,HIGH);
    digitalWrite(PULB,HIGH);
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
     
    delayMicroseconds(10);
    
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
  
    delayMicroseconds(10);
    p_timerM=timerM;
  }
}

void right_step(int sp=10) {//向右走一步
  direction_key=-2;
if (p_timerM+10-sp<timerM){
    digitalWrite(DIRA,HIGH);
    digitalWrite(DIRB,LOW);
    digitalWrite(DIRC,LOW);
    digitalWrite(DIRD,HIGH);
    digitalWrite(PULA,HIGH);
    digitalWrite(PULB,HIGH);
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
     
    delayMicroseconds(10);
    
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
    
    delayMicroseconds(10);
    p_timerM=timerM;
}
}

//转弯（函数命名）以车正在前进的方向为前
void turn_right_step() {//左侧两轮前进一步，向右转一点点,
  if(direction_key==1){//往前开时右转
    digitalWrite(PULA,HIGH);
    digitalWrite(PULC,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULA,LOW);
    digitalWrite(PULC,LOW);
    delayMicroseconds(1000);
  }
  else if(direction_key==2){//往左开时右转
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
    delayMicroseconds(1000);
  }
  else if(direction_key==-1){//往后开时右转
    digitalWrite(PULB,HIGH);
    digitalWrite(PULD,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULB,LOW);
    digitalWrite(PULD,LOW);
    delayMicroseconds(1000);
  }
  else{//direction_key==-2 往右开时右转
    digitalWrite(PULA,HIGH);
    digitalWrite(PULB,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    delayMicroseconds(1000);
  }
}

void turn_left_step(){

  if(direction_key==1){
    digitalWrite(PULB,HIGH);
    digitalWrite(PULD,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULB,LOW);
    digitalWrite(PULD,LOW);
    delayMicroseconds(1000);
  }
  else if(direction_key==2){
    digitalWrite(DIRA,HIGH);
    digitalWrite(DIRB,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULA,LOW);
    digitalWrite(PULB,LOW);
    delayMicroseconds(1000);
  }
  else if(direction_key==-1){
    digitalWrite(PULA,HIGH);
    digitalWrite(PULC,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULA,LOW);
    digitalWrite(PULC,LOW);
    delayMicroseconds(1000);
  }
  else{//-2
    digitalWrite(PULC,HIGH);
    digitalWrite(PULD,HIGH);
    delayMicroseconds(10);
    digitalWrite(PULC,LOW);
    digitalWrite(PULD,LOW);
    delayMicroseconds(1000);
  }
}

void rotate_cw_step(int sp=10){//clockwise
  if (p_timerM+10-sp<timerM){
  digitalWrite(DIRA,LOW);
  digitalWrite(DIRB,HIGH);
  digitalWrite(DIRC,LOW);
  digitalWrite(DIRD,HIGH);

  digitalWrite(PULA,HIGH);
  digitalWrite(PULB,HIGH);
  digitalWrite(PULC,HIGH);
  digitalWrite(PULD,HIGH);
  
  delayMicroseconds(10);
  
  digitalWrite(PULA,LOW);
  digitalWrite(PULB,LOW);
  digitalWrite(PULC,LOW);
  digitalWrite(PULD,LOW);

  delayMicroseconds(10);
  p_timerM=timerM;
  }
}

void rotate_ccw_step(int sp=10){//counterclockwise
  if (p_timerM+10-sp<timerM){
  digitalWrite(DIRA,HIGH);
  digitalWrite(DIRB,LOW);
  digitalWrite(DIRC,HIGH);
  digitalWrite(DIRD,LOW);

  digitalWrite(PULA,HIGH);
  digitalWrite(PULB,HIGH);
  digitalWrite(PULC,HIGH);
  digitalWrite(PULD,HIGH);
  
  delayMicroseconds(10);
  
  digitalWrite(PULA,LOW);
  digitalWrite(PULB,LOW);
  digitalWrite(PULC,LOW);
  digitalWrite(PULD,LOW);

  delayMicroseconds(10);
  p_timerM=timerM;
  }
}

void rotate_cw_90(){
  for (int i=0;i<2106;i++){ //2110 is a changeable parameter
    rotate_cw_step();
    delayMicroseconds(2000); // 2000 depends on the frequency of timer interrupt
  }
}

void rotate_cw_180(){
  for (int i=0;i<2106*2;i++){
    rotate_cw_step();
    delayMicroseconds(2000);
  }
}


void rotate_ccw_90(){
  for (int i=0;i<2106;i++){ //2110 is a changeable parameter
    rotate_ccw_step();
    delayMicroseconds(2000); // 2000 depends on the frequency of timer interrupt
  }
}

void rotate_ccw_180(){
  for (int i=0;i<2106*2;i++){
    rotate_ccw_step();
    delayMicroseconds(2000);
  }
}

void p2pFSM(task curr_task){
//先左右走后前后走可能可以让停车的时候姿态更准一点
  static int delta_x=curr_task.target.x-p_car.x;
  static int delta_y=curr_task.target.y-p_car.y;
  static int state=0;
  static int guard_counter=0;//用于排除走上黑线时的抖动
  static int guard_counter2=0;//用于排除走下黑线时的抖动
  static int grid_counter=0;//数走过了多少黑线


  static int step_counter=0;//temp
  //Serial.println(delta_x);
  //Serial.println(state);
  
  switch (state){
    case ready_line_lr:{ //set up for left/right move
      delta_x=curr_task.target.x-p_car.x;
      delta_y=curr_task.target.y-p_car.y;
      if(delta_x>0) direction_key=-2;
      else if(delta_x<0) direction_key=2;
      else {state=ready_line_fb;break;}//移动距离为0直接跳过
   
      digitalWrite(pwr_for,HIGH);digitalWrite(pwr_middle,HIGH);digitalWrite(pwr_beh,HIGH);
      digitalWrite(QTI5,HIGH);digitalWrite(QTI2,HIGH);digitalWrite(QTI4,HIGH);
      val5=analogRead(QTI5);val2=analogRead(QTI2);val4=analogRead(QTI4);
      //启动前先照一下更新下数据
      guard_counter=0;grid_counter=0;
      state+=1; //进入左右巡线状态
      delay(500);
      break;
    }    
    case run_line_lr:{//左右移动，巡双线，判定单线，走n格的话检测到第n次黑线立刻停车（一开始理论上为黑黑黑）
      if(direction_key==2) {left_step();Correction();}
      if(direction_key==-2) {right_step();Correction();}//Correction()会更新巡线传感器数据
      if(val4>=aver && val2>=aver && val5>=aver)   guard_counter++;//碰到线，开始确认是否稳态
      else guard_counter=0;//抖动，非稳态，或是从黑线上驶离
      if(guard_counter==guard){//确认到线。如果持续在黑线上(>2),grid_counter只会增加1,直到退出黑线后自动清空guard_counter
        //guard_counter=0;
        grid_counter++;
      }
      if(delta_x>0 && grid_counter==delta_x+1 || delta_x<0 && grid_counter==-(delta_x-1)) {state++;grid_counter=0;}//因为从黑线驶离，一开始会判定一次黑线
      break;
    }

    case ready_line_fb:{//setup for forward/backward move
      if(delta_y>0) direction_key=1;
      else if(delta_y<0) direction_key=-1;
      //else return;//移动距离为0直接结束函数。但现在调试的时候不能return
      else {state=run_end; break;}
      
      digitalWrite(pwr_left,HIGH);digitalWrite(pwr_middle,HIGH);digitalWrite(pwr_right,HIGH);
      digitalWrite(QTI1,HIGH);digitalWrite(QTI2,HIGH);digitalWrite(QTI3,HIGH);
      val1=analogRead(QTI1);val2=analogRead(QTI2);val3=analogRead(QTI3);
      //启动前先照一下更新下数据
      guard_counter=0;grid_counter=0;
      state+=1; //进入前后巡线状态
      break;
    }
    case run_line_fb:{//前后移动，巡单线，判定双线，走n格的话检测到2n次黑线退出后停车（一开始理论上为白黑白）
      if(direction_key==1) {forward_step();Correction();}
      if(direction_key==-1) {backward_step();Correction();}
      if(val1>=aver && val2>=aver && val3>=aver) guard_counter++;
      else guard_counter=0;
      if(guard_counter==guard) grid_counter++;Serial.println(grid_counter);
      if(grid_counter==2*delta_y || grid_counter==-2*delta_y){
        if (guard_counter==0) guard_counter2++;//可能走下黑线
        else guard_counter2=0;
        if (guard_counter2==guard){//经过2n次黑线并走下黑线，运行结束
          guard_counter2=0;
          guard_counter=0;
          grid_counter=0;
          state=0;
          delay(1000);
          //return;//调试的时候不能return
          state=run_end;
        }
     }
     break;
   }
   case run_end:{
      p_car=curr_task.target;
      tail(curr_task);
      state=0;
      break;
      
   }
   case stop_line: while(1){}//死循环卡死，代表运行完成，调试时临时使用
   default:break;
  }
}

void tail(task curr_task)
{
  static int guard_counter=0;//用于排除走上黑线时的抖动
  static int guard_counter2=0;//用于排除走下黑线时的抖动
  //前后方向
//  int distance;
//  int s=2*pi*radius;
//  int sum=distace/s;//圈数
  int sum=2000;
  if(curr_task.target.y==5){
    for(int i=0;i<sum;++i)
    {forward_step();Correction();delayMicroseconds(500);}//到达，执行抓取

    delay(1000);//temp
    
    while(guard_counter<=guard){
      backward_step();Correction();delayMicroseconds(500);
        if(val1>=aver&&val2>=aver&&val3>=aver) guard_counter++;
        else guard_counter=0;
    }
     guard_counter=0;
    while(guard_counter2<=guard){
      backward_step();Correction();delayMicroseconds(500);
      if(!(val1>=aver&&val2>=aver&&val3>=aver)) guard_counter2++;
      else guard_counter2=0;
    }
    guard_counter2=0;
    finish=1;
    return;
  }
  
  //左右方向
  if(curr_task.target.x==5){//注意不能输入（5,5）
  for(int i=0;i<sum;++i){
    right_step();Correction();delayMicroseconds(500);}//到达，执行抓取
    
    delay(1000);//temp
    
   while(guard_counter<=guard)
   {
    left_step();Correction();delayMicroseconds(500);
      if(val4>=aver&&val2>=aver&&val5>=aver)guard_counter++;
      else guard_counter=0;
   }
   guard_counter=0;
   finish=1;
   return;
   }
   
   if(curr_task.target.y==1)
   {
    for(int i=0;i<sum;++i)
    {backward_step();Correction();delayMicroseconds(500);}//到达，执行抓取

    //delay(1000);//temp,用delay来表示正在执行抓取
    //rotate_cw_180();
    //delay(1000);
    //rotate_ccw_180();
    delay(1000); //在delay中定时器中断在不在计时？
//    if (curr_task.op==0) grab();
//    else if(curr_task.op==1) armrelease();
    
    while(guard_counter<=guard){
      forward_step();Correction();delayMicroseconds(1000);
        if(val1>=aver&&val2>=aver&&val3>=aver) guard_counter++;
        else guard_counter=0;
    }
     guard_counter=0;
    while(guard_counter2<=guard){
      forward_step();Correction();delayMicroseconds(1000);
      if(!(val1>=aver&&val2>=aver&&val3>=aver)) guard_counter2++;
      else guard_counter2=0;
    }
    guard_counter2=0;
    finish=1;
    return;
  }
}

//给p2pFSM传入目的地坐标
//p2pFSM(x,y)
//在run_end()中再次使用x,y来判断执行哪种run_end


//********************** 11/10
void runFSM(){
  static int curr_task=0;
    if(curr_task>=12) while(1);//进入返回出发区的state 
    else{
    p2pFSM(tasklist[curr_task]);//还需要传入参数决定是抓还是放
    if(finish) {
      curr_task++;
      finish=0;
    }
    //Serial.println(curr_task);
  }
}

//task task_temp={{2,5},1};
//void runFSM(){
//  static int curr_task=0;
////  if(curr_task>=12) //进入返回出发区的state 
////  else{
//    p2pFSM(task_temp);//还需要传入参数决定是抓还是放
//    if(finish) {
//      while(1){}
//    }
//  //}
//}


void loop() {
  //tail(3,5);
  runFSM();
  //right_step();
}
