#define LED 13

volatile unsigned char gISRFlag2   = 0;

unsigned int gReloadTimer1 = 100;   // corresponds to 0.4ms

#define BUFF_SIZE 20
char  gIncomingChar;
char  gCommsMsgBuff[BUFF_SIZE];
int   iBuff = 0;
byte  gPackageFlag = 0;
byte  gProcessDataFlag = 0;

char compareArray(char a[], char b[], int size)
{
  int i;
  char result = 1;  // default: the arrays are equal
  
  for(i = 0; i<size; i++)
  {
    if(a[i]!=b[i])
    {
      result = 0;
      break;
    }
  }
  return result;
}

void setup() {
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);

  // Initialize Timer1 (16bit) -> Used for Serial Comms
  // Speed of Timer1 = 16MHz/64 = 250 KHz
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = gReloadTimer1;            // max value 2^16 - 1 = 65535
  TCCR1A |= (1<<WGM11);
  TCCR1B = (1<<CS11) | (1<<CS10);   // 64 prescaler
  TIMSK1 |= (1<<OCIE1A);
  interrupts();
}

void loop() {
  char  auxMsgBuff[BUFF_SIZE];
  int auxCount = 0;
  unsigned char auxDigit = '0';
  
  // Attend Timer1 flag - receive commands through serial
  if(gISRFlag2 == 1)
  {    
    // Reset ISR Flag
    gISRFlag2 = 0;

    // Read serial
    gIncomingChar = Serial.read();

    // If normal character from package
    if(gPackageFlag == 1)
    {
      gCommsMsgBuff[iBuff] = gIncomingChar;
      iBuff++;

      // Safety mechanism in case "\n" is never sent
      if(iBuff == BUFF_SIZE)
      {
        gPackageFlag = 0;
        gProcessDataFlag = 1;
      }
    }

    // If start of the package
    if(gIncomingChar == '$')
    {    
      gPackageFlag = 1;  // Signal start of package
      
      // Clear Buffer
      for(int i=0; i<BUFF_SIZE; i++)
      {
        gCommsMsgBuff[i] = 0;
      }

      // set gCommsMsgBuff Index to zero
      iBuff = 0;
    }

    // If end of package
    if( (gIncomingChar == '\n') && (gPackageFlag == 1) )
    {
      // Signal end of package
      gPackageFlag = 0;
      gProcessDataFlag = 1;
    }
  }

  // Process serial commands
  if(gProcessDataFlag == 1)
  {
    gProcessDataFlag = 0;
    
    if(compareArray(gCommsMsgBuff, "STR", 3) == 1)
    {
      digitalWrite(LED, HIGH);
    }
  
    if(compareArray(gCommsMsgBuff, "STP", 3) == 1)
    {
      digitalWrite(LED, LOW);
    }

    if(compareArray(gCommsMsgBuff, "GET", 3) == 1)
    {
      // Send clock status
      Serial.print("$00:01\n");
    }
    // ------
  }
}

/**
 * @brief Timer 1 ISR
 * @param TIMER1_COMPA_vect
 * @return
 */
ISR(TIMER1_COMPA_vect)  // Timer1 interrupt service routine (ISR)
{
  if(Serial.available() > 0)
  {
    gISRFlag2 = 1;
  }
}
