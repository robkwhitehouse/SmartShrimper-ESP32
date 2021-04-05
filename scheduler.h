/*
 * Simple RealTimeScheduler for Arduino
 * R K Whitehouse - Jan 2021
 */

#define MAX_TASKS 20

class Scheduler {

  private:
    byte index =  0;
    
    //List of tasks
    struct Task {
      void (*taskPointer)();
      boolean runnable = false;
      unsigned long period;//Num microseconds between executions
      unsigned long nextRun = 0;//time of next execution
    } taskList[MAX_TASKS];

  public:
    unsigned add(void (*tptr)(), unsigned long per) {
      if (index == MAX_TASKS) {
        Serial.println("Error: Too many tasks");
      } else {
        taskList[index].taskPointer = tptr;
        taskList[index].runnable = true;
        taskList[index].period = per;
        index++;
      }
      return index-1;
    }

    void begin() {
    }

    void run(unsigned long now) {
      Task* task;
      register byte i;

      for (i = 0; i < index; i++) {
        task = &taskList[i];
        if ( task->runnable && now >= task->nextRun ) {
          task->nextRun = now + task->period;
          task->taskPointer();//Run the task
        }
      }
    }
};
