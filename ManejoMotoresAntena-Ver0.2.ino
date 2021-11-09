/*
 Nombre: ManejoMotoresAntena-Rev008
 
 Descripción: Manejo de los motores de las antenas del IAR
              Sistema de encendido y apagado de motores segun lo 
              requiera el sistema de observación para dirigir a la
              antena hasta la posición de la fuente que se desea 
              observar
 
 Fecha de creación: NOV 2019
 Autor: A. Donantueno
 Colaborador: C. Cristina Miguel
 Revisión: F. Hauscarriaga
*/

#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

// Inicialización de la placa de red con el número de MAC e IP
// Los valores de gateway y subnet son opcionales
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED 
};
IPAddress ip(163, 10, 43, 22);
IPAddress myDns(163, 10, 43, 1);
IPAddress gateway(163, 10, 43, 3);
IPAddress subnet(255, 255, 255, 0);

IPAddress IpRemota;

// Definiciones generales
#define PIN_SS                10 
#define PORT                  6666
#define BUFFER_SIZE           5
#define ANS_SIZE              6
#define PAYLOAD               3
#define COMANDOS              13

// Definiciones orientacion
#define SLOW_NORTH            0X80
#define FAST_NORTH            0X40
#define SLOW_SOUTH            0X20
#define FAST_SOUTH            0X10
#define SLOW_EAST             0X08
#define FAST_EAST             0X04
#define SLOW_WEST             0X02
#define FAST_WEST             0X01

// Definición "PINS ACTUADORES" RET - RETORNO - SENSADO
#define RET_DIR_NS          29 //PORTA7 
#define RET_DEF_NS          28 //PORTA6
#define RET_RAP_NS          27 //PORTA5
#define RET_LEN_NS          26 //PORTA4
#define RET_DIR_EW          25 //PORTA3
#define RET_DEF_EW          24 //PORTA2
#define RET_RAP_EW          23 //PORTA1 
#define RET_LEN_EW          22 //PORTA0
#define RET_PINS            PORTA

// Definiciones Reles de 2 a 9 
#define DIR_NS               4 //PORTH4 7
#define DEF_VEL_NS           5 //PORTH3 6
#define RAP_VEL_NS           6 //PORTE3 5
#define LEN_VEL_NS           7 //PORTG5 4
#define DIR_EW               8 //PORTE5 3
#define DEF_VEL_EW           9 //PORTE4 2
#define RAP_VEL_EW           2 //PORTH6 9
#define LEN_VEL_EW           3 //PORTH5 8
#define PINS                 (((PORTH & B00011000) << 3) | ((PORTE & B00001000) << 2) | ((PORTG & B00100000) >> 1)|((PORTE & B00110000) >> 2) | ((PORTH & B01100000) >> 5))
#define PINSACTUADORES       PORTA
//#define PINGRAL              ((PORTB & B00100000) >> 5)

// Definiciones de las salidas digitales a usar
#define GENERAL               11 //
#define RET_GENERAL           30 // PORTC7

// Definiciones STOP
#define STOP_NS               0XC0
#define STOP_EW               0XC1
#define STOP_ALL              0XC2

// Definiciones de los campos del protocolo
#define SYNC_BYTE             0xA0
#define COMMAND_USER          0x8E
#define COMMAND_ENGINEER      0x8D
#define ANSWER_CODE           0x8F
#define END_BYTE              0x9F

// Definiciones de respuesta del protocolo
#define INFO_ENGINE           0x11
#define INFO_ELECTRICAL       0x12
#define INFO_CMD_EXECUTE      0x13
#define WARNING               0x02
#define ERR_CMD_INVALID       0x31
#define ERR_CMD_FAILED        0x32
#define ERR_CONDITION_INVALID 0x33


// Definiciones de funciones dinámicas
typedef void (*command)(byte *respuesta, boolean ing);

struct Command {
  byte comando;
  command cmd; 
};

// Definición arreglo para lookup table
Command comandosValidos[COMANDOS];

// Definiciones de variables
byte dataSerie[BUFFER_SIZE];                  // LECTURA VIA UART
byte thisChar;                                // LECTURA VIA ETHERNET
byte buffer[BUFFER_SIZE];                     // BUFFER 
byte respuesta[ANS_SIZE];                     // PAQUETE DE RESPUESTA
byte estadoReles;
byte estadoRetornoReles;                      // MANEJO ESTADO MOTORES
bool cmdIngenieria;
int i;

