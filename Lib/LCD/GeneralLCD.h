// Title: General LCD Handler
// Status: Unstable! Untested.
// Version: 1.0.0
// Description:
//  - A simple way to interact with LCDs cleanly and effectively

// Uses globals:
//  - Timer T4 for timings

// Usage:
//  - Call lcdInit() once in your main class

// TODO:
//  - Make general methods for g

// ---------------------------------------Notes-------------------------------------------

// Make sure your LCD is pragma'd in your main! It needs to be done manually. Example:
// ..?

// To change what a button combination does, see lcdButtonTask

// Nothing in here is set in stone. The infastructure for button presses, autorefreshing, and
// button holding can be completely overwritten to your liking. Although the other methods can
// be overwritting, it's suggested to not as this is currently the cleanest known way of doing
// things.

// If you're not using button hold down times, it's suggested you disable the line starting it's
// task within lcdInit()

// ----------------------------------------------------------------------------------------

// Page vars
int currentPage = 1; // Can be whatever page you want within your set
int minPage = 1; // Should be the first page number in your set
int maxPage = 2; // Should be the last page number in your set
bool cycles = true; // Make pages a continuous loop (loops to start when at the end)

// Data vars (Millis to 1milli accuracy)
long lastRefresh = 0.0; // Last time we've refreshed in millis
long refreshTime = 1500.0; // How many millis to wait before refreshing. Increments of 100ms minimum.
long holdStarted = 0.0; // Last time we've started a hold in millis
long lastHoldTime = -1.0; // How long we held a button last time we did.
                          // NOTE: When interacting with this, make sure to check userInteraction
                          //      to insure that you're not doing redundant tasks

// Example page vars
int menuMode = 1;
int menuMin = 1;
int menuMax = 3;

// Clears the pages and redraws
// Set userInteraction to wether or not a user caused this to refresh.
// --> Example, true = user pressed button, false = autoRefresh
void lcdUpdatePage( bool userInteraction )
{
	clearLCDLine(0);
	clearLCDLine(1);

	// Pages are defined under here. Messy, but it works.

	// Page 1
	if (currentPage == 1) {
		// TODO Draw the page
		displayLCDCenteredString(0,"You are on page one");
	}

	// Page 2
	else if (currentPage == 2) {
		// TODO Draw the page
		displayLCDCenteredString(0,"You are on page two");
	}

	// Page 3 (Example menu)
	else if (currentPage == 3) {
		// TODO Draw the page
		displayLCDCenteredString(0,"You are on page three (Menu)");
		if (menuMode == 1) {
			displayLCDCenteredString(1,"< Menu opt. 1 >");
			} else if (menuMode == 2) {
			displayLCDCenteredString(1,"< Menu opt. 2 >");
			} else if (menuMode == 3) {
			displayLCDCenteredString(1,"< Menu opt. 3 >");
			} else {
			displayLCDCenteredString(1,"! Menu opt. unknwn");
		}
	}

	// And pages carry on... make sure to increment maxPage for your page count!
}

// Returns the lcd to it's first page and redraws
// Triggered by middle button
void lcdHome()
{
	currentPage = minPage;
	lcdUpdatePage();
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
	lcdUpdatePage();
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
	lcdUpdatePage();
}

// - lcdNext() and lcdBack() are called when an in-page action is clicked, for example
//   using for right or left on a menu.
// - lcdNext() is fired by middle+right, and lcdBack() is fired by middle+left.
// - This is not required for every page, only those wanting to handle these actions.
// - You should NOT be making changes to the lcd here. You should be updating a variable then
//   calling lcdUpdatePage(), where the variable is then handled.

void lcdNext() {
	// Example menu for page 3
	if (currentPage == 3) {
		if (menuMode > menuMin) {
			menuMode -= 1;
			} else {
			menuMode = menuMax;
		}
		lcdUpdatePage();
	}
}

void lcdBack() {
	// Example menu for page 3
	if (currentPage == 3) {
		if (menuMode < menuMax) {
			menuMode += 1;
			} else {
			menuMode = menuMin;
		}
		lcdUpdatePage();
	}
}

// Reset the last time we've refreshed to now
void lcdResetAutoRefresh() {
	lastRefresh = time1[T4];
}

// Task to update the page if we have no activity in refreshTime millis
task lcdAutoRefreshTask() {
	lastHoldTime = -1.0;
	lcdUpdatePage(); // Insure a page is already drawn
	while (true)
	{
		wait1Msec(100);
		// If it's been refreshTime since last update
		if (time1[T4] - lastRefresh >= refreshTime) {
			lastHoldTime = -1.0;
			lcdUpdatePage();
			lcdResetAutoRefresh();
		}
	}
}

// Sets the start of a button hold
void lcdStartHold() {
	holdStarted = time1[T4];
}

// Calculates how long it's been since we've started holding a button
// Returns how long it's been
// NOTE: When interacting with this, make sure to check userInteraction
//      to insure that you're not doing redundant tasks
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
	while(true){
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

// Sets everything up how it should be, don't be afraid to change to preference
// Any initial setup and readings should be done here before autoRefreshTask is
// This should be called in your main file
void lcdInit() {
	bLCDBacklight = true; // False for green backlight to be off
	clearTimer(T4); // Reset our timer
	StartTask( lcdAutoRefreshTask ); // Initialize autorefreshing [Delete line to disable/use your own]
	StartTask( lcdButtonTask ); // Initialize button checking [Delete line to disable/use your own]
}
