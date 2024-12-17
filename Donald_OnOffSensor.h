#ifndef DONALD_ONOFFSENSOR_H
  #define DONALD_ONOFFSENSOR_H
#include <string>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h" // for gpio_num_t

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