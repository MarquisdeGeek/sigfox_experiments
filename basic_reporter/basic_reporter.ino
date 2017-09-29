#include <SoftwareSerial.h>

#define TX 4
#define RX 5

#define PHOTO_PIN A0
#define LED_PIN   LED_BUILTIN

#define TRANSMIT_DELAY_BETWEEN_MESSAGES     (1*1000)   // Number of milliseconds
#define TRANSMIT_MESSAGES_PER_BATCH         4     // So one packet sent every DELAY * PER_BATCH seconds
#define TRANSMIT_PACKET_SIZE                (2+(TRANSMIT_MESSAGES_PER_BATCH*2))

#define TRANSMIT_BLINK_DURATION             1000     // Number of milliseconds the LED is on

const bool g_TraceConsole = false;

SoftwareSerial sigfox(RX,TX);

/*
 * Typical GET request from SIGFOX will appear as:
 * 
 * URL parameters 
  [id] => 17354
  [time] => 1506615497
  [data] => B:5b5c60625b
 */
/*
 * The Serial port should be configured:
 * 
 * Baud Rate                -    9600 bps
 * Parity                   -    None
 * Data Bits                -    8
 * Stop Bits                -    1
 * Flow Control             -    None
 * Local Echo (Optional)    -    Yes
 */

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  if (g_TraceConsole) {
    Serial.begin(9600); // Initalise Serial connection
    Serial.println("Running...");
  }

  sigfox.begin(9600);
  sigfox.write("AT$ID?\n");
}


void loop(){
  int messages = 0;
  String msg("B:");
  
  while(messages < TRANSMIT_MESSAGES_PER_BATCH) {
    // Read data
    byte brightness = analogRead(PHOTO_PIN) / 4; // convert 1023 analog into 0-255 for 2 byte hex values

    // Pad hex to two digits
    char buffer[3];
    sprintf (buffer, "%.2x", brightness);
    msg += buffer;

    // Move to next message
    ++messages;
    delay(TRANSMIT_DELAY_BETWEEN_MESSAGES);
  }

  // Indicate that we're sending
  if (g_TraceConsole) {
    Serial.println(msg);
  }
  digitalWrite(LED_BUILTIN, HIGH);

  // Send it
  sendSigfoxString(msg);
  delay(TRANSMIT_BLINK_DURATION);
  digitalWrite(LED_BUILTIN, LOW);

  // Drop back to loop() and continue
}


void sendSigfoxString(String frame){
  String msg;
  String concat("");
  
  for(int i=0;i<frame.length();++i) {
    msg += concat;
    String digits(frame.charAt(i), HEX);
    if (digits.length() == 1) {
      digits = "0" + digits;
    }
    msg += digits;
    concat = " ";
  }
  sendSigfox(msg);
}

void sendSigfox(String frame){
 if (frame.length() % 2 != 0){
   frame = "0"+frame;
 }

 sigfox.write("AT$SF=");
 sigfox.print(frame);
 sigfox.write("\n");
}


