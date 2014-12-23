// Pg1_raycast_psdeo.cpp : 
// CSC-561 Computer Graphics Programming Assignment 1
// Defines the entry point for the application.
#include "tiny_obj_loader.h"
#include "stdafx.h"
#include "Pg1_raycast_psdeo.h"
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#define MAX_LOADSTRING 100
#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)

//psdeo: Same method as tinyObj has for parsing input file
static inline int parseInt(const char*& token)
{
  token += strspn(token, " \t");
  int i = atoi(token);
  token += strcspn(token, " \t\r");
  return i;
}

static HWND sHwnd;
//Color
//static COLORREF redColor = RGB(255, 0, 0);
//static COLORREF blueColor = RGB(0, 0, 255);
//static COLORREF greenColor = RGB(0, 255, 0);
static COLORREF blackColor = RGB(0, 0 , 0);

static COLORREF tempColor;
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PG1_RAYCAST_PSDEO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PG1_RAYCAST_PSDEO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PG1_RAYCAST_PSDEO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PG1_RAYCAST_PSDEO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
void SetWindowHandle(HWND hwnd)
{
	printf("setwindowhandle");
	sHwnd = hwnd;
}
void setPixel(int x, int y, COLORREF& color)
{
	printf("setpixel");
	if (sHwnd == NULL)
	{		
		exit(0);
	}
	HDC hdc = GetDC(sHwnd);
	SetPixel(hdc, x, y, color);
	ReleaseDC(sHwnd, hdc);
	return;
}
//_________________________________________________________________________
//psdeo: in order for intersection code to work, some modifications are required
// such as structure definitions & operator overloadings that are assumed
class Point{
	public:float x;
	public:float y;
	public:float z;
	
	Point* operator= (Point p)
	{
		return (&p);
	}
}I , tempPt;
class Vector
{
public:float x;
public:float y;
public:float z;

	   Vector ()
	   {
		   x=0;
		   y=0;
		   z=0;
	   }

	   Vector(int i)
	   {
		   x=i;
		   y=i;
		   z=i;
	   }

	   Vector (double a,double b,double c)
	   {
		   x =a;
		   y=b;
		   z=c;
	   }
	   Vector operator= (Point p)
	   {
		   Vector v;
			v.x=p.x;
			v.y=p.y;
			v.z=p.z;
			return v;
	   }

	   Vector operator* (float lt[3])
	   {
		   Vector v1;
		   v1.x=x*lt[0];
		   v1.y=y*lt[1];
		   v1.z=z*lt[2];
		   return v1;
	   }

	   Vector mult (float r)
	   {
		   Vector v;
		   v.x= x*r;
		   v.y=y*r;
		   v.z=z*r;
		   return v;
	   }

	   Vector cross (Vector v)
	   {
		  Vector r;
		  r.x = y*v.z - z*v.y;
		  r.y = x*v.z - z*v.x;
		  r.z = x*v.y - y*v.x;

		  return r;
	   }

	   bool operator==(Vector v)
	   {
		   if (x== v.x && y==v.y && z==v.z)
			   return true;
		   else return false;
	   }

	   void normalize()
	   {
		   double len = sqrt ((x*x) + (y*y) + (z*z));
		   x /= len;
		   y /= len;
		   z /= len;
	   }
};
Point operator+ (Vector v,Point p)
	   {
		   I.x = v.x+p.x;
		   I.y = v.y+p.y;
		   I.z = v.z+p.z;

		   return(I);
	   }
Vector operator-(Point p1,Point p2)
{
	Vector r;
	r.x = p1.x - p2.x;
	r.y = p1.y - p2.y;
	r.z = p1.z - p2.z;
	return r;
}
Vector operator+ (Vector v1,Vector v2)
	   {
		   Vector v;
		   v.x = v1.x + v2.x;
		   v.y = v1.y +v2.y;
		   v.z = v1.z + v2.z;
		   return v;
	   }
Vector operator- (Vector v1,Point v2)
	   {
		   Vector v;
		   v.x-= v1.x - v.x;
		   v.y-= v1.y - v.y;
		   v.z-= v1.z - v.z;
		   return v;
	   }
