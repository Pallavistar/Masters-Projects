#include "stdafx.h"
#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <GL/glut.h>
#include <stdio.h>
#include "bmp.h"
#include <string>
#include "Resource.h"
#include <iostream>
#include <mmsystem.h>

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=FALSE;	// Fullscreen Flag Set To false By Default
bool	gameStart, firstLevel, gameOver, levelTrans, Bam, Bonus, extraLife;		
float	exitVal = -5.5;		
bool contact=false;
HMENU hmenu;				// for creating menu bar

//for text display
GLuint  base;                           // Base Display List For The Font Set
GLfloat cnt1;                           // 1st Counter Used To Move Text & For Coloring
GLfloat cnt2;

typedef struct aliensVx
{
	float x1, y1, x2, y2;
}aliensVx;

float xtrans, ytrans, ztrans;		// Translation variables of alien

int Tfactor = 1;			//
bool UFOFlag=true;			// Collision of UFO
bool alienFlag[4][11];		// Collision of alien
aliensVx alien[4][11];
float barrScale [4] = {1.5,1.5,1.5,1.5};	// For scaling down barrirs when hit by cannon bullet

bool ASLfactor = false, USLfactor = false, PSLfactor = false;			// to control speed of the game
int ucounter = 0, pwCounter =0, acounter=0;
int random =(rand() % 1234)+1234;
int random2 =(rand() % 1234)+1234;
int powerupID=0;

float trans = 0.0, transPwy= 13, transPwx=-10;
int ufoFlag = 1;
int ufoFactor = 1;
float lowx = -5.5, highx = 5.5;
int globali=3, globalj=0;
int globali2 = 3, globalj2 = 10;
int lives=5;
float ufox1,ufox2;

float cannonTrans=0.0f;
float cannonR=0.0f;
int cannonFactor=1;
int level=1,levelFlag=1;
float ufoBulletx=17.0, ufoBullety=12.5;

int bulletCount=0, bombCount =0;
float bulletx[5]={-40.0,-40.0,-40.0,-40.0,-40.0}, bullety[5]={-4.0,-4.0,-4.0,-4.0,-4.0};
float bombX[5], bombY[5];

long int score = 0;
float diff;
int countDeadAlien = 0;
int bottomRow=3, aliveAliens = 11;

// Texture variables
char* texName = "./bck/Cover.bmp";		
char* barrierTexture = "./bck/Crate.bmp";
GLuint	texture[1],textureBarrier[1];
GLuint	filter;
int barrierTexFlag = 0;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File = NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File = fopen(Filename, "r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);							// Close The Handle

		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}

AUX_RGBImageRec *TextureImage[1], *TextureImageBarrier[1];	// Create Storage Space For The Texture

int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int Status = FALSE;									// Status Indicator

	memset(TextureImage, 0, sizeof(void *)* 1);          // Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0] = LoadBMP(texName))
	{
		Status = TRUE;									// Set The Status To TRUE
		glGenTextures(1, &texture[0]);

		// Create Nearest Filtered Texture
			
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	return Status;										// Return The Status
}

