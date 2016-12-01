#define ALLEGRO_USE_CONSOLE
#include <math.h>
#include <allegro.h>
#include <iostream>
#include <time.h>

#define PI 3.14159265
BITMAP* buffer;

#pragma region Function Prototypes

void ResetAllValues(double const yHeight, double &xRand, int &numTimer, double &introTimer, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double angPointA[], double angPointB[], double angPointC[], double projPointA[], int &calcStage, int &menuOption, int &cursorX, int &cursorY, double &firingAng, double &angToRad, double &lengthAB, double &lengthAC, double &lengthBC, double &velocity, double &xVelocity, double &yVelocity, bool &incrV, double &collLineA, double &collLineB, double &denominator, bool &p1Turn, bool &p1Wins, bool &p2Wins, double &t, double &windForce, int &windChange);			//Reset the values of the game.
void ResetSomeValues(int &calcStage, double &velocity, double &firingAng);
void MenuSelection(int &menuOption);
void DrawMainMenu(const int scrx, const int scry);
void DrawHelpMenu(const int scrx, const int scry);
void MenuReturn(int &menuOption);
void DrawIntroScreen(const int scrx, const int scry);
void DrawAvatars(double p1PointB[], double p1PointC[], double p1PointD[], double p2PointB[], double p2PointC[], double p2PointD[]);
void DrawGUI(bool p1Turn, double &windForce);
void DrawAngle(double angPointA[], double angPointB[], double angPointC[]);
void DrawVBar(double velocity, double xVelocity, double yVelocity, double maxV, double vBarPointA[], double vBarPointB[]);
void DrawProjectile(double projPointA[], double projPointB[]);
void DrawVictoryScreen(const int scrx, const int scry, bool p1Wins, bool p2Wins);


void LocationGenerator(double &xRand, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double projPointA[], int &numTimer);	//Randomly generate a starting position for both the player and the enemy.
void CalcPVelocity(int &calcStage, double const minV, double const maxV, bool &incrV, double &velocity, double &xVelocity, double &yVelocity, double &firingAng, double &angToRad);		//Calculate the Players Firing Velocity.
void CalcPAngle(int &calcStage, int cursorX, int cursorY, double &firingAng, double &angToRad, double p1PointA[], double p2PointA[], double angPointA[], double angPointB[], double angPointC[], double projPointA[], double projPointB[], double &lengthAB, double &lengthAC, double &lengthBC, bool p1Turn);		//Calculate the Players Firing Angle.
void CalcPProjectile(int &calcStage, double &velocity, double &xVelocity, double &yVelocity, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double projPointA[], double projPointB[], double const gravAccel, double &windForce, int &windChange, double firingAng, double &t, double &dt, double collLineA, double collLineB, double denominator, bool &p1Turn, bool &p1Wins, bool &p2Wins);			//Calculate the Players Firing Trajectory.
void CalcProjCollision(double lineA1[], double lineA2[], double lineB1[], double lineB2[], double collLineA, double collLineB, double denominator, bool &pTurn, bool &p1Wins, bool &p2Wins);
#pragma endregion

#pragma region Update Screen
//Function that takes what has been drawn on the Buffer and places it on the screen.
void UpdateScreen(void)
{
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}
#pragma endregion

#pragma region Initialise Double Buffering
int InitDoubleBuffering()
{
	//Create the memory buffer
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	show_mouse(screen);

	if(!buffer)
	{
		return FALSE;
	}
	return TRUE;
}
#pragma endregion

