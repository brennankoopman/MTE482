#include <Arduino.h>

//#include <Wire.h>

// Use this include because the author of the library was dumb
// and put C++ code in TaskScheduler.h so we get duplicated symbols
#include <TaskSchedulerDeclarations.h>

Scheduler taskManager;

// Times in milliseconds
/*
Task t_readSensors(50UL, TASK_FOREVER, &init_sensors, &taskManager, true);
Task t_missionControl(50UL, TASK_FOREVER, &MissionControl::init, &taskManager, true);
Task t_motorControl(10UL, TASK_FOREVER, &MotorControl::init_motor_control, &taskManager, true);
Task t_localize(50UL, TASK_FOREVER, &ObjectDetection::localize, &taskManager, true);
Task t_processSensors(50UL, TASK_FOREVER, &init_process_sensors, &taskManager, true);
*/

void setup() {
    Serial.begin(115200);
}

void loop() {
    taskManager.execute();
}