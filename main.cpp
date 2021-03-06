/**************************************
*                                     *
*   Jeff Molofee's Basecode Example   *
*          nehe.gamedev.net           *
*                2001                 *
*                                     *
**************************************/

#include <windows.h>												// Header File For Windows
#include <gl\gl.h>													// Header File For The OpenGL32 Library
#include <gl\glu.h>													// Header File For The GLu32 Library
//#include <gl\glaux.h>												// Header File For The GLaux Library
#include <stdio.h>

#include "NeHeGL.h"													// Header File For NeHeGL

#include "math.h"												    // NEW: Needed For Sqrtf
#include "ArcBall.h"												// NEW: ArcBall Header
#include "block.hpp"

#include "sclog4c/sclog4c.h"
/*
#pragma comment( lib, "opengl32.lib" )								// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )									// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )									// Search For GLaux.lib While Linking
*/
#ifndef CDS_FULLSCREEN												// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4											// Compilers. By Defining It This Way,
#endif																// We Can Avoid Errors

GL_Window*	g_window;
Keys*		g_keys;

// User Defined Variables
GLUquadricObj *quadratic;											// Used For Our Quadric

const float PI2 = 2.0*3.1415926535f;								// PI Squared

Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,					// NEW: Last Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,					// NEW: This Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

ArcBallT    ArcBall(640.0f, 480.0f);				                // NEW: ArcBall Instance
Point2fT    MousePt;												// NEW: Current Mouse Point
bool        isClicked  = false;										// NEW: Clicking The Mouse?
bool        isRClicked = false;										// NEW: Clicking The Right Mouse Button?
bool        isDragging = false;					                    // NEW: Dragging The Mouse?

Block * blocks[DIM][DIM][DIM];


//enum TEXTURES
//{
//	LAVA,
//	NUM_TEXTURES
//}
GLuint texture[NUM_TEXTURES];

char * TexFileNames[]={
	"lava.bmp",
	"rock.bmp"
};

BOOL Initialize (GL_Window* window, Keys* keys)						// Any GL Init Code & User Initialiazation Goes Here
{
	g_window	= window;
	g_keys		= keys;

	// Start Of User Initialization
    isClicked   = false;								            // NEW: Clicking The Mouse?
    isDragging  = false;							                // NEW: Dragging The Mouse?

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);							// Black Background
	glClearDepth (1.0f);											// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);										// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);										// Enable Depth Testing
	glShadeModel (GL_FLAT);											// Select Flat Shading (Nice Definition Of Objects)
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// Set Perspective Calculations To Most Accurate

	quadratic=gluNewQuadric();										// Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);						// Create Smooth Normals
	gluQuadricTexture(quadratic, GL_TRUE);							// Create Texture Coords

	glEnable(GL_LIGHT0);											// Enable Default Light
	glEnable(GL_LIGHTING);											// Enable Lighting

	glEnable(GL_COLOR_MATERIAL);									// Enable Color Material

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_TEXTURE_2D);	
	
	freopen("log.txt","a",stderr);
		
	sclog4c_level=SL4C_ALL;
	
	 for(int i=0;i<DIM;++i)
    {
    	for(int j=0;j<DIM;++j)
    	{
    		for(int k=0;k<DIM;++k)
    		{
    			blocks[i][j][k]=new Block(ivec(i,j,k),(rand()%3==0)?Rock::air : Rock::granite);
    			if(blocks[i][j][k]->rock==Rock::air)
    			{
    				blocks[i][j][k]->temperature=1;
				}
			}
		}
	}
	return TRUE;													// Return TRUE (Initialization Successful)
}

void Deinitialize (void)											// Any User DeInitialization Goes Here
{
	gluDeleteQuadric(quadratic);
}

void Update (DWORD milliseconds)									// Perform Motion Updates Here
{
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)						// Is ESC Being Pressed?
		TerminateApplication (g_window);							// Terminate The Program

	if (g_keys->keyDown [VK_F1] == TRUE)							// Is F1 Being Pressed?
		ToggleFullscreen (g_window);						// Toggle Fullscreen Mode
		
	for(int i=0;i<DIM;++i)
    {
    	for(int j=0;j<DIM;++j)
    	{
    		for(int k=0;k<DIM;++k)
    		{
    			blocks[i][j][k]->update();
			}
		}
	}

/*
    if (isRClicked)													// If Right Mouse Clicked, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }
*/
/*
    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
        }
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;

            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
            isDragging = false;
    }
    */
}

