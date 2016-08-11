#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    powerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    gyroMain,       sensorGyro)
#pragma config(Sensor, in3,    SongDial,       sensorPotentiometer)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           intakeL,       tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           driveFR,       tmotorVex393_MC29, PIDControl, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port3,           driveBR,       tmotorVex393_MC29, PIDControl, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port4,           launcherRO,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           launcherRI,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           launcherLI,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           launcherLO,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveFL,       tmotorVex393_MC29, PIDControl, reversed, driveRight, encoderPort, I2C_3)
#pragma config(Motor,  port9,           driveBL,       tmotorVex393_MC29, PIDControl, reversed, driveRight, encoderPort, I2C_4)
#pragma config(Motor,  port10,          intakeR,       tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c" // Competition includes [DO NOT TOUCH]
#include "sensors.c" // Sensor handling and math methods
#include "drive.c" // Handles pid, joystick input, etc
#include "auton.c" // Autonomous controller
#include "lcd.c" // LCD methods

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 1
#define VERSION_TYPE "ALPHA"

int song[165][3] = {{1568,192,1764.704},{1568,36,2095.586},{1568,36,2426.468},{1568,24,2647.056},{1568,1,2656.2471666666665},{1568,35,2977.938},{1568,36,3308.82},{2093,24,3529.4080000000004},{2093,36,3860.2900000000004},{2093,36,4191.1720000000005},{1865,24,4411.76},{1865,36,4742.642},{1865,36,5073.523999999999},{1397,24,5294.111999999999},
	{1568,36,5624.993999999999},{1568,36,5955.875999999998},{1568,24,6176.463999999998},{1568,36,6507.345999999998},{1568,36,6838.227999999997},{2093,24,7058.815999999997},{1568,36,7389.697999999997},{1568,36,7720.579999999996},{1568,24,7941.167999999996},{1568,36,8272.049999999996},{1568,36,8602.931999999995},{2093,24,8823.519999999995},{1568,36,9154.401999999995},{1568,36,9485.283999999994},{1568,24,9705.871999999994},{1568,36,10036.753999999994},
	{1568,36,10367.635999999993},{2093,24,10588.223999999993},{2093,36,10919.105999999992},{2093,36,11249.987999999992},{1865,24,11470.575999999992},{1865,36,11801.457999999991},{1865,36,12132.339999999991},{1397,24,12352.92799999999},{1568,36,12683.80999999999},{1568,36,13014.69199999999},{1568,24,13235.27999999999},{1568,36,13566.16199999999},{1568,36,13897.043999999989},{2093,24,14117.631999999989},{1568,36,14448.513999999988},{1568,36,14779.395999999988},
	{1568,24,14999.983999999988},{1568,36,15330.865999999987},{1568,36,15661.747999999987},{1568,24,15882.335999999987},{1568,12,15992.629999999986},{1568,12,16102.923999999986},{1568,12,16212.29888333332},{1568,0,16213.217999999986},{1568,12,16323.511999999986},{1568,24,16543.18088333332},{1568,0,16544.099999999988},{1568,24,16764.687999999987},{1568,36,17095.56999999999},{1568,1,17104.761166666656},{1568,35,17426.45199999999},{2093,24,17647.03999999999},
	{2093,1,17656.231166666657},{2093,35,17977.92199999999},{2093,36,18308.803999999993},{1865,24,18529.391999999993},{1865,36,18860.273999999994},{1865,36,19191.155999999995},{1568,24,19411.743999999995},{1568,12,19522.037999999997},{1568,12,19632.332},{1568,12,19742.626},{1568,12,19852.920000000002},{1568,24,20073.508},{1568,24,20294.096},{1568,36,20624.978000000003},{1568,36,20955.860000000004},{2093,24,21176.448000000004},
	{1568,36,21507.330000000005},{1568,36,21838.212000000007},{1568,24,22058.800000000007},{1568,36,22389.682000000008},{1568,36,22720.56400000001},{1568,24,22941.15200000001},{1568,12,23051.44600000001},{1568,12,23161.740000000013},{1568,12,23272.034000000014},{1568,12,23382.328000000016},{1568,24,23602.916000000016},{1568,22,23802.36431666668},{1568,2,23823.504000000015},{1568,12,23933.798000000017},{1568,1,23942.070050000017},{1568,11,24044.092000000015},
	{1568,12,24154.386000000017},{1568,12,24264.68000000002},{1568,24,24485.26800000002},{1568,24,24705.856000000018},{1568,12,24816.15000000002},{1568,12,24926.44400000002},{1568,12,25036.738000000023},{1568,12,25147.032000000025},{1568,24,25367.620000000024},{1568,24,25588.208000000024},{1568,12,25698.502000000026},{1568,12,25808.796000000028},{1568,12,25919.09000000003},{1568,12,26029.38400000003},{1568,24,26249.97200000003},{1568,24,26470.56000000003},
	{1568,12,26580.854000000032},{1568,12,26691.148000000034},{1568,12,26801.442000000035},{1568,12,26911.736000000037},{1568,12,27022.03000000004},{1568,12,27132.32400000004},{1568,12,27242.618000000042},{1568,12,27352.912000000044},{1568,12,27463.206000000046},{1568,12,27573.500000000047},{1568,12,27683.79400000005},{1568,12,27794.08800000005},{1568,12,27904.382000000052},{1568,12,28014.676000000054},{1568,12,28124.970000000056},{1568,12,28235.264000000057},
	{1568,12,28345.55800000006},{1568,12,28455.85200000006},{1568,12,28566.146000000062},{1568,12,28676.440000000064},{1568,12,28786.734000000066},{1568,12,28897.028000000068},{1568,12,29007.32200000007},{1568,12,29117.61600000007},{1568,96,29999.96800000007},{1568,12,30110.26200000007},{1568,12,30220.556000000073},{1568,12,30330.850000000075},{1568,12,30441.144000000077},{1568,24,30661.732000000076},{1568,12,30772.026000000078},{1568,12,30882.32000000008},
	{1568,12,30992.61400000008},{1568,12,31102.908000000083},{1568,12,31213.202000000085},{1568,12,31323.496000000086},{1568,24,31544.084000000086},{2093,12,31654.378000000088},{2093,12,31764.67200000009},{2093,12,31874.96600000009},{2093,12,31985.260000000093},{2093,12,32095.554000000095},{2093,12,32205.848000000096},{2093,24,32426.436000000096},{1865,12,32536.730000000098},{1865,12,32647.0240000001},{1865,12,32757.3180000001},{1865,12,32867.6120000001},
	{1865,12,32977.906000000105},{1865,12,33088.200000000106},{1865,24,33308.78800000011},{1397,12,33419.08200000011},{1397,12,33529.37600000011},{1568,12,33639.670000000115},{1568,12,33749.964000000116}};

