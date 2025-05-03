#include "CANHandler.h"

#if defined(NATIVE_CAN_AVAILABLE)
// Global variables
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t rxMsg;
CAN_message_t txMsg;

void transCAN_Init(uint32_t baudRate) {
    can1.begin();
    can1.setBaudRate(baudRate);
    can1.setMaxMB(16); // Use all available mailboxes
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.mailboxStatus();
}

void transCAN_ProcessMessages() {
    Serial.println("transCAN_ProcessMessages");
    while (can1.read(rxMsg)) {
        Serial.println("transCAN_ProcessMessages - received message");
        // Process received message
        // This is where you would add your message handling logic
        
        // EXAMPLE: Reading a 16-bit value from a Haltech CAN message
        // Message ID 0x650 contains transmission data
        // We want to read a 16-bit value starting at byte 2 (3rd byte)
        // The value is stored in big-endian format (most significant byte first)
        //if (rxMsg.id == 0x650) {
        //    // rxMsg.buf contains the 8 bytes of data from the CAN message
        //    // offset 2 means we start reading from the 3rd byte (0-based indexing)
        //    // false means we use big-endian format (Haltech's default)
        //    uint16_t value = transCAN_ParseUint16AtOffset(rxMsg.buf, 2, false);
        //    
        //    // Now you can use the value, for example:
        //    // - If it's a gear position: value will be 1-6
        //    // - If it's a temperature: value might need scaling (e.g., value * 0.1 for °C)
        //    // - If it's a pressure: value might need scaling (e.g., value * 0.1 for PSI)
        //}
    }
}

void transCAN_SendMessage(uint32_t id, uint8_t length, uint8_t* data, bool isExtended) {
    txMsg.id = id;
    txMsg.len = length;
    txMsg.flags.extended = isExtended;
    memcpy(txMsg.buf, data, length);
    can1.write(txMsg);
}
#else
// Stub implementations when NATIVE_CAN_AVAILABLE is not defined
void transCAN_Init(uint32_t baudRate) {
    // Do nothing when CAN is not available
}

void transCAN_ProcessMessages() {
    // Do nothing when CAN is not available
    Serial.println("transCAN_ProcessMessages - NATIVE_CAN_AVAILABLE is not defined");
}

void transCAN_SendMessage(uint32_t id, uint8_t length, uint8_t* data, bool isExtended) {
    // Do nothing when CAN is not available
}
#endif

// Message parsing helpers
uint16_t transCAN_ParseUint16(uint8_t* data, bool isLittleEndian) {
    uint16_t value;
    memcpy(&value, data, sizeof(uint16_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint16_t));
    }
    return value;
}

uint32_t transCAN_ParseUint32(uint8_t* data, bool isLittleEndian) {
    uint32_t value;
    memcpy(&value, data, sizeof(uint32_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint32_t));
    }
    return value;
}

int16_t transCAN_ParseInt16(uint8_t* data, bool isLittleEndian) {
    int16_t value;
    memcpy(&value, data, sizeof(int16_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int16_t));
    }
    return value;
}

int32_t transCAN_ParseInt32(uint8_t* data, bool isLittleEndian) {
    int32_t value;
    memcpy(&value, data, sizeof(int32_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int32_t));
    }
    return value;
}

float transCAN_ParseFloat(uint8_t* data, bool isLittleEndian) {
    float value;
    memcpy(&value, data, sizeof(float));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(float));
    }
    return value;
}

// Message building helpers
void transCAN_BuildUint16(uint8_t* buffer, uint16_t value, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint16_t));
    }
    memcpy(buffer, &value, sizeof(uint16_t));
}

void transCAN_BuildUint32(uint8_t* buffer, uint32_t value, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint32_t));
    }
    memcpy(buffer, &value, sizeof(uint32_t));
}

void transCAN_BuildInt16(uint8_t* buffer, int16_t value, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int16_t));
    }
    memcpy(buffer, &value, sizeof(int16_t));
}

void transCAN_BuildInt32(uint8_t* buffer, int32_t value, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int32_t));
    }
    memcpy(buffer, &value, sizeof(int32_t));
}

void transCAN_BuildFloat(uint8_t* buffer, float value, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(float));
    }
    memcpy(buffer, &value, sizeof(float));
}

// Helper function to swap bytes for endian conversion
void transCAN_SwapBytes(uint8_t* data, uint8_t length) {
    uint8_t temp;
    for (uint8_t i = 0; i < length / 2; i++) {
        temp = data[i];
        data[i] = data[length - 1 - i];
        data[length - 1 - i] = temp;
    }
}

// New offset-based parsing functions
uint16_t transCAN_ParseUint16AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian) {
    uint16_t value;
    memcpy(&value, data + offset, sizeof(uint16_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint16_t));
    }
    return value;
}

uint32_t transCAN_ParseUint32AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian) {
    uint32_t value;
    memcpy(&value, data + offset, sizeof(uint32_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint32_t));
    }
    return value;
}

int16_t transCAN_ParseInt16AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian) {
    int16_t value;
    memcpy(&value, data + offset, sizeof(int16_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int16_t));
    }
    return value;
}

int32_t transCAN_ParseInt32AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian) {
    int32_t value;
    memcpy(&value, data + offset, sizeof(int32_t));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int32_t));
    }
    return value;
}

float transCAN_ParseFloatAtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian) {
    float value;
    memcpy(&value, data + offset, sizeof(float));
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(float));
    }
    return value;
}

// New offset-based building functions
void transCAN_BuildUint16AtOffset(uint8_t* buffer, uint16_t value, uint8_t offset, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint16_t));
    }
    memcpy(buffer + offset, &value, sizeof(uint16_t));
}

void transCAN_BuildUint32AtOffset(uint8_t* buffer, uint32_t value, uint8_t offset, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(uint32_t));
    }
    memcpy(buffer + offset, &value, sizeof(uint32_t));
}

void transCAN_BuildInt16AtOffset(uint8_t* buffer, int16_t value, uint8_t offset, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int16_t));
    }
    memcpy(buffer + offset, &value, sizeof(int16_t));
}

void transCAN_BuildInt32AtOffset(uint8_t* buffer, int32_t value, uint8_t offset, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(int32_t));
    }
    memcpy(buffer + offset, &value, sizeof(int32_t));
}

void transCAN_BuildFloatAtOffset(uint8_t* buffer, float value, uint8_t offset, bool isLittleEndian) {
    if (!isLittleEndian) {
        transCAN_SwapBytes((uint8_t*)&value, sizeof(float));
    }
    memcpy(buffer + offset, &value, sizeof(float));
} 