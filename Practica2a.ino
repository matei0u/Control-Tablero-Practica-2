#include <Controllino.h>

// --- Definicion de Pines ---

// LEDs de la matriz 3x3
int numeros[9] = {
  CONTROLLINO_D0, CONTROLLINO_D6, CONTROLLINO_D12,
  CONTROLLINO_D13, CONTROLLINO_D14, CONTROLLINO_D8,
  CONTROLLINO_D2, CONTROLLINO_D1, CONTROLLINO_D7
};
// Constante con el numero total de LEDs
const int N_LEDS = sizeof(numeros) / sizeof(numeros[0]);

// Botones
const int boton_espiral = CONTROLLINO_I16;  // Boton 1: Espiral normal
const int boton_inverso = CONTROLLINO_I17;  // Boton 2: Espiral inversa
const int boton_reset = CONTROLLINO_I18;    // Boton 3: Reset (PIN ASIGNADO)

// --- Maquina de Estados (FSM) ---

// 1. Definicion de los estados del sistema
enum EstadoFSM {
  APAGADO,
  ESPIRAL_NORMAL,
  ESPIRAL_INVERSA
};

// 2. Variable para guardar el estado actual
EstadoFSM estado_actual = APAGADO;

// --- Variables de Temporizacion (No bloqueante) ---
unsigned long t_previo_espiral = 0;    
unsigned long intervalo_espiral = 500; 

// --- Variables de Punteros ---
// Puntero al pin del LED actual
int *ptr_led_actual = numeros;
// Puntero al inicio del array (para comparaciones)
const int *ptr_led_inicio = numeros;
// Puntero al final del array (para comparaciones)
const int *ptr_led_fin = numeros + (N_LEDS - 1);


// --- Variables para Deteccion de Botones ---
int b1_estado_ant = LOW; // Estado anterior del boton 1
int b2_estado_ant = LOW; // Estado anterior del boton 2
int b3_estado_ant = LOW; // Estado anterior del boton 3


// --- Funcion Auxiliar para apagar todos los LEDs ---
void apagarTodosLosLeds() {
  // Recorre el array de LEDs y los pone en BAJO
  for (int i = 0; i < N_LEDS; i++) {
    digitalWrite(numeros[i], LOW);
  }
}

void setup() {
  // Configurar todos los LEDs como SALIDA
  for (int i = 0; i < N_LEDS; i++) {
    pinMode(numeros[i], OUTPUT);
  }

  // Configurar los botones como ENTRADA
  pinMode(boton_espiral, INPUT);
  pinMode(boton_inverso, INPUT);
  pinMode(boton_reset, INPUT);
}



void loop() {
  // --- 1. Lectura de Entradas (Botones) ---
  int b1_estado_actual = digitalRead(boton_espiral);
  int b2_estado_actual = digitalRead(boton_inverso);
  int b3_estado_actual = digitalRead(boton_reset);

  // --- 2. Logica de Transiciones de Estado (FSM) ---

  // Transicion: Reset (Boton 3) - Se presiono el boton 3?
  if (b3_estado_actual == HIGH && b3_estado_ant == LOW) {
    estado_actual = APAGADO; // Cambia al estado APAGADO
    apagarTodosLosLeds();
  }
  
  // Transicion: Espiral Normal (Boton 1) - Se presiono el boton 1?
  else if (b1_estado_actual == HIGH && b1_estado_ant == LOW) {
    if (estado_actual != ESPIRAL_NORMAL) { // Actua solo si no estaba ya en ese modo
      estado_actual = ESPIRAL_NORMAL;
      apagarTodosLosLeds();
      ptr_led_actual = ptr_led_inicio;   // Pone el puntero al inicio del array
      digitalWrite(*ptr_led_actual, HIGH); // Enciende el primer LED
      t_previo_espiral = millis();       // Inicia temporizador
    }
  }

  // Transicion: Espiral Inversa (Boton 2) - Se presiono el boton 2?
  else if (b2_estado_actual == HIGH && b2_estado_ant == LOW) {
     if (estado_actual != ESPIRAL_INVERSA) { // Actua solo si no estaba ya en ese modo
      estado_actual = ESPIRAL_INVERSA;
      apagarTodosLosLeds();
      ptr_led_actual = ptr_led_fin;        // Pone el puntero al final del array
      digitalWrite(*ptr_led_actual, HIGH); // Enciende el ultimo LED
      t_previo_espiral = millis();       // Inicia temporizador
     }
  }

  // Guardar estado actual de botones para proxima iteracion (deteccion de flanco)
  b1_estado_ant = b1_estado_actual;
  b2_estado_ant = b2_estado_actual;
  b3_estado_ant = b3_estado_actual;


  // --- 3. Acciones de Estado (FSM) ---
  unsigned long t_actual = millis(); // Obtiene el tiempo actual

  // Ejecuta codigo diferente segun el estado actual
  switch (estado_actual) {
    
    case APAGADO:
      // No hacer nada
      break;

    case ESPIRAL_NORMAL:
      // Comprueba si ya paso el tiempo de intervalo
      if (t_actual - t_previo_espiral >= intervalo_espiral) {
        digitalWrite(*ptr_led_actual, LOW); // Apaga el LED actual
        
        ptr_led_actual++; // Avanza el puntero al siguiente LED
        
        // Si el puntero se paso del final, vuelve al inicio
        if (ptr_led_actual > ptr_led_fin) {
          ptr_led_actual = ptr_led_inicio;
        }
        
        digitalWrite(*ptr_led_actual, HIGH); // Enciende el nuevo LED
        t_previo_espiral = t_actual;         // Resetea el temporizador
      }
      break;

    case ESPIRAL_INVERSA:
      // Comprueba si ya paso el tiempo de intervalo
      if (t_actual - t_previo_espiral >= intervalo_espiral) {
        digitalWrite(*ptr_led_actual, LOW); // Apaga el LED actual
        
        ptr_led_actual--; // Retrocede el puntero al LED anterior
        
        // Si el puntero se paso del inicio, vuelve al final
        if (ptr_led_actual < ptr_led_inicio) {
          ptr_led_actual = ptr_led_fin;
        }
        
        digitalWrite(*ptr_led_actual, HIGH); // Enciende el nuevo LED
        t_previo_espiral = t_actual;         // Resetea el temporizador
      }
      break;
  }
}