int main()
{
	#pragma region Initialisation
		#pragma region Initialise

	//Initialisation for Allegro.
	allegro_init();
	if(allegro_init() != 0)
	{
		allegro_message("Cannot initialise allegro.\n");
		return -1;
	}

	set_window_title("Artillery");
	set_color_depth(desktop_color_depth());
	install_timer();
	install_mouse();
	install_keyboard();

	const int scrx = 800;
	const int scry = 640;

	if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, scrx, scry, 0, 0))
	{
		allegro_message("Video Error: %s. \n", allegro_error);
		return 1;
	}
	if(!InitDoubleBuffering())
	{
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Unable to initiliase page flipping. \n");
		return 1;
	}
	#pragma endregion

		#pragma region Game Variables

	//Initialisation for all game variables.
	srand(time(NULL));
	const double yHeight = 639.0;
	double xRand = 0.0;
	int numTimer = 100;
	double introTimer = 100.0;
	
	//Player 1s coordinates.
	double p1PointA[2], p1PointB[2], p1PointC[2], p1PointD[2];
		   p1PointA[0] = 0.0;		//Point A X1
		   p1PointA[1] = yHeight;	//Point A Y1
		   p1PointB[0] = 0.0;		//Point B X2
		   p1PointB[1] = 0.0;		//Point B Y2
		   p1PointC[0] = 0.0;
		   p1PointC[1] = 0.0;
		   p1PointD[0] = 0.0;
		   p1PointD[1] = 0.0;

	//Player 2s coordinates.
	double p2PointA[2], p2PointB[2], p2PointC[2], p2PointD[2];
		   p2PointA[0] = 0.0;		//Point A X1
		   p2PointA[1] = yHeight;		//Point A Y1
		   p2PointB[0] = 0.0;		//Point B X2
		   p2PointB[1] = 0.0;		//Point B Y2
		   p2PointC[0] = 0.0;
		   p2PointC[1] = 0.0;
		   p2PointD[0] = 0.0;
		   p2PointD[1] = 0.0;

	//Angle Arc coordinates
	double angPointA[2], angPointB[2], angPointC[2];
		   angPointA[0] = 0.0;		//Point A X1
		   angPointA[1] = 0.0;		//Point A Y1
		   angPointB[0] = 0.0;		//Point B X2
		   angPointB[1] = 0.0;		//Point B Y2
		   angPointC[0] = 0.0;		//Point C X3
		   angPointC[1] = 0.0;		//Point C Y3
	
		   //Projectile coordinates.
	double projPointA[2], projPointB[2];
		   projPointA[0] = 0.0;
		   projPointA[1] = yHeight;
		   projPointB[0] = 0.0;
		   projPointB[1] = yHeight;

		   //Velocity Power bar coordinates.
	double vBarPointA[2], vBarPointB[2];
		   vBarPointA[0] = 15.0;
		   vBarPointA[1] = 15.0;
		   vBarPointB[0] = 15.0;
		   vBarPointB[1] = 30.0;

	int calcStage = 0;
	int menuOption = 0;
	int cursorX = 0;
	int cursorY = 0;
	int posTracker = 0;
	//Firing Angle data.
	double firingAng = 0.0;			//Angle that the projectile will be fired at.
	double angToRad = 0.0;			//Converts the Firing Angle to Radians.
	double lengthAB = 0.0;
	double lengthAC = 0.0;
	double lengthBC = 0.0;

	//Firing Velocity data.
	const double gravAccel = 9.8;	//Used to store the Gravity to manipulate the Y value.
	double velocity = 0.0;			//Initial velocity that the projectile will be fired at.
	double xVelocity = 0.0;			//Velocity value along the X axis.
	double yVelocity = 0.0;			//Velocity value along the Y axis.
	bool incrV = true;
	double windForce = 0.0;
	int windChange = 100;
	const double minV = 0.1; 
	const double maxV = 100.0;

	double collLineA = 0.0;
	double collLineB = 0.0;
	double denominator = 0.0;

	double shotDistance = 0.0;		//Distance that the projectile will travel from the firing origin.
	double distance = 0.0;			//Distance between the players X origin and the projectiles landing X coordinate.
	double time = 0.0;				//Time that the projectile will be in the air.

	bool p1Turn = true;
	bool p1Wins = false;
	bool p2Wins = false;

	double t = 0.0;					//
	double dt = 1.0 / 60.0;	//Delta Time

	#pragma endregion
	#pragma endregion

	#pragma region Game Code

	//Main Game Loop
	while(!key[KEY_ESC])
	{
		#pragma region Game Logic Calculations

		//Game Logic
		t += dt;	//Delta Time

		//Prevents the use of Menu Controls when in-game or in the Help Menu.
		if(menuOption != 1 && menuOption != 2)
		{
			MenuSelection(menuOption);
		}

		//Allows the user to return to the main menu by pressing 0;
		if(key[KEY_0])
		{
			ResetAllValues(yHeight, xRand, numTimer, introTimer, p1PointA, p1PointB, p1PointC, p1PointD, p2PointA, p2PointB, p2PointC, p2PointD, angPointA, angPointB, angPointC, projPointA, calcStage, menuOption, cursorX, cursorY, firingAng, angToRad, lengthAB, lengthAC, lengthBC, velocity, xVelocity, yVelocity, incrV, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins, t, windForce, windChange);
		}
		
		//Runs all the game code if the user selected "Play" (Option 1) from the main menu.
		if(menuOption == 1)
		{
			#pragma region CalcStages Game Loop
			if(!p1Wins && !p2Wins)
			{
				//Calculation Stage 0 - Run a Timer to determine how long the Introduction text should be displayed.
				if(calcStage == 0)
				{
					LocationGenerator(xRand, p1PointA, p1PointB, p1PointC, p1PointD, p2PointA, p2PointB, p2PointC, p2PointD, projPointA, numTimer);
					introTimer -= (10 * dt);
					if(introTimer < 0)
					{
						calcStage = 1;
					}
				}
				
				//Calculation Stage 1 - Calculate the players Firing Angle.
				if(calcStage == 1)
				{
					CalcPAngle(calcStage, cursorX, cursorY, firingAng, angToRad, p1PointA, p2PointA, angPointA, angPointB, angPointC, projPointA, projPointB, lengthAB, lengthAC, lengthBC, p1Turn);
				}
				
				//Calculation Stage 2 - Calculate the players Velocity.
				if(calcStage == 2)
				{
					CalcPVelocity(calcStage, minV, maxV, incrV, velocity, xVelocity, yVelocity, firingAng, angToRad);
				}
				
				//Calculation Stage 3 - Calculate and fire the projectile along the trajectory.
				if(calcStage == 3)
				{
					CalcPProjectile(calcStage, velocity, xVelocity, yVelocity, p1PointA, p1PointB, p1PointC, p1PointD, p2PointA, p2PointB, p2PointC, p2PointD, projPointA, projPointB, gravAccel, windForce, windChange, firingAng, t, dt, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins);
				}
			}
		#pragma endregion
		}

		//Quits the game if the user selected "Quit" (Option 3) from the main menu.
		if(menuOption == 3)
		{
			return 0;
		}
			#pragma endregion

		#pragma region Draw To The Buffer

		//Draw to the Buffer/Screen
		clear(buffer);

		//Draw Main Menu if no option is selected (Default).
		if(menuOption == 0)
		{
			DrawMainMenu(scrx, scry);
		}

		//Draws to the Buffer/Screen when "Play" (Option 1) is selected from the main menu.
		if(menuOption == 1)
		{
			#pragma region CalcStages Buffer
			if(!p1Wins && !p2Wins)
			{
				//Game Play Drawing Code
				//DrawPlayers

				//Calculation Stage 0 - Draws the Introduction Screen to the Buffer/Screen.
				if(calcStage == 0)
				{
					DrawIntroScreen(scrx, scry);
				}
				if(calcStage != 0)
				{
					//Draw Avatars & GUI
					DrawAvatars(p1PointB, p1PointC, p1PointD, p2PointB, p2PointC, p2PointD);
					DrawGUI(p1Turn, windForce);
				}

				//Calculation Stage 1 - Draws the Firing Angle to the Buffer/Screen.
				if(calcStage == 1)
				{
					//Draw Angle Arc
					DrawAngle(angPointA, angPointB, angPointC);
				}

				//Calculation Stage 2 - Draws the Velocity Power Bar to the Buffer/Screen.
				if(calcStage == 2)
				{
					DrawVBar(velocity, xVelocity, yVelocity, maxV, vBarPointA, vBarPointB);
				}

				//Calculation Stage 3 - Draws the Velocity Power Bar and Projectile to the Buffer/Screen.
				if(calcStage == 3)
				{
					DrawVBar(velocity, xVelocity, yVelocity, maxV, vBarPointA, vBarPointB);
					DrawProjectile(projPointA, projPointB);
				}
			}
			DrawVictoryScreen(scrx, scry, p1Wins, p2Wins);
		#pragma endregion
		}

		//Draws the Help Menu to the Buffer/Screen when "Help" (Option 2) is selected from the main menu.
		if(menuOption == 2)
		{
			DrawHelpMenu(scrx, scry);
		}

		UpdateScreen();
		#pragma endregion
		
	}
	#pragma endregion
}

