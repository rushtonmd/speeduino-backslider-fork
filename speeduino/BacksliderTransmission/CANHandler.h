#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <Arduino.h>

// CAN message structure
typedef struct CAN_message_t {
    uint32_t id = 0;         // can identifier
    uint16_t timestamp = 0;  // time when message arrived
    uint8_t idhit = 0;       // filter that id came from
    struct {
        bool extended = 0;     // identifier is extended (29-bit)
        bool remote = 0;       // remote transmission request packet type
        bool overrun = 0;      // message overrun
        bool reserved = 0;
    } flags;
    uint8_t len = 8;         // length of data
    uint8_t buf[8] = { 0 };  // data
    int8_t mb = 0;           // used to identify mailbox reception
    uint8_t bus = 1;         // used to identify where the message came (CAN1, CAN2 or CAN3)
    bool seq = 0;            // sequential frames
} CAN_message_t;

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