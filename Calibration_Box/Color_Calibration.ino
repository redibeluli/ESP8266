



int flag_cnt = 0;

int red_count  = 0;


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
 Serial.print(redFrequency);
 Serial.print(", ");
 Serial.println(redColor);
 Serial.print("G = ");
 Serial.print(greenFrequency);
 Serial.print(", ");
 Serial.println(greenColor);
 Serial.print("B = ");
 Serial.print(blueFrequency);
 Serial.print(", ");
 Serial.println(blueColor);
 //delay(300);



if(redColor > blueColor && redColor > greenColor)
{

   red_count++;
   if(red_count > 2){
     Serial.println("red detected");
     delay(2000);
     red_count = 0;
   }
}
else{
  red_count = 0;
}


 Serial.println("looping");
  

}