//Function to reset every value in the game. Called when the user exits back to the menu.
void ResetAllValues(double const yHeight, double &xRand, int &numTimer, double &introTimer, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double angPointA[], double angPointB[], double angPointC[], double projPointA[], int &calcStage, int &menuOption, int &cursorX, int &cursorY, double &firingAng, double &angToRad, double &lengthAB, double &lengthAC, double &lengthBC, double &velocity, double &xVelocity, double &yVelocity, bool &incrV, double &collLineA, double &collLineB, double &denominator, bool &p1Turn, bool &p1Wins, bool &p2Wins, double &t, double &windForce, int &windChange)
{
	xRand = 0.0;
	numTimer = 100;
	introTimer = 100.0;

	p1PointA[0] = 0.0;
	p1PointA[1] = yHeight;
	p1PointB[0] = p1PointA[0] - 10.0;
	p1PointB[1] = p1PointA[1];
	p1PointC[0] = p1PointA[0] + 10.0;
	p1PointC[1] = p1PointA[1];
	p1PointD[0] = p1PointA[0];
	p1PointD[1] = p1PointA[1] - 10.0;

	p2PointA[0] = 0.0;
	p2PointA[1] = yHeight;
	p2PointB[0] = p2PointA[0] - 10.0;
	p2PointB[1] = p2PointA[1];
	p2PointC[0] = p2PointA[0] + 10.0;
	p2PointC[1] = p2PointA[1];
	p2PointD[0] = p2PointA[0];
	p2PointD[1] = p2PointA[1] - 10.0;

	angPointA[2], angPointB[2], angPointC[2];
	angPointA[0] = 0.0;		//Point A X1
	angPointA[1] = 0.0;		//Point A Y1
	angPointB[0] = 0.0;		//Point B X2
	angPointB[1] = 0.0;		//Point B Y2
	angPointC[0] = 0.0;		//Point C X3
	angPointC[1] = 0.0;		//Point C Y3
	
	projPointA[2];
	projPointA[0] = 0.0;
	projPointA[1] = yHeight;

	calcStage = 0;
	menuOption = 0;
	cursorX = 0;
	cursorY = 0;
	
	firingAng = 0.0;
	angToRad = 0.0;
	lengthAB = 0.0;
	lengthAC = 0.0;
	lengthBC = 0.0;

	velocity = 0.0;
	xVelocity = 0.0;
	yVelocity = 0.0;
	incrV = true;
	windForce = 0.0;
	windChange = 100;

	collLineA = 0.0;
	collLineB = 0.0;
	denominator = 0.0;

	p1Turn = true;
	p1Wins = false;
	p2Wins = false;

	//t = 0.0;
}

