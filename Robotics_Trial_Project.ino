#include <Wire.h>
#include <Servo.h>

//Function Declarations 
void getData(float*);
void moveServo(float);
void printData(float*);
int getOffset();

//Variable and Constant Declarations
float data[4];
Servo ser;
const byte SERVO_PIN = 5;
const int MPU_ADDRESS = 0x68; //This is the default I2C address used for MPU6050 boards
const int BAUD_RATE = 9600;   //To set the baud rate at which serial monitor should take data
int offset;

void setup(){
  Serial.begin(BAUD_RATE); 
  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);         //Register used to power on the MPU6050 board
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.println("MPU6050 has started");
  offset=getOffset();      //Get the offset from getOffset() function
  ser.attach(SERVO_PIN);
  ser.write(90-offset);    //If there is an offset supplied, it can be subtracted so the servo starts from that position
  delay(500);
}

void loop(){
  getData(data);          //get X, Y and Z acceleration data as well as the roll angle data by putting the values in the data array supplied
  printData(data);
  if(data[3]<=180){                 //Since the servo only moves 180 degrees
    ser.write(data[3]-offset);      //Directly enter the angle already calculated from getData() into the servo and subtract any offset provided
  }
  delay(100);
}

//Function used to get offset from the user at the start from the serial monitor
int getOffset(){
  Serial.println("Please Enter the Offset: ");      
  int offset;
  while(Serial.available()==0){}     //If Serial.available() is not zero, It means user has entered the offset. putting a while loop will stop program from executing until the user inputs an offset 
  offset=Serial.parseInt();          //Put the offset in the offset variable
  if(offset>=-90 && offset<=90){     // Will only accept a -90 to 90 degree offset as it is a 180 degree servo
    return offset;
  }
  return 0;                          //If the offset is anything which is not within the specified constraints, it will simply set offset as 0 for safety
}

//Function to enter data from the accelerometer into the data array provided
void getData(float* data){
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);                               //Address to read sensor data
  Wire.requestFrom(MPU_ADDRESS,6,true);           //Wire command to read the next six bytes from 0x3B which contain accelerometer X, Y, Z data (each position contains two bites)
  data[0]=(Wire.read()<<8| Wire.read())/16384.0;  // Since Wire.read() reads only a singular byte, we have to left shift the first byte to get correct ordering for the second data from bitwise OR operation to be entered into the variable to maintain ordering.
  data[1]=(Wire.read()<<8| Wire.read())/16384.0;  //Each axis acceleration data takes two bytes, hence 6 bytes total
  data[2]=(Wire.read()<<8| Wire.read())/16384.0;  //dividing by 16384.0 to get the default and precise sensitivity readings (+-2g) from the data.
  Wire.endTransmission(true);
  if(data[1]<0){
    data[3] = 360+(atan2(data[1],data[0])*57.29578); //We can find the roll angle by finding the tan inverse of ratio of X and Y forces
  }                                                  //atan2 only handles angles till 180 degrees, hence the if-else statement
  else{
    data[3] =(atan2(data[1],data[0])*57.29578) ;
  }
}

//Function to print out data from the accelerometer into the serial monitor
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