#include <FlexCAN_T4.h>
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1; // Use CAN2
CAN_message_t msg;
CAN_message_t txMsg; // Message for transmitting

// Haltech CAN ID for the 8-byte message containing RPM, MAP, TPS, Coolant Pressure
#define HALTECH_DATA_ID 0x360

// Variables for simulated data
uint16_t rpm = 0;        // 0-8000 RPM
uint16_t mapValue = 0;   // kPa (0-255)
uint16_t tps = 0;        // 0-100%
uint16_t coolantPress = 0; // kPa (0-255)
bool countingUp = true;
unsigned long lastSentTime = 0;
const unsigned long sendInterval = 100; // 10Hz = 100ms

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Haltech CAN Transmitter...");
  
  // Initialize CAN1
  can1.begin();
  can1.setBaudRate(500000);
  can1.enableFIFO();
  
  // Print status information
  Serial.println("CAN2 Configuration:");
  Serial.println(" TX Pin: 24 (CTX1)");
  Serial.println(" RX Pin: 23 (CRX1)");
  Serial.println(" Baud Rate: 500000");
  
  Serial.println("Ready to send Haltech messages...");
}

void sendHaltechMessage() {
  txMsg.id = HALTECH_DATA_ID;
  txMsg.len = 8;  // Fixed 8-byte message
  txMsg.flags.extended = 0; // Standard ID (not extended)
  
  // Format the data in little-endian format (LSB first)
  txMsg.buf[0] = rpm & 0xFF;        // RPM LSB
  txMsg.buf[1] = (rpm >> 8) & 0xFF; // RPM MSB
  txMsg.buf[2] = mapValue & 0xFF;        // MAP LSB
  txMsg.buf[3] = (mapValue >> 8) & 0xFF; // MAP MSB
  txMsg.buf[4] = tps & 0xFF;        // TPS LSB
  txMsg.buf[5] = (tps >> 8) & 0xFF; // TPS MSB
  txMsg.buf[6] = coolantPress & 0xFF;        // Coolant Pressure LSB
  txMsg.buf[7] = (coolantPress >> 8) & 0xFF; // Coolant Pressure MSB
  
  if (can1.write(txMsg)) {
    Serial.print("Sent Haltech message - ID: 0x");
    Serial.print(HALTECH_DATA_ID, HEX);
    Serial.print(" Data: ");
    for (uint8_t i = 0; i < 8; i++) {
      if(txMsg.buf[i] < 0x10) Serial.print("0");
      Serial.print(txMsg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" (RPM: ");
    Serial.print(rpm);
    Serial.print(" MAP: ");
    Serial.print(mapValue);
    Serial.print(" TPS: ");
    Serial.print(tps);
    Serial.print(" CoolantPress: ");
    Serial.print(coolantPress);
    Serial.println(")");
  } else {
    Serial.println("Failed to send message!");
  }
}

void loop() {
  // Read incoming messages
  if (can1.read(msg)) {
    Serial.print("Received message - ID: 0x");
    Serial.print(msg.id, HEX);
    Serial.print(" Length: ");
    Serial.print(msg.len);
    Serial.print(" Data: ");
    for (uint8_t i = 0; i < msg.len; i++) {
      if(msg.buf[i] < 0x10) Serial.print("0");
      Serial.print(msg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  // Send messages every 100ms (10Hz)
  unsigned long currentTime = millis();
  if (currentTime - lastSentTime >= sendInterval) {
    lastSentTime = currentTime;
    
    // Update simulated values
    if (countingUp) {
      rpm += 10;
      mapValue += 1;
      tps += 1;
      coolantPress += 1;
      if (rpm >= 8000) countingUp = false;
    } else {
      rpm -= 10;
      mapValue -= 1;
      tps -= 1;
      coolantPress -= 1;
      if (rpm <= 0) countingUp = true;
    }
    
    // Clamp values to reasonable ranges
    rpm = constrain(rpm, 0, 8000);
    mapValue = constrain(mapValue, 0, 255);  // kPa
    tps = constrain(tps, 0, 100);  // percentage
    coolantPress = constrain(coolantPress, 0, 255);  // kPa
    
    // Send the complete 8-byte message
    sendHaltechMessage();
  }
  
  // Print a heartbeat message every 5 seconds
  static uint32_t lastHeartbeat = 0;
  if (currentTime - lastHeartbeat > 5000) {
    Serial.println("Still running...");
    lastHeartbeat = currentTime;
  }
} 