//Function to reset some of the values in the game. Called when the players swap turns.
void ResetSomeValues(int &calcStage, double &velocity, double &firingAng)
{
	velocity = 0.0;
	firingAng = 0.0;
	calcStage = 1;
}

//Function to determine the menu option that the user selects.
void MenuSelection(int &menuOption)
{
	if(key[KEY_0])
	{
		menuOption = 0;
	}
	if(key[KEY_1])
	{
		menuOption = 1;
	}
	if(key[KEY_2])
	{
		menuOption = 2;
	}
	if(key[KEY_3])
	{
		menuOption = 3;
	}
}

//Function to draw the Main Menu.
void DrawMainMenu(const int scrx, const int scry)
{
	textout_ex(buffer, font, "ARTILLERY", scrx/2-50, scry/2-60, makecol(255,255,255), -1);
	textout_ex(buffer, font, "Press 1 to play!", scrx/2-75, scry/2, makecol(255,255,255), -1);
	textout_ex(buffer, font, "Press 2 for help!", scrx/2-78, scry/2+30, makecol(255,255,255), -1);
	textout_ex(buffer, font, "Press 3 to quit!", scrx/2-75, scry/2+60, makecol(255,255,255), -1);
}

//Function to draw the Help Menu.
void DrawHelpMenu(const int scrx, const int scry)
{
	textout_ex(buffer, font, "ARTILLERY", scrx/2-50, scry/2-60, makecol(255,255,255), -1);
	textout_ex(buffer, font, "Navigate the mouse on the screen to calculate your firing angle.", scrx/2-270, scry/2, makecol(126,126,0), -1);
	textout_ex(buffer, font, "Click the left-mouse button to confirm your firing angle.", scrx/2-245, scry/2+30, makecol(126,126,0), -1);
	textout_ex(buffer, font, "After confirming the angle, left-click again to confirm the power of your shot.", scrx/2-325, scry/2+60, makecol(0,126,126), -1);
	textout_ex(buffer, font, "This is indicated by the power-bar at the top-left of the screen.", scrx/2-270, scry/2+90, makecol(0,126,126), -1);
	textout_ex(buffer, font, "Press 0 to return to the main menu from here and in-game.", scrx/2-245, scry/2+120, makecol(255,255,255), -1);
}

