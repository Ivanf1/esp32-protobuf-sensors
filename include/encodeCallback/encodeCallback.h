#ifndef ESP32PROTOBUF_ENCODE_H_
#define ESP32PROTOBUF_ENCODE_H_

#include <pb_encode.h>

bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);
bool encode_sensor_message(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);

#endif