// Definición ambiente millis() para chequeo de retorno de reles
int periodo = 2000; // (un segundo)
unsigned long ahora = 0;

// Definicion del puerto del servidor
EthernetServer server(PORT);

bool alreadyConnected = false;                // whether or not the client was connected previously

void setup() {
  //Disable WatchDog, until initialize loop
  wdt_disable();
    // Define reles pins as output
  pinMode(DIR_NS,OUTPUT);
  pinMode(DEF_VEL_NS,OUTPUT);
  pinMode(RAP_VEL_NS,OUTPUT);
  pinMode(LEN_VEL_NS,OUTPUT);
   
  pinMode(DIR_EW,OUTPUT);
  pinMode(DEF_VEL_EW,OUTPUT);
  pinMode(RAP_VEL_EW,OUTPUT);
  pinMode(LEN_VEL_EW,OUTPUT);

/*
  pinMode(RET_DIR_NS,INPUT);
  pinMode(RET_DEF_VEL_NS,INPUT);
  pinMode(RET_RAP_VEL_NS,INPUT);
  pinMode(RET_LEN_VEL_NS,INPUT);
   
  pinMode(RET_DIR_EW,INPUT);
  pinMode(RET_DEF_VEL_EW,INPUT);
  pinMode(RET_RAP_VEL_EW,INPUT);
  pinMode(RET_LEN_VEL_EW,INPUT);
*/

  // Define rele para energizar todo el sistema
  pinMode(GENERAL,OUTPUT);
  
  // Define initial values (HIGH = OFF)
  digitalWrite(DIR_NS,HIGH);
  digitalWrite(DEF_VEL_NS,HIGH);
  digitalWrite(RAP_VEL_NS,HIGH);
  digitalWrite(LEN_VEL_NS,HIGH);
  
  digitalWrite(DIR_EW,HIGH);
  digitalWrite(DEF_VEL_EW,HIGH);
  digitalWrite(RAP_VEL_EW,HIGH);
  digitalWrite(LEN_VEL_EW,HIGH);

  // Define initial value GENERAL rele
  // digitalWrite(GENERAL,HIGH);
  
  memset(comandosValidos, 0, sizeof(comandosValidos));
  comandosValidos[0].comando        = 0x80;
  comandosValidos[0].cmd            = norteLento; 
  comandosValidos[1].comando        = 0x40;
  comandosValidos[1].cmd            = norteRapido; 
  comandosValidos[2].comando        = 0x20;
  comandosValidos[2].cmd            = surLento; 
  comandosValidos[3].comando        = 0x10;
  comandosValidos[3].cmd            = surRapido; 
  comandosValidos[4].comando        = 0x08;
  comandosValidos[4].cmd            = esteLento; 
  comandosValidos[5].comando        = 0x04;
  comandosValidos[5].cmd            = esteRapido; 
  comandosValidos[6].comando        = 0x02;
  comandosValidos[6].cmd            = oesteLento; 
  comandosValidos[7].comando        = 0x01;
  comandosValidos[7].cmd            = oesteRapido;
  comandosValidos[8].comando        = 0xc0;
  comandosValidos[8].cmd            = pararNorteSur;
  comandosValidos[9].comando        = 0xc1;
  comandosValidos[9].cmd            = pararEsteOeste;
  comandosValidos[10].comando       = 0xc2;
  comandosValidos[10].cmd           = pararMotores;
  comandosValidos[11].comando       = 0xb0;
  comandosValidos[11].cmd           = encender;
  comandosValidos[12].comando       = 0xb1;
  comandosValidos[12].cmd           = apagar;  

  memset(respuesta, 0, sizeof(respuesta));
  respuesta[0] = 160;
  respuesta[1] = 143;
  respuesta[2] = 2;
  respuesta[5] = 159; 

  // Configuración del pin CS (pin conectado para arduino Mega Slave Selector)
  Ethernet.init(PIN_SS); 

  // Inicialización de la placa ethernet
  Ethernet.begin(mac, ip, myDns, gateway, subnet);

  // Apertura del puerto serie de comunicaciones y se espera la conexión (necesario solo para puertos USB nativos) 
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Verifica que el hardware ethernet este presente
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("No se encontró a la placa Ethernet.  No se puede ejecutar sin este hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  // Verifica que el cable de red este conectado 
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("El cable de red no esta conectado.");
  }

  // Iniciación del sistema 
  pararMotores(respuesta, cmdIngenieria);
  encender(respuesta, cmdIngenieria);
  
  // Setting timeOUT of WatchDog in 4 seconds
  wdt_enable(WDTO_4S);  
  
  // Start listening to the clientss
  server.begin();
}
void loop() {
  wdt_reset();
  // Obtiene al cliente que esta conectado con datos para entregar, SINO DEVUELVE FALSO
  EthernetClient client = server.available();
  // Reading through Ethernet Shield
  if (client) {
    if (!alreadyConnected) {
      client.flush();                                     // clean BUFFER
      // Keep the client IP 
      IpRemota = client.remoteIP();
      alreadyConnected = true;
    }
    // Se verifica que el cliente que intenta enviar es el que venía haciéndolom sino lo detiene y mantiene a la otra conexión
    if (client.remoteIP() != IpRemota) {
      client.println("*** LA CONEXIÓN SE CERRARÁ YA HAY UN USUARIO CONECTADO ***");
      client.stop();
    }
    // Mientras haya Bytes del cliente se arma el paquete recibido
    // Control del armado del Buffer
    i = 0; // Para el armado de los paquetes
    thisChar = client.read();
    if (thisChar == SYNC_BYTE){
      buffer[i] = thisChar;
      while (client.available() > 0 && thisChar != END_BYTE)
      {
        i = i + 1;
        thisChar = client.read();
        buffer[i] = thisChar;
      }  
      verificarPayload(buffer);
    }
  }else if (Serial.available()){
  // Reading through UART
     i = 0;
     thisChar = Serial.read(); // Verificar que la lectura en el Serial 1 se haga así
     if (thisChar == SYNC_BYTE){ 
       buffer[i] = thisChar;
       while (Serial.available() > 0 && thisChar != END_BYTE)
       {
         i = i + 1;
         thisChar = client.read();
         buffer[i] = thisChar;
       }
       // Comprobación de armado de paquete
         Serial.println(statusMotores(),BIN);
         verificarPayload(buffer);
     }
   }
   if (millis()> (ahora + periodo)){
     ahora = millis();
     Serial.println("Se verifica status de motores con instruccion recibida");
     Serial.println(PINSACTUADORES, BIN);
   } 
}// FIN loop()

