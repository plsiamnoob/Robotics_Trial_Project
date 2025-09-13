#include <Wire.h>
#include <Servo.h>

void getData(float*);
void moveServo(float);
void printData(float*);
int getOffset();

float data[4];
Servo ser;
const byte SERVO_PIN = 5;
const int MPU_ADDRESS = 0x68;
int offset;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.println("MPU6050 has started");
  offset=getOffset();
  ser.attach(SERVO_PIN);
  ser.write(90-offset);
  delay(500);
}

void loop(){
  getData(data);  
  printData(data);
  if(data[3]<=180){
    ser.write(data[3]-offset);
  }
  delay(100);
}


int getOffset(){
  Serial.println("Please Enter the Offset: ");
  int offset;
  while(Serial.available()==0){}
  offset=Serial.parseInt();
  if(offset>=-90 && offset<=90){
    return offset;
  }
  return 0;
}

void getData(float* data){
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.requestFrom(MPU_ADDRESS,6,true);
  data[0]=(Wire.read()<<8| Wire.read())/16384.0;
  data[1]=(Wire.read()<<8| Wire.read())/16384.0;
  data[2]=(Wire.read()<<8| Wire.read())/16384.0;
  Wire.endTransmission(true);
  if(data[1]<0){
    data[3] = 360+(atan2(data[1],data[0])*57.29578);
  }
  else{
    data[3] =(atan2(data[1],data[0])*57.29578) ;
  }
}

void printData(float* data){
  Serial.print("X: ");
  Serial.print(data[0]);
  Serial.print(" ");   
  Serial.print("Y: ");
  Serial.print(data[1]);
  Serial.print(" ");  
  Serial.print("Z: ");
  Serial.print(data[2]);
  Serial.print(" ");  
  Serial.print("Roll: ");
  Serial.print(data[3]);
  Serial.println("");
}