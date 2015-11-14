#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <mqttsn-messages.h>

#define TOPIC "test"

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
MQTTSN mqttsn;

uint16_t u16TopicID;
bool suscribed = false;

void setup() {
	Serial1.begin(9600);

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
		mqttsn.connect(0, 10, "arduino");
		return;
	}

	u16TopicID = mqttsn.find_topic_id(TOPIC, &index);
	if (u16TopicID == 0xffff) {
		mqttsn.register_topic(TOPIC);
		return;
	}

	if (!suscribed) {
		mqttsn.subscribe_by_name(0, TOPIC);
		suscribed = true;
		lcd_print("INICIADO");
		return;
	}
}

void MQTTSN_serial_send(uint8_t *message_buffer, int length) {
	Serial1.write(message_buffer, length);
	Serial1.flush();
}

void MQTTSN_publish_handler(const msg_publish *msg) {
	lcd_print(msg->data);
}

void MQTTSN_gwinfo_handler(const msg_gwinfo *msg) {
}

void CheckSerial() {
	uint16_t cnt = 0;
	uint8_t offset = 0;
	uint8_t msg_len = 0;
	uint8_t buffer[512];
	uint8_t *msg = NULL;
	int i = 0;
	int j = 0;

	while (Serial1.available()) {
		buffer[cnt++] = Serial1.read();
	}

	// Recibo datos
	if (cnt > 0) {

		while (cnt >= msg_len) {
			msg_len = buffer[offset];

			msg = (uint8_t *) calloc(msg_len, sizeof(uint8_t));
			for (i = 0 ; i < msg_len ; i++, j++) {
				msg[i] = buffer[j];
			}

			cnt -= msg_len;
			offset += msg_len;
			mqttsn.parse_stream(msg, msg_len);
			free(msg);
		}
	}
}

void lcd_print(const char *str) {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(str);
	display.display();
}