int LoadGLTexturesBarrier()									// Load Bitmaps And Convert To Textures
{
	int Status = FALSE;									// Status Indicator
	barrierTexFlag = 1;

	memset(TextureImageBarrier, 0, sizeof(void *)* 1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit

	if (TextureImageBarrier[0] = LoadBMP(barrierTexture))
	{
		Status = TRUE;									// Set The Status To TRUE
		glGenTextures(1, &textureBarrier[0]);

		// Create Nearest Filtered Texture
		glBindTexture(GL_TEXTURE_2D, textureBarrier[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImageBarrier[0]->sizeX, TextureImageBarrier[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImageBarrier[0]->data);
	}

	if (TextureImageBarrier[0])								// If Texture Exists
	{
		if (TextureImageBarrier[0]->data)						// If Texture Image Exists
		{
			free(TextureImageBarrier[0]->data);				// Free The Texture Image Memory
		}
		free(TextureImageBarrier[0]);							// Free The Image Structure
	}

	barrierTexFlag = 0;
	return Status;										// Return The Status
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

GLvoid BuildFont(GLvoid)                    // Build Our Bitmap Font
{
	HFONT   font;                       // Windows Font ID
	HFONT   oldfont;					// Used For Good House Keeping

	base = glGenLists(96);              // Storage For 96 Characters ( NEW )
	font = CreateFont(-24,              // Height Of Font ( NEW )
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_BOLD,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
		"Courier");						// Font Name
	oldfont = (HFONT)SelectObject(hDC, font);       // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);           // Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);             // Selects The Font We Want
	DeleteObject(font);                 // Delete The Font
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures() || !LoadGLTexturesBarrier())	// Jump To Texture Loading Routine
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}
	
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	BuildFont();										// Build The Font

	return TRUE;										// Initialization Went OK
}

GLvoid KillFont(GLvoid)                     // Delete The Font List
{
	glDeleteLists(base, 96);                // Delete All 96 Characters ( NEW )
}

GLvoid glPrint(const char *fmt, ...)                // Custom GL "Print" Routine
{
	char        text[256];              // Holds Our String
	va_list     ap;                 // Pointer To List Of Arguments
	if (fmt == NULL)                    // If There's No Text
		return;                     // Do Nothing
	va_start(ap, fmt);                  // Parses The String For Variables
	vsprintf(text, fmt, ap);                // And Converts Symbols To Actual Numbers
	va_end(ap);                     // Results Are Stored In Text
	glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits     ( NEW )
	glListBase(base - 32);                  // Sets The Base Character to 32    ( NEW )
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text  ( NEW )
	glPopAttrib();                      // Pops The Display List Bits   ( NEW )
}

void drawBck()
{
	glColor3f(0.8, 0.8, 0.8);
	glTranslatef(0.0f, 0.0f, -50.0f);						// Move into The Screen 50.0
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);				
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-45.0,40.5, 0.0f);					
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-45.0,-40.0, 0.0f);					
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( 45.0,-40.0, 0.0f);					
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( 45.0,40.5, 0.0f);	
	glEnd();
	glTranslatef(0.0f, 0.0f, 50.0f);
}

void newLevel()
{
	// set all aliens to true
	//#pragma loop(hint_parallel(2000))
	levelFlag = 1;
	for(int i=0;i<4;i++)
		//#pragma loop(hint_parallel(2000))
		for(int j=0;j<11;j++)
			alienFlag[i][j] = true;
	
	xtrans = 0.0f; 	ytrans = 0.0f;	ztrans = 0.0f;		
	Tfactor = 1;		

	UFOFlag = true;		
	ASLfactor = false;
	USLfactor = false;	
	PSLfactor = false;

	exitVal = -5.5;
	ucounter = 0; acounter = 0; pwCounter=0;
	random = (rand() % 1234) + 1234;
	random2 = (rand() % 1234) + 1234;

	trans = 0.0; transPwy= 13,
	powerupID =0;
	ufoFlag = 1; ufoFactor = 1;
	lowx = -5.5, highx = 5.5;
	globali = 3, globalj = 0;
	globali2 = 3, globalj2 = 10;
	
	cannonTrans = 0.0f;
	cannonR = 0.0f;
	cannonFactor = 1;
	bulletCount = 0;

	bottomRow=3, aliveAliens = 11;
		
	for(int i=0;i<5;i++)
	{
		bulletx[i] = -40.0;
		bullety[i] = -4.0;
	}

	barrierTexFlag = 0;
	countDeadAlien = 0;

	texName = "./bck/Cover.bmp";
	LoadGLTextures();
}

