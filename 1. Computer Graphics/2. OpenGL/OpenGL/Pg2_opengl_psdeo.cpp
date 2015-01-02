#include "stdafx.h"
#include <stdlib.h>
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <Gl/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include "tiny_obj_loader.h"

using namespace std;

unsigned char* data;
unsigned int width, height;
static GLuint texName;

// Menu items
enum MENU_TYPE
{
        TILE,
        STONE,
        SQUARES,
        REDWAVES,
		KOALA
};
// Assign a default value
MENU_TYPE images = TILE;
string img = "./images/Tile.bmp"; // initializing default image
const char* cimg = img.c_str();
// Menu handling function declaration
void menu(int);

//psdeo: Same method as tinyObj has for parsing input file
static inline int parseInt(const char*& token)
{
  token += strspn(token, " \t");
  int i = atoi(token);
  token += strcspn(token, " \t\r");
  return i;
}

int loadBMP(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height)
{

	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Open the file in binary mode
	FILE* fp = fopen(path,"rb");
	if (!fp)                              
	{
		cout<<"Image could not be opened\n"; 
		return 0;
	}

	if ( fread(header, 1, 54, fp)!=54 )
	{ 
		// If not 54 bytes read : problem
		cout<<"Not a correct BMP file\n";
		return 0;
	}

	if ( header[0]!='B' || header[1]!='M' )
	{
		cout<<"Not a correct BMP file\n";
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    
		imageSize=(width)*(height)*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      
		dataPos=54; // The BMP header is done 

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	if (fread(data,1,imageSize,fp) != imageSize)
	{
		fclose(fp);
		return 0;
	}

	//Everything is in memory now, the file can be closed
	fclose(fp);
	return 1;
}

void init()
{
	if (!loadBMP(cimg ,data, width, height))
		cout<<"Error loading bmp\n";
	else
		cout<<"Loaded bmp successfully!\n";

	glClearColor (0.6f, 0.3f, 0.9f, 0.0f);			
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
   //specify pixels are byte-aligned while unpacking from memory
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texName);//generate
	glBindTexture(GL_TEXTURE_2D, texName);//bind to a 2-D texture
	//wrap the image along s coordinate (width)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//wrap the image along t coordinate (height)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//how to treat magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//how to treat de-magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//bmp stores BGR instead of RGB
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	// Lighting
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_ambient[] = { 0.0, 1.0, 0.0, 1.0 };
   GLfloat mat_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 1.0 };
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void display()
{
	static unsigned short count = 0;
	static unsigned short dir = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//tumble randomly psdeo:deleted for assignment purpose
	//glRotatef(count<640?1:count<1152?-1:(count=0,1),dir<768?1:0,1.0,dir<1792?0:(dir<2816?1:(dir=0,0)));
	//count++;
	//dir++;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);	
	/*----- Juhee's code for cube-----------------------------
	glBegin(GL_QUADS);
	//top
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f,-1.0f);
	
	//bottom
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f,-1.0f, 1.0f);

	//front	
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);
	
	//back	
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		
	//left
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f(-1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f,-1.0f);	
		
	//right
		glTexCoord2f(0.0, 0.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		
	glEnd();-----------------------------------------------*/
	std::string inputfile = "./inputs/al.obj";
	std::vector<tinyobj::shape_t> shapes;
	std::string err = tinyobj::LoadObj(shapes, inputfile.c_str()); //populate "shapes" struct

	// psdeo: reading window size from window.txt
	// referred file redeading code from tinyObjLoader
	double winX,winY,len,ht;
	std::stringstream err1;
		std::ifstream win("window.txt");
	if (!win) {
		err1 << "Cannot open file window.txt" << std::endl;
		return;
	}
	std::istream& inStream = win;
	int maxchars = 100;				 // window size should not exceed those many digits
	std::vector<char> buf(maxchars);  // Alloc enough size.
	
	while (inStream.peek() != -1) {
    inStream.getline(&buf[0], maxchars);
	std::string linebuf(&buf[0]);

	 // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size()-1] == '\n') linebuf.erase(linebuf.size()-1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size()-1] == '\r') linebuf.erase(linebuf.size()-1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }
	    const char* token = linebuf.c_str();
		winX = (double) parseInt(token);
		winY = (double) parseInt(token);
		len = (double) parseInt(token);
		ht = (double) parseInt(token);
	}

	float x[2048] , y[2048];
	//to scale the object as per the window
	double  scaleX, scaleY;
	scaleX = winX/ 256;
	scaleY = winY/ 256;

	glBegin(GL_TRIANGLES);								// Drawing Using Triangles
	float i=0,j=0;										//for texture looping
	for(int k=0;k<shapes.size();k++)
		for(int l=0;l<shapes[k].mesh.indices.size();l+=3)
		{
			std::vector<float> v21(3,0.0);
			v21[0]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3];
			v21[1]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+1] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+1];
	 		v21[2]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+2] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+2];
			std::vector<float> v31(3,0.0);
			v31[0]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3];
			v31[1]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+1] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+1];
	 		v31[2]= shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+2] - shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+2];
			std::vector<float> res(3,0.0);
			res[0] = (v21[1] * v31[2]) - (v21[2] * v31[1]);
			res[1] = -((v21[0] * v31[2]) - (v21[2] * v31[0]));
			res[2] = (v21[0] * v31[1]) - (v21[1] * v31[0]);

			float temp = sqrt(res[0]*res[0] + res[1]*res[1] + res[2]*res[2]);
			res[0]/= temp;
			res[1]/= temp;
			res[2]/= temp;
			// res is the normal vector now!
			glNormal3f(res[0],res[1],res[2]);
			glColor3f (0.9, 0.1, 0.1);	glTexCoord2f(i,j); 
			glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3]*scaleX,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+1]*scaleY,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+2]);
			if (j==1.0) j= -1.0;
			glColor3f (0.1, 0.9, 0.1);	glTexCoord2f(i,++j);
			glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3]*scaleX,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+1]*scaleY,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+2]);
			if (i==1.0) i=-1.0;
			glColor3f (0.1, 0.1, 0.9);	glTexCoord2f(++i,j);
			glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3]*scaleX,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+1]*scaleY,
				shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+2]);
		}
	glEnd();											// Finished Drawing The Triangle



	glFlush();
	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}

