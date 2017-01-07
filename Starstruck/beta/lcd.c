// Page vars
int homePage = 4; // The page to go to when we hit the 'home' button
int currentPage = homePage; // Can be whatever page you want within your set
int minPage = 1; // Should be the first page number in your set
int maxPage = 9; // Should be the last page number in your set
bool cycles = false; // Make pages a continuous loop (loops to start when at the end)

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
/* For the slider example
int linePosition = 0;
int linePosMin = 0;
int linePosMax = 13;
*/

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
		string toDisplay = "";
		int autonMode = getAutonMode();
		switch(autonMode) {
		case 1 :
			sprintf(toDisplay, "%c Do Nothing %c", 0xBC, 0xBB);
			break;
		case 2 :
			sprintf(toDisplay, "%c Max Points %c", 0xBC, 0xBB);
			break;
		case 3 :
			sprintf(toDisplay, "%c Shoot & Sit %c", 0xBC, 0xBB);
			break;
		case 4 :
			sprintf(toDisplay, "%c Skills %c", 0xBC, 0xBB);
			break;
		default :
			sprintf(toDisplay, "%c Mode ?: %i %c", 0xBC, autonMode, 0xBB);
		}
		displayLCDCenteredString(1,toDisplay);
		return;
	}

	// Page 2 [Auton position selector]
	if (currentPage == 2) {
		string title = "";
		sprintf(title,"StartPos%c",0xF6);
		displayLCDString(0,0,title);
		string toDisplay = "";
		if (getAutonPosition()) { // Right
			sprintf(toDisplay, "%c Right %c", 0xBC, 0xBB);
			} else { // Left
			sprintf(toDisplay, "%c Left %c", 0xBC, 0xBB);
		}
		displayLCDCenteredString(1,toDisplay);
		return;
	}

	// Page 3 [Auton color selector]
	if (currentPage == 3) {
		string title = "";
		sprintf(title,"TeamColor%c",0xF6);
		displayLCDString(0,0,title);
		string toDisplay = "";
		if (getAutonColor()) { // Blue
			sprintf(toDisplay, "%c Blue %c", 0xBC, 0xBB);
			} else { // Red
			sprintf(toDisplay, "%c Red %c", 0xBC, 0xBB);
		}
		displayLCDCenteredString(1,toDisplay);
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
		string backupvolt = "";
		sprintf(backupvolt,"%0.2fv",getBackupBatteryVoltage());
		displayLCDChar(0,9,'!');
		displayLCDString(0,10,backupvolt);
		string mainvolt = "";
		sprintf(mainvolt,"%0.2fv",getMainBatteryVoltage());
		displayLCDChar(1,0,'!');
		displayLCDString(1,1,mainvolt);
		return;
	}

	/*
	[Slider example/test]
	else if (currentPage == 0) {
	string title = "";
	sprintf(title,"SliderExample %c",0xF6);
	displayLCDString(0,0,title);

	displayLCDString(1,1,"--------------");
	displayLCDChar(1,linePosition+1,'O');
	return;
	}
	*/
	// Page 6 [Game Time]
	else if (currentPage == 6){
		string title = "";
		sprintf(title,"GameTime%c",0xF6);
		displayLCDString(0,0,title);
		displayLCDNumber(1,0,getGameTime());
		displayLCDNumber(1,8,getGameTimeRunning());
	}

	// Page 7 [Launcher arm position]
	else if (currentPage == 7){
		string title = "";
		sprintf(title,"LaunchPot%c",0xF6);
		displayLCDString(0,0,title);
		displayLCDNumber(1,0,SensorValue[potLauncher]);
	}

	// Page 8 [BR Encoder position]
	else if (currentPage == 8) {
		string title = "";
		sprintf(title,"DriveEnc%c",0xF6);
		displayLCDString(0,0,title);
		displayLCDNumber(1,0,nMotorEncoder[driveBR]);
	}

	// Page 9 [Gyroscope value]
	else if (currentPage == 9) {
		string title = "";
		sprintf(title,"Gyro%c", 0xF6);
		displayLCDString(0,0,title);
		displayLCDNumber(1,0,SensorValue[gyroMain]);
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
	// Page 1 [Auton mode selector]
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

	// Page 2 [Auton position selector]
	if (currentPage == 2) {
		setAutonPosition(!getAutonPosition()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Page 3 [Auton color selector]
	if (currentPage == 3) {
		setAutonColor(!getAutonColor()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	/* [Slider example/test]
	if (currentPage == 0) {
	if (linePosition < linePosMax) {
	linePosition += 1;
	lcdUpdatePage(true);
	}
	return;
	}
	*/
}

// View above
void lcdNext() {
	// Page 1 [Auton mode selector]
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

	// Page 2 [Auton position selector]
	if (currentPage == 2) {
		setAutonPosition(!getAutonPosition()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	// Page 3 [Auton color selector]
	if (currentPage == 3) {
		setAutonColor(!getAutonColor()); // Only ever true/false
		lcdUpdatePage(true);
		return;
	}

	/* [Slider example/test]
	if (currentPage == 0) {
	if (linePosition > linePosMin) {
	linePosition -= 1;
	lcdUpdatePage(true);
	}
	return;
	}
	*/
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
