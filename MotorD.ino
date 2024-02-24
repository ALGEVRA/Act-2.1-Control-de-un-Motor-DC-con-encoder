//Zaida Irais López Mendieta A01708755
//Francisco Alejandro Velázquez Ledesma - A01709475
//Julio David Reséndiz Cruz - A01709375

// Definición de pines para el control del motor y los encoders
#define IN_1 26
#define IN_2 27
#define EN 14
#define EN_A 18
#define EN_B 19

// Configuración del PWM para controlar la velocidad del motor
#define PWM_CH 0
#define FREQ 5000
#define RES 8

// Parámetros para la estimación de la velocidad del motor
#define DT 20000    // Intervalo de tiempo para la estimación de la velocidad (en microsegundos)
#define REDUCTOR 34 // Relación de reducción del motor
#define CPR 12      // Cuentas por revolución del encoder

// Variables para almacenar información del motor y la velocidad
float voltaje_motor;
float referencia = 0;
double pos = 0;
double pos_ant = 0;
double timer = 0;
float vel = 0;
float vel_rad_s = 0; // Velocidad en radianes por segundo

// Función de interrupción para contar pulsos del encoder
void IRAM_ATTR int_callback() {
  if (digitalRead(EN_B) == 0) {
    pos += 1;
  } else {
    pos -= 1;
  }
}

void setup() {
  Serial.begin(115200); // Iniciar comunicación serial para depuración

  // Configurar pines de control del motor
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(EN, OUTPUT);

  // Configurar pines de los encoders
  pinMode(EN_A, INPUT);
  pinMode(EN_B, INPUT);

  // Adjuntar la interrupción al pin del encoder A
  attachInterrupt(EN_A, int_callback, FALLING);

  // Configurar el PWM para el control de velocidad del motor
  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(EN, PWM_CH);
}

void loop() {
  // Verificar si hay datos disponibles en el puerto serial
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n'); // Leer la referencia de velocidad del puerto serial
    referencia = inputString.toFloat(); // Convertir la referencia a float

    // Calcular el voltaje necesario para alcanzar la referencia de velocidad
    if (referencia >= 0) {
      voltaje_motor = (referencia + 86.0) / 31.33; // Fórmula original
    } else {
      voltaje_motor = (referencia - 55.2) / (-29.4); // Nueva fórmula para velocidad negativa
    }

    int duty_cycle = map(voltaje_motor, 0, 12, 0, 255); // Asegurar que el duty_cycle esté en el rango 0-255
  
    // Controlar el sentido de giro del motor en base a la referencia
    if (referencia >= 0) {
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
    } else {
      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
    }

    ledcWrite(PWM_CH, duty_cycle); // Establecer la velocidad del motor
  }

  // Estimar la velocidad del motor y convertirla a rad/s
  if ((micros() - timer) > DT) {
    timer = micros();
    vel = (((pos - pos_ant) / DT) / (CPR * REDUCTOR)) * (1000000.0 * 60.0);
    vel_rad_s = vel * (2.0 * PI / 60.0); // Convertir RPM a rad/s
    pos_ant = pos;
    
    // Imprimir información por el puerto serial para depuración
    Serial.print("Referencia: ");
    Serial.print(referencia);
    Serial.print("   Voltaje: ");
    Serial.print(voltaje_motor);
    Serial.print("   Velocidad (RPM): ");
    Serial.print(vel);
    Serial.print("   Velocidad (rad/s): ");
    Serial.println(vel_rad_s);
  }
}