void rotate(int value)
{
	//we aren't doing anything with value here
	glutPostRedisplay();
	glutTimerFunc(10,rotate,0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,6,0,0,0,0,1,0);
	glTranslatef(0.0f,1.0f,-3.0f);	// z=-3 for al, -60.6 for car
}

float temp =0.0;			// For keyboard functions
void keyboard (unsigned char key, int x, int y)
{
	temp =0.0;
	switch (key) 
	{
	case 'z':
		// Code for zoom in
		temp = temp + 0.2;
		glTranslatef(0.0f,0.0f,++temp);	
		break;
	case 'x':
		// Code for zoom out
		temp = temp + 0.2;
		glTranslatef(0.0f,0.0f,--temp);
		break;
	case '[':
		// Coe for translation along +Z
		temp = temp + 0.2;
		glTranslatef(0.0f,0.0f,++temp);	
		break;
	case ']': 
		// Code for translation along -Z
		temp = temp + 0.2;
		glTranslatef(0.0f,0.0f,--temp);	
		break;
	case 'w':
		// Code for clockwise rotation (i.e. along Z axis)
		glRotatef(5.0f,0.0f,0.0f,1.0f);
		break;
	case 'q':
		// Code for anti clockwise rotation 
		glRotatef(5.0f,0.0f,0.0f,-1.0f);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}
void ArrowKeys(int key, int x, int y)
{
	temp =0.0;
	switch (key)
	{
	case GLUT_KEY_UP:
		temp = temp + 0.2;
		glTranslatef(0.0f,++temp,0.0f);	
		break;
	case GLUT_KEY_DOWN:
		temp = temp + 0.2;
		glTranslatef(0.0f,--temp,0.0f);	
		break;
	case GLUT_KEY_LEFT:
		temp = temp + 0.2;
		glTranslatef(--temp,0.0f,0.0f);	
		break;
	case GLUT_KEY_RIGHT:
		temp = temp + 0.2;
		glTranslatef(++temp,0.0f,0.0f);	
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glEnable(GL_DEPTH_TEST); 
	// For window size, reading window.txt again
	// psdeo: reading window size from window.txt
	// referred file redeading code from tinyObjLoader
	double winX,winY,len,ht;
	std::stringstream err1;
		std::ifstream win("window.txt");
	if (!win) {
		err1 << "Cannot open file window.txt" << std::endl;
		return 0;
	}
	std::istream& inStream = win;
	int maxchars = 100;				 // window size should not exceed those many digits
	std::vector<char> buf(maxchars);  // Alloc enough size.
	
	while (inStream.peek() != -1) {
    inStream.getline(&buf[0], maxchars);
	std::string linebuf(&buf[0]);

	 // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size()-1] == '\n') linebuf.erase(linebuf.size()-1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size()-1] == '\r') linebuf.erase(linebuf.size()-1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }
	    const char* token = linebuf.c_str();
		winX = (double) parseInt(token);
		winY = (double) parseInt(token);
		len = (double) parseInt(token);
		ht = (double) parseInt(token);
	}
	glutInitWindowSize(winX, winY);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Pg2_opengl_psdeo");
	init();
	// Create a menu
    glutCreateMenu(menu);
	// Add menu items
    glutAddMenuEntry("Tile", TILE);
    glutAddMenuEntry("Stones", STONE);
    glutAddMenuEntry("Squares", SQUARES);
    glutAddMenuEntry("Red Waves", REDWAVES);
	glutAddMenuEntry("Koala", KOALA);
	// Associate a mouse button with menu
    glutAttachMenu(GLUT_LEFT_BUTTON);
	//init();
	glutDisplayFunc(display);
	glutTimerFunc(20,rotate,0);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(ArrowKeys);
	glutMainLoop();
	
	delete data;
	return 0; 
}

// Menu handling function definition
void menu(int item)
{
        switch (item)
        {
        case TILE:
			img = "./images/Tile.bmp";
			cimg = img.c_str();
			break;
		case STONE:
			img = "./images/Stone.bmp";
			cimg = img.c_str();
			break;
        case SQUARES:
			img = "./images/Squares.bmp";
			cimg = img.c_str();
			break;
        case REDWAVES:
			img = "./images/RedWaves.bmp";
			cimg = img.c_str();
			break;
        case KOALA:
			img = "./images/Koala.bmp";
			cimg = img.c_str();
			break;
        default:
			img = "./images/Tile.bmp";
			cimg = img.c_str();
			break;
        }
        glutPostRedisplay();
		init();
        return;
}