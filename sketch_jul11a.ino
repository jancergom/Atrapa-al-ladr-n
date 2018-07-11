// DESCOBRINT L’INTERNET DE LES COSES A TRAVÉS D’ARDUINO
// - UPF Campus Junior 2018
// - Atrapa a un Intruso

// SENSOR DE LUZ
int PIN_SENSOR_LUZ = A0;  // PIN para el input del sensor de luz
int valor_sensor_luz;     // Variable para guardar el valor que leemos del sensor
int alarma_activada = 0;
int estado_switch = 0;

// LED DE ALARMA
const int PIN_LED_ALARMA = 8;           // PIN digital para activar el LED de alarma
const int THRESHOLD_ALARMA_LUZ = 600;   // Valor minimo que se debe percibir para NO activar la alarma (si es menor, se activa la alarma)

// SWITCH DE ACTIVACION DEL SISTEMA

const int PIN_LED_INTEGRADO = 13;   // PIN digital para activar el LED integrado
const int PIN_SWITCH = 2;  // PIN para el input del switch
int sistema_activo = 0;  // Variable para detectar si el sistema esta activo (ON) o no (OFF)


// BUZZER
const int PIN_BUZZER = 3;   // PIN para el buzzer (alarma sonora)

const int frecuencia_buzzer = 880;  // Frecuencia sonido buzzer en Hz
//***************************************

// SENSOR DE PROXIMIDAD HR-SR04
const int PIN_HR_ECHO = 7;        // PIN echo del HR-SR04
const int PIN_HR_TRIGG = 12;      // PIN trigger del HR-SR04
long duracion_hr;                 // Duracion entre rayos (ida y vuelta) 
long distancia_hr;                // Distancia estimada del objeto detectado
const int UMBRAL_ALARMA_DISTANCIA = 100; // Valor minimo para saltar alarma por distancia [cm]
//**********************************
int tiempo_entre_medidas = 1000;  // Tiempo entre dos medidas consecutivas [ms]


void setup() {

  pinMode(PIN_LED_ALARMA, OUTPUT);  // Inicializar PIN del LED de alarma como salida (output)
  pinMode(PIN_LED_INTEGRADO, OUTPUT);  // Inicializar PIN del LED integrado como salida (output)
  pinMode(PIN_SWITCH, INPUT); 
  pinMode(PIN_BUZZER, OUTPUT);      // Inicializar PIN del BUZEER como salida (output)
  pinMode(PIN_HR_TRIGG, OUTPUT);    // Inicializar PIN del TRIGGER del sensor de proximidad como salida (output). Para el rayo que lanzamos.
  pinMode(PIN_HR_ECHO, INPUT);      // Inicializar PIN del ECHO del sensor de proximidad como entrada (input). Para el rayo rebotado que recibimos.
  
  Serial.begin(9600); // Setear comunicación serial con la placa Arduino (NO TOCAR)
  
  Serial.println("¡SISTEMA DE ALARMA DEL GRUPO 2!");            

}

void loop() {

  estado_switch = digitalRead(PIN_SWITCH); // Leer de sensor digital (0 o 1)
 
    if(!alarma_activada && estado_switch){
      Serial.println("SISTEMA ACTIVO");
    } else if(!estado_switch && alarma_activada) {
      Serial.println("SISTEMA DESACTIVADO");
    }

  alarma_activada = estado_switch;
 
  if(alarma_activada == 1){
    
    digitalWrite(PIN_LED_INTEGRADO,HIGH); // Encender LED integrado
   
    valor_sensor_luz = analogRead(PIN_SENSOR_LUZ);  // Leer el valor percibido por el sensor
    Serial.print("- Valor de luz detectado: ");   // Printar el valor percibido por el sensor por consola
    Serial.println(valor_sensor_luz); 
                 
   // SENSOR DE PROXIMIDAD
    // Tomar muestra de distancia
    digitalWrite(PIN_HR_TRIGG, LOW);          // Desactivar muestreo
    delayMicroseconds(2);                     // Pequeno retraso para medir distancia
    digitalWrite(PIN_HR_TRIGG, HIGH);         // Activar muestreo
    delayMicroseconds(10);                    // Pequeno retraso para medir distancia
    digitalWrite(PIN_HR_TRIGG, LOW);          // Activar muestreo de nuevo
    duracion_hr = pulseIn(PIN_HR_ECHO, HIGH); // Medir tiempo
    distancia_hr = (duracion_hr/2) / 29.1;    // Estimar distancia
    Serial.print("- Distancia: ");
    Serial.print(distancia_hr); // Printar el valor percibido por el sensor por consola
    Serial.println(" cm");


    if(valor_sensor_luz < THRESHOLD_ALARMA_LUZ || distancia_hr < UMBRAL_ALARMA_DISTANCIA ){
      Serial.println("  (+) Luz alarmante.");  
      // Encender LED con parpadeo
      for (int i=0; i <= 5; i++){
        digitalWrite(PIN_LED_ALARMA, HIGH); // Encender LED de alarma
        delay(25);
        digitalWrite(PIN_LED_ALARMA, LOW);  // Apagar LED de alarma
        delay(25);

       tone(3,500);
      }
      digitalWrite(PIN_LED_ALARMA, HIGH); // Encender LED de alarma
    } else {
      Serial.println("  (-) Luz NO alarmante.");
      digitalWrite(PIN_LED_ALARMA, LOW);  // Apagar LED de alarma
      noTone(3);  
    }

  } else{
    digitalWrite(PIN_LED_INTEGRADO,LOW);
    digitalWrite(PIN_LED_ALARMA,LOW); 
    noTone(3); 
    
    } 
    delay(100);           
   }