Vector operator-(Point v1, Vector v2)
{
	 Vector v;
		   v.x-= v1.x - v.x;
		   v.y-= v1.y - v.y;
		   v.z-= v1.z - v.z;
		   return v;
}
class Ray{
public:Point P0;
public:Point P1;
}R;
class Triangle{
public:Point V0;
public:Point V1;
public:Point V2;
}T;

// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application
// intersect3D_RayTriangle(): find the 3D intersection of a ray with a triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 =  disjoint (no intersect)
//             1 =  intersect in unique point I1
//             2 =  are in the same plane

int intersect3D_RayTriangle( Ray R, Triangle T, Point* I )
{
    Vector  u, v, n;              // triangle vectors
    Vector	dir, w0, w;           // ray vectors
    float   r, a, b;              // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = T.V1 - T.V0;
    v = T.V2 - T.V0;
    n = u.cross(v);					// cross product

    if (n == (Vector)0)             // triangle is degenerate
        return -1;                  // do not deal with this case

    dir = R.P1 - R.P0;              // ray direction vector
    w0 = R.P0 - T.V0;
    a = -dot(n,w0);
    b = dot(n,dir);
    if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
        if (a == 0)                 // ray lies in triangle plane
            return 2;
        else return 0;              // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                    // ray goes away from triangle
        return 0;                   // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect
	dir = dir.mult(r);
    *I = dir+ R.P0;            // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = dot(u,u);
    uv = dot(u,v);
    vv = dot(v,v);
    w = *I - T.V0;
    wu = dot(w,u);
    wv = dot(w,v);
    D = uv * uv - uu * vv;

    // get and test parametric coords	
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)         // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                       // I is in T
}

//Global variables for intensity calculations to work
std::vector<tinyobj::shape_t> shapes;
int tempShape = -1;
Point tempI;
Vector intensity;

