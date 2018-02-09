#include "main.h"

#define ASYNC_ACTION_TYPES 0 // The amount of asyncronous action types you wish to run

asyncFunction actions[ASYNC_ACTION_TYPES]; // array of actions to perform at specified time
int runTime[ASYNC_ACTION_TYPES]; // array of scheduled times to run
bool actionRun[ASYNC_ACTION_TYPES]; // array indicating whether the action has already been run

int actionsCreated = 0;

void createAction(asyncFunction action) {
  if(actionsCreated < ASYNC_ACTION_TYPES)
    actions[actionsCreated++] = action;
  else {
    printf("ERROR IN autoasync.c!!! TRIED TO CREATE MORE ACTIONS THAN ");
    printf("PERMITTED BY DEFINE 'ASYNC_ACTION_TYPES'\n");
  }
}

void scheduleAction(int actionNum, int timeDelay) {
  runTime[actionNum] = millis() + timeDelay;
  actionRun[actionNum] = false;
}

void runAsync(void* param) {

}