//check in the lookup table if the entered command corresponds to one of the list
void verificarPayload (byte *mensaje){
  boolean cmdValido = false;
  //if (PINGRAL == 1){
   // Serial.println(PINGRAL);
    if (mensaje[1] == COMMAND_ENGINEER){
      cmdIngenieria = true; 
    }
    for (int i = 0; i <= 12; i++) {
      if (mensaje[PAYLOAD] == comandosValidos[i].comando){
        cmdValido = true; // Is valid command
        (*comandosValidos[i].cmd)(respuesta, cmdIngenieria);
      }
    }
    if (!cmdValido){
      Serial.println("El comando recibido es invalido");
      enviarPaquete(ERR_CMD_INVALID,buffer[3],respuesta);
    }
  //}else{
   // Serial.println("Sistema desenergizado");
 // }
   
}

// Funciones de dirección y velocidad de los motores de la antena
// NORTE LENTO 1100
void norteLento (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_NS,HIGH);
    digitalWrite(RAP_VEL_NS,HIGH);
    digitalWrite(DEF_VEL_NS,LOW);
    digitalWrite(DIR_NS,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,SLOW_NORTH,ans);
  }else {
    if (operarNS(statusMotores())){
      digitalWrite(DEF_VEL_NS,LOW);
      digitalWrite(DIR_NS,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,SLOW_NORTH,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,SLOW_NORTH,ans); //EL SEGUNDO PARAMETRO DEBERÍA SER LO QUE QUIZO EJECUTAR O LO QUE ESTA OCURRIENDO EN ESTE MOMENTO (?)
    }  
  }
}
// NORTE RAPIDO 1000
void norteRapido (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_NS,HIGH);
    digitalWrite(RAP_VEL_NS,HIGH);
    digitalWrite(DEF_VEL_NS,HIGH);
    digitalWrite(DIR_NS,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,FAST_NORTH,ans);
  }else{
    if (operarNS(statusMotores())){
      digitalWrite(DIR_NS,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,FAST_NORTH,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,FAST_NORTH,ans);
    }  
  }
}
// SUR LENTO 1101
void surLento (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_NS,LOW);
    digitalWrite(RAP_VEL_NS, HIGH);
    digitalWrite(DEF_VEL_NS,LOW);
    digitalWrite(DIR_NS,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,SLOW_SOUTH,ans);
  }else{
    if (operarNS(statusMotores())){
      digitalWrite(LEN_VEL_NS,LOW);
      digitalWrite(DEF_VEL_NS,LOW);
      digitalWrite(DIR_NS,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,SLOW_SOUTH,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,SLOW_SOUTH,ans);
    }  
  }
}
// SUR RAPIDO 1010
void surRapido (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_NS,HIGH);
    digitalWrite(RAP_VEL_NS,LOW);
    digitalWrite(DEF_VEL_NS,HIGH);
    digitalWrite(DIR_NS,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,FAST_SOUTH,ans);
  }else{
    if (operarNS(statusMotores())){
      digitalWrite(RAP_VEL_NS,LOW);
      digitalWrite(DIR_NS,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,FAST_SOUTH,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,FAST_SOUTH,ans);
    } 
  }
}
void esteLento (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_EW,HIGH);
    digitalWrite(RAP_VEL_EW,HIGH);
    digitalWrite(DEF_VEL_EW,LOW);
    digitalWrite(DIR_EW,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,SLOW_EAST,ans);
  }else{
    if (operarEO(statusMotores())){
      digitalWrite(DEF_VEL_EW,LOW);
      digitalWrite(DIR_EW,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,SLOW_EAST,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,SLOW_EAST,ans);
    }  
  }
}
void esteRapido (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_EW,HIGH);
    digitalWrite(RAP_VEL_EW,HIGH);
    digitalWrite(DEF_VEL_EW,HIGH);
    digitalWrite(DIR_EW,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,FAST_EAST,ans);
  }else{
    if (operarEO(statusMotores())){
      digitalWrite(DIR_EW,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,FAST_EAST,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,FAST_EAST,ans);    
    }  
  }
}
void oesteLento (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_EW,LOW);
    digitalWrite(RAP_VEL_EW,HIGH);
    digitalWrite(DEF_VEL_EW,LOW);
    digitalWrite(DIR_EW,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,SLOW_WEST,ans);
  }else{
    if (operarEO(statusMotores())){
      digitalWrite(LEN_VEL_EW,LOW);
      digitalWrite(DEF_VEL_EW,LOW);
      digitalWrite(DIR_EW,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,SLOW_WEST,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,SLOW_WEST,ans);
    }  
  }
}
void oesteRapido (byte *ans, boolean ing)
{
  if (ing == true){
    digitalWrite(LEN_VEL_EW,HIGH);
    digitalWrite(RAP_VEL_EW,LOW);
    digitalWrite(DEF_VEL_EW,HIGH);
    digitalWrite(DIR_EW,LOW);
    enviarPaquete(INFO_CMD_EXECUTE,FAST_WEST,ans);
  }else{
    if(operarEO(statusMotores())){
      digitalWrite(RAP_VEL_EW,LOW);
      digitalWrite(DIR_EW,LOW);
      enviarPaquete(INFO_CMD_EXECUTE,FAST_WEST,ans);
    }else{
      Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
      enviarPaquete(ERR_CONDITION_INVALID,FAST_WEST,ans);  
    }  
  }
}
// Funciones para parar los motores de la antena
void pararNorteSur (byte *ans, boolean ing)
{
  //if (!operarNS(statusMotores())){ // va negado (!) 
    digitalWrite(DIR_NS,HIGH);
    digitalWrite(DEF_VEL_NS,HIGH);
    digitalWrite(RAP_VEL_NS,HIGH);
    digitalWrite(LEN_VEL_NS,HIGH);
    enviarPaquete(INFO_CMD_EXECUTE,STOP_NS,ans);
  //} else {
    //Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
    //enviarPaquete(ERR_CONDITION_INVALID,STOP_NS,ans);
 // }
}
void pararEsteOeste (byte *ans, boolean ing)
{
  //if (!operarEO(statusMotores())){
    digitalWrite(DIR_EW,HIGH);
    digitalWrite(DEF_VEL_EW,HIGH);
    digitalWrite(RAP_VEL_EW,HIGH);
    digitalWrite(LEN_VEL_EW,HIGH);
    enviarPaquete(INFO_CMD_EXECUTE,STOP_EW,ans);
  //} else {
   // Serial.println("LA OPERACIÓN REQUERIDA ES INVALIDA");
    //enviarPaquete(ERR_CONDITION_INVALID,STOP_EW,ans);
 // }
}
void pararMotores (byte *ans, boolean ing)
{
  //EN ESTA SITUACIÓN NO SE CHEQUEA SINO DIRECTAMENTE SE ENVÍA LA INSTRUCCIÓN DE PARO
  //Serial.println("OPERACION REQUERIDA: Parar todos los motores");
  // STOP NS MOTOR
  digitalWrite(DIR_NS,HIGH);
  digitalWrite(DEF_VEL_NS,HIGH);
  digitalWrite(RAP_VEL_NS,HIGH);
  digitalWrite(LEN_VEL_NS,HIGH);
  //STOP EW MOTOR
  digitalWrite(DIR_EW,HIGH);
  digitalWrite(DEF_VEL_EW,HIGH);
  digitalWrite(RAP_VEL_EW,HIGH);
  digitalWrite(LEN_VEL_EW,HIGH);
  enviarPaquete(INFO_CMD_EXECUTE,STOP_ALL,ans);
}