void drawUFOBullet()
{
	
	if (USLfactor && UFOFlag)
	{
		if (rand() % 2 == 0)
		{

			if (ufoBulletx <= 4.0)
			{
				glTranslatef(0.0, 0.0, 0.0);
				//ufoBullety = 12;
				glColor3f(0.5, 0.5, 0.5);
				glBegin(GL_QUADS);
				glVertex3f(ufoBulletx, ufoBullety - 0.5, 0.0f);
				glVertex3f(ufoBulletx, ufoBullety, 0.0f);
				glVertex3f(ufoBulletx + 0.5, ufoBullety, 0.0f);
				glVertex3f(ufoBulletx + 0.5, ufoBullety - 0.5, 0.0f);
				glEnd();
				ufoBullety -= 0.25;
			}
		}
		//if (transPwy <= -9 && transPwy >= -10)
if(!contact)
		if (ufoBullety<= -9.0 && ufoBullety >= -10)
		{
			if (ufoBulletx >= cannonTrans && ufoBulletx <= cannonTrans + 0.5 || ufoBulletx + 0.5 >= cannonTrans && ufoBulletx + 0.5 <= cannonTrans + 0.5)
			{

				
				lives -= 1;
				ufoBullety=-20;
				contact=true;
				if (lives <= 0)
				{
					newLevel();
					gameStart = false;
					gameOver = true;
					firstLevel = true;
				}
				
			}
		}
	}
}

void drawUFO()
{
	
	if(USLfactor && UFOFlag)
	{
		ufoBulletx += trans;
		glTranslatef(trans, 0.0,0.0);
		trans -= 0.01;
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_QUADS);									
			glVertex3f( 17.0,10.5, 0.0f);					
			glVertex3f( 17.0,11.0, 0.0f);					
			glVertex3f( 15.0,11.0, 0.0f);					
			glVertex3f( 15.0,10.5, 0.0f);	
		glEnd();

		glColor3f(1.0, 0.4, 0.4);
		glBegin(GL_QUADS);									
			glVertex3f( 15.0,10.5, 0.0f);					
			glVertex3f( 17.0,10.5, 0.0f);					
			glVertex3f( 17.2,10.3, 0.0f);					
			glVertex3f( 15.2,10.3, 0.0f);	
		glEnd();
		
		glColor3f(1.0, 0.9, 0.9);
		glBegin(GL_QUADS);									
			glVertex3f( 17.0,10.5, 0.0f);					
			glVertex3f( 17.0,11.0, 0.0f);					
			glVertex3f( 17.2,10.8, 0.0f);					
			glVertex3f( 17.2,10.3, 0.0f);	
		glEnd();

		if (trans < -35)
		{
			random =(rand() % 1234)+1234;
			trans = 0.0;
			ucounter = 0;
		}
		ufox1 = 15.0 + trans;
		ufox2 = 17.0 + trans;

		
	}	
}

void drawBarriers()
{
	float x, y;
	x = -11.0;	// this is the center of each barrier
	y = -5.5;	// y will be fixed 

	glBindTexture(GL_TEXTURE_2D, textureBarrier[0]);
	//#pragma loop(hint_parallel(2000))
	for (int countBar = 0; countBar<4; countBar++)
	{
		glColor3f(0.9,0.9,0.9);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x-barrScale[countBar], y-barrScale[countBar], 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x +barrScale[countBar], y-barrScale[countBar], 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x +barrScale[countBar], y +barrScale[countBar], 0.0f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(x-barrScale[countBar], y +barrScale[countBar], 0.0f);
		glEnd();

		// Making it 3D.. Like a BOSS!
		glColor3f(0.4, 0.4, 0.4);
		glBegin(GL_QUADS);
			glVertex3f(x-barrScale[countBar],(y +barrScale[countBar]),0.0f);
			glVertex3f((x +barrScale[countBar]),(y +barrScale[countBar]),0.0f);
			glVertex3f((x +barrScale[countBar]+barrScale[countBar]/2),(y +barrScale[countBar]+barrScale[countBar]/2),0.0f);
			glVertex3f((x-barrScale[countBar]+barrScale[countBar]/2),(y +barrScale[countBar]+barrScale[countBar]/2),0.0f);
		glEnd();
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_QUADS);
			glVertex3f((x +barrScale[countBar]), (y +barrScale[countBar]), 0.0f);
			glVertex3f((x +barrScale[countBar]), (y-barrScale[countBar]), 0.0f);
			glVertex3f((x +barrScale[countBar]+barrScale[countBar]/2),(y-barrScale[countBar]+barrScale[countBar]/2),0.0f);
			glVertex3f((x +barrScale[countBar]+barrScale[countBar]/2),(y +barrScale[countBar]+barrScale[countBar]/2),0.0f);
		glEnd();
		x += 7.2;
	}
}

