



int flag_cnt = 0;




#define S0 2
#define S1 13
#define S2 12
#define S3 14
#define sensorOut 5

#include <ESP8266WiFi.h>
extern "C" {
    #include <espnow.h>
}

    // this is the MAC Address of the slave which receives the data
uint8_t remoteMac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};

#define WIFI_CHANNEL 4
#define led_pin 10
#define LED 12     //D6

    // must match the slave struct
struct __attribute__((packed)) DataStruct {
    byte message;
    unsigned long time;
};

DataStruct myData;

unsigned long lastSentMillis;
unsigned long sendIntervalMillis = 500;
unsigned long sentMicros;
unsigned long ackMicros;

unsigned long lastBlinkMillis;
unsigned long fastBlinkMillis = 50;
unsigned long slowBlinkMillis = 300;
unsigned long blinkIntervalMillis = slowBlinkMillis;


bool data_sent = true;


// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Stores the red. green and blue colors
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int redCount = 0;

boolean blue = false;
boolean red = false;
boolean green = false;
boolean white = false;
boolean start = false;


void setup() {
  Serial.begin(115200);

  pinMode(led_pin, INPUT);
  
  // Setting the outputs
    
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT); 
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

   Serial.begin(115200); Serial.println();
   Serial.println("Starting EspnowController.ino");

    WiFi.mode(WIFI_STA); // Station mode for esp-now controller
    WiFi.disconnect();

    Serial.printf("This mac: %s, ", WiFi.macAddress().c_str());
    Serial.printf("slave mac: %02x%02x%02x%02x%02x%02x", remoteMac[0], remoteMac[1], remoteMac[2], remoteMac[3], remoteMac[4], remoteMac[5]);

    Serial.printf(", channel: %i\n", WIFI_CHANNEL);

    if (esp_now_init() != 0) {
        Serial.println("*** ESP_Now init failed");
        while(true) {};
    }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_add_peer(remoteMac, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);

    esp_now_register_send_cb(sendCallBackFunction);

    myData.message = 5;
    //strcpy(myData.text, "Hello World");
    Serial.print("Message "); Serial.println(myData.message);


    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("Setup finished");
}

void loop() {


  //*****************Color Sensing Begins*********************//

  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  // Reading the output frequency
  redFrequency = pulseIn(sensorOut, LOW); 
  // redColor = map(redFrequency, 70, 120, 255,0);
  redColor = map(redFrequency, 84, 205, 255,0);
  delay(2);
 
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  

  greenFrequency = pulseIn(sensorOut, LOW);
  // greenColor = map(greenFrequency, 100, 199, 255, 0);
  greenColor = map(greenFrequency, 81, 269, 255, 0);
  delay(2);
  
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  blueFrequency = pulseIn(sensorOut, LOW);
   //blueColor = map(blueFrequency, 38, 84, 255, 0);
  blueColor = map(blueFrequency, 45, 90, 255, 0);
  delay(2);
 Serial.print("R = ");
 Serial.println(redColor);
 Serial.print("G = ");
 Serial.println(greenColor);
 Serial.print("B = ");
 Serial.println(blueColor);
 //delay(300);

 if(redColor > greenColor && redColor > blueColor /*&& blueColor < -1800 && (redColor - blueColor) > 1800 && (redColor - greenColor > 50)*/){
   //Serial.println(" - Red detected!"); 
   redCount++;

   if(/*scaled_pulse - last_scaled > 50 &&*/ redCount >2){

      
      flag_cnt++;
      redCount = 0;

      if(flag_cnt != 1 || digitalRead(led_pin) == HIGH){ 
        Serial.print("Passed Diverter: ");
        Serial.println(flag_cnt - 1);
        sendData();
      }
      delay(150);
   }  
   //check this 
   //delay(20);
  
 }

 if(greenColor > redColor && greenColor > blueColor){
    redCount = 0;
 }

 if(blueColor > redColor && blueColor > greenColor){
  redCount = 0;
 }

 Serial.println("looping");
  

}

//*************************FUNCTIONS START****************************

void sendData() {
    //if (millis() - lastSentMillis >= sendIntervalMillis) {
      //  lastSentMillis += sendIntervalMillis;
        myData.time = millis();
        uint8_t bs[sizeof(myData)];
        memcpy(bs, &myData, sizeof(myData));
        sentMicros = micros();
        esp_now_send(NULL, bs, sizeof(myData)); // NULL means send to all peers
        digitalWrite(LED_BUILTIN,LOW);
        delay(200);
        digitalWrite(LED_BUILTIN,HIGH);
       // delay(200);
        Serial.println("sent data");
    //}
}

void sendCallBackFunction(uint8_t* mac, uint8_t sendStatus) {
    ackMicros = micros();
    Serial.print("Trip micros "); Serial.println(ackMicros - sentMicros);
    Serial.printf("Send status = %i", sendStatus);
    Serial.println();
    Serial.println();
    if (sendStatus == 0) {
        blinkIntervalMillis = fastBlinkMillis;
    }
    else {
        blinkIntervalMillis = slowBlinkMillis;
    }
}