//Function to determine when the user wants to exit to the main menu.
void MenuReturn(int &menuOption)
{
	if(key[KEY_0])
	{
		menuOption = 0;
	}
}

//Function to determine where the player(s) and/or enemy start.
void LocationGenerator(double &xRand, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double projPointA[], int &numTimer)
{
	if(numTimer > 0)
	{
		numTimer--;
		if(numTimer == 50)
		{
			xRand = rand()%399+10;
			p1PointA[0] = xRand;
			p1PointB[0] = p1PointA[0] - 10.0;
			p1PointB[1] = p1PointA[1];
			p1PointC[0] = p1PointA[0] + 10.0;
			p1PointC[1] = p1PointA[1];
			p1PointD[0] = p1PointA[0];
			p1PointD[1] = p1PointA[1] - 10.0;
		}

		if(numTimer == 1)
		{
			xRand = rand()%389+401;
			p2PointA[0] = xRand;
			p2PointB[0] = p2PointA[0] - 10.0;
			p2PointB[1] = p2PointA[1];
			p2PointC[0] = p2PointA[0] + 10.0;
			p2PointC[1] = p2PointA[1];
			p2PointD[0] = p2PointA[0];
			p2PointD[1] = p2PointA[1] - 10.0;
		}
	}
}

//Function to calculate the Firing Angle of the projectile, based on user input.
void CalcPAngle(int &calcStage, int cursorX, int cursorY, double &firingAng, double &angToRad, double p1PointA[], double p2PointA[], double angPointA[], double angPointB[], double angPointC[], double projPointA[], double projPointB[], double &lengthAB, double &lengthAC, double &lengthBC, bool p1Turn)
{
	cursorX = mouse_x;
	cursorY = mouse_y;

	#pragma region Player 1 Firing Angle
	if(p1Turn)
	{
		projPointA[0] = p1PointA[0];
		projPointA[1] = p1PointA[1];
		projPointB[0] = p1PointA[0] - 5;
		projPointB[1] = p1PointA[1];

		angPointA[0] = p1PointA[0];
		angPointA[1] = p1PointA[1];
		angPointB[0] = cursorX;
		angPointB[1] = cursorY;
		angPointC[0] = cursorX;
		angPointC[1] = p1PointA[1];

		if(mouse_b & 1 && cursorX > p1PointA[0])
		{
			lengthAB = sqrt((((angPointB[0] - angPointA[0])*(angPointB[0] - angPointA[0])) + ((angPointB[1] - angPointA[1]) * (angPointB[1] - angPointA[1]))));
			lengthAC = sqrt((((angPointC[0] - angPointA[0])*(angPointC[0] - angPointA[0])) + ((angPointC[1] - angPointA[1]) * (angPointC[1] - angPointA[1]))));
			lengthBC = sqrt((((angPointC[0] - angPointB[0])*(angPointC[0] - angPointB[0])) + ((angPointC[1] - angPointB[1]) * (angPointC[1] - angPointB[1]))));

			firingAng = acos(((lengthAC * lengthAC) + (lengthAB * lengthAB) - (lengthBC * lengthBC)) / (2 * lengthAC * lengthAB)) * 180.0 / PI;
			angToRad = firingAng * (PI / 180.0);
			rest(500);
			calcStage = 2;
		}
	}
#pragma endregion
	
	#pragma region Player 2 Firing Angle
	if(!p1Turn)
	{
		projPointA[0] = p2PointA[0];
		projPointA[1] = p2PointA[1];
		projPointB[0] = p2PointA[0] + 5;
		projPointB[1] = p2PointA[1];

		angPointA[0] = p2PointA[0];
		angPointA[1] = p2PointA[1];
		angPointB[0] = cursorX;
		angPointB[1] = cursorY;
		angPointC[0] = cursorX;
		angPointC[1] = p2PointA[1];

		if(mouse_b & 1 && cursorX < p2PointA[0])
		{
			lengthAB = sqrt((((angPointB[0] - angPointA[0])*(angPointB[0] - angPointA[0])) + ((angPointB[1] - angPointA[1]) * (angPointB[1] - angPointA[1]))));
			lengthAC = sqrt((((angPointC[0] - angPointA[0])*(angPointC[0] - angPointA[0])) + ((angPointC[1] - angPointA[1]) * (angPointC[1] - angPointA[1]))));
			lengthBC = sqrt((((angPointC[0] - angPointB[0])*(angPointC[0] - angPointB[0])) + ((angPointC[1] - angPointB[1]) * (angPointC[1] - angPointB[1]))));

			firingAng = acos(((lengthAC * lengthAC) + (lengthAB * lengthAB) - (lengthBC * lengthBC)) / (2 * lengthAC * lengthAB)) * 180.0 / PI;
			angToRad = firingAng * (PI / 180.0);
			rest(500);
			calcStage = 2;
		}
	}
#pragma endregion
}

