#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <Arduino.h>

// Define NATIVE_CAN_AVAILABLE if not already defined
#ifndef NATIVE_CAN_AVAILABLE
#define NATIVE_CAN_AVAILABLE 1
#endif

#if defined(NATIVE_CAN_AVAILABLE)
#include <FlexCAN_T4.h>

// CAN instance declaration
//extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

// CAN message structures from comms_CAN.cpp
extern CAN_message_t inMsg;
extern CAN_message_t outMsg;
#endif

// CAN message parsing helpers with byte offsets
uint16_t transCAN_ParseUint16AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian = false);
uint32_t transCAN_ParseUint32AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian = false);
int16_t transCAN_ParseInt16AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian = false);
int32_t transCAN_ParseInt32AtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian = false);
float transCAN_ParseFloatAtOffset(uint8_t* data, uint8_t offset, bool isLittleEndian = false);

// CAN message building helpers with byte offsets
void transCAN_BuildUint16AtOffset(uint8_t* buffer, uint16_t value, uint8_t offset, bool isLittleEndian = false);
void transCAN_BuildUint32AtOffset(uint8_t* buffer, uint32_t value, uint8_t offset, bool isLittleEndian = false);
void transCAN_BuildInt16AtOffset(uint8_t* buffer, int16_t value, uint8_t offset, bool isLittleEndian = false);
void transCAN_BuildInt32AtOffset(uint8_t* buffer, int32_t value, uint8_t offset, bool isLittleEndian = false);
void transCAN_BuildFloatAtOffset(uint8_t* buffer, float value, uint8_t offset, bool isLittleEndian = false);

// Original CAN message parsing helpers (kept for backward compatibility)
uint16_t transCAN_ParseUint16(uint8_t* data, bool isLittleEndian = true);
uint32_t transCAN_ParseUint32(uint8_t* data, bool isLittleEndian = true);
int16_t transCAN_ParseInt16(uint8_t* data, bool isLittleEndian = true);
int32_t transCAN_ParseInt32(uint8_t* data, bool isLittleEndian = true);
float transCAN_ParseFloat(uint8_t* data, bool isLittleEndian = true);

// Original CAN message building helpers (kept for backward compatibility)
void transCAN_BuildUint16(uint8_t* buffer, uint16_t value, bool isLittleEndian = true);
void transCAN_BuildUint32(uint8_t* buffer, uint32_t value, bool isLittleEndian = true);
void transCAN_BuildInt16(uint8_t* buffer, int16_t value, bool isLittleEndian = true);
void transCAN_BuildInt32(uint8_t* buffer, int32_t value, bool isLittleEndian = true);
void transCAN_BuildFloat(uint8_t* buffer, float value, bool isLittleEndian = true);

// CAN bus functions
void transCAN_Init(uint32_t baudRate = 500000); // Default to 500kbps if not specified
void transCAN_ProcessMessages();
void transCAN_SendMessage(uint32_t id, uint8_t length, uint8_t* data, bool isExtended = false);

// Helper function
void transCAN_SwapBytes(uint8_t* data, uint8_t length);

#endif // CAN_HANDLER_H 