// Funciones de energizado del sistema de motores
void encender (byte *ans, boolean ing){
  Serial.println("OPERACION REQUERIDA: Energizar el sistema");
  //STOP NS MOTOR
  digitalWrite(DIR_NS,HIGH);
  digitalWrite(DEF_VEL_NS,HIGH);
  digitalWrite(RAP_VEL_NS,HIGH);
  digitalWrite(LEN_VEL_NS,HIGH);
  //STOP EW MOTOR
  digitalWrite(DIR_EW,HIGH);
  digitalWrite(DEF_VEL_EW,HIGH);
  digitalWrite(RAP_VEL_EW,HIGH);
  digitalWrite(LEN_VEL_EW,HIGH);
  //ENERGIZA LLAVE GENERAL
  digitalWrite(GENERAL,LOW); // SIENDO LOW EL VALOR PARA QUE SE ACTIVE EL RELE QUE ES CONFIGURABLE
  enviarPaquete(INFO_CMD_EXECUTE,STOP_ALL,ans); // Resta codificar correctamente la respuesta
}
void apagar (byte *ans, boolean ing){
  
  Serial.println("OPERACION REQUERIDA: Desenergizar el sistema");
  //STOP NS MOTOR
  digitalWrite(DIR_NS,HIGH);
  digitalWrite(DEF_VEL_NS,HIGH);
  digitalWrite(RAP_VEL_NS,HIGH);
  digitalWrite(LEN_VEL_NS,HIGH);
  //STOP EW MOTOR
  digitalWrite(DIR_EW,HIGH);
  digitalWrite(DEF_VEL_EW,HIGH);
  digitalWrite(RAP_VEL_EW,HIGH);
  digitalWrite(LEN_VEL_EW,HIGH);
  //DESENERGIZA LLAVE GENERAL
  digitalWrite(GENERAL,HIGH);     
  enviarPaquete(INFO_CMD_EXECUTE,STOP_ALL,ans);
}

void enviarPaquete (byte tipoRespuesta, byte respuesta, byte paquete[6]){
  paquete[3] = tipoRespuesta;
  paquete[4] = respuesta;
  server.write(paquete, 6);
}

// Comprueba que no este en operación ningún contactor del mecanismo Norte-Sur 
bool operarNS (byte statusMotores){
  if (((statusMotores & B11110000) >> 4 ) == B1111){
    return true;
  }else {
    return false;
  }
}

// Comprueba que no este en operación ningún contactor del mecanismo Este-Oeste 
bool operarEO (byte statusMotores){
  if ((statusMotores & B00001111) == B1111){
    return true;
  }else{
    return false;  
  }
}
byte statusMotores(){
  byte motores;
  motores = PINS;
  return motores;
}