//Function to calculate the Velocity of the projectile, based on user input.
void CalcPVelocity(int &calcStage, double const minV, double const maxV, bool &incrV, double &velocity, double &xVelocity, double &yVelocity, double &firingAng, double &angToRad)
{
	//Increase Velocity
	if(incrV && velocity < maxV)
	{
		velocity += 0.1;

		if(velocity > maxV)
		{
			velocity = maxV;
			incrV = false;
		}
	}

	//Decrease Velocity
	if(!incrV && velocity > minV)
	{
		velocity -= 0.1;

		if(velocity < minV)
		{
			velocity = minV;
			incrV = true;
		}
	}

	//When the mouse is clicked, the velocity is then broken down into its X and Y components.
	if(mouse_b & 1)
	{
		xVelocity = velocity * cos(angToRad);
		yVelocity = velocity * sin(angToRad);
		calcStage = 3;
	}
}

//Function to calculate the Projectiles flight.
void CalcPProjectile(int &calcStage, double &velocity, double &xVelocity, double &yVelocity, double p1PointA[], double p1PointB[], double p1PointC[], double p1PointD[], double p2PointA[], double p2PointB[], double p2PointC[], double p2PointD[], double projPointA[], double projPointB[], double const gravAccel, double &windForce, int &windChange, double firingAng, double &t, double &dt, double collLineA, double collLineB, double denominator, bool &p1Turn, bool &p1Wins, bool &p2Wins)
{
	//Player 1 Projectile Manipulation.
	if(p1Turn)
	{
		//Manipulate the projectile if it hasn't hit the floor.
		if(projPointA[1] < 640)
		{
			//Manipulate the projectiles position with X and Y velocity.
			projPointA[0] += (xVelocity) * dt;
			projPointA[1] -= (yVelocity) * dt;
			projPointB[0] += (xVelocity) * dt;
			projPointB[1] -= (yVelocity) * dt;

			//Apply gravity to the Y velocity and wind force to X velocity.
			yVelocity = yVelocity - (gravAccel * dt);
			xVelocity = xVelocity + (windForce * dt);

			//Count down from the Wind Change timer.
			windChange--;

			//When the Wind Change timer is fully counted down, generate a random number between 5 and -5 to determine wind force and direction.
			//Reset Wind Change timer.
			if(windChange <= 1)
			{
				windForce = rand()%10 + (-5);
				windChange = 300;
			}

			//Check for collisions between the Projectile and the lines that make up Player 2.
			CalcProjCollision(projPointA, projPointB, p2PointB, p2PointD, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins);
			CalcProjCollision(projPointA, projPointB, p2PointD, p2PointC, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins);
		}

		//If the projectile goes "below" the world, it's considered a miss.
		if(projPointA[1] > 640)
		{
			p1Turn = false;
			ResetSomeValues(calcStage, velocity, firingAng);
			return;
		}
	}

	//Player 2 Projectile Manipulation.
	if(!p1Turn)
	{
		//Manipulate the porjectile if it hasn't hit the floor.
		if(projPointA[1] < 640)
		{
			projPointA[0] -= (xVelocity) * dt;
			projPointA[1] -= (yVelocity) * dt;
			projPointB[0] -= (xVelocity) * dt;
			projPointB[1] -= (yVelocity) * dt;

			//Apply gravity to the Y velocity and wind force to X velocity.
			yVelocity = yVelocity - (gravAccel * dt);
			xVelocity = xVelocity - (windForce * dt);

			//Wind Force and Wind Chance
			//windChange = 100;
			windChange--;


			//When the Wind Change timer is fully counted down, generate a random number to determine wind force and direction.
			//Reset Wind Change timer.
			if(windChange <= 1)
			{
				windForce = rand()%10 + (-5);
				windChange = 300;
			}

			//Check for collisions between the Projectile and the lines that make up Player 1.
			CalcProjCollision(projPointA, projPointB, p1PointB, p1PointD, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins);
			CalcProjCollision(projPointA, projPointB, p1PointD, p1PointC, collLineA, collLineB, denominator, p1Turn, p1Wins, p2Wins);
		}

		//If the projectile goes "below" the world, it's considered a miss.
		if(projPointA[1] > 640)
		{
			p1Turn = true;
			ResetSomeValues(calcStage, velocity, firingAng);
			return;
		}
	}
}