const int songLength = 165;

float songSpeed = 1.0;

const int noteThreshold = 5;

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 2616E Starstruck
//
// Welcome to the 2616E Starstruck program.
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Pinmap: https://drive.google.com/open?id=1mSrSJ1BPUzoXAdo2x8ED-OwT-OZY7r5Y9P7FmuNDcLY
// Controller map: // TODO Add link
// Trello: https://trello.com/b/oXrKvfA7/2616e-starstruck
//
// - Ask Cameron if you need/want permission to any of the above sites
// - Please keep these up-to-date when you do any pins or buttons.
// - Make sure you notify builders if you change ports up, and drivers if buttons!
// - The maps above [hopefully] will hopefully be printed on every release version for reference
//   -> and stored in the team boxes.
//
/////////////////////////////////////////////////////////////////////////////////////////
// Timer distribution:
// - T1: Unassigned
// - T2: Unassigned
// - T3: Game time tracking
// - T4: LCD Hanlding
/////////////////////////////////////////////////////////////////////////////////////////
// Global TODO:
// - Replace data/unchanging variables to defitions
// - Comment bare code
// - Remove unused testing code
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			  Pre-Autonomous
//
// Gets rid of useless errors, does general setup.
//
/////////////////////////////////////////////////////////////////////////////////////////

//Process the song and shorten all repetitive notes, to make them more distinct
void processSong()
{
	//Loop through all but the last note in the song
	for (int i = 0; i < (songLength-1); i ++) {
		//If the notes are about the same
		if(abs(song[i][0] - song[i+1][0])<=5){
			//Take 2 milliseconds off of the note
			song[i][1]-=4;
			//If the note is really short
			if(song[i][1]<5){
				song[i][1]+=4;
			}
		}
	}
}

