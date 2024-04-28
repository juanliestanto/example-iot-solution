#define LED_PIN 2

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); 
}

void loop() {
  char input;
  if(Serial.available() > 0){
    input = Serial.read();
    if(input == '1'){
      Serial.println("LED ON");
      digitalWrite(LED_PIN, HIGH);
    }else if(input == '0'){
      Serial.println("LED OFF");
      digitalWrite(LED_PIN, LOW);
    }else{
      Serial.println("INVALID INPUT");
    }
  }
}