void drawCanBul()
{
	glColor3f(0, 0, 0);
	glTranslatef(0, 0,-1.3);
	if (bulletCount >0)
	{
		//#pragma loop(hint_parallel(2000))
		for(int i=0;i<bulletCount;i++)
		{
			glBegin(GL_TRIANGLES);
				glVertex3f(bulletx[i],bullety[i] -5.0, 0.0f);
				glVertex3f(bulletx[i]+0.125,bullety[i] -4.5, 0.0f);
				glVertex3f(bulletx[i]+0.25, bullety[i] -5.0, 0.0f);
			glEnd();
			bullety[i]+=0.07;
			if(bullety[i] > 18)
			{
				int sh;
				//#pragma loop(hint_parallel(2000))
				for (sh = 0; sh < bulletCount-1; sh++)
				{
					bulletx[sh] = bulletx[sh + 1];
					bullety[sh] = bullety[sh + 1];
				}
				bulletx[sh] =-40.0;
				bullety[sh] =-4.0;
				bulletCount--;
			}
		}
	}
}

void drawAlienBomb()
{
	int tempx = (rand() % 4), tempy = rand() % 11;
	if (alienFlag[tempx][tempy] && bombCount <5)
	{
		bombX[bombCount] = alien[tempx][tempy].x1 + diff;
		bombY[bombCount] = alien[tempx][tempy].y1;
		bombCount++;
	}
	
	glColor3f((rand()%1000)/100,0,0);
	for(int i=0;i<bombCount;i++)
	{
		glBegin(GL_TRIANGLES);
			glVertex3f(bombX[i], bombY[i], 0.0f);
			glVertex3f(bombX[i] + 0.125, bombY[i]-1.5, 0.0f);
			glVertex3f(bombX[i] + 0.25, bombY[i], 0.0f);
		glEnd();
		bombY[i] -= 0.05;
		if(bombY[i] < -20)
		{
			int sh;
			//#pragma loop(hint_parallel(2000))
			for (sh = 0; sh < bombCount-1; sh++)
			{
				bombX[sh] = bombX[sh + 1];
				bombY[sh] = bombY[sh + 1];
			}
				
			bombX[sh] =-40.0;
			bombY[sh] =-4.0;
			bombCount--;
		}
	}
}

void powerUp()
{
	if(PSLfactor)
	{
		transPwy -= 0.05;
		glColor3f((rand()%1000)/200,(rand()%1000)/100,(rand()%1000)/200);
		glPushMatrix();
			glTranslatef(transPwx, transPwy,0);
			glutSolidSphere(0.6, 50, 50);
		glPopMatrix();
		
		if (trans < -20)
		{
			random2 =(rand() % 1234)+1234;
			transPwy = 13.0; transPwx+= 5;
			pwCounter = 0;
		}
	}	
}

void drawCannon()
{
	if (cannonTrans>15) cannonTrans = -15.0;
	if (cannonTrans<-15) cannonTrans = 15.0;

	glTranslatef(cannonTrans,0.0f,0.0f);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_TRIANGLES);
		glVertex3f(0.0, -10.0, 0.0f);
		glVertex3f(0.25, -9.0, 0.0f);
		glVertex3f(0.5, -10.0, 0.0f);
	glEnd();
}

void drawGround()
{
	glColor3f(0.2, 1.0, 0.2);
	glBegin(GL_QUADS);
		glVertex3f(-13.0, -11.0, 0.0f);
		glVertex3f(13.0, -11.0, 0.0f);
		glVertex3f(13.0, -11.2, 0.0f);
		glVertex3f(-13.0, -11.2, 0.0f);
	glEnd();
}

