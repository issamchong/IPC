
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

volatile TaskHandle_t Task1Handle = NULL;
volatile TaskHandle_t Task2Handle = NULL;

volatile QueueHandle_t QueueTask;

