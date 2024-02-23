//Zaida Irais López Mendieta A01708755
//Francisco Alejandro Velázquez Ledesma - A01709475
//Julio David Reséndiz Cruz - A01709375

#define IN_1 26
#define IN_2 27
#define EN 14

#define EN_A 18
#define EN_B 19

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

//Función para los cambios (FALLING) en el canal A
void IRAM_ATTR int_callback(){
  if(digitalRead(EN_B) == 0){
    pos = pos + 1;
  } else {
    pos = pos - 1;
  }
}

void setup() {
  Serial.begin(115200);
  //Se asigna funcion de los pines, sin es salida o entrada
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(EN_A, INPUT);
  pinMode(EN_B, INPUT);

  attachInterrupt(EN_A, int_callback, FALLING);

  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(EN, PWM_CH);
}

void loop() {
  static unsigned long lastTime = 0;
  static float voltage = 0.0; // Voltaje inicial
  const float maxVoltage = 12.0; // Voltaje máximo
  const float voltageStep = 0.05; // Incremento de voltaje
  const unsigned long stepDelay = 50; // Retardo de tiempo en milisegundos
  digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW );
  // Incrementar el voltaje cada 50ms
  if(millis() - lastTime >= stepDelay && voltage < maxVoltage) {
    lastTime = millis();
    voltage += voltageStep;
    
    // Calcular el valor de duty cycle correspondiente al voltaje actual
    int dutyCycle = (int)(voltage / maxVoltage * 255);
    ledcWrite(PWM_CH, dutyCycle);
    
    // Imprimir el voltaje y la velocidad actual
    Serial.print("Voltaje: ");
    Serial.print(voltage);
  //  Serial.print(" ");
    Serial.print(" V, Velocidad: ");
    Serial.println(vel); // Velocidad impresa en el cálculo de abajo
  }

  // Estimar la velocidad del motor y convertirla a rad/s
  if ((micros() - timer) > DT) {
    timer = micros();
    vel = (((pos - pos_ant) / DT) / (CPR * REDUCTOR)) * (1000000.0*60.0);
    vel_rad_s = vel * (2.0 * PI / 60.0); // Convertir RPM a rad/s
    pos_ant = pos;
  }
  
  
}
