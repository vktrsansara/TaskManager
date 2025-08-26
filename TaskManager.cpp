#include "TaskManager.h"
#include <algorithm>

TaskManager::TaskManager() {
  tasks.reserve(10);
}

uint32_t TaskManager::addTask(std::function<void()> callback, TaskType type, unsigned long interval, String name) {
  Task newTask;
  newTask.id = nextId++;
  newTask.callback = callback;
  newTask.type = type;
  newTask.interval = interval;
  newTask.lastExecution = millis();
  newTask.active = true;
  newTask.name = name;
  
  tasks.push_back(newTask);
  return newTask.id;
}

bool TaskManager::removeTask(uint32_t id) {
  for (auto it = tasks.begin(); it != tasks.end(); ++it) {
    if (it->id == id) {
      tasks.erase(it);
      return true;
    }
  }
  return false;
}

bool TaskManager::removeTaskByName(String name) {
  bool removed = false;
  for (auto it = tasks.begin(); it != tasks.end(); ) {
    if (it->name == name) {
      it = tasks.erase(it);
      removed = true;
    } else {
      ++it;
    }
  }
  return removed;
}

void TaskManager::removeAllTasks() {
  tasks.clear();
}

void TaskManager::removeInactiveTasks() {
  tasks.erase(
    std::remove_if(tasks.begin(), tasks.end(), 
      [](const Task& t) { return !t.active; }),
    tasks.end()
  );
}

void TaskManager::update() {
  unsigned long currentTime = millis();
  
  for (auto& task : tasks) {
    if (!task.active) continue;
    
    if (currentTime - task.lastExecution >= task.interval) {
      task.callback();
      task.lastExecution = currentTime;
      
      if (task.type == TASK_ONCE) {
        task.active = false;
      }
    }
  }
  removeInactiveTasks();
}

bool TaskManager::taskExists(uint32_t id) {
  for (const auto& task : tasks) {
    if (task.id == id) {
      return true;
    }
  }
  return false;
}

bool TaskManager::taskExistsByName(String name) {
  for (const auto& task : tasks) {
    if (task.name == name) {
      return true;
    }
  }
  return false;
}

size_t TaskManager::getTaskCount() {
  return tasks.size();
}

size_t TaskManager::getActiveTaskCount() {
  size_t count = 0;
  for (const auto& task : tasks) {
    if (task.active) {
      count++;
    }
  }
  return count;
}

void TaskManager::listTasks() {
  Serial.println("=== Task List ===");
  for (const auto& task : tasks) {
    Serial.printf("ID: %u, Name: %s, Type: %s, Interval: %lu ms, Active: %s\n",
                 task.id,
                 task.name.c_str(),
                 task.type == TASK_PERIODIC ? "Periodic" : "Once",
                 task.interval,
                 task.active ? "Yes" : "No");
  }
  Serial.printf("Total: %zu tasks\n", tasks.size());
}

bool TaskManager::pauseTask(uint32_t id) {
  for (auto& task : tasks) {
    if (task.id == id) {
      task.active = false;
      return true;
    }
  }
  return false;
}

bool TaskManager::pauseTaskByName(String name) {
  bool paused = false;
  for (auto& task : tasks) {
    if (task.name == name) {
      task.active = false;
      paused = true;
    }
  }
  return paused;
}

bool TaskManager::resumeTask(uint32_t id) {
  for (auto& task : tasks) {
    if (task.id == id) {
      task.active = true;
      task.lastExecution = millis();
      return true;
    }
  }
  return false;
}

bool TaskManager::resumeTaskByName(String name) {
  bool resumed = false;
  unsigned long currentTime = millis();
  for (auto& task : tasks) {
    if (task.name == name) {
      task.active = true;
      task.lastExecution = currentTime;
      resumed = true;
    }
  }
  return resumed;
}

bool TaskManager::setTaskInterval(uint32_t id, unsigned long newInterval) {
  for (auto& task : tasks) {
    if (task.id == id) {
      task.interval = newInterval;
      return true;
    }
  }
  return false;
}

bool TaskManager::setTaskIntervalByName(String name, unsigned long newInterval) {
  bool updated = false;
  for (auto& task : tasks) {
    if (task.name == name) {
      task.interval = newInterval;
      updated = true;
    }
  }
  return updated;
}

uint32_t TaskManager::findTaskIdByName(String name) {
  for (const auto& task : tasks) {
    if (task.name == name) {
      return task.id;
    }
  }
  return 0;
}