void drawAliens()
{
	float x,y,diff,alienColor;
	glColor3f (0.5, 0.5, 0.5);	
	x = 1.0; y = 1.0; diff = 1.0;
	alienColor = 0.5;
	glTranslatef(xtrans, ytrans,ztrans);
	if (ASLfactor)	// compute translation
	{
		ASLfactor = false;
		acounter = 0;
		xtrans = xtrans + Tfactor;
		//get the max and min x of every rendered alien and then check condition for EOL
		if (xtrans> highx)
		{
			ytrans -= 1.0;		// Move one row down
			ztrans += 1.2;		// Come in front
			Tfactor = -1;		// Translate X in reverse direction
			ASLfactor += 5;	// Increase the speed of the game
		}
		if (xtrans<lowx)
		{
			ytrans -= 1.0;		// Move one row down
			ztrans += 1.2;		// Come in front
			Tfactor = 1;		// Translate X in reverse direction
			ASLfactor += 5;		// Increase the speed of the game
		}

		if(ytrans<exitVal)		// Display new screen- start over? reset : escape
		{
				newLevel();
				gameStart = false;
				gameOver = true;
				firstLevel = true;
		}
	}
	// Translation computation over, now render the aliens
	//#pragma loop(hint_parallel(2000))
	for (int countRow = 0; countRow < 4; countRow++)
	{
		x = -7.0 - countRow*1.35;
		y = 8.0 - countRow*(diff + 0.1);
		alienColor += 0.1;
		//#pragma loop(hint_parallel(2000))
		for (int countCol = 0; countCol < 11; countCol++)
		{
			alien[countRow][countCol].x1 = x+xtrans;
			alien[countRow][countCol].x2 = x + diff + xtrans;
			alien[countRow][countCol].y1 = y + ytrans;
			alien[countRow][countCol].y2 = y + diff + ytrans;
			//draw only if the flag is set
			if (alienFlag[countRow][countCol] == true)
			{
				glColor3f(alienColor, (1 - alienColor), (1 - alienColor));
				glBegin(GL_QUADS);
					glVertex3f(x, y, 0.0f);
					glVertex3f((x + diff), y, 0.0f);
					glVertex3f((x + diff), (y + diff), 0.0f);
					glVertex3f(x, (y + diff), 0.0f);
				glEnd();
				// Eyes
				glColor3f(0,0,0);
				glBegin(GL_QUADS);
					glVertex3f((x+diff/6),(y+diff/2), 0.0f);
					glVertex3f((x+diff/3),(y+diff/2), 0.0f);
					glVertex3f((x+diff/3),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff/6),(y+diff*2/3), 0.0f);
				glEnd();
				glBegin(GL_QUADS);
					glVertex3f((x+diff*2/3),(y+diff/2), 0.0f);
					glVertex3f((x+diff*5/6),(y+diff/2), 0.0f);
					glVertex3f((x+diff*5/6),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff*2/3),(y+diff*2/3), 0.0f);
				glEnd();
				// Eyebrows
				glBegin(GL_QUADS);
					glVertex3f((x+diff/6),(y+diff-0.1), 0.0f);
					glVertex3f((x+diff*5/12),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff*5/12+0.15),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff/6+0.15),(y+diff-0.1), 0.0f);
				glEnd();
				glBegin(GL_QUADS);
					glVertex3f((x+diff*5/6-0.15),(y+diff-0.1), 0.0f);
					glVertex3f((x+diff*5/12+0.15),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff*5/12+0.3),(y+diff*2/3), 0.0f);
					glVertex3f((x+diff*5/6),(y+diff-0.1), 0.0f);
				glEnd();
				// Mouth
				glBegin(GL_TRIANGLES);
					glVertex3f((x+diff/3),(y+diff/6), 0.0f);
					glVertex3f((x+diff*2/3),(y+diff/6), 0.0f);
					glVertex3f((x+diff/2),(y+diff/3), 0.0f);
				glEnd();
				// Making it 3D.. Like a BOSS!
				glColor3f(0.2,0.2,0.2);
				glBegin(GL_QUADS);
					glVertex3f(x, (y+diff), 0.0f);
					glVertex3f((x+diff), (y+diff), 0.0f);
					glVertex3f((x+diff+0.15), (y+diff+0.15), 0.0f);
					glVertex3f((x+0.15), (y+diff+0.15), 0.0f);
				glEnd();
				glBegin(GL_QUADS);
					glVertex3f((x+diff), (y+diff), 0.0f);
					glVertex3f((x+diff+0.15), (y+diff+0.15), 0.0f);
					glVertex3f((x+diff+0.15), (y+0.15), 0.0f);
					glVertex3f((x+diff), y, 0.0f);
				glEnd();	
			}
			// diff will increment irrespective of whether or not that particular alien is displayed.
			x += (diff + 0.25);
		}
		diff += 0.25;
	}
	glTranslatef(xtrans, ytrans, ztrans);
}

