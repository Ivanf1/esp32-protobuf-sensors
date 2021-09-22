#ifndef ESP32PROTOBUF_ENCODE_H_
#define ESP32PROTOBUF_ENCODE_H_

#include <Arduino.h>
#include <pb_encode.h>
#include "nanopbGenerated/update.pb.h"

// using the same convention as Nanopb
typedef struct _SensorUpdateMsgArray {
  size_t size;
  SensorUpdateMsg** messages;
} SensorUpdateMsgArray;

#define SensorUpdateMsgArray_init_zero {0, NULL}

bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);
bool encode_sensor_message(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);
bool encode_sensor_message_array(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);

#endif