void Torus(float MinorRadius, float MajorRadius)					// Draw A Torus With Normals
{
	int i, j;
	glBegin( GL_TRIANGLE_STRIP );									// Start A Triangle Strip
		for (i=0; i<20; i++ )										// Stacks
		{
			for (j=-1; j<20; j++)									// Slices
			{
				float wrapFrac = (j%20)/(float)20;
				float phi = PI2*wrapFrac;
				float sinphi = float(sin(phi));
				float cosphi = float(cos(phi));

				float r = MajorRadius + MinorRadius*cosphi;

				glNormal3f(float(sin(PI2*(i%20+wrapFrac)/(float)20))*cosphi, sinphi, float(cos(PI2*(i%20+wrapFrac)/(float)20))*cosphi);
				glVertex3f(float(sin(PI2*(i%20+wrapFrac)/(float)20))*r,MinorRadius*sinphi,float(cos(PI2*(i%20+wrapFrac)/(float)20))*r);

				glNormal3f(float(sin(PI2*(i+1%20+wrapFrac)/(float)20))*cosphi, sinphi, float(cos(PI2*(i+1%20+wrapFrac)/(float)20))*cosphi);
				glVertex3f(float(sin(PI2*(i+1%20+wrapFrac)/(float)20))*r,MinorRadius*sinphi,float(cos(PI2*(i+1%20+wrapFrac)/(float)20))*r);
			}
		}
	glEnd();														// Done Torus
}

void Draw (void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Screen And Depth Buffer
	glLoadIdentity();												// Reset The Current Modelview Matrix
	glTranslatef(-10.0f,-10.0f,-40.0f);									// Move Left 1.5 Units And Into The Screen 6.0
	glColor3f(1,0,0);
    for(int i=0;i<DIM;++i)
    {
    	for(int j=0;j<DIM;++j)
    	{
    		for(int k=0;k<DIM;++k)
    		{
    			blocks[i][j][k]->draw(i,j,k);
			}
		}
	}
    //glPushMatrix();													// NEW: Prepare Dynamic Transform
//    glMultMatrixf(Transform.M);										// NEW: Apply Dynamic Transform
	//glColor3f(0.75f,0.75f,1.0f);
	//Torus(0.30f,1.00f);
    //glPopMatrix();													// NEW: Unapply Dynamic Transform

	//glLoadIdentity();												// Reset The Current Modelview Matrix
	//glTranslatef(1.5f,0.0f,-6.0f);									// Move Right 1.5 Units And Into The Screen 7.0

    //glPushMatrix();													// NEW: Prepare Dynamic Transform
    //glMultMatrixf(Transform.M);										// NEW: Apply Dynamic Transform
	//glColor3f(1.0f,0.75f,0.75f);
	//gluSphere(quadratic,1.3f,20,20);
//    glPopMatrix();													// NEW: Unapply Dynamic Transform

	glFlush ();														// Flush The GL Rendering Pipeline
}



GLubyte *LoadBmp(const char *Path, int *Width, int *Height);
int LoadGLTextures()
{
	GLubyte *data=NULL;
	int width=0,height=0;
	//char FileName[64];
	glGenTextures(NUM_TEXTURES,&texture[0]);
	for(int i=0;i<NUM_TEXTURES;i++)
	{
		//sprintf(FileName,"Data\\%s\0",TexFileNames[i]);
		//FileName[63]='\0';
		if(data=LoadBmp(TexFileNames[i],&width,&height))
		{
			glBindTexture(GL_TEXTURE_2D,texture[i]);
			glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			if(data)free(data);
			data=NULL;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

GLubyte *LoadBmp(const char *Path, int *Width, int *Height)
{ 
	HANDLE hFile;
	DWORD FileSize, dwRead; 
	BITMAPFILEHEADER *fh=NULL; 
	BITMAPINFOHEADER *ih; 
	BYTE *pRaster; 
	hFile=CreateFileA(Path,GENERIC_READ,0,NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
	if (hFile==INVALID_HANDLE_VALUE) 
	{ 
		return NULL; 
	} 
	FileSize=GetFileSize(hFile,NULL); 
	fh=(BITMAPFILEHEADER *)malloc(FileSize); 
	ReadFile(hFile,fh,FileSize,&dwRead,NULL); 
	CloseHandle(hFile); 
	int len = FileSize - fh->bfOffBits;
	pRaster=(GLubyte *)malloc(len); 
	memcpy(pRaster, (BYTE *)fh+fh->bfOffBits, len); // RGB로 순서를 바꾼다. 
	for (BYTE *p=pRaster;p < pRaster + len - 3;p+=3) 
	{
		BYTE b = *p; 
		*p = *(p+2); 
		*(p+2) = b; 
	} 
	ih=(BITMAPINFOHEADER *)((PBYTE)fh+sizeof(BITMAPFILEHEADER)); 
	*Width=ih->biWidth;
	*Height=ih->biHeight; 
	free(fh); 
	return pRaster; 
}


