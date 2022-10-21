#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include<FlexLibrary.h>
#include "model.h"
Eloquent::ML::Port::RandomForest classifier;

#define NUM_SAMPLES 10
#define NUM_AXES 11
#define TRUNCATE 20
#define ACCEL_THRESHOLD 5
#define myMPU6050_ADDR 0x68
int INT_PIN  = 0;
Flex flex(34);
Flex flex1(35);
Flex flex2(32);
Flex flex3(33);
Flex flex4(25);

Adafruit_MPU6050 mpu;

int sample_read = 0;

double baseline[NUM_AXES];

float features[NUM_AXES * NUM_SAMPLES];

volatile boolean StartDetection = false;

void ICACHE_RAM_ATTR IntCallback()
{
  StartDetection=true;
}

void setup(void) {
  Serial.begin(115200);
  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), IntCallback, FALLING);
    while(!StartDetection)
  {
    delay(10);
  }
  if(!mpu.begin()){
    Serial.println("myMPU6050 does not respond");
  }
  else{
    Serial.println("myMPU6050 is connected");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  Serial.println("");
  delay(100);
  calibrate();
   for(int i = 0; i < 1000; i++){
    flex.Calibrate();
    flex1.Calibrate();
    flex2.Calibrate();
    flex3.Calibrate();
    flex4.Calibrate();
  }
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  int f1,f2,f3,f4,f5;
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  ax = constrain(a.acceleration.x - baseline[0], -TRUNCATE, TRUNCATE);
  ay = constrain(a.acceleration.y - baseline[1], -TRUNCATE, TRUNCATE);
  az = constrain(a.acceleration.z - baseline[2], -TRUNCATE, TRUNCATE);
  
  if (!motionDetected(ax, ay, az)) {
      delay(10);
      return;
    }
  recordIMU();
  delay(100);
  String predictionClass = classifier.predictLabel(features);
  if (predictionClass.equals("A")) {
    Serial.println("A");
    Serial.println();
  }
  else if (predictionClass.equals("B")) {
    Serial.println("B");
    Serial.println();
  }
  else if (predictionClass.equals("C")) {
    Serial.println("C");
    Serial.println();
  }
  else if (predictionClass.equals("D")) {
    Serial.println("D");
    Serial.println();
  }
  else if (predictionClass.equals("E")) {
    Serial.println("E");
    Serial.println();
  }
  else if (predictionClass.equals("F")) {
    Serial.println("F");
    Serial.println();
  }
  else if (predictionClass.equals("G")) {
    Serial.println("G");
    Serial.println();
  }
  else if (predictionClass.equals("H")) {
    Serial.println("H");
    Serial.println();
  }
  else if (predictionClass.equals("I")) {
    Serial.println("I");
    Serial.println();
  }
  else if (predictionClass.equals("J")) {
    Serial.println("J");
    Serial.println();
  }
  else if (predictionClass.equals("K")) {
    Serial.println("K");
    Serial.println();
  }
  else if (predictionClass.equals("L")) {
    Serial.println("L");
    Serial.println();
  }
  else if (predictionClass.equals("M")) {
    Serial.println("M");
    Serial.println();
  }
  else if (predictionClass.equals("N")) {
    Serial.println("N");
    Serial.println();
  }
  else if (predictionClass.equals("O")) {
    Serial.println("O");
    Serial.println();
  }
  else if (predictionClass.equals("P")) {
    Serial.println("P");
    Serial.println();
  }
  else if (predictionClass.equals("V")) {
    Serial.println("V");
    Serial.println();
  }
}

void calibrate()
{
    sensors_event_t a, g, temp;
    Serial.println("Calibrating...");
    for (int i = 0; i < 10; i++) {
        mpu.getEvent(&a, &g, &temp);
        delay(100);
    }
    Serial.println("done Calibrating!");
    baseline[0] = double(a.acceleration.x);
    baseline[1] = double(a.acceleration.y);
    baseline[2] = double(a.acceleration.z); 
    baseline[3] = double(g.acceleration.x); 
    baseline[4] = double(g.acceleration.y); 
    baseline[5] = double(g.acceleration.z);
    Serial.println("Do gesture");  
}

void recordIMU() {
    float ax, ay, az;
    float gx, gy, gz;
    int f1,f2,f3,f4,f5;
    sensors_event_t a, g, temp;
    for (int i = 0; i < NUM_SAMPLES; i++) {
      mpu.getMotionInterruptStatus();
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      ax = constrain(a.acceleration.x - baseline[0], -TRUNCATE, TRUNCATE);
      ay = constrain(a.acceleration.y - baseline[1], -TRUNCATE, TRUNCATE);
      az = constrain(a.acceleration.z - baseline[2], -TRUNCATE, TRUNCATE);
      gx = constrain(g.acceleration.x - baseline[0], -TRUNCATE, TRUNCATE);
      gy = constrain(g.acceleration.y - baseline[1], -TRUNCATE, TRUNCATE);
      gz = constrain(g.acceleration.z - baseline[2], -TRUNCATE, TRUNCATE);
      f1= analogRead(34);
      f2= analogRead(35);
      f3= analogRead(32);
      f4= analogRead(33);
      f5= analogRead(25);
      int index = i*NUM_AXES;
      features[index] = ax;
      features[index+1] = ay;
      features[index+2] = az;
      features[index+3] = gx;
      features[index+4] = gy;
      features[index+5] = gz;
      features[index+6] = f1;
      features[index+7] = f2;
      features[index+8] = f3;
      features[index+9] = f4;
      features[index+10] = f5;
    }  
}

bool motionDetected(float ax, float ay, float az) {
    return (abs(ax) + abs(ay) + abs(az)) > ACCEL_THRESHOLD;
}
