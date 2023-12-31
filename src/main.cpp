/*Conexiones 

D1 ----> SCL oximetro y oled 
D2 ----> SDA oximetro y oled
5V ----> 5V  oximetro y oled
G  ----> GND de todos los dispositivos

Bateria 
positivo a los 5V del Wemos
Gnd es general

Conectar la oled y el max30100 al wemos, Probamos la pagina web con el max y la oled, que funcione eso primero, despues cambiamos los pines del gps RX/TX al D5/D6 del Wemos
con codigo hacemos que llame al gps solo cuando sea necesario, dejarlo aparte del programa y solo ir con el Timer (TICKER)
*/
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/*const char* ssid ="Kojono";                   //"6380465";
const char* password ="Racing2023";               //"12345678";
ESP8266WebServer server(80);*/

#include <MAX30100_PulseOximeter.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display( 128, 64, &Wire, -1);

#define ENABLE_MAX30100 1
#if ENABLE_MAX30100
#define REPORTING_PERIOD_MS     5000
PulseOximeter pox;
#endif


uint32_t tsLastReport = 0;

int cuenta = 0;
int flagcambio = 0;
int pantalla = 0;
void Contador(void);

int dispositivos = 0;

  int bpm = 0;
  int spo2 = 0;
  float latitude, longitude;
  
