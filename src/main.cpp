
#include <Arduino.h>
#include <SoftPWM.h>
#include <Servo.h>

// Motores DC da MRTduino
const int MR2_DIR = 0;
const int MR2_PWM = 6;

const int ML2_DIR = 1;
const int ML2_PWM = 8;

// Servomotor na porta 9 da MRTduino
const int SERVO_PIN = 5;

Servo braco;

const int VELOCIDADE = 100;
const unsigned long TEMPO_MOVIMENTO = 100;

// Limites iniciais para testes
const int SERVO_MIN = 75;
const int SERVO_MAX = 105;

int anguloServo = 90;
bool servoAtivado = false;

unsigned long inicioMovimento = 0;
bool motorAtivo = false;

void pararMotores() {
  SoftPWMSet(MR2_PWM, 0);
  SoftPWMSet(ML2_PWM, 0);
  motorAtivo = false;
}

void moverMotor(int direcao, int pwm, bool sentido) {
  pararMotores();

  digitalWrite(direcao, sentido ? HIGH : LOW);
  SoftPWMSet(pwm, VELOCIDADE);

  inicioMovimento = millis();
  motorAtivo = true;
}

void moverServo(int incremento) {
  // O servo so e ativado no primeiro comando.
  // ATENCAO: ao ativar, pode mover-se para 90 graus.
  if (!servoAtivado) {
    braco.attach(SERVO_PIN);
    braco.write(anguloServo);
    servoAtivado = true;
    Serial.println("Servo ativado");
    return;
  }

  anguloServo = constrain(
    anguloServo + incremento,
    SERVO_MIN,
    SERVO_MAX
  );

  braco.write(anguloServo);

  Serial.print("Servo: ");
  Serial.println(anguloServo);
}

void setup() {
  digitalWrite(ML2_DIR, LOW);

  pinMode(MR2_DIR, OUTPUT);
  pinMode(ML2_DIR, OUTPUT);

  Serial.begin(9600);

  SoftPWMBegin();
  pararMotores();

  Serial.println("MRTduino pronta");
}

void loop() {
  if (motorAtivo &&
      millis() - inicioMovimento >= TEMPO_MOVIMENTO) {
    pararMotores();
  }

  if (Serial.available() > 0) {
    char comando = Serial.read();

    switch (comando) {
      case 'D':
      case 'd':
        moverMotor(MR2_DIR, MR2_PWM, HIGH);
        Serial.println("Base: sentido 1");
        break;

      case 'E':
      case 'e':
        moverMotor(MR2_DIR, MR2_PWM, LOW);
        Serial.println("Base: sentido 2");
        break;

      case 'A':
      case 'a':
        moverMotor(ML2_DIR, ML2_PWM, HIGH);
        Serial.println("Garra: sentido 1");
        break;

      case 'F':
      case 'f':
        moverMotor(ML2_DIR, ML2_PWM, LOW);
        Serial.println("Garra: sentido 2");
        break;

      case 'U':
      case 'u':
        moverServo(5);
        break;

      case 'J':
      case 'j':
        moverServo(-5);
        break;

      case 'P':
      case 'p':
        pararMotores();
        Serial.println("Motores DC parados");
        break;

      default:
        break;
    }
  }
}
