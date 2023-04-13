#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER    6

#define DATA      9
#define LATCH     8
#define CLOCK     7

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13

unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};

volatile unsigned char onescount = 0;
volatile unsigned char tenscount = 0;


volatile unsigned char isr_1_flag = 0;
volatile unsigned char isr_2_flag = 0;
 
 void setup() {
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzer Pins
  pinMode(BUZZER, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

}

void Display(int digit, unsigned char num)
{
  disp_on(digit);
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num);
  digitalWrite(LATCH, HIGH);
}

void Active_Buzzer()
{
  unsigned char i;
  unsigned char sleep_time = 1; // ms
  
  for (i=0; i<100; i++)
   {
    digitalWrite(BUZZER,HIGH);
    delay(sleep_time);//wait for 1ms
    digitalWrite(BUZZER,LOW);
    delay(sleep_time);//wait for 1ms
    }
}

void FlashLEDs() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  delay(150);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
}

void Countdown()
{
  while(onescount != 0) {
    onescount--;
    delay(1000);
    Display(10, onescount);
  }
  /*
    if (onescount == 0) {
      for (int i = 0; i < 3; i++){
      FlashLEDs();
      Active_Buzzer();
      delay(25);
      Active_Buzzer();
      delay(50);
      Active_Buzzer();
      delay(25);
    }
    */
}

void Count() {
  Display(10, onescount);
  onescount++;
  delay(10);
  if(onescount == 9) {
    onescount = 0;
    tenscount++;
    Display(11, tenscount);
  }
}

void disp_on(int digit)
{
   digitalWrite(DIGIT_1, HIGH);
   digitalWrite(DIGIT_2, HIGH);
   digitalWrite(DIGIT_3, HIGH);
   digitalWrite(DIGIT_4, HIGH);
   digitalWrite(digit, LOW);
}

void Button_1_ISR()
{
  unsigned int buttonState = 0;  // variable for reading the pushbutton status

  // Set ISR Flag
  isr_1_flag = 1;

  buttonState = digitalRead(BUTTON_1);
  digitalWrite(RED_LED, buttonState);

  Count();
}

void Button_2_ISR()
{ 
  unsigned int buttonState = 0;  // variable for reading the pushbutton status

  // Set ISR Flag
  isr_2_flag = 1;
  
  buttonState = digitalRead(BUTTON_2);
  digitalWrite(GREEN_LED, buttonState);
}

void loop() {
  if(isr_2_flag == 1) {
    isr_2_flag = 0;
    Countdown();
  }
}
