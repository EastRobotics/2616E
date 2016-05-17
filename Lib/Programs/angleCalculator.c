
//This is to take in the joystick measurements
//And convert them into angle measures with
//trigonometry.

float sidesToAngle(int sideOne, int sideTwo){
	//when using with a controller, sideOne should be the X axis
	//and side two should be the y axis

	//if the value is 0,0, meaning joystick not moved,
	//return -1 to avoid errors, and allow caller to handle accordingly
	if(sideOne==0&&sideTwo==0){
		return -1;
	}
	
	//Handle angles that fall on axis, to again avoid errors later
	if(sideOne==0||sideTwo==0){
		if(sideOne==0){
			if(sideTwo>0){
				return 90;
			} else {
				return 270;
			}
		} else {
			if(sideOne>0){
				return 0;
			} else {
				return 180;
			}
		}
	}
	
	float angle = abs(atan(sideOne/sideTwo));
	angle = ((angle*180)/3.14);
	//Determine the quadrant of the angle, because of atan's range restrictions
	int quadrant;
	if((sideOne*sideTwo)<0){
		if(sideOne>0){
			quadrant = 4;
			} else {
			quadrant = 2;
		}
		} else if(sideOne<0){
		quadrant = 3;
		} else {
		quadrant =1;
	}
	/*Use the quadrant of the angle added to the angle, depending on if the x or y
	is greater.  If the x is greater, it adds the angle to the last cardinal direction
	degree that came before.  If the y is greater it subtracts from the next cardinal
	direction degree that is coming.  This allows us to have angles in more than just
	the first and second quadrants (which is atan's range).
	*/
	if(abs(sideOne)>=abs(sideTwo)){
		angle+=((quadrant-1)*90);
		} else {
		angle = (quadrant*90)-angle;
	}
	return angle;
}

task main()
{
	// Stress test the program for check issues. Run every possible calculation
	for (int x=-127; x < 128; x++) {
		for (int y=-127; y < 128; y++) {
			sidesToAngle(x, y); // TODO Run checks with the number to make sure output is valid
		}
	}

	while(true){
		//clear the LCD
		clearLCDLine(0);
		clearLCDLine(1);
		//Print out the angle of the right joystick to an LCD
		displayLCDNumber(0,0,sidesToAngle(vexRT[Ch1],vexRT[Ch2]));
		wait1Msec(1);
	}

}