//Function to calculate whether a line intersection has happened or not.
void CalcProjCollision(double lineA1[], double lineA2[], double lineB1[], double lineB2[], double collLineA, double collLineB, double denominator, bool &pTurn, bool &p1Wins, bool &p2Wins)
{
	denominator = ((lineB2[1] - lineB1[1])*(lineA2[0] - lineA1[0]))-((lineB2[0] - lineB1[0])*(lineA2[1] - lineA1[1]));

	//Calculate collisions for the two lines being tested.
	collLineA = (((lineB2[0] - lineB1[0])*(lineA1[1] - lineB1[1])) - ((lineB2[1] - lineB1[1])*(lineA1[0] - lineB1[0]))) / denominator;
	collLineB = (((lineA2[0] - lineA1[0])*(lineA1[1] - lineB1[1])) - ((lineA2[1] - lineA1[1])*(lineA1[0] - lineB1[0]))) / denominator;

	//Collision detection for Player 1.
	if(collLineA > 0 && collLineA < 1 && collLineB > 0 && collLineB < 1 && pTurn)
	{
		p1Wins = true;
	}

	//Collision detection for Player 2.
	if(collLineA > 0 && collLineA < 1 && collLineB > 0 && collLineB < 1 && !pTurn)
	{
		p2Wins = true;
	}
}

//Function to draw the Introduction Screen.
void DrawIntroScreen(const int scrx, const int scry)
{
	textout_ex(buffer, font, "WELCOME TO ARTILLERY!", scrx/2-100, scry/2, makecol(255,255,255), -1);
}

//Function to draw the Players Avatars to the Buffer/Screen.
void DrawAvatars(double p1PointB[], double p1PointC[], double p1PointD[], double p2PointB[], double p2PointC[], double p2PointD[])
{
	//Draw Player 1
	line(buffer, p1PointB[0], p1PointB[1], p1PointD[0], p1PointD[1], makecol(0,126,126));
	line(buffer, p1PointD[0], p1PointD[1], p1PointC[0], p1PointC[1], makecol(0,126,126));

	//Draw Player 2
	line(buffer, p2PointB[0], p2PointB[1], p2PointD[0], p2PointD[1], makecol(126,126,0));
	line(buffer, p2PointD[0], p2PointD[1], p2PointC[0], p2PointC[1], makecol(126,126,0));
}

