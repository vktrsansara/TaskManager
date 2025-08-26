#include <Arduino.h>
#include "TaskManager.h"

TaskManager taskManager;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Мигание LED каждые 500ms
  taskManager.addTask([]() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }, TASK_PERIODIC, 500, "blink");
  
  // Одноразовое сообщение через 5 секунд
  taskManager.addTask([]() {
    Serial.println("System started successfully!");
  }, TASK_ONCE, 5000, "start_message");
}

void loop() {
  taskManager.update();
  delay(10);
}
