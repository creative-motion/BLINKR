#include <FastLED.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE 9
#define CSN 10

#define BAUDRATE 57600

RF24 radio(CE, CSN);
const uint64_t newAddress = 0xABCDABCD71LL;
const byte address[6] = "00001";

int left_on = 0;
int right_on = 0;
int center_on = 0;
int payload = 0;

#define DATA_PIN_LEFT 3
#define DATA_PIN_CENTER 4
#define DATA_PIN_RIGHT 5

#define BLINK_LEFT 3
#define BLINK_CENTER 4
#define BLINK_RIGHT 5

#define NO_LEDS_CENTER 30
#define NO_LEDS_LEFT 30
#define NO_LEDS_RIGHT 30

CRGB leds_mid[NO_LEDS_CENTER];
CRGB leds_left[NO_LEDS_LEFT];
CRGB leds_right[NO_LEDS_RIGHT];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  set_radio();
  set_leds();
  test_leds();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (radio.available()) {
    radio.read(&payload, sizeof(int));
    Serial.println(payload);
    
    if (payload == BLINK_LEFT) {
      light_left(CRGB::Yellow);
    } else if (payload == BLINK_RIGHT) {
      light_right(CRGB::Yellow);
    } else if (payload == BLINK_CENTER) {
      light_all(CRGB::Yellow);
    } else {
      light_all(CRGB::Black);
    }

    //Ignores old commands
    radio.flush_rx();
    FastLED.show();
  } else {
    Serial.println("NO DATA");
  }
    delay(100);

}

void set_leds() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN_LEFT>(leds_left, NO_LEDS_LEFT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_RIGHT>(leds_right, NO_LEDS_RIGHT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_CENTER>(leds_mid, NO_LEDS_CENTER);
}

void set_radio() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void set_leds_color(CRGB leds[], CRGB color, uint16_t no_leds) {
  uint16_t index;
  for (index = 0; index < no_leds; index++) {
    leds[index] = color;
  }
}

void light_all(CRGB color) {
  set_leds_color(leds_left, color, NO_LEDS_LEFT);
  set_leds_color(leds_right, color, NO_LEDS_RIGHT);
  set_leds_color(leds_mid, color, NO_LEDS_CENTER); 

  if (color) {
    for (int i = 85; i >= 0; i--) {
      FastLED.setBrightness(i*3);
      FastLED.show();
      delay(1);
    }
    
    set_leds_color(leds_left, CRGB::Black, NO_LEDS_LEFT);
    set_leds_color(leds_right, CRGB::Black, NO_LEDS_RIGHT);
    set_leds_color(leds_mid, CRGB::Black, NO_LEDS_CENTER); 
    FastLED.show(); 
    delay(40);
    
    FastLED.setBrightness(255);
  } else {
    FastLED.show(); 
  }
}

void light_left(CRGB color) {
  for (int i = 0; i < NO_LEDS_LEFT; i++) {
    leds_left[i] = color;
    FastLED.show();
    delay(15);
  }

  for (int i = 0; i < NO_LEDS_LEFT; i++) {
    leds_left[i] = CRGB::Black; 
  }
    FastLED.show();
    delay(15);
}

void light_right(CRGB color) {
  for (int i = 0; i < NO_LEDS_RIGHT; i++) {
    leds_right[i] = color;
    FastLED.show();
    delay(15);
  }

  for (int i = 0; i < NO_LEDS_RIGHT; i++) {
    leds_right[i] = CRGB::Black; 
  }
    FastLED.show();
    delay(15);
}

void test_leds(){
  int counter;
  for (counter = 0; counter <= 2; counter++) {
    light_all(CRGB::Red);
    delay(400);
    light_all(CRGB::Black);
    delay(200);
  }
}