TinyGPS gps;
SoftwareSerial serialgps(12,14);//   (RX,TX)
static const unsigned char PROGMEM logomemorylane[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x3f, 0xf0, 0x1f, 0xf8, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x1e, 0x00, 0x01, 0xff, 0xfc, 0x1c, 0x30, 0x7f, 0xff, 0x00, 0x01, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x38, 0x00, 0x03, 0x87, 0x98, 0x4c, 0x62, 0x33, 0xc3, 0xc0, 0x00, 0x78, 0x00, 0x00, 
  0x00, 0x00, 0xe0, 0x00, 0x06, 0x07, 0x3a, 0x7c, 0x7e, 0x39, 0xc0, 0xc0, 0x00, 0x1e, 0x00, 0x00, 
  0x00, 0x03, 0xc0, 0x00, 0x06, 0xe1, 0x9e, 0x1c, 0x30, 0xf1, 0x8e, 0x40, 0x00, 0x07, 0x00, 0x00, 
  0x00, 0x07, 0x00, 0x00, 0x3f, 0xc3, 0x00, 0x06, 0xe0, 0x01, 0x83, 0xf8, 0x00, 0x01, 0xc0, 0x00, 
  0x00, 0x1c, 0x00, 0x00, 0x77, 0x06, 0x00, 0x06, 0xc0, 0x00, 0xc1, 0xcc, 0x00, 0x00, 0xe0, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x46, 0x0c, 0x3f, 0xfe, 0xff, 0xf8, 0x60, 0x46, 0x00, 0x00, 0x30, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0xc0, 0xbf, 0x61, 0x1c, 0x71, 0x0d, 0xfa, 0x06, 0x00, 0x00, 0x1c, 0x00, 
  0x00, 0xe0, 0x00, 0x01, 0xed, 0x83, 0xcf, 0xce, 0x63, 0xe7, 0xc3, 0x6f, 0x80, 0x00, 0x0e, 0x00, 
  0x01, 0xc0, 0x00, 0x07, 0x19, 0x80, 0xfe, 0x8c, 0x62, 0xfa, 0x03, 0x31, 0xc0, 0x00, 0x07, 0x00, 
  0x03, 0x80, 0x00, 0x0d, 0xf9, 0x80, 0x60, 0x06, 0x60, 0x0c, 0x03, 0x3f, 0x60, 0x00, 0x03, 0x00, 
  0x03, 0x00, 0x00, 0x18, 0x8f, 0x81, 0xdf, 0x06, 0x41, 0xf7, 0x03, 0xf1, 0x30, 0x00, 0x01, 0x80, 
  0x06, 0x00, 0x00, 0x18, 0x0c, 0xff, 0x9e, 0x06, 0x60, 0xfb, 0xfe, 0x60, 0x30, 0x00, 0x00, 0xc0, 
  0x0c, 0x00, 0x00, 0x0b, 0xd8, 0x7e, 0x06, 0x3c, 0x7c, 0xc0, 0xfc, 0x3f, 0xb0, 0x00, 0x00, 0xe0, 
  0x0c, 0x00, 0x00, 0x0c, 0x70, 0x06, 0x03, 0x38, 0x38, 0xc0, 0xc0, 0x1e, 0x70, 0x00, 0x00, 0x60, 
  0x1c, 0x00, 0x00, 0x3e, 0x30, 0x03, 0x0f, 0xd8, 0x37, 0xe1, 0xc0, 0x08, 0xf8, 0x00, 0x00, 0x70, 
  0x18, 0x00, 0x00, 0x33, 0x30, 0x06, 0x0c, 0xec, 0x6e, 0x20, 0xc0, 0x09, 0x8c, 0x00, 0x00, 0x30, 
  0x38, 0x00, 0x00, 0x76, 0x38, 0x03, 0xfc, 0x06, 0x60, 0x3f, 0xc0, 0x18, 0xcc, 0x00, 0x00, 0x30, 
  0x30, 0x00, 0x00, 0xfc, 0x7e, 0x00, 0xf0, 0x06, 0x40, 0x1f, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x30, 
  0x30, 0x00, 0x01, 0x8c, 0xc7, 0x80, 0x7e, 0xe6, 0x47, 0x7e, 0x03, 0xc6, 0x63, 0x00, 0x00, 0x38, 
  0x30, 0x00, 0x01, 0x87, 0x08, 0x01, 0xc7, 0xf6, 0x5f, 0xc7, 0x00, 0x21, 0xc3, 0x00, 0x00, 0x18, 
  0x30, 0x00, 0x01, 0xf3, 0x0f, 0xbf, 0x00, 0x1e, 0x70, 0x01, 0xfd, 0xe1, 0x9f, 0x00, 0x00, 0x18, 
  0x30, 0x00, 0x00, 0xff, 0x01, 0xff, 0x00, 0x3c, 0x38, 0x01, 0xef, 0x00, 0xfe, 0x00, 0x00, 0x18, 
  0x30, 0x00, 0x01, 0xbf, 0x80, 0x01, 0x83, 0xfc, 0x7f, 0x81, 0x80, 0x03, 0xff, 0x00, 0x00, 0x38, 
  0x30, 0x00, 0x01, 0xbc, 0xf8, 0x00, 0xb2, 0x16, 0xf0, 0x99, 0x00, 0x3e, 0x7d, 0x00, 0x00, 0x30, 
  0x38, 0x00, 0x01, 0xc7, 0x39, 0x0e, 0x34, 0x1c, 0x70, 0x58, 0xe1, 0xb9, 0xc3, 0x00, 0x00, 0x30, 
  0x18, 0x00, 0x00, 0xf3, 0xc3, 0x07, 0xff, 0xcc, 0x63, 0xff, 0xc1, 0x87, 0x9e, 0x00, 0x00, 0x30, 
  0x18, 0x00, 0x00, 0x38, 0xc3, 0x18, 0x01, 0xc6, 0xc3, 0x00, 0x31, 0x86, 0x18, 0x00, 0x00, 0x70, 
  0x0c, 0x00, 0x00, 0x18, 0xc1, 0xff, 0xff, 0x82, 0xc3, 0xff, 0xff, 0x86, 0x30, 0x00, 0x00, 0x60, 
  0x0c, 0x00, 0x00, 0x1f, 0x60, 0x03, 0xfe, 0x02, 0xc0, 0xff, 0x80, 0x0d, 0xf0, 0x00, 0x00, 0xe0, 
  0x06, 0x00, 0x00, 0x30, 0x38, 0x01, 0xc7, 0x86, 0xc1, 0xc3, 0x00, 0x38, 0x18, 0x00, 0x00, 0xc0, 
  0x07, 0x00, 0x00, 0x30, 0x1f, 0xff, 0x80, 0x06, 0x60, 0x03, 0xff, 0xf0, 0x18, 0x00, 0x01, 0x80, 
  0x03, 0x00, 0x00, 0x18, 0x00, 0x79, 0xf0, 0x0e, 0x60, 0x0f, 0x1c, 0x00, 0x30, 0x00, 0x03, 0x80, 
  0x01, 0x80, 0x00, 0x0e, 0x00, 0xc0, 0xf8, 0x0a, 0xf0, 0x3e, 0x87, 0x00, 0x60, 0x00, 0x07, 0x00, 
  0x00, 0xc0, 0x00, 0x07, 0xff, 0xbf, 0x0e, 0x06, 0xe0, 0xf0, 0xfb, 0xff, 0xc0, 0x00, 0x0e, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0xff, 0x3e, 0x67, 0xfe, 0xff, 0xc4, 0x78, 0xfe, 0x00, 0x00, 0x1c, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x33, 0xe7, 0x60, 0x3c, 0x78, 0x0d, 0xcf, 0x88, 0x00, 0x00, 0x38, 0x00, 
  0x00, 0x1c, 0x00, 0x00, 0x18, 0x03, 0xfc, 0x7e, 0x7c, 0x7f, 0x80, 0x38, 0x00, 0x00, 0x70, 0x00, 
  0x00, 0x0e, 0xe0, 0x00, 0x0f, 0xff, 0x0c, 0x06, 0xc0, 0x61, 0xff, 0xe0, 0x00, 0x19, 0xc0, 0x00, 
  0x00, 0x03, 0xe0, 0x00, 0x01, 0xc3, 0xc0, 0x2e, 0x68, 0x03, 0x8d, 0x00, 0x00, 0x33, 0x00, 0x00, 
  0x00, 0x06, 0xde, 0x00, 0x00, 0x00, 0xff, 0xf8, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0xc0, 0x00, 
  0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x07, 0xf0, 0x00, 0x00, 0x02, 0x07, 0x80, 0x00, 
  0x00, 0x00, 0x70, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x0e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xc0, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x36, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x41, 0xf0, 0x44, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xc1, 0x98, 0x7c, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x83, 0xf0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char PROGMEM logooxi[] = {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x08, 0x3e, 0x00, 0x00, 0x1c, 0x3e, 0x00, 
 0x00, 0x1c, 0x3e, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f, 0x00, 0xf0, 0x00, 0x7e, 0x00, 0xf0, 
  0x00, 0xfe, 0x30, 0xf0, 0x00, 0xfc, 0xfc, 0x00, 0x01, 0xfc, 0xfc, 0x00, 0x03, 0xfc, 0xfc, 0x00, 
  0x07, 0xfe, 0xf8, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x0f, 0xff, 0x86, 0x00, 0x0f, 0xc7, 0xfe, 0x00, 
  0x1f, 0x39, 0xff, 0x00, 0x3f, 0x39, 0xff, 0x00, 0x3f, 0x7d, 0xff, 0x00, 0x3f, 0x7d, 0xff, 0x00, 
  0x3f, 0x38, 0x1f, 0x00, 0x3f, 0x82, 0xdf, 0x00, 0x3f, 0xc7, 0x9f, 0x00, 0x1f, 0xfe, 0x7e, 0x00, 
  0x1f, 0xfe, 0x1e, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x03, 0xff, 0xe0, 0x00, 
  0x00, 0xff, 0x00, 0x00
};
static const unsigned char PROGMEM logobpm[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x3f, 0x00, 0x1f, 0xf8, 0xff, 0xc0, 
  0x38, 0xfd, 0xff, 0xe0, 0x77, 0xff, 0xff, 0xf0, 0x6f, 0xff, 0xff, 0xf0, 0xdf, 0xff, 0xff, 0xf8, 
  0xdf, 0xf7, 0xff, 0xf8, 0xdf, 0xf7, 0xff, 0xf8, 0xff, 0xeb, 0xff, 0xf8, 0xf3, 0xeb, 0xfe, 0x78, 
  0x65, 0xdb, 0xfd, 0x30, 0x64, 0x1b, 0x91, 0x30, 0x71, 0xfd, 0x7c, 0x70, 0x3f, 0xfd, 0x7f, 0xe0, 
  0x1f, 0xfd, 0x7f, 0xc0, 0x0f, 0xfe, 0xff, 0x80, 0x07, 0xfe, 0xff, 0x00, 0x03, 0xff, 0xfe, 0x00, 
  0x01, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xc0, 0x00, 
  0x00, 0x0f, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

// DECLARACION DE FUNCIONES 

void onBeatDetected()
{
  Serial.println("Beat!");
}

void display_data(int bpm, int spo2);

void data_gps (float lati, float longi);

void Contador ();


void setup() 
{                                   
  ///////////////////////////////////////////////////////                                 DISPLAY                            ///////////////
  Serial.begin(115200);
  Serial.println("SSD1306 128x64 OLED TEST");
  ///////////////////////////////////////////////////////                               INICIAR GPS                          //////////////////////////////////////
 delay(200);
  serialgps.begin(9600);
  
delay(200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c )) 
  {   // SCREEN_ADDRESS es el nombre que detecta la placa en I2C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.cp437(true);  //activar codigo 437 https://elcodigoascii.com.ar
  display.clearDisplay();
  Serial.print("Iniciando POX");
  ////////////////////////////////////////////////////////               CONFIGURACION TIMER1 (TICKER)                 ///////////////////////////////////////////////////////
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);//                   TIM_DIV256 = 31.25KHz  TIM_EDGE= Configuración defecto (puede ser por flancos asc o des)
                                                  
  timer1_write(3125000);// 5segundos                                  TIM_LOOP = Vuelve siempre que se cumpla el tiempo
                   
  timer1_attachInterrupt(Contador);//                                                
  //                                                                  ESP8266 posee libreria Timer0 y Timer1 (Ticker.h), Timer0 ya es utilizada para la conexion al servidor
  
  ////////////////////////////////////////////////////////                    MAX                                      //////////////////////////////////////////////////////////
#if ENABLE_MAX30100 
   if (!pox.begin()) 
 {
    Serial.println("FAILED");          
    for (;;);
 } 
  else 
 {
    Serial.println("SUCCESS");
 }
  // Corriente default IR Led = 50mA, se configuró con una configuracion alternativa que indicaba la libreria Register.h 
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
  display_data(0, 0);
#endif
//////////////////////////////////////////////////////////                   SERVER                                    //////////////////////////////////////////////
 // Conectar a la red Wi-Fi
/*  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }
  Serial.println("Conexión exitosa");

  // Definir las rutas para las páginas web
  server.on("/", HTTP_GET, handleRoot);

  IPAddress ip = WiFi.localIP();
  Serial.print("Dirección IP: ");
  Serial.println(ip);
  
  // Iniciar el servidor
  server.begin();
  Serial.println("Servidor web iniciado");*/
}


void loop() 
{ 
  // Manejar las solicitudes de los clientes
//server.handleClient();
  
  //////////////////////////////////////                         GPS                            ///////////////////
if(flagcambio == 1)
{
 #if ENABLE_MAX30100
  // Make sure to call update as fast as possible
  pox.update();
//  int bpm = 0;
//  int spo2 = 0;

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
  {
    bpm = pox.getHeartRate();
    spo2 = pox.getSpO2();
    Serial.println(bpm);
    Serial.println(spo2);
  //  Serial.println("chamou");
    tsLastReport = millis();
    display_data(bpm, spo2);
  }
#endif
}
if(flagcambio==2)
{
   if(serialgps.available())
  {
   int c = serialgps.read();
   if(gps.encode(c))
    {
    // float latitude, longitude;
     gps.f_get_position(&latitude, &longitude);
     Serial.print("Latitud/Longitud:  ");
     Serial.print(latitude,5);
     Serial.print(", ");
     Serial.println(longitude,5);
     data_gps (latitude, longitude);
    }
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);             
    display.println(F("      MEMORYLANE"));
    display.setCursor(20,30);
    display.println(F("No se detecta satelite"));
    display.display();
  }
   
}
//switch (pantalla)
  //{
   /* case 0:
    
    display.clearDisplay();
    display.drawBitmap(0, 0, logomemorylane,128, 64, 1);
    display.display();
   
    break;
    
    case 1:
    if(flagcambio==1)
   {
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);             
    display.println(F("      MEMORYLANE"));
    display.drawBitmap(6, 12, logobpm,29, 28, 1);
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(11,45);             
    display.println(xbpm);
    display.drawBitmap(92, 11, logooxi,28, 29, 1);
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(95,46);             
    display.println(xspo2);
    display.display();
   }
    if(flagcambio==0)
   {
     display.clearDisplay();
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,0);             
     display.println(F("      MEMORYLANE"));
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,40);             
     display.println(F("LAT"));
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,20);             
     display.println(F("LONG"));
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(30,20);             
     display.println(xlat,3);
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(30,40);             
     display.println(xlon,3);
     display.display();          
   }
    break;
  }*/
//}
if (flagcambio ==0)
{
  
    display.clearDisplay();
    display.drawBitmap(0, 0, logomemorylane,128, 64, 1);
    display.display();
}
}




