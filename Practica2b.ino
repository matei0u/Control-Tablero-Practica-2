#include <Controllino.h>

// --- Definicion de Pines ---
// Pines para el Semaforo A
const int SEM_A_ROJO = CONTROLLINO_D0;
const int SEM_A_AMARILLO = CONTROLLINO_D1;
const int SEM_A_VERDE = CONTROLLINO_D2;

// Pines para el Semaforo B
const int SEM_B_ROJO = CONTROLLINO_D6;
const int SEM_B_AMARILLO = CONTROLLINO_D7;
const int SEM_B_VERDE = CONTROLLINO_D8;

// --- Definicion de Tiempos ---
const unsigned long T_VERDE = 4000;     // 4 segundos
const unsigned long T_AMARILLO = 1000;  // 1 segundo

// --- Maquina de Estados (FSM) ---

// 1. Define los nombres de los estados
enum EstadoInterseccion {
  A_VERDE,
  A_AMARILLO,
  B_VERDE,
  B_AMARILLO
};

// 2. Define una plantilla (struct) para guardar los datos de un estado
struct FSM_Estado {
  // Salidas (HIGH o LOW) para cada LED
  uint8_t out_A_R, out_A_Y, out_A_G;
  uint8_t out_B_R, out_B_Y, out_B_G;
  
  // Tiempo que dura este estado
  unsigned long duracion; 
  
  // Estado que sigue despues
  EstadoInterseccion proximo_estado; 
};

// 3. Crea la tabla con la logica de todos los estados
const FSM_Estado tabla_fsm[] = {
  // A_R, A_Y, A_G,   B_R, B_Y, B_G,   Duracion,     Proximo Estado
  { LOW, LOW, HIGH,  HIGH, LOW, LOW,   T_VERDE,      A_AMARILLO  }, // Estado A_VERDE
  { LOW, HIGH, LOW,  HIGH, LOW, LOW,   T_AMARILLO,   B_VERDE     }, // Estado A_AMARILLO
  { HIGH, LOW, LOW,  LOW, LOW, HIGH,   T_VERDE,      B_AMARILLO  }, // Estado B_VERDE
  { HIGH, LOW, LOW,  LOW, HIGH, LOW,   T_AMARILLO,   A_VERDE     }  // Estado B_AMARILLO
};


// --- Variables Globales ---

// Guarda el estado en el que estamos ahora
EstadoInterseccion estado_actual;     

// Guarda el tiempo en que comenzo el estado actual
unsigned long t_inicio_estado = 0;  


// --- Funcion para cambiar las luces ---
// Lee la tabla 'tabla_fsm' y aplica las salidas
void setLuces(EstadoInterseccion estado) {
  digitalWrite(SEM_A_ROJO,     tabla_fsm[estado].out_A_R);
  digitalWrite(SEM_A_AMARILLO, tabla_fsm[estado].out_A_Y);
  digitalWrite(SEM_A_VERDE,    tabla_fsm[estado].out_A_G);
  
  digitalWrite(SEM_B_ROJO,     tabla_fsm[estado].out_B_R);
  digitalWrite(SEM_B_AMARILLO, tabla_fsm[estado].out_B_Y);
  digitalWrite(SEM_B_VERDE,    tabla_fsm[estado].out_B_G);
}


// --- Configuracion Inicial ---
void setup() {
  // Configura los 6 pines como SALIDA
  pinMode(SEM_A_ROJO, OUTPUT);
  pinMode(SEM_A_AMARILLO, OUTPUT);
  pinMode(SEM_A_VERDE, OUTPUT);
  pinMode(SEM_B_ROJO, OUTPUT);
  pinMode(SEM_B_AMARILLO, OUTPUT);
  pinMode(SEM_B_VERDE, OUTPUT);

  // Inicia la maquina de estados
  estado_actual = A_VERDE;       // El primer estado es A_VERDE
  setLuces(estado_actual);       // Pone las luces de A_VERDE
  t_inicio_estado = millis();    // Inicia el temporizador
}

// --- Bucle Principal ---
void loop() {
  
  // Lee el tiempo actual
  unsigned long t_actual = millis();

  // Comprueba si ya paso el tiempo del estado actual
  if (t_actual - t_inicio_estado >= tabla_fsm[estado_actual].duracion) {
    
    // 1. Cambia al proximo estado
    estado_actual = tabla_fsm[estado_actual].proximo_estado;
    
    // 2. Aplica las luces del nuevo estado
    setLuces(estado_actual);
    
    // 3. Reinicia el temporizador
    t_inicio_estado = t_actual;
  }
}