#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <FS.h>
#include<FlexLibrary.h>

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

volatile boolean addFeature = false;

volatile boolean isCaptureStarted = false;

void ICACHE_RAM_ATTR IntCallback()
{
  addFeature = true;  
  isCaptureStarted = true;
}

void setup(void) {
  Serial.begin(115200);
  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), IntCallback, FALLING);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS mount failed");
    return;
  }
  while(!isCaptureStarted)
  {
    delay(10);
  }
  readFile();
  Serial.println("Booting .....");

  if(!mpu.begin()){
    Serial.println("myMPU6050 does not respond");
  }
  else{
    Serial.println("myMPU6050 is connected");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
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
  if(addFeature)
  {
    File file = SPIFFS.open("/Gesture.txt", "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
    if (file.print("\n\t\tNew Feature\n\n\n")) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
  addFeature = false;
  Serial.println("Added new feature!");
  delay(1000);
  Serial.println("start recording");
  }

  ax = constrain(a.acceleration.x - baseline[0], -TRUNCATE, TRUNCATE);
  ay = constrain(a.acceleration.y - baseline[1], -TRUNCATE, TRUNCATE);
  az = constrain(a.acceleration.z - baseline[2], -TRUNCATE, TRUNCATE);
  gx = constrain(g.acceleration.x - baseline[0], -TRUNCATE, TRUNCATE);
  gy = constrain(g.acceleration.y - baseline[1], -TRUNCATE, TRUNCATE);
  gz = constrain(g.acceleration.z - baseline[2], -TRUNCATE, TRUNCATE);
  

  if (!motionDetected(ax, ay, az)) {
      delay(10);
      return;
    }
  recordIMU();
  delay(100);

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
    baseline[3] = double(g.acceleration.z); 
    baseline[4] = double(g.acceleration.z); 
    baseline[5] = double(g.acceleration.z);
    Serial.println("Start Recording");  
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

    // print the data in CSV format
    //Serial.println(""+String(ax)+","+String(ay)+","+String(az)+","+String(gx)+","+String(gy)+","+String(gz)+"\n");
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
    for(int i = 0; i < 10; i++)
    {
      Serial.println();
    }
     for(int i = 0; i < NUM_SAMPLES;i++)
    {
      int index = i*NUM_AXES;
      String data = String(features[index])+","+String(features[index+1])+","+String(features[index+2])+","+String(features[index+3])+","+String(features[index+4])+","+String(features[index+5])+","+String(features[index+6])+","+String(features[index+7])+","+String(features[index+8])+","+String(features[index+9])+","+String(features[index+10])+"\n"; 
      Serial.print(data);
    } 
    appendFile();   
}

bool motionDetected(float ax, float ay, float az) {
    return (abs(ax) + abs(ay) + abs(az)) > ACCEL_THRESHOLD;
}

void readFile() {
  File file = SPIFFS.open("/Gesture.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void appendFile() {
  String data;
  File file = SPIFFS.open("/Gesture.txt", "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  for(int i = 0; i < NUM_SAMPLES;i++)
  {
    int index = i*NUM_AXES;
    data = String(features[index])+","+String(features[index+1])+","+String(features[index+2])+","+String(features[index+3])+","+String(features[index+4])+","+String(features[index+5])+","+String(features[index+6])+","+String(features[index+7])+","+String(features[index+8])+","+String(features[index+9])+","+String(features[index+10])+"\n"; 
    if (file.print(""+data)) {
       Serial.println("Message appended");
    } else {
       Serial.println("Append failed");
     }
  }
  
  if (file.print("\n\n")) {
    Serial.println("added space");
  } else {
    Serial.println("Append failed");
  }
  file.close();
  Serial.println("Done Recording");
  delay(1000);
  Serial.println("Start Recording");
}
