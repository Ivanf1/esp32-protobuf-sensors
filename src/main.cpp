#include <Arduino.h>
#include <HTTPClient.h>
#include <pb_decode.h> // headers from nanopb
#include <pb_encode.h> // -------------------

#include "nanopbGenerated/update.pb.h" // header generated based on update.proto
#include "encodeCallback/encodeCallback.h"
#include "secrets.h"

#define DEVICE_ID "testdev001"
#define SENSOR_ID "senstest002"
#define NUMBER_OF_SENSORS 2

// buffer that will contain the encoded message
uint8_t buffer[128];
SensorUpdateMsgArray sensorMessages = SensorUpdateMsgArray_init_zero;

bool readSensorsAndEncode(uint8_t* buffer, pb_ostream_t* stream, SensorUpdateMsgArray* sensorMessages) {
  // initialize struct
  DeviceUpdateMsg update = DeviceUpdateMsg_init_zero;

  // set the 'arg' for the encode callback for the deviceId field
  update.deviceId.arg = (void*)DEVICE_ID;
  // set 'encode_string' as encode callback for the deviceId field
  update.deviceId.funcs.encode = &encode_string;

  // set 'encode_sensor_message_array' as encode callback for the sensor messages
  update.sensor.funcs.encode = &encode_sensor_message_array;
  // set the 'arg' for the encode callback
  update.sensor.arg = sensorMessages;

  SensorUpdateMsg sensUpdate = SensorUpdateMsg_init_zero;
  sensUpdate.sensorId.arg = (void*)SENSOR_ID;
  sensUpdate.sensorId.funcs.encode = &encode_string;
  sensUpdate.value.valueInt = 12;
  // since 'value' is a union type, we need to set 'which_value' with the correct tag
  sensUpdate.which_value = SensorUpdateMsg_valueInt_tag;

  sensorMessages->messages[0] = &sensUpdate;
  sensorMessages->messages[1] = &sensUpdate;
  sensorMessages->size = NUMBER_OF_SENSORS;

  // encode whole message and write it to buffer
  if (!pb_encode(stream, DeviceUpdateMsg_fields, &update)) {
    Serial.println("failed to encode device update message");
    return false;
  }

  return true;
}

void sendProtobufToServer(uint8_t* payload, size_t size, const char* serverUrl) {
  HTTPClient client;
  client.begin(serverUrl);
  client.addHeader("Content-Type", "application/octet-stream");
  client.POST(payload, size);
  client.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Error connecting to wifi");
    return;
  }
  
  sensorMessages.messages = (SensorUpdateMsg**) pvPortMalloc(sizeof(SensorUpdateMsg*) * NUMBER_OF_SENSORS);
  if (!sensorMessages.messages) {
    Serial.println("malloc failed");
    return;
  }
}

void loop() {
  // create a stream that will write to the buffer
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  if (readSensorsAndEncode(buffer, &stream, &sensorMessages)) {
    sendProtobufToServer(buffer, stream.bytes_written, SERVER_URL);
  }

  delay(5000);
}