void collDetect()
{
	//#pragma loop(hint_parallel(2000))
	for (int k = 0; k < bulletCount; k++)	// For each CANNON bullet
	{
		//#pragma loop(hint_parallel(2000))
		for (int i = 0; i<4; i++)			//alien collision
		//#pragma loop(hint_parallel(2000))
		for (int j=0;j<11; j++)
			if (alienFlag[i][j] == true)
			{
				if (bulletx[k] >= alien[i][j].x1 && bulletx[k] <= alien[i][j].x2 && (bullety[k]-3.0) >= alien[i][j].y1 && (bullety[k]-3.0) <= alien[i][j].y2)
				{
					score = score + 10;
					alienFlag[i][j] = false;
					countDeadAlien += 1;
					bullety[k] = 20;			// This removes the bullet from the picture.
					if (i == globali && j == globalj)
					{
						lowx = lowx - 2;
						globali = globali-1;
						globalj = globalj-1;
					}
					if (i == globali2 && j == globalj2)
					{
						globali2 = globali2 - 1;
						globalj2 = globalj2 - 1; 
						highx = highx + 2;
					}
					
					// Determine exitVal
					if(i==bottomRow)
					{
						if(--aliveAliens == 0)
						{
							bottomRow--;
							aliveAliens =11;
							exitVal -= 1.5;
						}
					}
				}
			}//check for alien collision completed.
			
			// Barrier collision
			float y=-5.5, x=-11.0;
			//#pragma loop(hint_parallel(2000))
			for (int b=0; b<4; b++)
			{
				if(bulletx[k] >= x-barrScale[b] && bulletx[k] <= x+barrScale[b]*1.5 && (bullety[k]-4.0) >= y-barrScale[b] && (bullety[k]-4.0) <= y+barrScale[b]*1.5)// Collision
				{
					barrScale[b] -=0.35;
					if (barrScale[b]<0.35) barrScale[b] =0;
					bullety[k] = 20;
				} 
						x+=7.2;
			}//check for barrier collision completed.
			
			//ufo collision
			if (USLfactor && (bullety[k]>=10.5 && bulletx[k]>=ufox1 && bulletx[k]<=ufox2 && bullety[k]<11.0))
			{
				score = score + 20;
				UFOFlag = false;
				ucounter = 0;
				break;
			}
	}			// Cannon bullets collisions done

	for(int i=0;i<bombCount; i++)	// For each bomb
	{
		if(bombX[i]>cannonTrans && bombX[i]<cannonTrans+0.5 && bombY[i]>=-10 && bombY[i]<=-9)	// collision with cannon
		{
			lives--;
			bombY[i]=-20;
			if(lives <=0)
			{
				newLevel();
				gameStart = false;
				gameOver = true;
				firstLevel = true;
			}
		}
		float y=-5.5, x=-11.0;
		for(int b=0;b<4;b++)// collision with barrier
		{
			if(bombX[i]> x-barrScale[b] && bombX[i]<x+barrScale[b]*1.2 && bombY[i]> y-barrScale[b] && bombY[i]< y+barrScale[b]*1.2)// Collision
				bombY[i]=-20;
			x+=7.2;
		}
	}

	// Power up detection
	if (transPwy <= -9 && transPwy >= -10)
		if((cannonTrans > transPwx && cannonTrans < transPwx+1) || (cannonTrans+0.5 > transPwx && cannonTrans+0.5 < transPwx+1))
		{
			glColor3f(0,0,0);
			switch(powerupID%3)
			{
				case 0: lives++;
						powerupID +=2;
						extraLife = true;
						break;
				case 2: for(int j=0;j<11;j++)
						{
							if(alienFlag[0][j] = true)
							{
								alienFlag[0][j] = false;
								countDeadAlien --;
							}
						}
						powerupID +=20;
						Bam = true;
						break;
				default:score += 100;
						powerupID +=10;
						Bonus  = true;
						break;
			}
				transPwy = -20;
		}
	
	//if all aliens are killed go to next level
	if (countDeadAlien == 44)
	{
		newLevel();
		gameStart = false;
		levelTrans = true;
	}
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(0.0f,0.0f,-30.0f);						// Move into The Screen 30.0
	
	if (gameOver)
		level = 1;
	// Position The Text On The Screen
	if (gameStart)
	{
		glColor3f(0, 1, 1);
		
	}
	else
		glColor3f(0,(rand()%1000)/100,(rand()%1000)/100);

	glRasterPos2f(-15.0f, 9.5f);
	glPrint("Score:%d", score);							// Print GL Text To The Screen
	glRasterPos2f(10.0f, 9.5f);
	glPrint("Lives:%d", lives);
	glRasterPos2f(-15.0f, 10.5f);
	glPrint("Level:%d", level);
	if (gameOver)
	{
		glRasterPos2f(-3.5, 0.7);
		glPrint("G A M E__O V E R!");
	}
	else if (levelTrans)
	{
		if (levelFlag == 1)
		{
			level = level + 1;
			levelFlag++;
		}
		glRasterPos2f(-9.5, 0.7);
		glPrint("CONGRATULATIONS! Press enter for next level!");
	}
	else if (extraLife)
	{
		extraLife = false;
		for (int p=0;p<300;p++)
		{
			glRasterPos2f(cannonTrans,-8);
			glPrint("EXTRA LIFE !!!");	
		}
	}
	else if (Bam)
	{
		Bam = false;
		for (int p=0;p<300;p++)
		{
			glRasterPos2f(cannonTrans,-8);
		    glPrint("BAM!");	
		}
	}
	else if (Bonus)
	{
		Bonus = false;
		for (int p=0;p<300;p++)
		{
			glRasterPos2f(cannonTrans,-8);
			glPrint("Bonus!");	
		}
	}
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	drawBck();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	
	collDetect();

	if(!gameStart) return true;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	drawBarriers();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	drawCannon();
	glPopMatrix();

	glPushMatrix();
	drawCanBul();
	glPopMatrix();

	glPushMatrix();
	drawAlienBomb();
	glPopMatrix();

	glPushMatrix();
	drawGround();
	glPopMatrix();

	glPushMatrix();
	drawAliens();
	glPopMatrix();

	glPushMatrix();
	drawUFOBullet();
	glPopMatrix();

	glPushMatrix();
	drawUFO();										// UFO may appear anytime randomly
	glPopMatrix();

	

	glPushMatrix();
	powerUp();										// Power up may appear anytime randomly
	glPopMatrix();

	glLoadIdentity();									// Reset The Current Modelview Matrix
	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}

	KillFont();                     // Destroy The Font
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= "SpaceMenu";							// We Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	hmenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID_Menu));		//create a menu for  background textures
	
	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}
	
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
	
	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								hmenu,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
			case WM_COMMAND:
			{
					   switch (LOWORD(wParam))
					   {
					   case ID_texture1:
					   {

										   texName = "./bck/Koala.bmp";
										   LoadGLTextures();


					   } break;

					   case ID_texture2:
					   {
										   texName = "./bck/white.bmp";
										   LoadGLTextures();

					   } break;

					   case ID_texture3:
					   {
										   texName = "./bck/Crate.bmp";
										   LoadGLTextures();
					   } break;

					   case ID_texture4:
					   {
										   texName = "./bck/sunset.bmp";
										   LoadGLTextures();
					   } break;

					   case ID_texture5:
					   {
										   texName = "./bck/slices.bmp";
										   LoadGLTextures();
					   } break;
					   case ID_texture6:
					   {
										   texName = "./bck/rocket.bmp";
										   LoadGLTextures();
					   } break;
					   case ID_help1:
					   {
										   //open a new window to display help section
										texName = "./bck/Cover.bmp";
										gameStart = false;
										LoadGLTextures();
					   } break;

					   }
					   
			} break;
			case WM_ACTIVATE:							// Watch For Window Activate Message
			{
				if (!HIWORD(wParam))					// Check Minimization State
				{
					active=TRUE;						// Program Is Active
				}
				else
				{
					active=FALSE;						// Program Is No Longer Active
				}

				return 0;								// Return To The Message Loop
			}

			case WM_SYSCOMMAND:							// Intercept System Commands
			{
				switch (wParam)							// Check System Calls
				{
					case SC_SCREENSAVE:					// Screensaver Trying To Start?
					case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;							// Prevent From Happening
				}
				break;									// Exit
			}

			case WM_CLOSE:								// Did We Receive A Close Message?
			{
				PostQuitMessage(0);						// Send A Quit Message
				return 0;								// Jump Back
			}

			case WM_KEYDOWN:							// Is A Key Being Held Down?
			{
				keys[wParam] = TRUE;					// If So, Mark It As TRUE
				return 0;								// Jump Back
			}

			case WM_KEYUP:								// Has A Key Been Released?
			{
				keys[wParam] = FALSE;					// If So, Mark It As FALSE
				return 0;								// Jump Back
			}

			case WM_SIZE:								// Resize The OpenGL Window
			{
				ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
				return 0;								// Jump Back
			}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	PlaySound("./sounds/bck.wav", NULL, SND_FILENAME|SND_LOOP|SND_NOSTOP|SND_ASYNC);
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// set all aliens to true
	//#pragma loop(hint_parallel(2000))
	for(int i=0;i<4;i++)
		//#pragma loop(hint_parallel(2000))
		for(int j=0;j<11;j++)
			alienFlag[i][j] = true;

	// Create Our OpenGL Window
	if (!CreateGLWindow("SPACE INVADERS",1024,720,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done=TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{		
						if (keys[VK_RETURN])
						{
							gameStart = true; gameOver = false; levelTrans = false;
							texName = "./bck/White.bmp";
							LoadGLTextures();
							if(firstLevel)
							{
								score =0;
								lives =5;
								barrScale[0] = 1.5;
								barrScale[1] = 1.5;
								barrScale[2] = 1.5;
								barrScale[3] = 1.5;
							}
						}
						if (keys[VK_LEFT])
  							cannonTrans = cannonTrans - cannonFactor*0.05f;
	
						if (keys[VK_RIGHT])
							cannonTrans = cannonTrans + cannonFactor*0.05f;
						
						ufoBulletx = 17.0 + trans;

						if (keys[VK_SPACE])
						{
							/*PlaySound(NULL, NULL, SND_ASYNC);
							PlaySound("./sounds/laser.wav", NULL, SND_FILENAME|SND_NOSTOP|SND_ASYNC);
							PlaySound("./sounds/bck.wav", NULL, SND_FILENAME|SND_LOOP|SND_NOSTOP|SND_ASYNC);*/
							keys[VK_SPACE] = false;
							if (bulletCount == 4)
							{
  								int jhggy = 1;
							}
							if (bulletCount <5)
							{
							bulletx[bulletCount] = cannonTrans+0.12;
							bulletCount++;
							}
						}
						
						if (keys['P'])
						{
							texName = "./bck/Cover.bmp";
							gameStart = false;
							LoadGLTextures();
						}
						if (random>0) random--; else ucounter++;	// For UFO
						if (ucounter > 50) { USLfactor = true;  }
						else USLfactor = false;
						if (random2>0) random2--; else pwCounter++;	// For power ups
						if(pwCounter>50) PSLfactor = true; else PSLfactor = false;
						acounter++; if(acounter==100) { ASLfactor = true; powerupID++;}

						Sleep(7);
						DrawGLScene();					// Draw The Scene
						SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("SPACE INVADERS",1024,720,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}