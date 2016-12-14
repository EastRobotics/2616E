#include "main.h"

typedef void (*updateLCDFunction)(bool, int);
typedef void (*menuPressMethod)(int);

// TODO: Add back in hold time detection
// TODO: Rewrite to be much cleaner and easier to use

// Custom LCD methods
updateLCDFunction lcdUpdatePage;
menuPressMethod menuNext;
menuPressMethod menuBack;

// Page vars
int homePage; // The page to go to when we hit the 'home' button
int currentPage; // Can be whatever page you want within your set
int minPage; // Should be the first page number in your set
int maxPage; // Should be the last page number in your set
bool cycles = true; // Make pages a continuous loop (loops to start when at the end)
unsigned long refreshTimeMillis = 250;

void setCycles(bool _cycles) {
  cycles = _cycles;
}

void setRefreshTime(unsigned long _refreshTimeMillis) {
  refreshTimeMillis = _refreshTimeMillis;
}

void lcdPrintTitle(const char * title) {
  lcdPrint(uart2, 1, "%s%c",title, 0xF6);
}

// Returns the lcd to it's first page and redraws
// Triggered by middle button
void lcdHome() {
	currentPage = homePage;
	lcdUpdatePage(true, currentPage);
}

void lcdSetPage(int page) {
	// TODO Checks and stuff
	currentPage = page;
}

// Handles a request to go to the next page.
//  -> If there is one, it goes to it
//  -> If there isn't one and cycle is enable, it goes to first page
// Triggered by right button
void lcdNextPage() {
	if (currentPage < maxPage) { // Theres another page to go to
		currentPage += 1;
		} else if (cycles) { // Go to the start of the loop if we cycle
		currentPage = minPage;
	}
	lcdUpdatePage(true, currentPage);
}

// Handles a request to go to the previous page.
//  -> If there is one, it goes to it
//  -> If there isn't one and cycle is enable, it goes to last page
// Triggered by left button
void lcdLastPage() {
	if (currentPage > minPage) { // Theres another page to go back to
		currentPage -= 1;
		} else if (cycles) { // Go to the end of the loop if we cycle
		currentPage = maxPage;
	}
	lcdUpdatePage(true, currentPage);
}

int lastRefresh;

// Reset the last time we've refreshed to the current time
void lcdResetAutoRefresh() {
	lastRefresh = millis();
}

// Task to update the page if we have had no activity in refreshTime milliseconds
void lcdAutoRefresh(void * param) {
	//lastHoldTime = -1.0;
	lcdUpdatePage(false, currentPage); // Ensure a page is already drawn
	while (true)
	{
			//lastHoldTime = -1.0;
      taskDelay(20);
      if (millis() - lastRefresh >= refreshTimeMillis) {
			     lcdUpdatePage(false, currentPage);
        lcdResetAutoRefresh();
      }
	}
}

void lcdManager(void * param) {
	bool buttonReleased = true;
	uint8_t highestCombination = 0;
	while (true) {
		if (lcdReadButtons(uart2) == 0) { // A button wasn't pressed
			if (buttonReleased == false) { // Button was pressed then released. Let's handle presses
				//lcdEndHold(); // Update our last held count
				if (highestCombination == 1) { // Left button pressed
					lcdLastPage();
					buttonReleased = false;
				}
				else
					if (highestCombination == 2) { // Center button pressed
					lcdHome();
					buttonReleased = false;
				}
				else
					if (highestCombination == 4) { // Right button pressed
					lcdNextPage();
					buttonReleased = false;
				}
				else
					if (highestCombination == 3) { // Left & center pressed
            if (menuNext != NULL)
					menuNext(currentPage);
				}
				else
					if (highestCombination == 6) { // Right & center pressed
            if (menuBack != NULL)
          menuBack(currentPage);
				}
				buttonReleased = true;
				highestCombination = 0;
			} // Else: Nothing was pressed yet
		}
		else // A button was pressed but we haven't released
		{
			lcdResetAutoRefresh(); // Reset our countdown for resfreshing
			if (buttonReleased) { // If this is our first button pressed
				buttonReleased = false; // Tell us we have a button pushed down
				//lcdStartHold(); // Start a button hold down
			}

			if (lcdReadButtons(uart2) > highestCombination) {
				highestCombination = lcdReadButtons(uart2);
			}
		}
	}
}

void lcdStartMenu() {
  print("Starting the LCD menu\n");
  taskCreate(lcdAutoRefresh, TASK_DEFAULT_STACK_SIZE,
     NULL, TASK_PRIORITY_DEFAULT); // Start auto refresher
   //taskCreate(lcdManager, TASK_DEFAULT_STACK_SIZE,
     //NULL, TASK_PRIORITY_DEFAULT); // Start the given manager
}

void lcdInitMenu(int _minPage, int _maxPage, int _homePage) {
  lcdClear(uart2); // Make sure the LCD is initialized
  lcdInit(uart2);
  lcdSetBacklight(uart2, true);

  lcdSetText(uart2,1,"LCD Init Done");
  // Set all of the local variables to their respective values
  homePage = _homePage;
  currentPage = homePage;
  minPage = _minPage;
  maxPage = _maxPage;
}

void lcdSetUpdater(updateLCDFunction _lcdUpdatePage) {
  lcdUpdatePage = _lcdUpdatePage;
}

void lcdSetMenuNext(menuPressMethod _menuNext) {
  menuNext = _menuNext;
}

void lcdSetMenuBack(menuPressMethod _menuBack) {
  menuBack = _menuBack;
}
