
/*
Donald Fraser 9 December 2024
Test project for Donald_ESPComp_OnOffSensor library
*/

#define MQTT_MAX_PACKET_SIZE 1024
#include <string>
#include <Donald_OnOffSensor.h>
#include "esp_log.h"

using namespace Glengyle;

string mQTTClientID="OnoffSensorTest";
static const char *TAG = "Glengyle::onoffsensor_test";
OnOffSensorCollection* sensors;
string GetSensorJSON(OnOffSensor& s, string deviceID);
void onSensorEvent( OnOffSensor& onOffSensor);
// ************************ Remote Parameters ****************************
//static const char *testTAG = "onoffsensor_test";

extern "C" void app_main(void)
{
    // gpio_install_isr_service used in DonaldOnOffSensorCollection fails if called before app_main is called.
    // Need to explicitly construct DonaldOnOffSensorCollection instead of having an automatic global variable
   // std::vector<OnOffSensor> pins = {{15}, {13}, {19}, {20}, {21}, {22}};
    std::vector<OnOffSensor> pins = {{15}, {13}};
    sensors = new OnOffSensorCollection(pins);
    sensors->RegisterSensorEventCallback(onSensorEvent);
}

// { 
//  "device": "ESPAlarm",
//  "sensor": 1,
//  "sensorState": true,
// }
string GetSensorJSON(OnOffSensor& s, string deviceID)
{
  string retVal = "{";
  retVal = retVal + "\"device\": \"" + deviceID + "\"";
  retVal = retVal + ",\"sensor\": " + to_string(s.pin);
  retVal = retVal + ",\"sensorState\": " + s.SensorStateAsString();
  retVal = retVal + "}";
  return retVal;
}

void onSensorEvent(OnOffSensor& onOffSensor)
{
    ESP_LOGI(TAG, "onSensorEvent: event for pin %d", (int)onOffSensor.pin);
}


