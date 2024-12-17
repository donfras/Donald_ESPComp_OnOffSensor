#define MQTT_MAX_PACKET_SIZE 1024

#include "Donald_OnOffSensor.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
static const char *TAG = "donald_onoffsensor";

#define ESP_INTR_FLAG_DEFAULT 0
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    DonaldOnOffSensor* s = (DonaldOnOffSensor*)arg;
    xQueueSendFromISR(gpio_evt_queue, &s, NULL);
}
sensorEventCallback DonaldOnOffSensorCollection::callback = 0;

DonaldOnOffSensorCollection::DonaldOnOffSensorCollection(std::vector<DonaldOnOffSensor> pins) 
							: sensorGPIOPins(pins)
{
	ESP_LOGI(TAG, "DonaldOnOffSensorCollection constructor entry");
	
    // Create a queue to handle gpio events from the gpio interrupt service routine
    gpio_evt_queue = xQueueCreate(10, sizeof(DonaldOnOffSensor*));
	
    SetupGPIOs();
	
	xTaskCreate(
		GPIOTask,    // Function that should be called
		"GPIOTask",  // Name of the task (for debugging)
		8000,      // Stack size (bytes)
		(void*)0,       // Parameter to pass
		1,          // Task priority
		&gpioTask       // Task handle
	);
}

DonaldOnOffSensorCollection::~DonaldOnOffSensorCollection()
{
	callback = 0; // Prevent any further callbacks
	vTaskDelete(gpioTask);
}

void DonaldOnOffSensorCollection::SetupGPIOs(void)
{  
	ESP_LOGI(TAG, "SetupGPIOs");
	
    // Install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	 //gpio_install_isr_service(ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM);

    // configure all of the input pins
	for (auto& s : sensorGPIOPins)
	{
	  ESP_LOGI(TAG, "SetupGPIOs: adding pin %d", (int)s.pin);
	  gpio_set_direction(s.pin, GPIO_MODE_INPUT);
	  gpio_set_intr_type(s.pin, GPIO_INTR_ANYEDGE);	  
	  gpio_set_pull_mode(s.pin, GPIO_PULLUP_ONLY);
      gpio_isr_handler_add(s.pin, gpio_isr_handler, (void*)&s);
	  gpio_intr_enable(s.pin);
	}
}

void DonaldOnOffSensorCollection::RegisterSensorEventCallback(sensorEventCallback callback)
{
	this->callback = callback;
}

// Reads the gpio event queue for any sensor pin level changes.
// Calls any configured callback if the actual pin logic level is different from the last recorded pin
// level - acts as a debounce.
void DonaldOnOffSensorCollection::GPIOTask(void* parameter)
{
	while (1) 
	{ 
		DonaldOnOffSensor* s;
        if (xQueueReceive(gpio_evt_queue, &s, portMAX_DELAY)) 
		{
			if(callback)
			{
				bool measuredSensorLevel = gpio_get_level(s->pin); // read the actual current logic level

				if ( measuredSensorLevel != s->state)
				{
					ESP_LOGI(TAG, "GPIO: %d state: %d new state: %d", (int)s->pin, s->state, measuredSensorLevel);
					// Pin has changed state - call the callback
					s->state = measuredSensorLevel;
					callback(*s);
				}
			}
        }
    }
}
