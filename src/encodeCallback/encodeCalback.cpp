#include "encodeCallback/encodeCallback.h"

#include <Arduino.h>
#include <pb_encode.h>
#include "nanopbGenerated/update.pb.h"

// https://jpa.kapsi.fi/nanopb/docs/concepts.html#field-callbacks
bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
  const char* str = (const char*)(*arg);

  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }

  return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

bool encode_sensor_message(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
  SensorUpdateMsg* sensorMessage = (SensorUpdateMsg*)(*arg);

  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }

  return pb_encode_submessage(stream, SensorUpdateMsg_fields, sensorMessage);
}

bool encode_sensor_message_array(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
  SensorUpdateMsgArray* sensorMessages = (SensorUpdateMsgArray*)(*arg);

  for (int i = 0; i < sensorMessages->size; i++) {
    if (!pb_encode_tag_for_field(stream, field)) {
      return false;
    }

    if (!pb_encode_submessage(stream, SensorUpdateMsg_fields, sensorMessages->messages[0])) {
      return false;
    }
  }

  return true; 
}