char rx_byte = 0;
char junk = ' ';
String rx_str = "";
boolean not_number = false;
boolean got_num = false;
boolean got_sec_num = false;
boolean got_letter = false;
boolean asked = false;
boolean primary = false;
boolean secondary = false;
boolean D_1 = false;
boolean D_2 = false;
int result;

float module_divider_num[] = {15.62,15.85503,16.0,15.74088,15.745,15.645,
                              15.76,15.85,15.76,15.8255,15.8,15.7485,
                              15.82277,15.867,15.675,15.8499,15.75,15.8286,
                              15.74,15.74,15.78,15.8,15.78,15.89,15.8,15.725,
                              15.74,15.75,15.75,15.85,15.72,15.78952,15.8,
                              15.86925,15.91,15.825,15.875,15.78,15.78,15.92556,23};




void setup() {
  Serial.begin(9600);

    while(!got_letter){
       if(!asked){
        Serial.println("\nEnter 'p' for Primary 's' for Secondary");
        asked = true;
       }
       //Serial.flush();
       get_letter();
    }

    if(primary){
       Serial.println("Enter primary module number:");
       serialFlush();
       while(!got_num){
          get_priNum();
       }
    }
    else{
      Serial.println("Enter secondary module number:");
      serialFlush();
      while(!got_sec_num){
          get_secNum();
          //serialFlush();
       }
    }

    
}


void loop() {
Serial.println("looping");
while(true){
  delay(200);
  
}
  
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
        Serial.println("Not a known module number");
      }
      else {
        result = rx_str.toInt();
        delay(500);
        Serial.print("Module: P");
        Serial.println(module_divider_num[result - 1]);
        delay(500);
        Serial.println("Setting primary divider to 15.7xxx");
        delay(500);
        Serial.println("Awaiting flags");
        got_num = true;
        
        
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

void get_secNum(){
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    
    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      rx_str += rx_byte;
    }
    else if (rx_byte == '\n') {
      // end of string
      if (not_number) {
        Serial.println("Not a number");
      }
      else {
        result = rx_str.toInt();

        if(result > 21 || result < 3){
          Serial.println("Not a valid module number");
          serialFlush();
          return;
        }
        delay(500);
        Serial.print("Module: P");
        Serial.println(result);
        delay(500);
        Serial.print("Secondary divider set to: ");
        Serial.println(module_divider_num[result - 1]);
        //inch_divider = module_divider_num[result - 1];
        delay(500);
        //Serial.println("Awaiting flags");
        got_sec_num = true;
        
        
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
    
       
        if(rx_byte == 'p'){
         Serial.println("Primary");
         primary = true;
         got_letter = true;
        }
        else if(rx_byte == 's'){
          Serial.println("Secondary");
          secondary = true;
          got_letter = true;
        }
        else if(rx_byte == '\n'){
          Serial.println("Enter 'y' or 'n'");
          //Serial.flush();
        }
        else{
          //do nothing
        }
  
    }
} 

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}  

  


