
void setup() {

  Serial.begin(57600);  //  initialize Serial communication
  while (!Serial);       //  wait for the serial monitor to open
  
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("hi");
    delay(1000);
}
