# TaskManager для ESP8266

Легковесный менеджер задач для ESP8266, позволяющий планировать выполнение функций по времени с использованием стандартных контейнеров C++.

## Оглавление

- [Возможности](#возможности)
- [Быстрый старт](#быстрый-старт)
- [Структуры данных](#структуры-данных)
- [API Reference](#api-reference)
- [Примеры использования](#примеры-использования)
- [Рекомендации](#рекомендации)
- [Ограничения](#ограничения)

## Возможности

- ✅ Создание периодических и одноразовых задач
- ✅ Управление задачами по ID или имени
- ✅ Автоматическое удаление выполненных задач
- ✅ Пауза и возобновление выполнения
- ✅ Динамическое изменение интервалов
- ✅ Мониторинг состояния задач
- ✅ Поддержка лямбда-функций и методов класса
- ✅ Thread-safe дизайн (для однопоточного ESP8266)

## Быстрый старт

```cpp
#include <Arduino.h>
#include "TaskManager.h"

TaskManager taskManager;

void setup() {
  Serial.begin(115200);
  
  // Мигание LED каждые 500ms
  taskManager.addTask([]() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.println("LED toggled");
  }, TASK_PERIODIC, 500, "blink");
  
  // Одноразовое сообщение через 5 секунд
  taskManager.addTask([]() {
    Serial.println("System started successfully!");
  }, TASK_ONCE, 5000, "start_message");
}

void loop() {
  taskManager.update(); // Обязательный вызов!
  delay(10);
}
```

## Структуры данных

### TaskType
```cpp
enum TaskType {
  TASK_ONCE,     // Задача выполняется один раз
  TASK_PERIODIC  // Задача выполняется периодически
};
```

### Task
```cpp
struct Task {
  uint32_t id;                 // Уникальный идентификатор
  std::function<void()> callback; // Функция для выполнения
  TaskType type;               // Тип задачи
  unsigned long interval;      // Интервал в миллисекундах
  unsigned long lastExecution; // Время последнего выполнения
  bool active;                 // Флаг активности
  String name;                 // Имя для удобства
};
```

## API Reference

### Конструктор
```cpp
TaskManager();
```
Создает экземпляр менеджера с предварительным резервированием памяти.

### Добавление задач

#### addTask()
```cpp
uint32_t addTask(std::function<void()> callback, TaskType type, 
                unsigned long interval, String name = "");
```
Добавляет новую задачу.

**Параметры:**
- `callback` - функция для выполнения
- `type` - `TASK_ONCE` или `TASK_PERIODIC`
- `interval` - интервал в миллисекундах
- `name` - опциональное имя задачи

**Возвращает:** ID созданной задачи

### Удаление задач

#### removeTask()
```cpp
bool removeTask(uint32_t id);
```
Удаляет задачу по ID.

#### removeTaskByName()
```cpp
bool removeTaskByName(String name);
```
Удаляет все задачи с указанным именем.

#### removeAllTasks()
```cpp
void removeAllTasks();
```
Удаляет все задачи.

#### removeInactiveTasks()
```cpp
void removeInactiveTasks();
```
Удаляет все неактивные задачи.

### Управление выполнением

#### update()
```cpp
void update();
```
**Обязательный метод!** Должен вызываться в `loop()`.

### Управление состоянием

#### pauseTask()
```cpp
bool pauseTask(uint32_t id);
```
Приостанавливает задачу по ID.

#### resumeTask()
```cpp
bool resumeTask(uint32_t id);
```
Возобновляет задачу по ID.

#### setTaskInterval()
```cpp
bool setTaskInterval(uint32_t id, unsigned long newInterval);
```
Изменяет интервал выполнения задачи.

### Информационные методы

#### taskExists()
```cpp
bool taskExists(uint32_t id);
```
Проверяет существование задачи.

#### getTaskCount()
```cpp
size_t getTaskCount();
```
Возвращает общее количество задач.

#### getActiveTaskCount()
```cpp
size_t getActiveTaskCount();
```
Возвращает количество активных задач.

#### listTasks()
```cpp
void listTasks();
```
Выводит информацию о задачах в Serial.

## Примеры использования

### Управление умным домом
```cpp
taskManager.addTask([]() {
  // Чтение датчиков
  int light = analogRead(A0);
  bool motion = digitalRead(D2);
  
  // Логика управления
  if (motion && light < 500) {
    digitalWrite(LED_PIN, HIGH);
  }
}, TASK_PERIODIC, 300, "smart_lighting");
```

### Отправка данных на сервер
```cpp
taskManager.addTask([]() {
  if (WiFi.status() == WL_CONNECTED) {
    String data = "temp=" + String(readTemperature());
    httpPost("http://api.example.com/data", data);
  }
}, TASK_PERIODIC, 30000, "data_upload");
```

### Планировщик событий
```cpp
// Утренние задачи
taskManager.addTask([]() {
  digitalWrite(CURTAIN_PIN, HIGH);
  Serial.println("Curtains opened");
}, TASK_PERIODIC, 3600000, "morning_routine"); // Каждый час

// Вечерние задачи
taskManager.addTask([]() {
  digitalWrite(LIGHTS_PIN, LOW);
  Serial.println("Lights dimmed");
}, TASK_ONCE, 64800000, "evening_routine"); // Через 18 часов
```

## Рекомендации

### Производительность
- Используйте интервалы ≥ 10ms для критичных задач
- Избегайте длительных операций в callback-функциях
- Удаляйте неиспользуемые задачи

### Память
- Библиотека резервирует память для 10 задач
- Одноразовые задачи автоматически удаляются
- Используйте имена только при необходимости

### Отладка
```cpp
void loop() {
  taskManager.update();
  
  // Отладочная информация каждые 5 секунд
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 5000) {
    lastDebug = millis();
    taskManager.listTasks();
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  }
}
```

## Ограничения

- Максимальный интервал: ~49 дней (ограничение `millis()`)
- Не предназначен для real-time задач
- Однопоточное выполнение (ESP8266 single core)
