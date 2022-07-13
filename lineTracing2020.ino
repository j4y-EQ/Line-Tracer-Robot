/*
   read line
*/


#define MID_POS 2500
#define MAX_POS 5000
#define BASE_SPEED 500
#define SIZE 6 //number of sensors
int nLastPos = MID_POS;
int lastPosError = 0;
//int delta_speed_I_last=0;

int max0 = 0; //to find the max value among a series of numbers, the variable to keep the value should be small.
int min0 = 1023;
int aMax[] = {800, 800, 800, 800, 800, 800, 800, 800};
int aMin[] = {40, 40, 40, 40, 40, 40, 40};
//int aMax[] = {0, 0, 0, 0, 0, 0, 0, 0};
//int aMin[] = {1023, 1023, 1023, 1023, 1023, 1023, 1023};
int aPin[6] = {A0, A1, A2, A3, A4, A5};
int aVal[SIZE] = {};


float Kp = 0.5;
float Kd = 0.1;
//float Ki = 0.001;

void calli(int cnt) { //finds the min and max of each sensor
  for (int i = 0; i < cnt; i++) {
    for (int j = 0; j < SIZE; j++) {
      aVal[j] = analogRead(aPin[j]);
      if (aMax[j] < aVal) {
        aMax[j] = aVal[j];
      }
      if (aMin[j] > aVal[j]) {
        aMin[j] = aVal[j];
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
#if 1
  Serial.println("Callibration starts...");
  calli(10000);
  delay(1000);
  Serial.println("Callibration ends...");
  delay(1000);
#endif
#if 0
  for (int i = 0; i < SIZE; i++) {
    Serial.print("max:");
    Serial.print(aMax[i]);
    Serial.print(",");
    Serial.print("min:");
    Serial.print(aMin[i]);
  }
  while (1) {

  }
#endif
}

int deltaSpeedCalc(int posError) {
  int delta_speed_P = Kp * posError;
  int delta_speed_D = Kd * (posError - lastPosError);
  lastPosError = posError;

  //int delta_speed_I = delta_speed_I_last + Ki * posError;
  //delta_speed_I_last= delta_speed_I;

  return delta_speed_P + delta_speed_D; //+ delta_speed_I;
}


int readLinePos() {
  float sumNum = 0.0;
  int sumDen = 0;
  for (int i = 0; i < SIZE; i++) {
    sumNum += aVal[i] * (i * 1000.0);
    sumDen += aVal[i];
  }
  /*Serial.print("Sums:");
  Serial.print(sumNum);
  Serial.print(",");
  Serial.print(sumDen);*/
  int  pos = (int)(sumNum / sumDen);
  if (sumDen != 0) {
    pos = sumNum / sumDen;//
    nLastPos = pos;
  }
  else {
    if (nLastPos < MID_POS) {
      pos = 0;
    }
    else {
      pos = MAX_POS;
    }
  }
  return pos;
}

void loop() {
  for (int j = 0; j < SIZE; j++ ) {
    aVal[j] = analogRead(aPin[j]);
    aVal[j] = constrain(aVal[j], aMin[j], aMax[j]); //limit
    aVal[j] = map(aVal[j], aMin[j], aMax[j], 0, 1000);//changed
    Serial.print(aVal[j]);
    Serial.print(",");

  }
  Serial.println();
  //return;
  int posError = MID_POS - readLinePos();
  Serial.print("pos");
  Serial.println(posError);
  delay(500);
  int delta_speed = deltaSpeedCalc(posError);
  //left and right wheel diff is 2x of delta speed
  // we can use any speed value as base_speed eg, base_speed = 500(rpm);
  int leftSpeed = BASE_SPEED - delta_speed; //delta speed depends on error
  int rightSpeed = BASE_SPEED + delta_speed;
  constrain(leftSpeed, 800, -800);
  constrain(rightSpeed, 800, -800);
  //mapping base_speed to pwm duty, eg, 500rpm-> 250(duty) 2rpm->1 duty
  int leftDuty = map(leftSpeed, 800, -800, 250, -250);
  int rightDuty = map(rightSpeed, 800, -800, 250, -250);
  /*
    Serial.print("left wheel pwm duty:");
    Serial.println(leftDuty);
    Serial.print("right wheel pwm duty:");
    Serial.println(rightDuty);
  */
  // motorCtrl(leftDuty, rightDuty);

}
/*
  //adjust wheels speed to compensate the pos error
  //use PID to generate the speed variation
  //delta_speed,leftwheel speed=leftwheel speed - delta_speed
  //delta_speed,rightwheel speed=rightwheel speed+delta_speed
  //error range:3000~-3000
  //motor speed control : PWM duty 0~255

  //proportional:gain Kp
  int delta_speed_P = Kp * posError;
  //derivative:gain Kd
  int delta_speed_D = Kd * (posError - lastPosError);

  //INtegral: gain Ki, error accumulation
  int delta_speed_I =Ki*sigma(posError1 + posError2+ posErrorN);
  basically int delta_speed_I = delta_speed_I_last + Ki * currentError;
  int delta_speed =delta_speed_P +delta_D +delta_speed_I;
*/
