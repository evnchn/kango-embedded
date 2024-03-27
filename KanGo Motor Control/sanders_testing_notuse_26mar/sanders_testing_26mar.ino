#define PIN 23

#define FREQ 50
#define RES_BIT 16

long us_to_duty(long us){
  long res = pow(2, RES_BIT); // 2 ^ RES_BIT;
  Serial.println(res);
  long inv_period = FREQ*1000;
  Serial.println(inv_period);
  long final_outcome = us * res / inv_period;
  Serial.println(final_outcome);
  //Serial.println(final_outcome);
  return final_outcome;
  // return us/period*res;
  return 0;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Begin");

  


  // put your setup code here, to run once:
  ledcSetup(0,FREQ,RES_BIT);
  ledcAttachPin(PIN,0);
  ledcWrite(0, us_to_duty(2000));
  delay(1000);
  ledcWrite(0, us_to_duty(1000));
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  ledcWrite(0, us_to_duty(1500));
  delay(1000);
  ledcWrite(0, us_to_duty(1700));
  delay(1000);

}