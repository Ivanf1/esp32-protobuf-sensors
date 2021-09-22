#include <Arduino.h>
#include <HTTPClient.h>
#include <pb_decode.h> // headers from nanopb
#include <pb_encode.h> // -------------------

#include "nanopbGenerated/update.pb.h" // header generated based on update.proto
#include "encodeCallback/encodeCallback.h"
#include "secrets.h"

#define DEVICE_ID "testdev001"
#define SENSOR_ID "senstest002"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Error connecting to wifi");
    return;
  }

  // initialize struct
  DeviceUpdateMsg update = DeviceUpdateMsg_init_zero;

  // buffer that will contain the encoded message
  uint8_t buffer[128];

  // create a stream that will write to the buffer
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  // set the 'arg' for the encode callback for the deviceId field
  update.deviceId.arg = (void*)DEVICE_ID;
  // set 'encode_string' as encode callback for the deviceId field
  update.deviceId.funcs.encode = &encode_string;



  SensorUpdateMsg sensUpdate = SensorUpdateMsg_init_zero;
  // pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  sensUpdate.sensorId.arg = (void*)SENSOR_ID;
  sensUpdate.sensorId.funcs.encode = &encode_string;

  sensUpdate.value.valueInt = 12;
  sensUpdate.which_value = SensorUpdateMsg_valueInt_tag;

  update.sensor.funcs.encode = &encode_sensor_message;
  update.sensor.arg = &sensUpdate;

  // encode and write to buffer
  if (!pb_encode(&stream, DeviceUpdateMsg_fields, &update)) {
    Serial.println("failed to encode device update message");
    return;
  }

  HTTPClient client;
  client.begin(SERVER_URL);
  client.addHeader("Content-Type", "application/octet-stream");
  client.POST(buffer, stream.bytes_written);
  client.end();
}

void loop() { }