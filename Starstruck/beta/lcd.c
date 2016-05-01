// Page vars
int homePage = 4; // The page to go to when we hit the 'home' button
int currentPage = homePage; // Can be whatever page you want within your set
int minPage = 1; // Should be the first page number in your set
int maxPage = 7; // Should be the last page number in your set
bool cycles = false; // Make pages a continuous loop (loops to start when at the end)

// Data vars (Millis to 1milli accuracy)
long lastRefresh = 0.0; // Last time we've refreshed in millis
long refreshTime = 1500.0; // How many millis to wait before refreshing. Increments of 100ms minimum.
long holdStarted = 0.0; // Last time we've started a hold in millis
long lastHoldTime = -1.0; // How long we held a button last time we did.
// NOTE:
//	When interacting with lastHoldtime value in a page update, make sure to check that userInteraction
//	 -> is true, meaning a user caused the action. You know a user caused it when a button pressed.
//   -> You shouldn't use button hold times anywhere else, because the value will not always be recent.

// Example page vars
int menuMode = 1;
int menuMin = 1;
int menuMax = 3;
int linePosition = 0;
int linePosMin = 0;
int linePosMax = 13;

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

	// Page 1 [Auton mode selector]
	if (currentPage == 1) {
		string title = "";
		sprintf(title,"AutonMode%c",0xF6);
		displayLCDString(0,0,title);

		return;
	}

	// Page 2 [Auton position selector]
	if (currentPage == 2) {
		string title = "";
		sprintf(title,"StartPos%c",0xF6);
		displayLCDString(0,0,title);

		return;
	}

	// Page 3 [Auton color selector]
	if (currentPage == 3) {
		string title = "";
		sprintf(title,"TeamColor%c",0xF6);
		displayLCDString(0,0,title);

		return;
	}

	// Page 4 [HOME]
	if (currentPage == 4) {
		displayLCDCenteredString(0,"_Essential Evil_"); // TODO Make a cooler home screen ;)

		// Draw out our battery status
		int highestBatStatus = 0;
		if (getMainBatteryStatus() > highestBatStatus)
			highestBatStatus = getMainBatteryStatus();
		if (getBackupBatteryStatus() > highestBatStatus)
			highestBatStatus = getBackupBatteryStatus();
		if (getExpanderBatteryStatus() > highestBatStatus)
			highestBatStatus = getExpanderBatteryStatus();
		displayLCDString(1,1,"Bat");
		// TODO Find better chars for these
		if (highestBatStatus == 1)
			displayLCDChar(1,0,'-');
		if (highestBatStatus == 2)
			displayLCDChar(1,0,'!');
		return;
	}

	// Page 5 [Battery values]
	else if (currentPage == 5) {
		string title = "";
		sprintf(title,"BatStat%c",0xF6);
		displayLCDString(0,0,title);

		displayLCDCenteredString(1,"TODO: Disply Bat");
		return;
	}

	// Page 6 [Slider example/test]
	else if (currentPage == 6) {
		string title = "";
		sprintf(title,"SliderExample%c",0xF6);
		displayLCDString(0,0,title);

		displayLCDString(1,1,"--------------");
		displayLCDChar(1,linePosition+1,'O');
		return;
	}

	// Page 7 [Menu example]
	else if (currentPage == 7) {
		// TODO Draw the page
		string title = "";
		sprintf(title,"MenuExample%c",0xF6);

		displayLCDString(0,0,title);
		string toDisplay = "";
		if (menuMode == 1) {
			sprintf(toDisplay, "%c Menu opt. 1 %c", 0xBC, 0xBB);
			} else if (menuMode == 2) {
			sprintf(toDisplay, "%c Menu opt. 2 %c", 0xBC, 0xBB);
			} else if (menuMode == 3) {
			sprintf(toDisplay, "%c Menu opt. 3 %c", 0xBC, 0xBB);
			} else {
			sprintf(toDisplay, "%c Menu opt. 4 %c", 0xBC, 0xBB);
		}
		displayLCDCenteredString(1,toDisplay);
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
void lcdNext() {
	// Auton mode selector for page 1
	if (currentPage == 1) {
		int autonMode = getAutonMode();
		int autonMin = getAutonModeMin();
		int autonMax = getAutonModeMax();
		if (autonMode < autonMax) {
			setAutonMode(autonMode + 1);
			} else {
			setAutonMode(autonMin);
		}
		lcdUpdatePage(true);
		return;
	}

	// Auton position selector for page 1
	if (currentPage == 1) {
		setAutonPosition(!getAutonPosition()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Auton color selector for page 1
	if (currentPage == 1) {
		setAutonColor(!getAutonColor()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Example slider for page 6
	if (currentPage == 6) {
		if (linePosition < linePosMax) {
			linePosition += 1;
			lcdUpdatePage(true);
		}
		return;
	}

	// Example menu for page 7
	if (currentPage == 7) {
		if (menuMode < menuMax) {
			menuMode += 1;
			} else {
			menuMode = menuMin;
		}
		lcdUpdatePage(true);
		return;
	}
}

// View above
void lcdBack() {
	// Auton mode selector for page 1
	if (currentPage == 1) {
		int autonMode = getAutonMode();
		int autonMin = getAutonModeMin();
		int autonMax = getAutonModeMax();
		if (autonMode > autonMin) {
			setAutonMode(autonMode - 1);
			} else {
			setAutonMode(autonMax);
		}
		lcdUpdatePage(true);
		return;
	}

	// Auton position selector for page 1
	if (currentPage == 1) {
		setAutonPosition(!getAutonPosition()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Auton color selector for page 1
	if (currentPage == 1) {
		setAutonColor(!getAutonColor()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Example slider for page 6
	if (currentPage == 6) {
		if (linePosition > linePosMin) {
			linePosition -= 1;
			lcdUpdatePage(true);
		}
		return;
	}

	// Example menu for page 7
	if (currentPage == 7) {
		if (menuMode > menuMin) {
			menuMode -= 1;
			} else {
			menuMode = menuMax;
		}
		lcdUpdatePage(true);
		return;
	}
}

// Reset the last time we've refreshed to now
void lcdResetAutoRefresh() {
	lastRefresh = time1[T4];
}

// Task to update the page if we have no activity in refreshTime millis
task lcdAutoRefreshTask() {
	lastHoldTime = -1.0;
	lcdUpdatePage(false); // Insure a page is already drawn
	while (true)
	{
		wait1Msec(100);
		// If it's been refreshTime since last update
		if (time1[T4] - lastRefresh >= refreshTime) {
			lastHoldTime = -1.0;
			lcdUpdatePage(false);
			lcdResetAutoRefresh();
		}
	}
}

// Sets the start of a button hold
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
//   and other methods anywhere. For example, from buttons on the controller.
// This implementation is really great for making sure we read the correct button presses,
//   because we're reading so fast and not every release happens perfectly.
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
