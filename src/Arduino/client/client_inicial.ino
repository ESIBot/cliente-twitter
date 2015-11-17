#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <mqttsn-messages.h>

// Definimos el nombre del topic al cual queremos suscribirnos
#define TOPIC "seminario6"

// Definiciones para la pantalla LCD
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// Tamaño del buffer que usaremos para recibir datos por Bluetooth
#define BUFFER_SIZE 256

// Estructura que usaremos para almacenar los datos que vamos recibiendo
struct buffer {
	uint8_t buf[BUFFER_SIZE];
	uint8_t offset;
	uint8_t len;
};

// Creamos los objetos para usar el LCD y MQTT

// Software SPI
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
MQTTSN mqttsn;

// Variables globales que usaremos
struct buffer mqtt_buffer;		// Buffer para almacenar los datos que llegan

// ID del topic al que nos suscribimos
//

// Para controlar si ya nos hemos suscrito
//

void setup() {
	Serial.begin(9600);				// Iniciamos puerto serie
	// Iniciamos display
	//

	// Ajustamos el contraste del display
	//

	// Esperamos a que el display termine de iniciarse
	//
}

void loop() {
	// Índice del topic con el que estamos
	//

	// Comprobamos si tenemos datos en el puerto serie y los leemos
	CheckSerial();

	// Si aún no hemos terminado de procesar una respuesta no continuamos
	//

	// Si no estamos conectado nos conectamos
	//

	// Buscamos un topic por su nombre en nuestra memoria
	//

	// Si no lo encontramos tenemos que registararlo
	//

	// Si no estamos sucrito al topic nos suscribimos
	//
}

/**
 * Definimos cómo se envían los datos por puerto serie de MQTT
 */
void MQTTSN_serial_send(uint8_t *message_buffer, int length) {

}

/**
 * Definimos lo que hacemos cuando recibimos un mensaje del topic
 */
void MQTTSN_publish_handler(const msg_publish *msg) {

}

/**
 * Definimos lo que hacemos cuando recibimos mensajes de una pasarela
 */
void MQTTSN_gwinfo_handler(const msg_gwinfo *msg) {

}

/**
 * En esta función se leen datos de puerto serie y se extraen los mensajes
 * MQTT. Una vez extraído se le pasa a la librería para que los interprete.
 */
void CheckSerial() {
	uint8_t *msg = NULL;		// Donde extraemos los mensajes leídos del buffer
	int msgs = 0;						// Número de mensajes leído del buffer
	uint8_t msg_len = 0;		// Tamaño del mensaje leído del buffer
	int i = 0;							// Índice para bucles

	// Comprobamos que haya datos en el buffer de la UART y se almacenan en
	// nuestro buffer
	while (Serial.available()) {
		mqtt_buffer.buf[mqtt_buffer.len++] = Serial.read();
	}

	// Tenemos, al menos, un mensaje completo en el buffer
	while ((msg_len = mqtt_buffer.buf[mqtt_buffer.offset]) <=
	        mqtt_buffer.len - mqtt_buffer.offset &&
	        msg_len != 0) {

		// Reservamos memoria para almacenar el mensaje leído
		msg = (uint8_t *) calloc(msg_len, sizeof(char));

		// Copiamos el mensaje en la variable "msg"
		for (i = 0 ; i < msg_len ; i++) {
			msg[i] = mqtt_buffer.buf[mqtt_buffer.offset++];
		}

		// Le pasamos el mensaje a la librería para que lo trate
		mqttsn.parse_stream(msg, msg_len);

		// Liberamos la memoria reservada
		free(msg);

		// Incrementamos el número de mensajes leídos
		msgs++;
	}

	// Si hemos procesado algún mensaje limpiamos el buffer.
	if (msgs) {
		mqtt_buffer.offset = 0;
		mqtt_buffer.len = 0;
		memset(mqtt_buffer.buf, 0, BUFFER_SIZE);
	}
}

/**
 * Imprime por LCD un texto
 */
void lcd_print(const char *str) {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(str);
	display.display();
}