//https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
 
#define LEDPIN 3//mosfet gate signal

//number of steps in brightness controll 
const int pwmIntervals = 500;

float R;//rate
int period = 5; //period from min to full brightness

 
void setup() {
  pinMode(LEDPIN, OUTPUT);//pwm output

  //exponential rate
  R = (pwmIntervals * log10(2))/(log10(255));
}
 
 
void loop() {
  int brightness = 0;
  

  for (int interval = 0; interval <= pwmIntervals; interval++) {
      // Calculate the required PWM value for this interval step
      brightness = pow (2, (interval / R)) - 1;
      // Set the LED output to the calculated brightness
      analogWrite(LEDPIN, brightness);
      delay(period*1000/pwmIntervals);
  }

}
