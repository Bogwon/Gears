
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__					// I have not tested any of this on Apple.
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#define	EXTERN
#include "structure.h"		// global data

#include "draw_gears.h"
#include "gl_drawing.h"
#include "load_data.h"
#include "prep_data.h"

	double	view_rotx	=  20.0;
	double	view_roty	=  30.0;
	double	view_rotz	=   0.0;
	float	pan			=  -2.0f;
	float	zoom		= -35.0f;
	float	_angle		= -70.0f;
//	int		SHADE		=	0;			// One of these days we will experiment with this.
//	#define	SHADE	3	// Set to 0 for no shading, 1 to set normals to each vertex
						// 2 to enable smooth shading, or 3 for both

	const	char*	help_message = 
//		"\nUse numbers 0 to 3 to control shading."
//		"\n    Up and down arrows to zoom in and out."
		"\nUse Up and down arrows to zoom in and out."
		"\n    Left and right arrows to pan."
		"\nChange viewpoint using the x, y & z keys."
		"\nUse UPPERCASE letters for clockwise rotation,"
		"\nand lowercase letters for counter-clockwise."
		"\nUse < & > (or , and .) to adjust speed."
		"\n    SPACEBAR for temporarily suspending animation."
		"\nPress ESCAPE to exit."
		"\n";

void processSpecialKeys(int key, int x, int y)
{
	#define	PAN_ADJUST	0.25
	#define	ZOOM_ADJUST	0.25
	
// Key values found on http://www.lighthouse3d.com/opengl/glut/index.php?5

	switch(key)
	{
	case GLUT_KEY_F1:		printf(help_message);	break;
	case GLUT_KEY_LEFT:		pan		-= PAN_ADJUST;	break;
	case GLUT_KEY_RIGHT:	pan		+= PAN_ADJUST;	break;
	case GLUT_KEY_UP:		zoom	+= ZOOM_ADJUST;	break;
	case GLUT_KEY_DOWN:		zoom	-= ZOOM_ADJUST;	break;

	default:	printf("Bad function key: 0x%x\n", key);
				printf(help_message);
				break;
	}
}



void handleKeypress(unsigned char key, int x, int y)	//Called when a key is pressed
{
	#define			AV_ADJUST	0.05F	// how much to change "factor" with each keypress
	static	int		stopped	=	0;
	static	float	factor	=	1.0;	// speed adjustment factor

	switch (key)
	{
	case 27:	exit(0);	//Escape key

	case 0x20:	if (stopped)
					adjust_speed(factor);
				else
					adjust_speed(0);
				stopped = !stopped;
				break;

/*	case '0':
	case '1':
	case '2':
	case '3':	SHADE = key - '0';		break;
*/
	case '?':	printf(help_message);	break;

	case 'x':	view_rotx+=5;	break;
	case 'X':	view_rotx-=5;	break;
	case 'y':	view_roty+=5;	break;
	case 'Y':	view_roty-=5;	break;
	case 'z':	view_rotz+=5;	break;
	case 'Z':	view_rotz-=5;	break;

	case ',':
	case '.':
	case '<':
	case '>':	if (((int)key == ',') || ((int)key == '<'))
					factor -= AV_ADJUST;
				else
					factor += AV_ADJUST;

				stopped = 0;
				adjust_speed(factor);
				break;

	default:	printf("Bad key: 0x%x\n", key);
				printf(help_message);
				break;
	}
}



void initRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}



void handleResize(int w, int h)
{
	if (h==0)
		h = 1;
	
	glMatrixMode	(GL_PROJECTION);
	glLoadIdentity	();			// Seems to work without this call,
								// but it is recommended as good practice.
	glViewport		(0, 0, w, h);

	gluPerspective	(45.0, (float)w / (float)h,    2.0, 200.0);
}



void drawScene(void)
{
	float ambientLight		[] = {  0.2f,  0.2f,  0.2f, 1.0f};
	float directedLight		[] = {  0.7f,  0.7f,  0.7f, 1.0f};
	float directedLightPos	[] = {-10.0f, 15.0f, 20.0f, 0.0f};

	glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode	(GL_MODELVIEW);
    glLoadIdentity	();
	glLightModelfv	(GL_LIGHT_MODEL_AMBIENT,  ambientLight);
	glLightfv		(GL_LIGHT0, GL_DIFFUSE,	 directedLight);
	glLightfv		(GL_LIGHT0, GL_POSITION, directedLightPos);

// Do translations and rotations here because that keeps the parameters local
// to this file.
//
// Do the translation first, so the center stays the same relative to the
// contraption. 
//
// Kludges based on image generated from testx.gr:
// - Shift a bit to the left so the image is centered on start up.
// - Initial value of zoom is set so we can see the whole thing.

	glTranslatef(pan, +3, zoom);	

// Rotate entire contraption as prescribed by keyboard inputs.

	glRotatef( view_rotx, 1.0, 0.0, 0.0 );
	glRotatef( view_roty, 0.0, 1.0, 0.0 );
	glRotatef( view_rotz, 0.0, 0.0, 1.0 );

	doRedraw();

	glutSwapBuffers();
}



int main(int argc, char** argv)
{
	#define	default_filename	"testx.gr"
	char	infile[0x100];
	int		ix;
	char*	ptr;

// Get configuration file name

	if (argc<2)
	{
		printf("No configuration file name supplied. Using " default_filename "\n\r");
		strcpy(infile, default_filename);
	}
	else
	{			// Giant kludge to accommodate single spaces within a path name

		ptr = infile;
		strcpy(ptr, argv[1]);
		for (ix=2; ix<argc; ix++)
		{
			ptr = strcat(ptr, " ");
			ptr = strcat(ptr, argv[ix]);
		}
	}
	getdata(infile);
	position_gears();
	assign_relative_speed();
	adjust_speed(1.0);

// void glutInitWindowPosition(int x, int y);	
// Setting x & y to -1 will cause the window manager to decide where the
// window will appear.

	glutInit				(&argc, argv);
	glutInitWindowPosition	(-1, -1);
	glutInitWindowSize		(400 * 2, 400 * 2);
	glutInitDisplayMode		(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow		("Gear Drawing Program using GLUT");

	initRendering();
	Generate_Display_Lists();
	
	glutDisplayFunc	(drawScene);
	glutIdleFunc	(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc	(processSpecialKeys);
	glutReshapeFunc	(handleResize);
	glutMainLoop	();
	return 0;
}


