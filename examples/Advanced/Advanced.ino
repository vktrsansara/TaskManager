#include <Arduino.h>
#include "TaskManager.h"

TaskManager taskManager;
uint32_t sensorTaskId;

void readSensor() {
  int value = analogRead(A0);
  Serial.printf("Sensor value: %d\n", value);
}

void controlTasks() {
  static bool paused = false;
  
  if (paused) {
    taskManager.resumeTaskByName("sensor");
    Serial.println("Sensor task resumed");
  } else {
    taskManager.pauseTask(sensorTaskId);
    Serial.println("Sensor task paused");
  }
  
  paused = !paused;
  
  // Показать статистику
  Serial.printf("Total tasks: %zu, Active: %zu\n",
               taskManager.getTaskCount(),
               taskManager.getActiveTaskCount());
}

void setup() {
  Serial.begin(115200);
  
  // Задача чтения датчика
  sensorTaskId = taskManager.addTask(
    readSensor, TASK_PERIODIC, 1000, "sensor"
  );
  
  // Задача управления (каждые 10 секунд)
  taskManager.addTask(
    controlTasks, TASK_PERIODIC, 10000, "control"
  );
  
  // Задача для отладки (каждые 5 секунд)
  taskManager.addTask(
    []() { taskManager.listTasks(); },
    TASK_PERIODIC, 5000, "debug"
  );
}

void loop() {
  taskManager.update();
  delay(10);
}
