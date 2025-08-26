#include <Arduino.h>
#include "TaskManager.h"

class MyDevice {
private:
  TaskManager& tm;
  int counter = 0;
  
public:
  MyDevice(TaskManager& taskManager) : tm(taskManager) {}
  
  void begin() {
    tm.addTask(
      [this]() { this->update(); },
      TASK_PERIODIC, 1000, "device_update"
    );
  }
  
  void update() {
    counter++;
    Serial.printf("Counter: %d\n", counter);
    
    if (counter >= 10) {
      tm.removeTaskByName("device_update");
      Serial.println("Update task removed");
    }
  }
};

TaskManager taskManager;
MyDevice device(taskManager);

void setup() {
  Serial.begin(115200);
  device.begin();
}

void loop() {
  taskManager.update();
  delay(10);
}
