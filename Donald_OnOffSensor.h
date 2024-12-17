#ifndef DONALD_ONOFFSENSOR_H
  #define DONALD_ONOFFSENSOR_H
#include <string>
#include <vector>
#include <esp_event.h>
#include "esp_timer.h"
#include "driver/gpio.h"

using namespace std;

struct DonaldOnOffSensor
{
  DonaldOnOffSensor(unsigned short pin) : pin((gpio_num_t)pin), state(true ){;}
  string SensorStateAsString(){return state ? "true" : "false";}
  gpio_num_t pin;
  bool state;
};

typedef  void (*sensorEventCallback)( DonaldOnOffSensor& onOffSensor);

class DonaldOnOffSensorCollection
{
  public:

    DonaldOnOffSensorCollection(vector<DonaldOnOffSensor> pins);
   ~DonaldOnOffSensorCollection();

    void RegisterSensorEventCallback(sensorEventCallback callback);
	
  private:
	  void SetupGPIOs(void);

	  static void GPIOTask(void* parameter);

    vector<DonaldOnOffSensor> sensorGPIOPins;
    TaskHandle_t gpioTask;
    static sensorEventCallback callback;
};
#endif