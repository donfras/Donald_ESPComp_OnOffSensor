#ifndef DONALD_ONOFFSENSOR_H
  #define DONALD_ONOFFSENSOR_H
#include <string>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h" // for gpio_num_t

using namespace std;
namespace Glengyle
{
  struct OnOffSensor
  {
    OnOffSensor(unsigned short pin) : pin((gpio_num_t)pin), state(true ){;}
    string SensorStateAsString(){return state ? "true" : "false";}
    gpio_num_t pin;
    bool state;
  };

  typedef  void (*sensorEventCallback)( OnOffSensor& onOffSensor);

  class OnOffSensorCollection
  {
    public:

      OnOffSensorCollection(vector<OnOffSensor> pins, gpio_pull_mode_t pullMode = GPIO_PULLUP_ONLY);
    ~OnOffSensorCollection();

      void RegisterSensorEventCallback(sensorEventCallback callback);
    
    private:
      void SetupGPIOs(gpio_pull_mode_t pullMode);

      static void GPIOTask(void* parameter);

      vector<OnOffSensor> sensorGPIOPins;
      TaskHandle_t gpioTask;
      static sensorEventCallback callback;
  };
}
#endif