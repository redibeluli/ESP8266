
char rx_byte = 0;
char junk = ' ';
String rx_str = "";
boolean not_number = false;
boolean got_num = false;
boolean primary = false;
boolean got_letter = false;
boolean asked = false;
int result;

float module_divider_num[] = {15.62,15.85503,16.0,15.74088,15.745,15.645,
                              15.76,15.85,15.76,15.8255,15.8,15.7485,
                              15.82277,15.867,15.675,15.8499,15.75,15.8286,
                              15.74,15.74,15.78,15.8,15.78,15.89,15.8,15.725,
                              15.74,15.75,15.75,15.85,15.72,15.78952,15.8,
                              15.86925,15.91,15.825,15.875,15.78,15.78,15.92556,15.8774,22.5,23,15.76245,15.84172,28};


const int Tach_Pin = 13;  
volatile long pulse_count = 0;
volatile long deltaT = 0;
volatile long pulse_start = 0;
volatile long last = 0;
long current_cnt = 0;
long raw_pulses = 0;
long prev_cnt = 0;
float raw_diff = 0;
float Sum_of_Diffs = 0;

bool data_recieved = false;

int flag_cnt = 0;
int last_hitpoint = 0;
int hp_delta = 174;
int diverter_num = 3;



float inch_divider = 0;
int scaled_pulse = 0; //changed 
int last_scaled = 0;


#include <ESP8266WiFi.h>
extern "C" {
    #include <espnow.h>
     #include <user_interface.h>
}


uint8_t mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};


void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
}

#define WIFI_CHANNEL 4

struct __attribute__((packed)) DataStruct {
    byte message;
    unsigned int time;
};

DataStruct myData;



void setup() {
  
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

   while(!got_letter){
       if(!asked){
        Serial.println("\nEnter 'p' for Primary 'o' for Outbound");
        asked = true;
       }
       get_letter();
   }
  
  if(primary){
     Serial.println("\nEnter primary module number:");
     serialFlush();
     while(!got_num){
     get_priNum();
    }   
  }
  else{
    Serial.println("Setting divider for Outbound module to 16");
    inch_divider = 16;
  }

  
  pinMode(Tach_Pin, INPUT);
  attachInterrupt(Tach_Pin, Encoder_ISR, CHANGE);
  
 // pinMode(Tach_Pin, INPUT_PULLUP);
  // Setting the outputs

   if (esp_now_init()!=0) {
        Serial.println("*** ESP_Now init failed");
        while(true) {};
    }

    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

    esp_now_register_recv_cb(receiveCallBackFunction);
    

    Serial.println("End of setup - waiting for flags...");
    

}

void loop() {

 
    cli();
    current_cnt = pulse_count;
    sei();
  
    //two triggers per pulse
    current_cnt = current_cnt/2;
    
    raw_diff = current_cnt - prev_cnt;
  
    //Serial.print("Dif: ");
    //Serial.println(raw_diff);
  
    Sum_of_Diffs += raw_diff;
  
    if(Sum_of_Diffs >= inch_divider){
      Sum_of_Diffs = Sum_of_Diffs - inch_divider;
      scaled_pulse++;
    }
  

  //Serial.println(current_cnt);
  //Serial.println(scaled_pulse);
  //Serial.println(inch_divider);
  //delay(1000);
  

  if(data_recieved){
   
    flag_cnt++;

    if(flag_cnt == 1){
      scaled_pulse = 0;
      Serial.println("Passed induct...");
    }
    else if(flag_cnt == 2){
      Serial.print("Diverter 1 HP: ");
      Serial.println(scaled_pulse);
      last_hitpoint = scaled_pulse;
    }
    else if(last_hitpoint + hp_delta - 3 <= scaled_pulse && scaled_pulse <= last_hitpoint + hp_delta + 3){ 
      Serial.print("Diverter ");
      Serial.print(diverter_num);
      Serial.print(" HP: ");
      Serial.println(scaled_pulse);
      diverter_num++;
      last_hitpoint = scaled_pulse;
      
    }
    else{
      Serial.println("Unexpected flag");
      Serial.print("Flag count: ");
      Serial.println(flag_cnt);
    }

    data_recieved = false;
    blinkk();
    //delay(300);
    
    
  }
  
  prev_cnt = current_cnt;

}

//*************************FUNCTIONS START****************************

void Encoder_ISR() {
  pulse_start = micros();
  deltaT = pulse_start - last;
  last = pulse_start; 

  if(deltaT > 200){
    pulse_count++;
  }
}


void receiveCallBackFunction(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
    memcpy(&myData, incomingData, sizeof(myData));
    //Serial.print("NewMsg ");
    //Serial.print("MacAddr ");
    //for (byte n = 0; n < 6; n++) {
        //Serial.print (senderMac[n], HEX);
   // }
    //Serial.print("  MsgLen ");
    //Serial.print(len);
   // Serial.print("  Text ");
    //Serial.print(myData.message);
    //Serial.print("  Time ");
    //Serial.print(myData.time);   
    //Serial.println();
    data_recieved = true;
}

void get_priNum(){
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    
    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      rx_str += rx_byte;
    }
    else if (rx_byte == '\n') {
      // end of string
      if (not_number) {
        Serial.println("Not a number. Please enter a number 1-46");
      }
      else {
        result = rx_str.toInt();

        if(result > 46 || result < 1){
          Serial.println("Not a valid module number. Enter 1-46");
          //Serial.println(not_number);
          rx_str = "";
          serialFlush();
          return;
        }
        if(result == 41){
          delay(500);
          Serial.println("Module: RC1");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;          
        }
        else if(result == 42){
          delay(500);
          Serial.println("Module: RC2");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;  
        }
        else if(result == 43){
          delay(500);
          Serial.println("Module: RC3");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;            
        }
        else if(result == 44){
          delay(500);
          Serial.println("Module: SSR1");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;            
          
        }
        else if(result == 45){
          delay(500);
          Serial.println("Module: SSR2");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;  
          
        }
        else if(result == 46){
          delay(500);
          Serial.println("Module: SSR4");
          //Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true; 
          
        }
        else{        
          delay(500);
          Serial.print("Module: P");
          Serial.println(result);
          delay(500);
          Serial.print("Primary divider set to: ");
          Serial.println(module_divider_num[result - 1],4);
          inch_divider = module_divider_num[result - 1];
          delay(500);
          //Serial.println("Awaiting flags");
          got_num = true;
        }
        
      }
      not_number = false;         // reset flag
      rx_str = "";                // clear the string for reuse
    }
    else {
      // non-number character received
      not_number = true;    // flag a non-number
    }
  } 

}

void get_letter(){
    if (Serial.available() > 0) {    // is a character available?
     rx_byte = Serial.read();       // get the character
    
       
        if(rx_byte == 'p' || rx_byte == 'P'){
         Serial.println("Primary");
         primary = true;
         got_letter = true;
        }
        else if(rx_byte == 'o' || rx_byte == 'O'){
          Serial.println("Outbound");
          primary = false;
          got_letter = true;
        }
        else if(rx_byte == '\n'){
          Serial.println("Enter 'p' or 'o'");
          //Serial.flush();
        }
        else{
          //do nothing
        }
  
    }
}


void serialFlush(){
  //Serial.println("flushing buffer");
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}  


void blinkk(){

        digitalWrite(LED_BUILTIN,LOW);
        delay(200);
        digitalWrite(LED_BUILTIN,HIGH);

}

