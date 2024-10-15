#include <Ultrasonic.h>
#include <Servo.h>

#define SERVO_1 5
#define TIME_ATTACHMENT 1000
#define PINO_TRIGGER 8
#define PINO_ECHO 7

Servo servo_1;
int pos;

int buzzer = 11;
int ledPinGreen = 12;
int ledPinRed = 13;

Ultrasonic ultrasonic(PINO_TRIGGER, PINO_ECHO);


struct LastTrainPassed
{
  bool HasValue;
  unsigned long Value;
};

LastTrainPassed lastTrainPassedTimestamp;

bool alarmIsPlaying = false;

void setup()
{
  servo_1.attach(SERVO_1);
  Serial.begin(9600);

  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  servo_1.write(0);
  lastTrainPassedTimestamp.HasValue = false;
  lastTrainPassedTimestamp.Value = 0;
}

void alarm(int frequencyMin, int frequencyTotal)
{
  for (int frequency = frequencyMin; frequency < frequencyTotal; frequency += 10)
  {
    tone(buzzer, frequency, 500);
    delay(3);
  }
  delay(750);
}

void loop()
{
  float cmMsec = ultrasonic.distanceRead(CM);
  unsigned long currentTime = millis();
  unsigned long difference = lastTrainPassedTimestamp.HasValue ? (currentTime - lastTrainPassedTimestamp.Value) / 1000 : 0;
  bool trainIsComing = cmMsec > 1 && cmMsec < 10;

  if (trainIsComing)
  {
    Serial.println("--------- Trem está chegando");
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinRed, HIGH);
    if (!lastTrainPassedTimestamp.HasValue)
    {
      servo_1.write(90);
      alarm(300, 1800);
      alarm(300, 1800);
      alarm(300, 1800);
    }
    lastTrainPassedTimestamp.HasValue = true;
    lastTrainPassedTimestamp.Value = millis();
    delay(1000);
    return;
  }

  if ((lastTrainPassedTimestamp.HasValue && difference < 7))
  {
    Serial.println("-------- Trem passando");
    tone(buzzer, 1000, 500);
    delay(100);
    return;
  }

  if (lastTrainPassedTimestamp.HasValue && difference > 7)
  {
    Serial.println("-------- Trem passou");
    lastTrainPassedTimestamp.HasValue = false;
    return;
  }

  if (!trainIsComing)
  {
    Serial.println("-------- Trem não está chegando");
    servo_1.write(0);
    digitalWrite(ledPinGreen, HIGH);
    digitalWrite(ledPinRed, LOW);
    delay(300);
    return;
  }

  delay(500);
  Serial.println("Fim do loop");
  return;
}
