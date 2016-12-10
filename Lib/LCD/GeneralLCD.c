// Page vars
int homePage = 1; // The page to go to when we hit the 'home' button
int currentPage = homePage; // Can be whatever page you want within your set
int minPage = 1; // Should be the first page number in your set
int maxPage = 3; // Should be the last page number in your set
bool cycles = true; // Make pages a continuous loop (loops to start when at the end)

// Data vars (Millis to 1milli accuracy)
long lastRefresh = 0.0; // Last time we've refreshed in millis
long refreshTime = 200.0; // How many millis to wait before refreshing. Increments of 100ms minimum.
long holdStarted = 0.0; // Last time we've started a hold in millis
long lastHoldTime = -1.0; // How long we held a button last time we did.
// NOTE:
//	When interacting with lastHoldtime value in a page update, make sure to check that userInteraction
//	 -> is true, meaning a user caused the action. You know a user caused it when a button pressed.
//   -> You shouldn't use button hold times anywhere else, because the value will not always be recent.

// Example page vars
int menuMode = 1; //menu page
int menuMin = 1; //menu minimum page
int menuMax = 3; //menu maximum page

int linePosition = 0; //slider position
int linePosMin = 0; //slider minimum position
int linePosMax = 13; //slider maximum position

// Clears an entire LCD (Vex, y u no have)
void clearLCD() {
	clearLCDLine(0);
	clearLCDLine(1);
}

// Clears the pages and redraws
// NOTE:
// 	I find it's useful to get a piece of graph paper to figure out the page layout.
//   -> You can sketch out a 16x2 box and get a better idea of your limits
// PARAMETERS:
//	bool: Wether or not a user caused this update
void lcdUpdatePage( bool userInteraction )
{
	clearLCD();

	// Pages are defined under here. Messy, but it works.

	// Page 1 [The first page]
	if (currentPage == 1) {
    displayLCDCenteredString(0,"This is page one");
	}

	// Page 2 [The second page]
	else if (currentPage == 2) {
		displayLCDCenteredString(0,"This is page two");
	}

  //Page 3 [Menu page]
  else if(currentPage == 3) {
    displayLCDCenteredString(0,"Page three (Menu)");
    switch(menuMode) {
      case 1 :
        displayLCDCenteredString(1,"< Menu opt. 1 >");
        break;
      case 2 :
        displayLCDCenteredString(1,"< Menu opt. 2 >");
        break;
      case 3 :
        displayLCDCenteredString(1,"< Menu opt. 3 >");
        break;
      default :
        displayLCDCenteredString(1,"! Menu opt. unknwn")
    }
  }

	//Page 4 [Slider example/test]
	else if (currentPage == 4) {
  	string title = "";
  	sprintf(title,"SliderExample %c",0xF6);
  	displayLCDString(0,0,title);

  	displayLCDString(1,1,"--------------");
  	displayLCDChar(1,linePosition+1,'O');
  	return;
	}


	// And pages carry on... make sure to increment maxPage for your page count!
}

// Returns the lcd to it's first page and redraws
// Triggered by middle button
void lcdHome()
{
	currentPage = homePage;
	lcdUpdatePage(true);
}

void lcdSetPage(int page) {
	// TODO Checks and stuff
	currentPage = page;
}

// Handles a request to go to the next page.
//  -> If there is one, it goes to it
//  -> If there isn't one and cycle is enable, it goes to first page
// Triggered by right button
void lcdNextPage()
{
	if (currentPage < maxPage) { // Theres another page to go to
		currentPage += 1;
		} else if (cycles) { // Go to the start of the loop if we cycle
		currentPage = minPage;
	}
	lcdUpdatePage(true);
}

// Handles a request to go to the previous page.
//  -> If there is one, it goes to it
//  -> If there isn't one and cycle is enable, it goes to last page
// Triggered by left button
void lcdLastPage()
{
	if (currentPage > minPage) { // Theres another page to go back to
		currentPage -= 1;
		} else if (cycles) { // Go to the end of the loop if we cycle
		currentPage = maxPage;
	}
	lcdUpdatePage(true);
}

