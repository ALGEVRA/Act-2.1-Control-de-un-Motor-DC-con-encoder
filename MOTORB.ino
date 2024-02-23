// ===========================================================
//Zaida Irais López Mendieta- A01708755
//Francisco Alejandro Velázquez Ledesma - A01708755
//Julio David Reséndiz Cruz - A01709375
// ===========================================================

#define IN_1 26
#define IN_2 27
#define EN 14

#define EN_A 18
#define EN_B 19

#define POT_PIN 34 // Pin ADC para leer el potenciómetro

#define PWM_CH 0
#define FREQ 5000
#define RES 8

#define DT 20000
#define REDUCTOR 34
#define CPR 12

double pos = 0;
double pos_ant = 0;
double timer = 0;
float vel = 0;
float vel_rad_s = 0; // Velocidad en radianes por segundo

bool direction = true; // true: forward, false: reverse
bool halfway_reached = false;

void IRAM_ATTR int_callback(){
  if(digitalRead(EN_B) == 0){
    pos = pos + 1;
  } else {
    pos = pos - 1;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(EN_A, INPUT);
  pinMode(EN_B, INPUT);

  pinMode(POT_PIN, INPUT);

  attachInterrupt(EN_A, int_callback, FALLING);

  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(EN, PWM_CH);
}

void loop() {
  int potValue = analogRead(POT_PIN);
  int duty_cycle;

  if (potValue < 2048) {
    // Si el potenciómetro está por debajo de la mitad, el motor gira en una dirección
    if (halfway_reached) {
      halfway_reached = false; // Reiniciar el indicador de mitad
      direction = !direction; // Cambiar la dirección
      digitalWrite(IN_1, LOW); // Detener el motor
      digitalWrite(IN_2, LOW);
    } else {
      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      duty_cycle = map(potValue, 0, 2047, 255, 0); // Mapear el valor del potenciómetro
    }
  } else {
    // Si el potenciómetro está por encima de la mitad, el motor gira en la dirección opuesta
    halfway_reached = true; // Indicar que se ha alcanzado la mitad
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    duty_cycle = map(potValue, 2048, 4095, 0, 255); // Mapear el valor del potenciómetro en el rango opuesto
  }

  ledcWrite(PWM_CH, duty_cycle);

  if ((micros() - timer) > DT) {
    timer = micros();
    vel = (((pos - pos_ant) / DT) / (CPR * REDUCTOR)) * (1000000.0*60.0);
    vel_rad_s = vel * (2.0 * PI / 60.0);
    pos_ant = pos;
    
    Serial.println(vel);
  }
}
