// SENSOR DE LUZ
int PIN_SENSOR_LUZ = A0;  // PIN para el input del sensor de luz
int valor_sensor_luz;     // Variable para guardar el valor que leemos del sensor

// LED DE ALARMA
const int PIN_LED_ALARMA = 8;           // PIN digital para activar el LED de alarma
const int THRESHOLD_ALARMA_LUZ = 500;     // Valor minimo que se debe percibir para NO activar la alarma (si es menor, se activa la alarma)

// SWITCH DE ACTIVACION DEL SISTEMA
const int PIN_LED_INTEGRADO = 13;   // PIN digital para activar el LED integrado
const int PIN_SWITCH = 2;  // PIN para el input del switch
int sistema_activo = 0;  // Variable para detectar si el sistema esta activo (ON) o no (OFF)

// SENSOR DE PROXIMIDAD HR-SR04
const int PIN_HR_ECHO = 7;        // PIN echo del HR-SR04
const int PIN_HR_TRIGG = 12;      // PIN trigger del HR-SR04
long duracion_hr;                 // Duracion entre rayos (ida y vuelta) 
long distancia_hr;                // Distancia estimada del objeto detectado
const int UMBRAL_ALARMA_DISTANCIA = 10; // Valor minimo para saltar alarma por distancia [cm]

int tiempo_entre_medidas = 1000;  // Tiempo entre dos medidas consecutivas [ms]

// MENSAJES
const char MENSAJE_INTRUSO_DETECTADO = 'D'; // Mensaje de intruso detectado a enviar al router
const char MENSAJE_TODO_BIEN = 'E';         // Mensaje de todo va bien a enviar al router 

void setup() {

  // Sensor de luz: el pin del sensor de luz es analógico y no hace falta inicializarlo
  
  pinMode(PIN_LED_ALARMA, OUTPUT);  // Inicializar PIN del LED de alarma como salida (output)
  pinMode(PIN_LED_INTEGRADO, OUTPUT);  // Inicializar PIN del LED integrado como salida (output)
  pinMode(PIN_SWITCH, INPUT);       // Inicializar PIN del switch como entrada (input)
  pinMode(PIN_HR_TRIGG, OUTPUT);    // Inicializar PIN del TRIGGER del sensor de proximidad como salida (output). Para el rayo que lanzamos.
  pinMode(PIN_HR_ECHO, INPUT);      // Inicializar PIN del ECHO del sensor de proximidad como entrada (input). Para el rayo rebotado que recibimos.

  Serial.begin(9600); // Setear comunicación serial con la placa Arduino (NO TOCAR)
   
}

void loop()
{

  int estado_switch = digitalRead(PIN_SWITCH); // Leer de sensor digital (0 o 1)
  
   
  // El estado del sistema coincide en cualquier caso con el estado del switch (ON o OFF)
  sistema_activo = estado_switch;

  // El sistema solo hace "cosas" si esta activo
  if(sistema_activo){

    digitalWrite(PIN_LED_INTEGRADO, HIGH); // Encender LED integrado
      
    valor_sensor_luz = analogRead(PIN_SENSOR_LUZ);  // Leer el valor percibido por el sensor
   
    // Tomar muestra de distancia
    digitalWrite(PIN_HR_TRIGG, LOW);          // Desactivar muestreo
    delayMicroseconds(2);                     // Pequeno retraso para medir distancia
    digitalWrite(PIN_HR_TRIGG, HIGH);         // Activar muestreo
    delayMicroseconds(10);                    // Pequeno retraso para medir distancia
    digitalWrite(PIN_HR_TRIGG, LOW);          // Activar muestreo de nuevo
    duracion_hr = pulseIn(PIN_HR_ECHO, HIGH); // Medir tiempo
    distancia_hr = (duracion_hr/2) / 29.1;    // Estimar distancia
  
    // Intruso detectado por sensor de luz o sensor de proximidad
    // - Lógica OR: haz que el sistema de la alarma tanto si se detecta al intruso por el sensor de luz o proximidad
    if( (valor_sensor_luz < THRESHOLD_ALARMA_LUZ) || (distancia_hr < UMBRAL_ALARMA_DISTANCIA && distancia_hr != 0) ){
      
      // Encender LED con parpadeo
      for (int i=0; i <= 5; i++){
        digitalWrite(PIN_LED_ALARMA, HIGH); // Encender LED de alarma
        delay(25);
        digitalWrite(PIN_LED_ALARMA, LOW);  // Apagar LED de alarma
        delay(25);
      }
      digitalWrite(PIN_LED_ALARMA, HIGH); // Encender LED de alarma

      // Enviar mensaje de "ha saltado la alarma" a la XBee router
      Serial.print(MENSAJE_INTRUSO_DETECTADO);                    


    // Ni el sensor de luz ni el de proximidad detecta intruso
    } else {
      digitalWrite(PIN_LED_ALARMA, LOW);  // Apagar LED de alarma
      // Enviar mensaje de "todo correcto" a la XBee router
      Serial.print(MENSAJE_TODO_BIEN); 
    }

    
  // Si el sistema esta apagado
  } else {
    digitalWrite(PIN_LED_INTEGRADO, HIGH); // Apagar LED integrado
    digitalWrite(PIN_LED_ALARMA, LOW);  // Apagar LED de alarma
  }

  delay(tiempo_entre_medidas);                    // Tiempo entre muestras en milisegundos
  
}