void pre_auton()
{
	bStopTasksBetweenModes = true; // Tasks stop when we need them to. Not doing this can be dangerous.
	bLCDBacklight = true; // Turn our LCD backlight on

	//Reset the motor encoders
	clearDriveEncoders();

	//Setup song
	processSong();

	// Never passing if statement. Lets us get rid of compile warnings so we can focus on the ones we need to see.
	if (false) {
		UserControlCodePlaceholderForTesting();
		AutonomousCodePlaceholderForTesting();
		sensorsErrorEscape(); // Get rid of unused methods for the sensor lib
		// Feel free to add currently un-used methods/tasks here if you're sure they are meant to not be used.
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous
//
// Runs the auton selector in auton.c
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Autonomous control task
*/
task autonomous()
{
	setupGameTimer(false); // sensors.c
	runAuton(); // auton.c
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control
//
// Handle controls from the user, run the LCD control, and generally allow user control of
// the robot.
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Test song :D
// A task to get past robotc's limitation of 10 notes
// in the tone queue, and play a song from an array of values
*/
task playSong(){
	while(true){
		int currentNote = 0;
		clearTimer(T1);
		while(currentNote<songLength){
			if(bSoundQueueAvailable){
				// writeDebugStreamLine("[%d][%d]",time1[T1],(song[currentNote][2])/10);
				if(abs(time1[T1]-(song[currentNote][2]/songSpeed))<noteThreshold){
					// writeDebugStreamLine("((%d))",song[currentNote][1]);
					playTone(song[currentNote][0],(song[currentNote][1]/songSpeed));
					currentNote++;
					} else if(time1[T1]>round((song[currentNote][2])/songSpeed)){
					currentNote++;
				}
			}
			wait1Msec(1);
		}
	}
}

//TODO: REMOVE LATER
int testing = 1;

/*
// Driver control task
*/
task usercontrol()
{
	setupGameTimer(true); // sensors.c

	// Inits for normal games and skills. See below for specifics
	lcdInit(); // Starts all tasks for handling the lcd. Check lcd.c
	int lastForwardSpeed, lastTurnSpeed, lastStrafeSpeed = 0;
	string potentiometerValDebug = "";
	float lastDirection = 0.0;
	startTask ( playSong );

	bool isSkills = false; // TODO: Set this somehow (LCD?)
	if (isSkills) {
		writeDebugStreamLine("Running Driver Control [Skills]...");
		} else {
		writeDebugStreamLine("Running Driver Control [Normal]...");
	}

	while (true)
	{
		//////////////////////////////
		// Controller handling start
		//////////////////////////////
		// Test auton
		if (vexRT[Btn7U] && vexRT[Btn7D] && vexRT[Btn7L] && vexRT[Btn7R]) {
			// Auton tester
			setAutonMode(2);
			runAuton();
			// End auton tester
		}

		// Drive train (forward speed, turn speed, strafe speed)(forward multiplier, turn multiplier, stafe multiplier
		if(!vexRT[Btn6D]) { // Normal drive... Bottom left bumper
			lastForwardSpeed = vexRT[Ch3];
			lastTurnSpeed = vexRT[Ch1];
			lastStrafeSpeed = vexRT[Ch4];
			lastDirection = gyroToFloat(SensorValue[gyroMain]);
			if (vexRT[Btn6U])
				driveWithLogic(lastForwardSpeed, lastTurnSpeed, lastStrafeSpeed, 0.25, 0.25, 0.25);
			else
				driveWithLogic(lastForwardSpeed, lastTurnSpeed, lastStrafeSpeed);
			} else { // Time for some CRS (Crazy rotational shit)
			/* TODO
			/* Here's where'd you use michael's program to find the degree target.
			/* We'd then find some way to mess with the degrees put into CRS to also turn towards that number
			/* This is why it's called CRS. It's crazy.
			/* TODO End */
			// driveWithCRS(lastForwardSpeed, lastStrafeSpeed, lastDirection, gyroToFloat(SensorValue[gyroMain]));
			driveTank(vexRT[Ch3],vexRT[Ch2]);
		}

		// TODO: Add encoders to keep the sides synchronized
		int intakeSpeed = 60;
		if (vexRT[Btn5U]) { // Upper right bumper
			motor[intakeL] = -1*intakeSpeed;
			motor[intakeR] = intakeSpeed;
			} else if (vexRT[Btn5D]) { // Lower right bumper
			motor[intakeL] = intakeSpeed;
			motor[intakeR] = -1*intakeSpeed;
			} else {
			motor[intakeL] = 0;
			motor[intakeR] = 0;
		}

		//Launcher code
		if(vexRT[Btn8R]) {
			motor[launcherRI] = motor[launcherRO] = motor[launcherLI] = motor[launcherLO] = 127;
		} else if(vexRT[Btn8L]){
			motor[launcherRI] = motor[launcherRO] = motor[launcherLI] = motor[launcherLO] = -127;
		} else {
			motor[launcherRI] = motor[launcherRO] = motor[launcherLI] = motor[launcherLO] = 0;
		}

		//TODO: REMOVE LATER
		if(vexRT[Btn8U]) {
			if(testing==1){
				setupMotorTicks(driveFL,1000);
				startTask( driveMotorToTargetPID, kHighPriority );
				testing = 0;
			}
		}
		endTimeSlice();
		//////////////////////////////
		// Controller handling end
		//////////////////////////////
	}
}
