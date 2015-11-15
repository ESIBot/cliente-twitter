#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <mqttsn-messages.h>

#define TOPIC "test"

#define BUFFER_SIZE 512

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

struct buffer {
	uint8_t buf[BUFFER_SIZE];
	uint8_t offset;
	uint8_t len;
};

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
MQTTSN mqttsn;

struct buffer mqtt_buffer;
uint8_t msg_len = 0;

uint16_t u16TopicID;
bool suscribed = false;

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);

	Serial.println("DISPLAY - Iniciando display...");
	display.begin();
	display.setContrast(50);
}

void loop() {
	uint8_t index;

	CheckSerial();
	delay(2000);

	if (mqttsn.wait_for_response()) {
		return;
	}

	if (!mqttsn.connected()) {
		lcd_print("CONECTANDO...");
		Serial.println("MQTT - Conectando...");
		mqttsn.connect(0, 10, "arduino");
		return;
	}

	u16TopicID = mqttsn.find_topic_id(TOPIC, &index);
	if (u16TopicID == 0xffff) {
		Serial.println("MQTT - Registrando topic");
		mqttsn.register_topic(TOPIC);
		return;
	}

	if (!suscribed) {
		mqttsn.subscribe_by_name(0, TOPIC);
		suscribed = true;
		Serial.println("MQTT - Suscrito");
		lcd_print("INICIADO");
		return;
	}
}

void MQTTSN_serial_send(uint8_t *message_buffer, int length) {
	Serial1.write(message_buffer, length);
	Serial1.flush();
}

void MQTTSN_publish_handler(const msg_publish *msg) {
	Serial.print("MQTT - PUBLISH: "); Serial.println(msg->data);
	lcd_print(msg->data);
}

void MQTTSN_gwinfo_handler(const msg_gwinfo *msg) {
}

void CheckSerial() {
	uint8_t *msg = NULL;
	int i = 0;
	int msgs = 0;

	while (Serial1.available()) {
		mqtt_buffer.buf[mqtt_buffer.len++] = Serial1.read();
	}

	// Serial.print("BUFFER - "); Serial.println(mqtt_buffer.len);
	// Tenemos, al menos, un mensaje completo en el buffer
	while ((msg_len = mqtt_buffer.buf[mqtt_buffer.offset]) <=
	        mqtt_buffer.len - mqtt_buffer.offset &&
	        msg_len != 0) {
		msgs++;
		msg = (uint8_t *) calloc(msg_len, sizeof(char));

		Serial.print("MESSAGE - LENGTH: "); Serial.println(msg_len);
		Serial.print("MESSAGE - DATA: ");
		for (i = 0 ; i < msg_len ; i++) {
			msg[i] = mqtt_buffer.buf[mqtt_buffer.offset++];
			Serial.print("0x"); Serial.print(msg[i], HEX); Serial.print(" ");
		}
		Serial.println("");

		mqttsn.parse_stream(msg, msg_len);
		free(msg);
	}

	if (msgs) {
		mqtt_buffer.offset = 0;
		mqtt_buffer.len = 0;
		memset(mqtt_buffer.buf, 0, BUFFER_SIZE);
	}
}

void lcd_print(const char *str) {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(str);
	display.display();
}