#include <Arduino.h>
#include <HTTPClient.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "nanopbGenerated/update.pb.h" // header generated based on update.proto
#include "encodeCallback/encodeCallback.h"
#include "utils/reboot.h"
#include "utils/time.h"
#include "secrets.h"

#define DEVICE_ID 1
#define SENSOR_ID 1
#define NUMBER_OF_SENSORS 2

// buffer that will contain the encoded message
uint8_t buffer[128];
SensorUpdateMsgArray sensorMessages = SensorUpdateMsgArray_init_zero;

bool readSensorsAndEncode(uint8_t* buffer, pb_ostream_t* stream, SensorUpdateMsgArray* sensorMessages) {
  // initialize struct
  DeviceUpdateMsg update = DeviceUpdateMsg_init_zero;

  // set the 'arg' for the encode callback for the deviceId field
  update.deviceId = DEVICE_ID;

  // set 'encode_sensor_message_array' as encode callback for the sensor messages
  update.sensor.funcs.encode = &encode_sensor_message_array;
  // set the 'arg' for the encode callback
  update.sensor.arg = sensorMessages;

  SensorUpdateMsg sensUpdate0 = SensorUpdateMsg_init_zero;
  sensUpdate0.sensorId = SENSOR_ID;
  sensUpdate0.value.valueInt = random(20, 22);
  // since 'value' is a union type, we need to set 'which_value' with the correct tag
  sensUpdate0.which_value = SensorUpdateMsg_valueInt_tag;
  sensUpdate0.type = SensorUpdateMsg_Type::SensorUpdateMsg_Type_TEMPERATURE;
  sensUpdate0.time = static_cast<int64_t>(getTime());

  sensorMessages->messages[0] = &sensUpdate0;

  SensorUpdateMsg sensUpdate1 = SensorUpdateMsg_init_zero;
  sensUpdate1.sensorId = SENSOR_ID;
  sensUpdate1.value.valueInt = random(48, 50);
  // since 'value' is a union type, we need to set 'which_value' with the correct tag
  sensUpdate1.which_value = SensorUpdateMsg_valueInt_tag;
  sensUpdate1.type = SensorUpdateMsg_Type::SensorUpdateMsg_Type_HUMIDITY;
  sensUpdate1.time = static_cast<int64_t>(getTime());

  sensorMessages->messages[1] = &sensUpdate1;

  sensorMessages->size = NUMBER_OF_SENSORS;

  // encode whole message and write it to buffer
  if (!pb_encode(stream, DeviceUpdateMsg_fields, &update)) {
    ESP_LOGE(TAG, "failed to encode device update message, %s", PB_GET_ERROR(stream));
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
    rebootWithMsg("Error connecting to wifi");
  }
  
  sensorMessages.messages = (SensorUpdateMsg**) pvPortMalloc(sizeof(SensorUpdateMsg*) * NUMBER_OF_SENSORS);
  if (!sensorMessages.messages) {
    rebootWithMsg("malloc for sensor messages array failed");
  }

  configTime(0, 3600, "pool.ntp.org");
}

void loop() {
  // create a stream that will write to the buffer
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  if (readSensorsAndEncode(buffer, &stream, &sensorMessages)) {
    sendProtobufToServer(buffer, stream.bytes_written, SERVER_URL);
  }

  delay(5000);
}