// - lcdNext() and lcdBack() are called when an in-page action is clicked, for example
//   using for right or left on a menu.
// - lcdNext() is fired by middle+right, and lcdBack() is fired by middle+left.
// - This is not required for every page, only those wanting to handle these actions.
// - You should NOT be making changes to the lcd here. You should be updating a variable then
//   calling lcdUpdatePage(), where the variable is then handled.

// View above
void lcdBack() {
	// Page 1 [The first page]
	if (currentPage == 1) {
		lcdUpdatePage(true);
	}

	// Page 2 [The second page]
	else if (currentPage == 2) {
		lcdUpdatePage(true);
	}

	// Page 3 [Menu page]
	else if (currentPage == 3) {
    if (menuMode < menuMax) {
			menuMode += 1;
			} else {
			menuMode = menuMin;
		}
		lcdUpdatePage(true);
	}

	//Page 4 [Slider example/test]
	else if (currentPage == 4) {
  	if (linePosition < linePosMax) {
    	linePosition += 1;
    	lcdUpdatePage(true);
  	}
	}
}

// View above
void lcdNext() {
	// Page 1 [The first page]
	if (currentPage == 1) {
		lcdUpdatePage(true);
	}

	// Page 2 [The second page]
	else if (currentPage == 2) {
		lcdUpdatePage(true);
	}

	// Page 3 [Menu page]
	else if (currentPage == 3) {
    if (menuMode > menuMin) {
			menuMode -= 1;
		} else {
			menuMode = menuMax;
		}
		lcdUpdatePage(true);
	}

	//Page 4 [Slider example/test]
	else if (currentPage == 4) {
  	if (linePosition > linePosMin) {
    	linePosition -= 1;
    	lcdUpdatePage(true);
  	}
	}

}

// Reset the last time we've refreshed to the current time
void lcdResetAutoRefresh() {
	lastRefresh = time1[T4];
}

// Task to update the page if we have had no activity in refreshTime milliseconds
task lcdAutoRefreshTask() {
	lastHoldTime = -1.0;
	lcdUpdatePage(false); // Insure a page is already drawn
	while (true)
	{
		wait1Msec(10); // You only need to change this with a refresh rate lower than 10ms
		// If it's been refreshTime since last update
		if (time1[T4] - lastRefresh >= refreshTime) {
			lastHoldTime = -1.0;
			lcdUpdatePage(false);
			lcdResetAutoRefresh();
		}
	}
}

// Starts tracking the time of the press
void lcdStartHold() {
	holdStarted = time1[T4];
}

// Calculates how long it's been since we've started holding a button
// NOTE:
//	When interacting with lastHoldtime value in a page update, make sure to check that userInteraction
//	 -> is true, meaning a user caused the action. You know a user caused it when a button pressed.
//   -> You shouldn't use button hold times anywhere else, because the value will not always be recent.
//  *You should never call this function* unless making a custom button integration
//   -> Use the variable lastHoldTime in your action handling/page updating
// RETURNS:
//	long: How long the button was held for
long lcdEndHold() {
	lastHoldTime = time1[T4] - holdStarted;
	return lastHoldTime;
}

// This is where all of the default button handles are done, however you can use lcdNext
//  -> and other methods anywhere. For example, from buttons on the controller.
// This implementation is really great for making sure we read the correct button presses,
//  -> because we're reading so fast and not every release happens perfectly.
task lcdButtonTask() {
	bool buttonReleased = true;
	int highestCombination = 0;
	while (true) {
		if (nLCDButtons == 0) { // A button wasn't pressed
			if (buttonReleased == false) { // Button was pressed then released. Let's handle presses
				lcdEndHold(); // Update our last held count
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
					lcdNext();
				}
				else
					if (highestCombination == 6) { // Right & center pressed
					lcdBack();
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
				lcdStartHold(); // Start a button hold down
			}

			if (nLCDButtons > highestCombination) {
				highestCombination = nLCDButtons;
			}
		}
	}
}

// Sets everything up how it should be.
void lcdInit() {
	bLCDBacklight = true; // Turn our backlight on
	clearTimer(T4); // Reset our timer
	startTask( lcdAutoRefreshTask ); // Initialize autorefreshing
	startTask( lcdButtonTask ); // Initialize button checking
}
