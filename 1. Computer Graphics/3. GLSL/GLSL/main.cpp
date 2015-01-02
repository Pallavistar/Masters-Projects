#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "tiny_obj_loader.h"

//-----------------------------------------------------------------------------
bool toonFlag;

class myWindow : public cwc::glutWindow
{
protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   GLuint ProgramObject;
   clock_t time0,time1;
   float timer010;					// timer counting 0->1->0
   bool bUp;						// flag if counting up or down.

public:
	myWindow(){}

	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
      //timer010 = 0.09; //for screenshot!
      glPushMatrix();
	  glTranslatef(0,0,-6.0);
	  if (shader) shader->begin();
         glRotatef(timer010*430, 5.5, 5.5f, 5.5);
         			std::string inputfile = "./inputs/cube.obj";
			std::vector<tinyobj::shape_t> shapes;
			std::string err = tinyobj::LoadObj(shapes, inputfile.c_str()); //populate "shapes" struct
			glBegin(GL_TRIANGLES);								// Drawing Using Triangles
			for(int k=0;k<shapes.size();k++)
				for(int l=0;l<shapes[k].mesh.indices.size();l+=3)
				{
					
					glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+1],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l])*3+2]);
					
					glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+1],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l+1])*3+2]);
					
					glVertex3f(shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+1],
						shapes[k].mesh.positions[(shapes[k].mesh.indices[l+2])*3+2]);
				}
			glEnd();											// Finished Drawing The Triangle
      if (shader) shader->end();
      glutSwapBuffers();
      glPopMatrix();

      UpdateTimer();

		Repaint();
	}

	virtual void OnIdle() {}

	virtual void OnInit()
	{
		glClearColor(0.2f, 0.2f, 0.6f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);

		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		if (shader==0) 
         std::cout << "Error Loading, compiling or linking shader\n";
		else
		{
         ProgramObject = shader->GetProgramObject();
		}

      time0 = clock();
      timer010 = 0.0f;
      bUp = true;

      DemoLight();

	}

	virtual void OnResize(int w, int h)
   {
      if(h == 0) h = 1;
	   float ratio = 1.0f * (float)w / (float)h;

      glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	
	   glViewport(0, 0, w, h);

      gluPerspective(45,ratio,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(0.0f,0.0f,4.0f, 
		          0.0,0.0,-1.0,
			       0.0f,1.0f,0.0f);
   }
	
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}

	virtual void OnKeyDown(int nKey, char cAscii)
	{       
		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		} 
		if(cAscii == 84 || cAscii ==116)
		{
			if (toonFlag) toonFlag = false;
			else toonFlag = true;
			if (toonFlag) {
				shader = SM.loadfromFile("vertexshader2.txt","fragmentshader2.txt"); // load (and compile, link) from file
				if (shader==0) 
				 std::cout << "Error Loading, compiling or linking shader\n";
				else
				{
				 ProgramObject = shader->GetProgramObject();
				}
				
			}
			else{
				shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
				if (shader==0) 
				 std::cout << "Error Loading, compiling or linking shader\n";
				else
				{
				 ProgramObject = shader->GetProgramObject();
				}
			}
		}

	};

	virtual void OnKeyUp(int nKey, char cAscii)
	{
      if (cAscii == 's')      // s: Shader
         shader->enable();
      else if (cAscii == 'f') // f: Fixed Function
         shader->disable();
	}

    void UpdateTimer()
   {
      time1 = clock();
      float delta = static_cast<float>(static_cast<double>(time1-time0)/static_cast<double>(CLOCKS_PER_SEC));
      delta = delta / 4;
      if (delta > 0.00005f)
      {
         time0 = clock();
         if (bUp)
         {
            timer010 += delta;
            if (timer010>=1.0f) { timer010 = 1.0f; bUp = false;}
         }
         else
         {
            timer010 -= delta;
            if (timer010<=0.0f) { timer010 = 0.0f; bUp = true;}
         }
      }
   }

    void DemoLight(void)
   {
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
     
     // Light model parameters:
     // -------------------------------------------
     
     GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
     glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
     glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
     // -------------------------------------------
     // Spotlight Attenuation
     
     GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
     GLint spot_exponent = 30;
     GLint spot_cutoff = 180;
     
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
     glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
     glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
     GLfloat Kc = 1.0;
     GLfloat Kl = 0.0;
     GLfloat Kq = 0.0;
     
     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
     // ------------------------------------------- 
     // Lighting parameters:

     GLfloat light_pos[] = {0.0f, 20.0f, -8.0f, 1.0f};
     GLfloat light_Ka[]  = {0.5f, 0.5f, 1.0f, 1.0f};
     GLfloat light_Kd[]  = {0.1f, 0.1f, 1.0f, 1.0f};
     GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

     // -------------------------------------------
     // Material parameters:

     GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
     GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
     GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
     GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
     GLfloat material_Se = 20.0f;

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
   }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() {std::cout << "psdeo_P4!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------

