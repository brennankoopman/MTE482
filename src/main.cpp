/*
  Battery Monitor
  This example creates a BLE peripheral with the standard battery service and
  level characteristic. The A0 pin is used to calculate the battery level.
  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.
  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <String.h>

 // BLE Battery Service
BLEService batteryService("180F");

String MAC;

// BLE Battery Level Characteristic
BLEUnsignedCharCharacteristic batteryLevelChar("2A19",  // standard 16-bit characteristic UUID
    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

BLEUnsignedCharCharacteristic vibrationChar("ABDC", BLEWriteWithoutResponse| BLEWrite | BLERead);

long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  long tstart = millis();
  // wait at least 2 seconds for serial to start up if using it to debug
  while (!Serial)
  {
    if(millis()-tstart > 2000)
      break;
  }
  Serial.println(1);

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("BatteryMonitor");
  BLE.setAdvertisedService(batteryService); // add the service UUID
  batteryService.addCharacteristic(batteryLevelChar); // add the battery level characteristic
  batteryService.addCharacteristic(vibrationChar);
  BLE.addService(batteryService); // Add the battery service
  batteryLevelChar.writeValue(0); // set initial value for this characteristic
  vibrationChar.writeValue(0);
  MAC = BLE.address();

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();
  MAC.remove(2,1);
  MAC.remove(4,1);
  MAC.remove(6,1);
  MAC.remove(8,1);

  Serial.println("Bluetooth device active, waiting for connections...");
}

void printHex(uint8_t num) {
  char hexCar[3];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);
    unsigned char vibration[20] = {0};
    int data = 10;
    // while the central is connected:
    while (central.connected()) {
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 200) {
        Serial.println(MAC);
        previousMillis = currentMillis;
        Serial.print("Data is now: ");
        Serial.println(data);
        batteryLevelChar.writeValue(data);  // and update the battery level characteristic
        data = (data-9) % 10 + 10;
        // if the vibration characteristic has been written to by central
        if(vibrationChar.written())
        {
          Serial.print("bytes read: ");
          Serial.println(vibrationChar.readValue(vibration, 20));
          Serial.print("Vibration is now: ");
          Serial.print(vibrationChar.value());
          // for(size_t i = 0; i < sizeof(vibration); i++)
          // {
          //   printHex(vibration[i]);  
          // }
          Serial.println();
        }
      }
      
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
