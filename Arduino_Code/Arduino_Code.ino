/*
  This program continouously measures the temperature and light, converts the values into degrees celsius and lux respectively, and prints them on the LCD
*/

#include "arduino_secrets.h"
#include "rgb_lcd.h"
#include "DHT.h"
#include <MKRWAN.h>

LoRaModem modem;
 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

DHT dht;
#define dhtSensorPin 2                 // The DHT sensor is connected to pin D2
int humidity;                          // humidity value 
int temperature;                       // temperature value 

#define RelayPin 0                     // The relay is attached to pin D0
byte relayStatus;                      // Variable to indicate whether the relay is on or off

rgb_lcd lcd;                           // Instantiate the rgb_lcd
const int colorR = 127;                // Set the Red value of the backlight
const int colorG = 127;                // Set the Green value of the backlight
const int colorB = 0;                  // Set the Blue value of the backlight 

unsigned long previousMillis = 0;
const long interval = 600000;          // interval in milliseconds

void setup() 
{
  dht.setup(dhtSensorPin);             // Setup the DHT sensor

  pinMode(RelayPin, OUTPUT);           // Set the relay as an output
  
  lcd.begin(16, 2);                    // set up the LCD's number of columns and rows: 
  lcd.setRGB(colorR, colorG, colorB);  // Set the colour of the display RGB backlight

  lcd.setCursor(0, 0);                 // set the cursor to column 0, line 0
  lcd.print("Humid: ");
  
  lcd.setCursor(1, 1);                 // set the cursor to column 1, line 1
  lcd.print("Temp: ");
  
  delay(1000);

  if (!modem.begin(EU868)) {                        // initialise the LoRaWAN modem
    Serial.println("Failed to start module");       // if it does not start notify the user
    while (1) {}                                    // and stop
  };
  int connected = modem.joinOTAA(appEui, appKey);   // Use the provided credentials to over the air auithenticate the device and connect 
  if (!connected) {                                 // If the device failed to connect
    Serial.println("Connection failed");            // notify the user
    while (1) {}                                    // and stop
  }
}

// ------------------------------------------------------------------- M A I N -------------------------------------------------------------------
void loop() 
{ 
  humidity = dht.getHumidity();       // Get humidity value 
  temperature = dht.getTemperature(); // Get temperature value 

  print_to_LCD();                                                               // print the data to the LCD

  if ((millis() - previousMillis >= interval) || (millis() < 10000))            // every 10 mins (and on boot)
  {
    previousMillis = millis();                          // save the time
    
    send_message(0);                                    // send Temperature and Humidity uplink 
  }

  send_message(1);                                      // send relay status uplink 
  
  delay(1000);

  int humidity2 = dht.getHumidity();       // Get humidity value again (fixes error where every other reading is NaN)
  int temperature2 = dht.getTemperature(); // Get temperature value again (fixes error where every other reading is NaN)

  // Delay between messages
  for(int i=1; i<=120; i++){
    delay(1000);
  }

  receive_message();                                   // check for downlink communications and change the relay state accordingly

  delay(5000);
}

// --------------------------------------------------------------- P R I N T   T O   L C D ---------------------------------------------------------------
void print_to_LCD()
{
  lcd.setCursor(7, 0);                           // set the cursor to column 7, line 0
  lcd.print("                ");                 // clear the previous humidity value from the display

  lcd.setCursor(7, 0);                           // set the cursor to column 7, line 0
  lcd.print(humidity);                           // print the humidity to the LCD
  lcd.print(" %");                               // print the units


  lcd.setCursor(7, 1);                           // set the cursor to column 7, line 1
  lcd.print("                ");                 // clear the previous temperature value from the display
  
  lcd.setCursor(7, 1);                           // set the cursor to column 7, line 1
  lcd.print(temperature);                        // print the temperature (in degrees celsius) to the LCD
  lcd.print(" deg C");                           // print the units
}

// --------------------------------------------------------------- S E N D   M E S S A G E ---------------------------------------------------------------
void send_message(int type)
{
  if (type == 0)
  {
    byte payload[5];                                     // initialise the payload array
    payload[0] = type;
    payload[1] = highByte(temperature);                     
    payload[2] = lowByte(temperature);
    payload[3] = highByte(humidity);                     
    payload[4] = lowByte(humidity);

    modem.beginPacket();                                 // initialise the uplink communication   
    modem.write(payload, sizeof(payload));               // send the payload
    int err = modem.endPacket(true);                     // Check the message sent correctly
    if (err > 0) {                                       // and inform the user whether it sent correctly or not
      Serial.println("Message sent correctly!");  
    } else {
      Serial.println("Error sending message :(");
    }
  }
  else if (type == 1)
  {
    byte payload[2];
    payload[0] = type;
    payload[1] = relayStatus;

    modem.beginPacket();                                 // initialise the uplink communication   
    modem.write(payload, sizeof(payload));               // send the payload
    int err = modem.endPacket(true);                     // Check the message sent correctly
    if (err > 0) {                                       // and inform the user whether it sent correctly or not
      Serial.println("Message sent correctly!");  
    } else {
      Serial.println("Error sending message :(");
    }
  }
}

// ------------------------------------------------------------ R E C E I V E   M E S S A G E ------------------------------------------------------------
void receive_message()
{
  // receive message:
  //  x00:        Relay OFF
  //  Otherwise:  Relay ON
  
  byte incomingData;                         // byte to store the downlink payload
  bool messageFlag = false;                  // bool to indicate when a new payload has been received
  
  while (modem.available())                  // if there is incoming data from the LoRaWAN network
  {
    incomingData = (byte)modem.read();       // store the payload in the incomingData variable
    messageFlag = true;                      // and indicate that a new payload has been received
  }

  if (messageFlag)                          
  {
    Serial.print("Received: ");               // Print the received payload to the serial monitor
    Serial.print(incomingData);

    if (incomingData == 0)                    // if a 0 was received
    {
      Serial.println(" Turning relay OFF");   // indicate that the relay is being turned off
      digitalWrite(RelayPin, LOW);            // turn off the relay
      relayStatus = 0;                        // and change the status of the relay to 0

      lcd.setRGB(colorR, colorG, 0);          // Set the colour of the display RGB backlight to indicate relay status
    }
    else                                      // if any other value was received
    {
      Serial.println(" Turning relay ON");    // indicate that the relay is being turned on
      digitalWrite(RelayPin, HIGH);           // turn on the relay
      relayStatus = 1;                        // and change the status of the relay to 1

      lcd.setRGB(colorR, colorG, 255);        // Set the colour of the display RGB backlight to indicate relay status
    }
  }
}