void display_data(int bpm, int spo2)   // defino las variables 
{   
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);             
    display.println(F("      MEMORYLANE"));
    display.drawBitmap(6, 12, logobpm,29, 28, 1);
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(11,45);             
    display.println(bpm);
    display.drawBitmap(92, 11, logooxi,28, 29, 1);
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(95,46);             
    display.println(spo2);
    display.display();
}

void data_gps (float lati, float longi)
{
    display.clearDisplay();
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,0);             
     display.println(F("      MEMORYLANE"));
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,40);             
     display.println(F("LAT: "));  
     display.println(longi,3);
     display.setTextSize(1);             
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,20);             
     display.println(F("LONG: "));                        
     display.println(lati,3);
     display.display();
  
}
/*void handleRoot() 
{
  String paginaHTML = "<html><body><h1>Hola Bienvenido a Memory Lane</h1>";

  paginaHTML += "<h2>BPM: " + String(bpm) + "</h2>";
  paginaHTML += "<h2>SpO2: " + String(spo2) + "</h2>";
  paginaHTML += "<h2>Latitud: " + String(latitude) + "</h2>";
  paginaHTML += "<h2>Longitud: " + String(longitude) + "</h2>";

  paginaHTML += "</body></html>";

  server.send(200, "text/html", paginaHTML);
} */
void Contador () //cambio de pantalla
{
 if(flagcambio < 1)
 flagcambio++;
 
 if(flagcambio >= 1)
 {
  cuenta++;
  if((flagcambio == 1) && (cuenta == 2))
   {
    cuenta = 0;
    flagcambio = 2;
   }
  if((flagcambio == 2) && (cuenta == 2))
   {
    cuenta = 0;
    flagcambio = 1;
   } 
  }
}
