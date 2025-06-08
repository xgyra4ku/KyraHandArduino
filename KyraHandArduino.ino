// ==================== НАСТРОЙКИ =================== //
#define AMOUNT 5  // Количество сервоприводов

// Пины сервоприводов
const int servoPins[AMOUNT] = {2, 3, 4, 5, 6};

// Библиотека для работы с сервоприводами и EEPROM
#include <Servo.h>
#include <EEPROM.h>

Servo servos[AMOUNT];  // Массив объектов сервоприводов

// Минимальные и максимальные углы для каждого сервопривода
int pos_min[AMOUNT] = {0, 0, 0, 0, 0};  // Минимальные углы
int pos_max[AMOUNT] = {180, 180, 180, 180, 180};  // Максимальные углы

// Текущие позиции сервоприводов
int currentPos[AMOUNT] = {90, 90, 90, 90, 90};

// Адреса в EEPROM для хранения данных
const int EEPROM_MIN_START = 0;  // Начальный адрес для минимальных значений
const int EEPROM_MAX_START = AMOUNT * sizeof(int);  // Начальный адрес для максимальных значений

void setup() {
  Serial.begin(9600);

  // Подключаем сервоприводы к пинам
  for (int i = 0; i < AMOUNT; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(currentPos[i]);  // Устанавливаем сервоприводы в начальное положение
  }

  // Загружаем настройки из EEPROM
  loadSettingsFromEEPROM();
  Serial.println("Система готова. Введите команду:");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Читаем команду до символа новой строки
    command.trim();  // Удаляем лишние пробелы

    if (command == "cal") {
      calibration();
    } else if (command.startsWith("min")) {
      setMin(command);
    } else if (command.startsWith("max")) {
      setMax(command);
    } else if (command.startsWith("amin")) {
      setaMin();
    } else if (command.startsWith("fack")) {
      fack();
    } else if (command.startsWith("amax")) {
      setaMax();
    } else if (command.startsWith("move")) {
      moveServo(command);
    } else if (command == "save") {
      saveSettingsToEEPROM();
      Serial.println("Настройки сохранены в EEPROM.");
    } else if (command == "load") {
      loadSettingsFromEEPROM();
      Serial.println("Настройки загружены из EEPROM.");
    } else {
      Serial.println("Неизвестная команда");
    }
  }
}

// Функция для калибровки сервоприводов
void calibration() {
  Serial.println("Начинаем калибровку. Введите номер сервопривода (0-4):");
  while (Serial.available() == 0) {}  // Ждем ввода
  int servo_num = Serial.parseInt();

  if (servo_num < 0 || servo_num >= AMOUNT) {
    Serial.println("Неверный номер сервопривода");
    return;
  }

  Serial.print("Калибровка сервопривода ");
  Serial.println(servo_num);

  while (true) {
    Serial.println("Введите '+', '-' для изменения угла, 'smin'/'smax' для сохранения, 'stop' для завершения.");
    while (Serial.available() == 0) {}  // Ждем ввода
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "+") {
      currentPos[servo_num] += 5;
      if (currentPos[servo_num] > 180) currentPos[servo_num] = 180;
      servos[servo_num].write(currentPos[servo_num]);
      Serial.print("Текущий угол: ");
      Serial.println(currentPos[servo_num]);
    } else if (input == "-") {
      currentPos[servo_num] -= 5;
      if (currentPos[servo_num] < 0) currentPos[servo_num] = 0;
      servos[servo_num].write(currentPos[servo_num]);
      Serial.print("Текущий угол: ");
      Serial.println(currentPos[servo_num]);
    } else if (input == "smin") {
      pos_min[servo_num] = currentPos[servo_num];
      Serial.print("Минимальный угол установлен: ");
      Serial.println(pos_min[servo_num]);
    } else if (input == "smax") {
      pos_max[servo_num] = currentPos[servo_num];
      Serial.print("Максимальный угол установлен: ");
      Serial.println(pos_max[servo_num]);
    } else if (input == "stop") {
      Serial.println("Калибровка завершена.");
      break;
    } else {
      Serial.println("Неизвестная команда");
    }
  }
}

// Сохранение настроек в EEPROM
void saveSettingsToEEPROM() {
  for (int i = 0; i < AMOUNT; i++) {
    EEPROM.put(EEPROM_MIN_START + i * sizeof(int), pos_min[i]);
    EEPROM.put(EEPROM_MAX_START + i * sizeof(int), pos_max[i]);
  }
  //EEPROM.commit();  // Фиксируем изменения (если используется ESP32)
}

// Загрузка настроек из EEPROM
void loadSettingsFromEEPROM() {
  for (int i = 0; i < AMOUNT; i++) {
    EEPROM.get(EEPROM_MIN_START + i * sizeof(int), pos_min[i]);
    EEPROM.get(EEPROM_MAX_START + i * sizeof(int), pos_max[i]);

    // Проверяем корректность загруженных данных
    if (pos_min[i] < 0 || pos_min[i] > 180) pos_min[i] = 0;
    if (pos_max[i] < 0 || pos_max[i] > 180) pos_max[i] = 180;

    Serial.print("Загружены настройки для сервопривода ");
    Serial.print(i);
    Serial.print(": min=");
    Serial.print(pos_min[i]);
    Serial.print(", max=");
    Serial.println(pos_max[i]);
  }
}
void setaMax(){
  setMin("min0");
  delay(100);
  setMin("min1");
  delay(100);
  setMin("min2");
  delay(100);
  setMin("min3");
  delay(100);
  setMin("min4");
  delay(10);
}
void setaMin(){
  setMax("max0");
  delay(100);
  setMax("max1");
  delay(100);
  setMax("max2");
  delay(100);
  setMax("max3");
  delay(100);
  setMax("max4");
}
void fack(){
  setMin("min0");
  delay(100);
  setMax("max1");
  delay(100);
  setMin("min2");
  delay(100);
  setMin("min3");
  delay(100);
  setMin("min4");
  delay(10);
}

// Установка минимального угла
void setMin(String command) {
  int servo_num = command.substring(3).toInt();
  if (servo_num < 0 || servo_num >= AMOUNT) {
    Serial.println("Неверный номер сервопривода");
    return;
  }
  servos[servo_num].write(pos_min[servo_num]);
  Serial.print("Установлен минимальный угол для сервопривода ");
  Serial.println(servo_num);
}

// Установка максимального угла
void setMax(String command) {
  int servo_num = command.substring(3).toInt();
  if (servo_num < 0 || servo_num >= AMOUNT) {
    Serial.println("Неверный номер сервопривода");
    return;
  }
  servos[servo_num].write(pos_max[servo_num]);
  Serial.print("Установлен максимальный угол для сервопривода ");
  Serial.println(servo_num);
}

// Перемещение сервопривода в указанное положение
void moveServo(String command) {
  int servo_num = command.substring(4).toInt();
  if (servo_num < 0 || servo_num >= AMOUNT) {
    Serial.println("Неверный номер сервопривода");
    return;
  }

  Serial.print("Введите угол для сервопривода ");
  Serial.println(servo_num);
  while (Serial.available() == 0) {}  // Ждем ввода
  int angle = Serial.parseInt();

  if (angle < 0 || angle > 180) {
    Serial.println("Угол должен быть в диапазоне 0-180");
    return;
  }

  servos[servo_num].write(angle);
  currentPos[servo_num] = angle;
  Serial.print("Сервопривод ");
  Serial.print(servo_num);
  Serial.print(" перемещен в угол ");
  Serial.println(angle);
}