//Function to draw the Firing Angle to the Buffer/Screen.
void DrawAngle(double angPointA[], double angPointB[], double angPointC[])
{
	line(buffer, angPointA[0], angPointA[1], angPointB[0], angPointB[1], makecol(255,255,255));
	line(buffer, angPointB[0], angPointB[1], angPointC[0], angPointC[1], makecol(255,255,255));
	line(buffer, angPointC[0], angPointC[1], angPointA[0], angPointA[1], makecol(255,255,255));
}

//Function to draw the Velocity Power Bar to the Buffer/Screen.
void DrawVBar(double velocity, double xVelocity, double yVelocity, double maxV, double vBarPointA[], double vBarPointB[])
{
	//Draws the updating Velocity Power Bar
	line(buffer, velocity+vBarPointA[0], vBarPointA[1], velocity+vBarPointA[0], vBarPointB[1], makecol(0,255,0));

	//Draws the static velocity bar using maxV
	line(buffer, vBarPointA[0], vBarPointA[1], vBarPointB[0], vBarPointB[1], makecol(255,0,0));
	line(buffer, vBarPointA[0], vBarPointA[1], vBarPointA[0]+maxV, vBarPointA[1], makecol(255,0,0));
	line(buffer, vBarPointB[0], vBarPointB[1], vBarPointB[0]+maxV, vBarPointB[1], makecol(255,0,0));
	line(buffer, maxV+vBarPointA[0], vBarPointA[1], maxV+vBarPointA[0], vBarPointB[1], makecol(255,0,0));
}

//Function to draw the Projectile to the Buffer/Screen.
void DrawProjectile(double projPointA[], double projPointB[])
{
	line(buffer, projPointA[0], projPointA[1], projPointB[0], projPointB[1], makecol(255,255,255));
}

//Function to draw various elements of the Graphical User Interface
void DrawGUI(bool p1Turn, double &windForce)
{
	if(p1Turn)
	{
		textout_ex(buffer, font, "Player 1s Turn", 15, 45, makecol(0,128,128), -1);
		textprintf_ex(buffer, font, 15, 75, makecol(0,128,128), -1, "Wind Force: %f", windForce);
	}
	if(!p1Turn)
	{
		textout_ex(buffer, font, "Player 2s Turn", 15, 45, makecol(128,128,0), -1);
		textprintf_ex(buffer, font, 15, 75, makecol(128,128,0), -1, "Wind Force: %f", windForce);
	}

	//Draws arrow, based on wind direction.
	line(buffer, 200, 78, 225, 78, makecol(255,255,255));
	if(windForce > 0)
	{
		line(buffer, 225, 78, 215, 73, makecol(255,255,255));
		line(buffer, 225, 78, 215, 83, makecol(255,255,255));
	}

	if(windForce < 0)
	{
		line(buffer, 200, 78, 210, 73, makecol(255,255,255));
		line(buffer, 200, 78, 210, 83, makecol(255,255,255));
	}
}

//Function to draw the Victory Screen to the Buffer/Screen.
void DrawVictoryScreen(const int scrx, const int scry, bool p1Wins, bool p2Wins)
{
	if(p1Wins)
	{
		textout_ex(buffer, font, "PLAYER 1 WINS!", scrx/2-75, scry/2-60, makecol(0,126,126), -1);
		textout_ex(buffer, font, "Press 0 to return to the menu!", scrx/2-135, scry/2, makecol(255,255,255), -1);
	}

	if(p2Wins)
	{
		textout_ex(buffer, font, "PLAYER 2 WINS!", scrx/2-75, scry/2-60, makecol(126,126,0), -1);
		textout_ex(buffer, font, "Press 0 to return to the menu!", scrx/2-135, scry/2, makecol(255,255,255), -1);
	}
}