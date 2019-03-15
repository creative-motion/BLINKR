#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LEFT 3
#define CENTER 4
#define RIGHT 5
#define OFF 0
#define PURGATORY -1
#define ON 1

#define CE 9
#define CSN 10

#define BAUDRATE 57600

RF24 radio(CE, CSN);
const uint64_t newAddress = 0xABCDABCD71LL;
const byte address[6] = "00001";

int left_state = OFF;
int right_state = OFF;
int center_state = OFF;

int payload = 0;
int prevRead = 0;
// 0 - no button pressed; 1 - button pressed
int buttons_state = OFF;
int blink_leds = 0;

bool flipFlop = false;
unsigned long long t1, t2 = millis();


void setup() {
  Serial.begin(BAUDRATE);
  Serial.println(">>>>>>>>>>>> SETUP  <<<<<<<<<<<<<");
  
  set_pins();
  set_radio();
}

void loop() {  
  set_payload_value();
  Serial.print("Payload: ");
  Serial.println(payload);

  
  radio.write(&payload, sizeof(int));
  delay(200);
}

void set_pins() {
  pinMode(LEFT, INPUT);
  pinMode(CENTER, INPUT);
  pinMode(RIGHT, INPUT);
}

void set_radio() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void reset_states() {
  left_state = OFF;
  center_state = OFF;
  right_state = OFF;
}

int set_payload_value() {
  int left = digitalRead(LEFT);
  int center = digitalRead(CENTER);
  int right = digitalRead(RIGHT);
  //Current time;
  t1 = millis();

  if (left) {
    prevRead = LEFT;
    payload = LEFT;
    flipFlop = false;
    t2 = millis();
  } else if (center) {  
    //Checks whether it's the first push of the button
    if (prevRead == 0) {
      flipFlop = !flipFlop;
    }
    
    t2 = 0;
    prevRead = CENTER;
  } else if (right) {
    prevRead = RIGHT;
    payload = RIGHT;
    flipFlop = false;
    t2 = millis();
  } else {
    prevRead = 0;
    //Center lights are based on a flip-flop switch
    if (flipFlop) {
      payload = CENTER;
    } else if (t1 - t2 >= 5000 || prevRead == CENTER) {
      payload = OFF;
    }   
  }
}