Vector calcIntensity(int k, int tempShape, Triangle T,Point tempI)
{
	//define light 
	Vector eye = (0,0,-2);
	Vector La[5],Ld[5],Ls[5];
	Vector Light1 = (0, 0.8 , 2);
	Vector N,L,V,H;

	Vector u = T.V1 - T.V0;
    Vector v = T.V2 - T.V0;
    
	N = u.cross(v);
	L = Light1 - tempI;
	V = tempI - eye;
	H = L + V;

	N.normalize();
	L.normalize();
	V.normalize();
	H.normalize();
	
	La[0] = (1,1,1);
	Ld[0] = (1,1,1);
	Ls[0] = (1,1,1);

	Vector ambient, diffuse, specular;

	ambient = La[0] * shapes[k].material.ambient;
	diffuse = (Ld[0] * shapes[k].material.diffuse).mult(dot(N,L));
	if (diffuse.x < 0) diffuse.x =0;
	if (diffuse.y < 0) diffuse.y =0;
	if (diffuse.z < 0) diffuse.z =0;
	if (diffuse.x > 1) diffuse.x =1;
	if (diffuse.y > 1) diffuse.y =1;
	if (diffuse.z > 1) diffuse.z =1;

	specular= (Ls[0] * shapes[k].material.specular).mult((int)(dot (H,N)) ^ 64);
	if (specular.x < 0) specular.x =0;
	if (specular.y < 0) specular.y =0;
	if (specular.z < 0) specular.z =0;
	if (specular.x > 1) specular.x =1;
	if (specular.y > 1) specular.y =1;
	if (specular.z > 1) specular.z =1;

	intensity = ((ambient + diffuse) + specular);
	if (intensity.x < 0) intensity.x =0;
	if (intensity.y < 0) intensity.y =0;
	if (intensity.z < 0) intensity.z =0;
	if (intensity.x > 1) intensity.x =1;
	if (intensity.y > 1) intensity.y =1;
	if (intensity.z > 1) intensity.z =1;
	return intensity;
}
void drawColor()
{		
	std::string inputfile = "./inputs/cube.obj";
	std::string err = tinyobj::LoadObj(shapes, inputfile.c_str()); //populate "shapes" struct
	unsigned int max=0; // max value among all vertices. required to scale down the image if it's big
	// code to scale down large objects to fit in the frustrum
	for (int cnt=0;cnt<shapes.size();cnt++)
		for (int cnt1=0;cnt1<shapes[cnt].mesh.positions.size();cnt1+=3)
		if (((shapes[cnt].mesh.positions[cnt1] * shapes[cnt].mesh.positions[cnt1] )>1)
			|| ((shapes[cnt].mesh.positions[cnt1+1] * shapes[cnt].mesh.positions[cnt1+1] )>1))
			if (shapes[cnt].mesh.positions[cnt1] > max)
				max = shapes[cnt].mesh.positions[cnt1];
	if (max > 1)
		for (int cnt=0;cnt<shapes.size();cnt++)
			for (int cnt1=0;cnt1<shapes[cnt].mesh.positions.size();cnt1++)
				shapes[cnt].mesh.positions[cnt1] /= max;

	// psdeo: Eye position for current requirement of assignment.
	R.P0.x = (float) 0;
	R.P0.y = (float) 0;
	R.P0.z = (float) -2;
	
	int intersection = 0;
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

	for (int i=0;i<winX;i++)
		for (int j=0;j<winY;j++)
		{
			intersection = 0;
			for(int k=0;k<shapes.size() && intersection!=1 ;k++)
			for(int l=0;l<shapes[k].mesh.indices.size() && intersection!=1 ;l+=3)
			{
			//write intersection detection algorithm for each pixel as below
			//1. find world coordinates of the pixel & do the following for each shape
			//2. find if there is an intersection by calling intersect3D_RayTriangle
			//3. if not, continue to next pixel for the same triangle;
			//   if yes, color the pixel acc to obj file
			//4. illuminate using data in mtl file
			/*________________________________________________________________*/
			//STEP 1-	FRONT CLIPPING PLANE CENTERED @ (0,0,-1). 
			//			SQUARE OF LENGTH 2
			//			TOP LEFT = (-1,1,-1), 1 PIXEL = 1/128 UNITS.
			//			Z COORDINATE WILL ALWAYS BE -1
				T.V0.x = shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3] * scaleX;
				T.V0.y = shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+1] * scaleY;
				T.V0.z = shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+2];
				T.V1.x = shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3] * scaleX;
				T.V1.y = shapes[k].mesh.positions[((shapes[k].mesh.indices[l+1])*3)+1]* scaleY;
				T.V1.z = shapes[k].mesh.positions[((shapes[k].mesh.indices[l+1])*3)+2];
				T.V2.x = shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3] * scaleX;
				T.V2.y = shapes[k].mesh.positions[((shapes[k].mesh.indices[l+2])*3)+1]* scaleY;
				T.V2.z = shapes[k].mesh.positions[((shapes[k].mesh.indices[l+2])*3)+2];
			
			x[i] = -1 + ((len/winX) * (i+1));
			y[j] = 1 - ((len/winY) * (j+1));
			R.P1.x = (float)x[i];
			R.P1.y = (float)y[j];
			R.P1.z = (float) -1;
			/*________________________________________________________________*/
			//STEP 2-	Intersection determination
			intersection = intersect3D_RayTriangle(R,T,&(I));
			/*________________________________________________________________*/
			//STEP 3-	Coloring the pixel
			if (intersection == 1)
			{
				if (tempShape == -1 || I.z < tempI.z) //either its first intersection or foremost yet
				{
					tempShape = l; // store index of intersecting triangle
					tempI = I; // store intersection point
				}
				intensity = calcIntensity(k,tempShape,T,tempI);
				tempColor = RGB((int)(255 * intensity.x),(int) (255 * intensity.y),(int) (255 * intensity.z));
				setPixel(i, j, tempColor);
			}
			else
				setPixel(i, j, blackColor); // background is assumed to be black
			}
		}	
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::cout<<"wndproc";
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		GetClientRect(hWnd, &rt);
		SetWindowHandle(hWnd);
		drawColor();
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	printf("callback");
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}