#pragma once
#include <Arduino.h>
#include <vector>
#include <functional>

enum TaskType {
  TASK_ONCE,     // Выполнить один раз
  TASK_PERIODIC  // Выполнять периодически
};

struct Task {
  uint32_t id;
  std::function<void()> callback;
  TaskType type;
  unsigned long interval;
  unsigned long lastExecution;
  bool active;
  String name;
};

class TaskManager {
private:
  std::vector<Task> tasks;
  uint32_t nextId = 1;

public:
  TaskManager();

  // Добавление задачи
  uint32_t addTask(std::function<void()> callback, TaskType type, unsigned long interval, String name = "");

  // Удаление задачи
  bool removeTask(uint32_t id);
  bool removeTaskByName(String name);
  void removeAllTasks();
  void removeInactiveTasks();

  // Обновление менеджера (должен вызываться в loop())
  void update();

  // Проверка существования задачи
  bool taskExists(uint32_t id);
  bool taskExistsByName(String name);

  // Получение информации о задачах
  size_t getTaskCount();
  size_t getActiveTaskCount();
  void listTasks();

  // Управление задачами
  bool pauseTask(uint32_t id);
  bool pauseTaskByName(String name);
  bool resumeTask(uint32_t id);
  bool resumeTaskByName(String name);
  bool setTaskInterval(uint32_t id, unsigned long newInterval);
  bool setTaskIntervalByName(String name, unsigned long newInterval);

  // Поиск задачи
  uint32_t findTaskIdByName(String name);
};